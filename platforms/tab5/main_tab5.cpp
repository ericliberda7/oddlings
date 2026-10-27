// ============================================================================
// ODDLING — M5Stack Tab5 Large-Scale Sanctuary & Observatory Hub (1280x720)
// ----------------------------------------------------------------------------
// Target Hardware:
//   - M5Stack Tab5 (ESP32-P4 Dual-Core 400 MHz RISC-V, 32 MB PSRAM, 16 MB Flash)
//   - 5.0" 1280x720 MIPI-DSI IPS Multi-Touch Display (3.0x HD 960x720 Sanctuary
//     Viewport + 320x720 Observatory & Fleet Command Deck)
//   - ESP32-C6 Co-Processor Wireless Bridge (2.4 GHz ESP-NOW Channel 1) with
//     automatic standalone / simulated CoreS3 fleet fallback (PRD Gate H3 / FR-TAB-007)
// ============================================================================

#include "tab5_hal.hpp"
#include "../../render/embedded/tab5_hub_renderer.hpp"

#if defined(ARDUINO)
#include <M5Unified.h>
#include <Preferences.h>
#include <SPI.h>
#include <SD.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include <esp_heap_caps.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#if __has_include(<esp_cache.h>)
#include <esp_cache.h>
#endif
#if __has_include(<lgfx/v1/platforms/esp32p4/Panel_DSI.hpp>)
#include <lgfx/v1/platforms/esp32p4/Panel_DSI.hpp>
#define ODDLING_HAS_PANEL_DSI 1
#endif
#include <cstring>
#include <cstdio>

SET_LOOP_TASK_STACK_SIZE(48 * 1024);

static constexpr const char* kTab5OsVersion      = "ODDLINGS TAB5 HUB v2.8";
static constexpr uint32_t    kTab5BuildNum       = 280;
static constexpr const char* kTab5OtaManifestUrl = "https://oddlings.vercel.app/firmware/manifest.json";
static constexpr const char* kTab5OtaFirmwareUrl = "https://oddlings.vercel.app/firmware/tab5.bin";

static constexpr uint8_t BROADCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct RawTab5RxFrame {
    uint8_t  mac[6];
    int8_t   rssi_dbm;
    uint16_t len;
    uint8_t  data[oddling::protocol::MAX_WIRE_PACKET_BYTES];
};

static QueueHandle_t g_tab5RxQueue = nullptr;
static oddling::tab5hub::Tab5SanctuaryHub* g_hub = nullptr;
static bool g_c6EspNowOnline = false;

// Ping-Pong RGB565 DMA Band Buffers for 960px Viewport and 320px Sidebar
static constexpr int kTab5DmaLines = 48;
static constexpr int kTab5DmaPixels = 960 * kTab5DmaLines; // 92,160 bytes per buffer
static uint16_t* g_tab5DmaBounce[2] = {nullptr, nullptr};
static uint8_t   g_tab5DmaIdx = 0;

static uint32_t g_lastFrameMs     = 0;
static uint32_t g_lastTouchMs     = 0;
static uint32_t g_lastButtonMs    = 0;
static uint32_t g_lastPresenceMs  = 0;
static uint32_t g_lastTelemetryMs = 0;
static uint8_t  g_displayRotation = 3; // Rotation 3 = upright landscape on M5Stack Tab5!

#if !defined(CONFIG_ESP_HOSTED_ENABLED) || !CONFIG_ESP_HOSTED_ENABLED
static void onTab5EspNowRecv(
    const esp_now_recv_info_t* recv_info,
    const uint8_t* data,
    int data_len
) {
    if (!recv_info || !data || data_len <= 0 || data_len > static_cast<int>(oddling::protocol::MAX_WIRE_PACKET_BYTES)) {
        return;
    }
    RawTab5RxFrame frame{};
    std::memcpy(frame.mac, recv_info->src_addr, 6);
    frame.rssi_dbm = (recv_info->rx_ctrl) ? static_cast<int8_t>(recv_info->rx_ctrl->rssi) : -62;
    frame.len = static_cast<uint16_t>(data_len);
    std::memcpy(frame.data, data, data_len);
    if (g_tab5RxQueue) {
        xQueueSend(g_tab5RxQueue, &frame, 0);
    }
}
#endif

static bool initTab5EspNowBridge(uint8_t channel) {
#if !defined(CONFIG_ESP_HOSTED_ENABLED) || !CONFIG_ESP_HOSTED_ENABLED
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() == ESP_OK) {
        esp_now_register_recv_cb(onTab5EspNowRecv);
        esp_now_peer_info_t broadcastPeer{};
        std::memcpy(broadcastPeer.peer_addr, BROADCAST_MAC, 6);
        broadcastPeer.channel = channel;
        broadcastPeer.ifidx = WIFI_IF_STA;
        broadcastPeer.encrypt = false;
        esp_now_add_peer(&broadcastPeer);
        return true;
    }
#else
    (void)channel;
#endif
    return false;
}

static void deinitTab5EspNowBridge() {
#if !defined(CONFIG_ESP_HOSTED_ENABLED) || !CONFIG_ESP_HOSTED_ENABLED
    esp_now_deinit();
#endif
}

static void sendTab5EspNowBroadcast(const uint8_t* data, size_t len) {
#if !defined(CONFIG_ESP_HOSTED_ENABLED) || !CONFIG_ESP_HOSTED_ENABLED
    if (data && len > 0) {
        esp_now_send(BROADCAST_MAC, data, len);
    }
#else
    (void)data;
    (void)len;
#endif
}

static void pushCanvasRectToDisplayDMA(
    const oddling::render::MoebiusCanvas& canvas,
    int dst_x, int dst_y,
    int rx0, int ry0, int rx1, int ry1
) {
    int cw = canvas.width;
    int ch = canvas.height;
    rx0 = std::clamp(rx0, 0, cw - 1);
    ry0 = std::clamp(ry0, 0, ch - 1);
    rx1 = std::clamp(rx1, 0, cw - 1);
    ry1 = std::clamp(ry1, 0, ch - 1);
    if (rx0 > rx1 || ry0 > ry1) return;

#if defined(ODDLING_HAS_PANEL_DSI)
    auto* dsi = static_cast<lgfx::Panel_DSI*>(M5.Display.getPanel());
    uint16_t* __restrict fb = dsi ? static_cast<uint16_t*>(dsi->config_detail().buffer) : nullptr;
    if (fb) {
        const uint8_t* __restrict base_src = canvas.pixels.data();
        uint8_t rot = M5.Display.getRotation() & 3u;
        uint16_t tile[16][16];

        if (rot == 3) {
            for (int by0 = ry0; by0 <= ry1; by0 += 16) {
                int bh = std::min(16, ry1 - by0 + 1);
                int fb_col_base = dst_y + by0;
                for (int bx0 = rx0; bx0 <= rx1; bx0 += 16) {
                    int bw = std::min(16, rx1 - bx0 + 1);
                    for (int ty = 0; ty < bh; ++ty) {
                        const uint8_t* __restrict src = base_src + (static_cast<size_t>(by0 + ty) * cw + bx0) * 3;
                        for (int tx = 0; tx < bw; ++tx) {
                            tile[tx][ty] = static_cast<uint16_t>(
                                ((static_cast<uint32_t>(src[0]) & 0xF8u) << 8) |
                                ((static_cast<uint32_t>(src[1]) & 0xFCu) << 3) |
                                (static_cast<uint32_t>(src[2]) >> 3)
                            );
                            src += 3;
                        }
                    }
                    int sx_base = dst_x + bx0;
                    for (int tx = 0; tx < bw; ++tx) {
                        int fb_row = 1279 - (sx_base + tx);
                        std::memcpy(&fb[fb_row * 720 + fb_col_base], tile[tx], static_cast<size_t>(bh) * sizeof(uint16_t));
                    }
                }
            }
#if defined(ESP_CACHE_MSYNC_FLAG_DIR_C2M)
            int fb_r0 = std::clamp(1279 - (dst_x + rx1), 0, 1279);
            int fb_r1 = std::clamp(1279 - (dst_x + rx0), 0, 1279);
            uintptr_t start = reinterpret_cast<uintptr_t>(&fb[fb_r0 * 720]) & ~127u;
            uintptr_t end   = (reinterpret_cast<uintptr_t>(&fb[(fb_r1 + 1) * 720]) + 127u) & ~127u;
            if (end > start) {
                esp_cache_msync(reinterpret_cast<void*>(start), end - start, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_TYPE_DATA);
            }
#endif
            return;
        } else if (rot == 1) {
            for (int by0 = ry0; by0 <= ry1; by0 += 16) {
                int bh = std::min(16, ry1 - by0 + 1);
                int fb_col_base = 719 - (dst_y + by0 + bh - 1);
                for (int bx0 = rx0; bx0 <= rx1; bx0 += 16) {
                    int bw = std::min(16, rx1 - bx0 + 1);
                    for (int ty = 0; ty < bh; ++ty) {
                        const uint8_t* __restrict src = base_src + (static_cast<size_t>(by0 + ty) * cw + bx0) * 3;
                        int dst_ty = bh - 1 - ty;
                        for (int tx = 0; tx < bw; ++tx) {
                            tile[tx][dst_ty] = static_cast<uint16_t>(
                                ((static_cast<uint32_t>(src[0]) & 0xF8u) << 8) |
                                ((static_cast<uint32_t>(src[1]) & 0xFCu) << 3) |
                                (static_cast<uint32_t>(src[2]) >> 3)
                            );
                            src += 3;
                        }
                    }
                    int sx_base = dst_x + bx0;
                    for (int tx = 0; tx < bw; ++tx) {
                        int fb_row = sx_base + tx;
                        std::memcpy(&fb[fb_row * 720 + fb_col_base], tile[tx], static_cast<size_t>(bh) * sizeof(uint16_t));
                    }
                }
            }
#if defined(ESP_CACHE_MSYNC_FLAG_DIR_C2M)
            int fb_r0 = std::clamp(dst_x + rx0, 0, 1279);
            int fb_r1 = std::clamp(dst_x + rx1, 0, 1279);
            uintptr_t start = reinterpret_cast<uintptr_t>(&fb[fb_r0 * 720]) & ~127u;
            uintptr_t end   = (reinterpret_cast<uintptr_t>(&fb[(fb_r1 + 1) * 720]) + 127u) & ~127u;
            if (end > start) {
                esp_cache_msync(reinterpret_cast<void*>(start), end - start, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_TYPE_DATA);
            }
#endif
            return;
        }
    }
#endif

    int rw = rx1 - rx0 + 1;
    int max_band_h = std::max(1, kTab5DmaPixels / rw);
    const uint8_t* __restrict base_src = canvas.pixels.data();

    for (int by0 = ry0; by0 <= ry1; by0 += max_band_h) {
        int by1 = std::min(ry1, by0 + max_band_h - 1);
        int bh  = by1 - by0 + 1;
        uint16_t* __restrict dst = g_tab5DmaBounce[g_tab5DmaIdx & 1u];
        g_tab5DmaIdx ^= 1u;
        if (!dst) return;

        int dst_idx = 0;
        for (int y = by0; y <= by1; ++y) {
            const uint8_t* __restrict src = base_src + (y * cw + rx0) * 3;
            for (int x = 0; x < rw; ++x) {
                uint32_t rgb565 = ((static_cast<uint32_t>(src[0]) & 0xF8u) << 8) | ((static_cast<uint32_t>(src[1]) & 0xFCu) << 3) | (static_cast<uint32_t>(src[2]) >> 3);
                dst[dst_idx++] = static_cast<uint16_t>((rgb565 >> 8) | (rgb565 << 8));
                src += 3;
            }
        }
        M5.Display.waitDMA();
        M5.Display.pushImageDMA(dst_x + rx0, dst_y + by0, rw, bh, dst);
    }
}

static void pushCanvasToDisplayDMA(
    const oddling::render::MoebiusCanvas& canvas,
    int dst_x, int dst_y
) {
    pushCanvasRectToDisplayDMA(canvas, dst_x, dst_y, 0, 0, canvas.width - 1, canvas.height - 1);
}

static void loadTab5VaultFromNVS() {
    if (!g_hub) return;
    Preferences prefs;
    if (prefs.begin("odd_tab5", false)) {
        for (int s = 0; s < oddling::platforms::tab5::kMaxVaultSlots; ++s) {
            char key[12];
            std::snprintf(key, sizeof(key), "vlt%d", s);
            if (prefs.getBytesLength(key) == sizeof(oddling::tab5hub::VaultSlotEntry)) {
                prefs.getBytes(key, &g_hub->vault[s], sizeof(oddling::tab5hub::VaultSlotEntry));
            }
        }
        prefs.end();
    }
}

static void saveTab5VaultToNVS() {
    if (!g_hub) return;
    Preferences prefs;
    if (prefs.begin("odd_tab5", false)) {
        for (int s = 0; s < oddling::platforms::tab5::kMaxVaultSlots; ++s) {
            if (g_hub->vault[s].used) {
                char key[12];
                std::snprintf(key, sizeof(key), "vlt%d", s);
                prefs.putBytes(key, &g_hub->vault[s], sizeof(oddling::tab5hub::VaultSlotEntry));
            }
        }
        prefs.end();
    }
}

static bool mountTab5SD() {
    static bool s_mounted = false;
    if (s_mounted) return true;
    int8_t sck  = M5.getPin(m5::pin_name_t::sd_spi_sclk);
    int8_t miso = M5.getPin(m5::pin_name_t::sd_spi_cipo);
    int8_t mosi = M5.getPin(m5::pin_name_t::sd_spi_copi);
    int8_t cs   = M5.getPin(m5::pin_name_t::sd_spi_cs);
    if (sck < 0)  sck  = 43;
    if (miso < 0) miso = 39;
    if (mosi < 0) mosi = 44;
    if (cs < 0)   cs   = 42;
    SPI.begin(sck, miso, mosi, cs);
    if (SD.begin(cs, SPI, 25000000)) {
        s_mounted = true;
        return true;
    }
    return false;
}

static bool exportTab5FirmwareToSD(bool only_if_missing = false) {
    if (!g_hub) return false;
    if (!mountTab5SD()) {
        if (!only_if_missing) {
            std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "SD NOT DETECTED FOR EXPORT");
        }
        return false;
    }
    const esp_partition_t* running = esp_ota_get_running_partition();
    if (!running) {
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "OTA PARTITION READ ERR");
        return false;
    }
    uint32_t sketch_size = ESP.getSketchSize();
    if (sketch_size == 0 || sketch_size > running->size) {
        sketch_size = running->size;
    }

    if (!SD.exists("/firmware"))  SD.mkdir("/firmware");
    if (!SD.exists("/firmwares")) SD.mkdir("/firmwares");
    if (!SD.exists("/downloads")) SD.mkdir("/downloads");

    if (only_if_missing && SD.exists("/firmware/Oddlings_Tab5.bin") && SD.exists("/firmwares/Oddlings_Tab5.bin") && SD.exists("/firmware/tab5.bin")) {
        File chk = SD.open("/firmware/Oddlings_Tab5.bin", FILE_READ);
        if (chk && chk.size() == sketch_size) {
            chk.close();
            std::printf("[TAB5 SD EXPORT] /firmware/Oddlings_Tab5.bin already up-to-date (%u bytes)\n", static_cast<unsigned>(sketch_size));
            return true;
        }
        if (chk) chk.close();
    }

    static const char* kExportPaths[6] = {
        "/firmware/Oddlings_Tab5.bin",
        "/firmware/tab5.bin",
        "/firmwares/Oddlings_Tab5.bin",
        "/firmwares/tab5.bin",
        "/downloads/Oddlings_Tab5.bin",
        "/Oddlings_Tab5.bin"
    };

    constexpr size_t kChunk = 4096;
    uint8_t* buf = static_cast<uint8_t*>(heap_caps_malloc(kChunk, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (!buf) buf = static_cast<uint8_t*>(std::malloc(kChunk));
    if (!buf) return false;

    int exported_count = 0;
    for (int p = 0; p < 6; ++p) {
        File out = SD.open(kExportPaths[p], FILE_WRITE);
        if (!out) continue;
        size_t offset = 0;
        while (offset < sketch_size) {
            size_t to_read = std::min(kChunk, static_cast<size_t>(sketch_size - offset));
            if (esp_partition_read(running, offset, buf, to_read) != ESP_OK) break;
            out.write(buf, to_read);
            offset += to_read;
        }
        out.flush();
        out.close();
        if (offset == sketch_size) {
            exported_count++;
            std::printf("[TAB5 SD EXPORT] Wrote %s (%u bytes)\n", kExportPaths[p], static_cast<unsigned>(sketch_size));
        }
    }
    std::free(buf);
    if (exported_count == 0) {
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "FAILED TO WRITE SD .BIN");
        return false;
    }
    std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "SAVED TO /firmware & /firmwares");
    std::snprintf(g_hub->status_banner, sizeof(g_hub->status_banner), "SD: /firmware/Oddlings_Tab5.bin (%u KB)", static_cast<unsigned>(sketch_size / 1024));
    g_hub->sidebar_dirty = true;
    return true;
}

static bool performTab5OtaUpdate() {
    if (!g_hub) return false;

    // 1. Check MicroSD first (/oddlings_tab5_update.bin, /firmware/tab5.bin, /firmwares/tab5.bin, /tab5.bin)
    if (mountTab5SD()) {
        static const char* kSdPaths[6] = {
            "/oddlings_tab5_update.bin",
            "/firmware/oddlings_tab5_update.bin",
            "/firmwares/oddlings_tab5_update.bin",
            "/firmware/tab5.bin",
            "/firmwares/tab5.bin",
            "/tab5.bin"
        };
        for (int p = 0; p < 6; ++p) {
            if (!SD.exists(kSdPaths[p])) continue;
            File f = SD.open(kSdPaths[p], FILE_READ);
            if (!f) continue;
            size_t fsize = f.size();
            uint8_t magic = 0;
            if (fsize > 65536 && f.read(&magic, 1) == 1 && magic == 0xE9) {
                f.seek(0);
                if (Update.begin(fsize, U_FLASH)) {
                    uint8_t buf[4096];
                    size_t written = 0;
                    while (f.available()) {
                        size_t rd = f.read(buf, sizeof(buf));
                        if (rd == 0) break;
                        Update.write(buf, rd);
                        written += rd;
                    }
                    f.close();
                    if (written == fsize && Update.end(true)) {
                        if (p < 3) SD.remove(kSdPaths[p]);
                        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "SD OTA COMPLETE! REBOOTING");
                        delay(600);
                        ESP.restart();
                        return true;
                    }
                }
            }
            f.close();
        }
    }

    // 2. Load Wi-Fi credentials from NVS or SD (/oddlings/wifi.txt or /wifi.txt)
    char ssid[36] = {0};
    char pass[64] = {0};
    Preferences prefs;
    if (prefs.begin("odd_save", true)) {
        String s = prefs.getString("wifi_ssid", "");
        String p = prefs.getString("wifi_pass", "");
        if (s.length() > 0) {
            std::snprintf(ssid, sizeof(ssid), "%s", s.c_str());
            std::snprintf(pass, sizeof(pass), "%s", p.c_str());
        }
        prefs.end();
    }
    if (ssid[0] == '\0' && mountTab5SD()) {
        static const char* kWifiFiles[2] = { "/oddlings/wifi.txt", "/wifi.txt" };
        for (int i = 0; i < 2; ++i) {
            if (!SD.exists(kWifiFiles[i])) continue;
            File wf = SD.open(kWifiFiles[i], FILE_READ);
            if (!wf) continue;
            String l1 = wf.readStringUntil('\n');
            String l2 = wf.readStringUntil('\n');
            wf.close();
            l1.trim();
            l2.trim();
            if (l1.length() > 0) {
                std::snprintf(ssid, sizeof(ssid), "%s", l1.c_str());
                std::snprintf(pass, sizeof(pass), "%s", l2.c_str());
                break;
            }
        }
    }

    if (ssid[0] == '\0') {
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "PUT /wifi.txt ON SD FOR OTA");
        std::snprintf(g_hub->status_banner, sizeof(g_hub->status_banner), "OTA: ADD /wifi.txt (SSID/PASS) OR /tab5.bin ON SD");
        g_hub->sidebar_dirty = true;
        return false;
    }

    std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "CONNECTING %.16s...", ssid);
    g_hub->renderObservatorySidebar();
    pushCanvasToDisplayDMA(g_hub->sidebar_canvas, oddling::platforms::tab5::kTab5SidebarX, 0);

    deinitTab5EspNowBridge();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    uint32_t t0 = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - t0 < 10000)) {
        delay(150);
    }
    if (WiFi.status() != WL_CONNECTED) {
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "WIFI CONNECT TIMEOUT");
        g_c6EspNowOnline = initTab5EspNowBridge(1);
        g_hub->sidebar_dirty = true;
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(15);

    // Optional manifest check
    {
        HTTPClient http;
        if (http.begin(client, kTab5OtaManifestUrl)) {
            int code = http.GET();
            if (code == HTTP_CODE_OK) {
                String body = http.getString();
                int bIdx = body.indexOf("\"tab5_build\":");
                if (bIdx < 0) bIdx = body.indexOf("\"build\":");
                if (bIdx >= 0) {
                    int colon = body.indexOf(':', bIdx);
                    if (colon >= 0) {
                        uint32_t remoteBuild = static_cast<uint32_t>(body.substring(colon + 1).toInt());
                        std::printf("[TAB5 OTA] Remote build=%u, Local=%u\n",
                                    static_cast<unsigned>(remoteBuild), static_cast<unsigned>(kTab5BuildNum));
                    }
                }
            }
            http.end();
        }
    }

    HTTPClient http;
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    if (!http.begin(client, kTab5OtaFirmwareUrl)) {
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "HTTPS INIT ERR");
        g_c6EspNowOnline = initTab5EspNowBridge(1);
        g_hub->sidebar_dirty = true;
        return false;
    }

    int code = http.GET();
    int totalLen = http.getSize();
    if (code != HTTP_CODE_OK || totalLen <= 65536) {
        http.end();
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "HTTP %d NO TAB5.BIN", code);
        g_c6EspNowOnline = initTab5EspNowBridge(1);
        g_hub->sidebar_dirty = true;
        return false;
    }

    if (!Update.begin(static_cast<size_t>(totalLen), U_FLASH)) {
        http.end();
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "FLASH PART TOO SMALL");
        g_c6EspNowOnline = initTab5EspNowBridge(1);
        g_hub->sidebar_dirty = true;
        return false;
    }

    WiFiClient* stream = http.getStreamPtr();
    uint8_t buf[2048];
    size_t written = 0;
    uint32_t lastActivity = millis();
    int lastPct = -1;

    while (http.connected() && written < static_cast<size_t>(totalLen)) {
        size_t avail = stream->available();
        if (avail > 0) {
            size_t toRead = std::min(avail, sizeof(buf));
            int rd = stream->readBytes(buf, toRead);
            if (rd > 0) {
                if (written == 0 && buf[0] != 0xE9) {
                    Update.abort();
                    http.end();
                    std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "INVALID .BIN HEADER");
                    g_c6EspNowOnline = initTab5EspNowBridge(1);
                    g_hub->sidebar_dirty = true;
                    return false;
                }
                Update.write(buf, static_cast<size_t>(rd));
                written += static_cast<size_t>(rd);
                lastActivity = millis();
                int pct = static_cast<int>((written * 100ULL) / static_cast<size_t>(totalLen));
                if (pct != lastPct && (pct % 5 == 0 || pct == 100)) {
                    lastPct = pct;
                    std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "OTA FLASHING %d%% (%u KB)",
                                  pct, static_cast<unsigned>(written / 1024));
                    g_hub->renderObservatorySidebar();
                    pushCanvasToDisplayDMA(g_hub->sidebar_canvas, oddling::platforms::tab5::kTab5SidebarX, 0);
                }
            }
        } else {
            if (millis() - lastActivity > 12000) break;
            delay(5);
        }
    }
    http.end();

    if (written == static_cast<size_t>(totalLen) && Update.end(true)) {
        std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "OTA COMPLETE! REBOOTING");
        g_hub->renderObservatorySidebar();
        pushCanvasToDisplayDMA(g_hub->sidebar_canvas, oddling::platforms::tab5::kTab5SidebarX, 0);
        delay(800);
        ESP.restart();
        return true;
    }

    Update.abort();
    std::snprintf(g_hub->ota_status, sizeof(g_hub->ota_status), "OTA STREAM INTERRUPTED");
    g_c6EspNowOnline = initTab5EspNowBridge(1);
    g_hub->sidebar_dirty = true;
    return false;
}

void setup() {
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.clear_display = true;
    cfg.output_power = true;
    cfg.internal_imu = true;
    cfg.internal_rtc = true;
    M5.begin(cfg);

    uint32_t last_sd_bld = 0;
    {
        Preferences prefs;
        if (prefs.begin("odd_tab5", true)) {
            g_displayRotation = prefs.getUChar("rot", 3);
            if (g_displayRotation != 1 && g_displayRotation != 3) g_displayRotation = 3;
            last_sd_bld = prefs.getUInt("sd_bld", 0);
            prefs.end();
        }
    }

    M5.Display.setRotation(g_displayRotation);
    M5.Display.setBrightness(230);
    M5.Display.startWrite();

    for (int b = 0; b < 2; ++b) {
        g_tab5DmaBounce[b] = static_cast<uint16_t*>(
            heap_caps_malloc(kTab5DmaPixels * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
        );
        if (!g_tab5DmaBounce[b]) {
            g_tab5DmaBounce[b] = static_cast<uint16_t*>(
                heap_caps_malloc(kTab5DmaPixels * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
            );
        }
    }

    uint8_t mac[6]{};
    esp_read_mac(mac, ESP_MAC_BASE);
    uint64_t dev_id = 0x7AB5000000000000ULL;
    for (int i = 0; i < 6; ++i) dev_id = (dev_id << 8) | mac[i];

    g_hub = new oddling::tab5hub::Tab5SanctuaryHub(dev_id, static_cast<uint32_t>(esp_random()));
    if (last_sd_bld != kTab5BuildNum) {
        if (exportTab5FirmwareToSD(false)) {
            Preferences prefs;
            if (prefs.begin("odd_tab5", false)) {
                prefs.putUInt("sd_bld", kTab5BuildNum);
                prefs.end();
            }
        }
    } else {
        exportTab5FirmwareToSD(true);
    }

    g_tab5RxQueue = xQueueCreate(24, sizeof(RawTab5RxFrame));
    g_c6EspNowOnline = initTab5EspNowBridge(1);
    g_hub->sim_fleet_active = !g_c6EspNowOnline;
    g_hub->initDefaultSanctuary();
    loadTab5VaultFromNVS();

    g_lastFrameMs = millis();
    std::printf("[%s] 1280x720 Sanctuary (3.0x HD 4-Pod Viewport + CoreS3 Fleet Deck + Wireless OTA) Ready!\n", kTab5OsVersion);
}

void loop() {
    uint32_t now_ms = millis();
    if (now_ms - g_lastTouchMs >= 12) {
        g_lastTouchMs = now_ms;
        M5.update();
    }
    float dt = std::clamp((now_ms - g_lastFrameMs) * 0.001f, 0.004f, 0.08f);
    g_lastFrameMs = now_ms;

    // 1. Drain incoming ESP-NOW frames from CoreS3 units
    RawTab5RxFrame rx{};
    while (g_tab5RxQueue && xQueueReceive(g_tab5RxQueue, &rx, 0) == pdTRUE) {
        oddling::protocol::WireHeader hdr{};
        if (g_hub->net_node.receiveFrame(rx.data, rx.len, rx.rssi_dbm, now_ms, &hdr)) {
            g_hub->rx_frame_count++;
            g_hub->sim_fleet_active = false; // Real CoreS3 peer discovered!
            g_hub->sidebar_dirty = true;
        }
    }

    // 2. Broadcast Hub PRESENCE + STATE_SNAPSHOT every 2.0 seconds so nearby CoreS3 units see the Tab5 Hub!
    if (g_c6EspNowOnline && (now_ms - g_lastPresenceMs >= 2000)) {
        g_lastPresenceMs = now_ms;
        const auto& resPod = g_hub->pods[0];
        oddling::protocol::StateSnapshotPayload snap{};
        snap.catalog_seed    = resPod.catalog_seed;
        snap.markings_seed   = resPod.genome.appearance.markings_seed;
        snap.seed_high       = resPod.genome.seed128.high;
        snap.seed_low        = resPod.genome.seed128.low;
        for (int f = 0; f < 5; ++f) snap.family_weights[f] = resPod.genome.family_weights_bp[f];
        snap.palette_id      = resPod.genome.appearance.palette_id;
        snap.hue_offset_deg  = static_cast<int8_t>(resPod.genome.appearance.hue_offset_deg);
        snap.species_idx     = resPod.species_idx;
        snap.alt_idx         = resPod.alt_idx;
        snap.pattern_variant = resPod.pattern_variant;
        snap.evo_stage       = resPod.evo_stage;
        snap.evo_xp          = resPod.evo_xp;
        snap.bond_hearts     = resPod.bond_hearts;
        snap.biome_variant   = g_hub->sanctuary_biome;
        snap.mood_state      = static_cast<uint8_t>(resPod.sim.state);
        snap.is_hub_node     = 1u;
        snap.ownership_state = 0u;
        std::snprintf(snap.name, sizeof(snap.name), "TAB5:%.14s", resPod.name);

        uint8_t wire[oddling::protocol::MAX_WIRE_PACKET_BYTES];
        size_t len = g_hub->net_node.encodeFrame(oddling::protocol::MessageType::STATE_SNAPSHOT, &snap, sizeof(snap), wire);
        if (len > 0) {
            sendTab5EspNowBroadcast(wire, len);
            g_hub->tx_frame_count++;
        }
    }

    // 3. Poll 1280x720 Capacitive Multi-Touch
    auto touch = M5.Touch.getDetail();
    if (touch.wasPressed() && (now_ms - g_lastButtonMs > 135)) {
        g_lastButtonMs = now_ms;
        uint8_t txWire[oddling::protocol::MAX_WIRE_PACKET_BYTES];
        size_t txLen = 0;
        g_hub->handleTouch1280x720(touch.x, touch.y, txWire, txLen);
        if (txLen > 0 && g_c6EspNowOnline) {
            sendTab5EspNowBroadcast(txWire, txLen);
        }
        if (g_hub->flip_screen_requested) {
            g_hub->flip_screen_requested = false;
            g_displayRotation = (g_displayRotation == 3) ? 1 : 3;
            M5.Display.setRotation(g_displayRotation);
            Preferences prefs;
            if (prefs.begin("odd_tab5", false)) {
                prefs.putUChar("rot", g_displayRotation);
                prefs.end();
            }
            g_hub->bg_cache_key = 0;
            g_hub->sidebar_dirty = true;
        }
        if (g_hub->deck_tab == 2) {
            saveTab5VaultToNVS();
        }
        if (g_hub->sd_export_requested) {
            g_hub->sd_export_requested = false;
            exportTab5FirmwareToSD(false);
        }
        if (g_hub->ota_update_requested) {
            g_hub->ota_update_requested = false;
            performTab5OtaUpdate();
        }
    }

    // 4. Step Sanctuary Physics & Render 960x720 (dirty-rect pushed) + 320x720 Sidebar (on-demand)
    g_hub->stepSanctuary(dt, now_ms);
    g_hub->renderSanctuaryViewport();
    pushCanvasRectToDisplayDMA(
        g_hub->viewport_canvas, 0, 0,
        g_hub->push_vx0, g_hub->push_vy0,
        g_hub->push_vx1, g_hub->push_vy1
    );

    bool refresh_telemetry = (g_hub->deck_tab == 3) && (now_ms - g_lastTelemetryMs >= 1000);
    if (g_hub->sidebar_dirty || refresh_telemetry) {
        if (refresh_telemetry) g_lastTelemetryMs = now_ms;
        g_hub->renderObservatorySidebar();
        pushCanvasToDisplayDMA(g_hub->sidebar_canvas, oddling::platforms::tab5::kTab5SidebarX, 0);
    }
}

#elif defined(ESP_PLATFORM)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
extern "C" void app_main(void) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
