#pragma once
#include "rasterizer.hpp"
#include "../../shared/genetics/catalog_presets.hpp"
#include "../../shared/animation/controllers.hpp"
#include "../../shared/simulation/simulation.hpp"
#include "../../shared/protocol/espnow_protocol.hpp"
#include "../../platforms/tab5/tab5_hal.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace oddling {
namespace tab5hub {

using platforms::tab5::kTab5DisplayWidth;
using platforms::tab5::kTab5DisplayHeight;
using platforms::tab5::kTab5ViewportWidth;
using platforms::tab5::kTab5ViewportHeight;
using platforms::tab5::kTab5SidebarX;
using platforms::tab5::kTab5SidebarWidth;
using platforms::tab5::kMaxSanctuaryPods;
using platforms::tab5::kMaxVaultSlots;
using platforms::tab5::HubPeerConnectionState;
using platforms::tab5::SanctuaryPodRole;

// ============================================================================
// BITMAP FONT GLYPHS & SCALABLE TYPOGRAPHY FOR TAB5 1280x720 HD DISPLAY
// ============================================================================
inline void getHubGlyph5x7(char c, uint8_t out[7]) {
    std::memset(out, 0, 7);
    if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
    switch (c) {
        case 'A': { uint8_t g[7]={0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}; std::memcpy(out,g,7); break; }
        case 'B': { uint8_t g[7]={0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}; std::memcpy(out,g,7); break; }
        case 'C': { uint8_t g[7]={0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case 'D': { uint8_t g[7]={0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}; std::memcpy(out,g,7); break; }
        case 'E': { uint8_t g[7]={0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}; std::memcpy(out,g,7); break; }
        case 'F': { uint8_t g[7]={0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}; std::memcpy(out,g,7); break; }
        case 'G': { uint8_t g[7]={0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}; std::memcpy(out,g,7); break; }
        case 'H': { uint8_t g[7]={0x11,0x11,0x11,0x1F,0x11,0x11,0x11}; std::memcpy(out,g,7); break; }
        case 'I': { uint8_t g[7]={0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}; std::memcpy(out,g,7); break; }
        case 'J': { uint8_t g[7]={0x07,0x02,0x02,0x02,0x12,0x12,0x0C}; std::memcpy(out,g,7); break; }
        case 'K': { uint8_t g[7]={0x11,0x12,0x14,0x18,0x14,0x12,0x11}; std::memcpy(out,g,7); break; }
        case 'L': { uint8_t g[7]={0x10,0x10,0x10,0x10,0x10,0x10,0x1F}; std::memcpy(out,g,7); break; }
        case 'M': { uint8_t g[7]={0x11,0x1B,0x15,0x15,0x11,0x11,0x11}; std::memcpy(out,g,7); break; }
        case 'N': { uint8_t g[7]={0x11,0x19,0x15,0x13,0x11,0x11,0x11}; std::memcpy(out,g,7); break; }
        case 'O': { uint8_t g[7]={0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case 'P': { uint8_t g[7]={0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}; std::memcpy(out,g,7); break; }
        case 'Q': { uint8_t g[7]={0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}; std::memcpy(out,g,7); break; }
        case 'R': { uint8_t g[7]={0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}; std::memcpy(out,g,7); break; }
        case 'S': { uint8_t g[7]={0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}; std::memcpy(out,g,7); break; }
        case 'T': { uint8_t g[7]={0x1F,0x04,0x04,0x04,0x04,0x04,0x04}; std::memcpy(out,g,7); break; }
        case 'U': { uint8_t g[7]={0x11,0x11,0x11,0x11,0x11,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case 'V': { uint8_t g[7]={0x11,0x11,0x11,0x11,0x11,0x0A,0x04}; std::memcpy(out,g,7); break; }
        case 'W': { uint8_t g[7]={0x11,0x11,0x11,0x15,0x15,0x1B,0x11}; std::memcpy(out,g,7); break; }
        case 'X': { uint8_t g[7]={0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}; std::memcpy(out,g,7); break; }
        case 'Y': { uint8_t g[7]={0x11,0x11,0x0A,0x04,0x04,0x04,0x04}; std::memcpy(out,g,7); break; }
        case 'Z': { uint8_t g[7]={0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}; std::memcpy(out,g,7); break; }
        case '0': { uint8_t g[7]={0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case '1': { uint8_t g[7]={0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}; std::memcpy(out,g,7); break; }
        case '2': { uint8_t g[7]={0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}; std::memcpy(out,g,7); break; }
        case '3': { uint8_t g[7]={0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case '4': { uint8_t g[7]={0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}; std::memcpy(out,g,7); break; }
        case '5': { uint8_t g[7]={0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case '6': { uint8_t g[7]={0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case '7': { uint8_t g[7]={0x1F,0x01,0x02,0x04,0x08,0x08,0x08}; std::memcpy(out,g,7); break; }
        case '8': { uint8_t g[7]={0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}; std::memcpy(out,g,7); break; }
        case '9': { uint8_t g[7]={0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}; std::memcpy(out,g,7); break; }
        case '-': { uint8_t g[7]={0x00,0x00,0x00,0x1F,0x00,0x00,0x00}; std::memcpy(out,g,7); break; }
        case '+': { uint8_t g[7]={0x00,0x04,0x04,0x1F,0x04,0x04,0x00}; std::memcpy(out,g,7); break; }
        case '<': { uint8_t g[7]={0x02,0x04,0x08,0x10,0x08,0x04,0x02}; std::memcpy(out,g,7); break; }
        case '>': { uint8_t g[7]={0x08,0x04,0x02,0x01,0x02,0x04,0x08}; std::memcpy(out,g,7); break; }
        case '#': { uint8_t g[7]={0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00}; std::memcpy(out,g,7); break; }
        case ':': { uint8_t g[7]={0x00,0x04,0x04,0x00,0x04,0x04,0x00}; std::memcpy(out,g,7); break; }
        case '.': { uint8_t g[7]={0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}; std::memcpy(out,g,7); break; }
        case ',': { uint8_t g[7]={0x00,0x00,0x00,0x00,0x06,0x04,0x08}; std::memcpy(out,g,7); break; }
        case '/': { uint8_t g[7]={0x01,0x02,0x02,0x04,0x08,0x08,0x10}; std::memcpy(out,g,7); break; }
        case '%': { uint8_t g[7]={0x19,0x19,0x02,0x04,0x08,0x13,0x13}; std::memcpy(out,g,7); break; }
        case '&': { uint8_t g[7]={0x0C,0x12,0x14,0x08,0x15,0x12,0x0D}; std::memcpy(out,g,7); break; }
        case '?': { uint8_t g[7]={0x0E,0x11,0x01,0x06,0x04,0x00,0x04}; std::memcpy(out,g,7); break; }
        case '_': { uint8_t g[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x1F}; std::memcpy(out,g,7); break; }
        case '[': { uint8_t g[7]={0x0E,0x08,0x08,0x08,0x08,0x08,0x0E}; std::memcpy(out,g,7); break; }
        case ']': { uint8_t g[7]={0x0E,0x02,0x02,0x02,0x02,0x02,0x0E}; std::memcpy(out,g,7); break; }
        case '!': { uint8_t g[7]={0x04,0x04,0x04,0x04,0x04,0x00,0x04}; std::memcpy(out,g,7); break; }
        case '*': { uint8_t g[7]={0x04,0x15,0x0E,0x1F,0x0E,0x15,0x04}; std::memcpy(out,g,7); break; }
        case '@': { uint8_t g[7]={0x0E,0x11,0x17,0x15,0x17,0x10,0x0E}; std::memcpy(out,g,7); break; }
        case '(': { uint8_t g[7]={0x02,0x04,0x08,0x08,0x08,0x04,0x02}; std::memcpy(out,g,7); break; }
        case ')': { uint8_t g[7]={0x08,0x04,0x02,0x02,0x02,0x04,0x08}; std::memcpy(out,g,7); break; }
        case '|': { uint8_t g[7]={0x04,0x04,0x04,0x04,0x04,0x04,0x04}; std::memcpy(out,g,7); break; }
        default: break;
    }
}

inline void drawHubText(
    render::MoebiusCanvas& fb, int x, int y, const char* text,
    render::ColorRGB col, int scale_x = 2, int scale_y = 2
) {
    int cx = x;
    for (size_t i = 0; text[i] != '\0'; ++i) {
        uint8_t g[7];
        getHubGlyph5x7(text[i], g);
        for (int row = 0; row < 7; ++row) {
            for (int bit = 0; bit < 5; ++bit) {
                if ((g[row] >> (4 - bit)) & 1u) {
                    for (int sy = 0; sy < scale_y; ++sy) {
                        for (int sx = 0; sx < scale_x; ++sx) {
                            int px = cx + bit * scale_x + sx;
                            int py = y + row * scale_y + sy;
                            fb.blendPixelPx(px + 1, py + 1, {10, 8, 16}, 0.90f);
                            fb.blendPixelPx(px,     py,     col, 1.0f);
                        }
                    }
                }
            }
        }
        cx += (5 * scale_x) + std::max(1, scale_x);
    }
}

inline void drawHubBox(
    render::MoebiusCanvas& fb,
    int x0, int y0, int x1, int y1,
    render::ColorRGB fill, float fill_alpha,
    render::ColorRGB border, int border_thick = 2
) {
    x0 = std::clamp(x0, 0, fb.width - 1);
    x1 = std::clamp(x1, 0, fb.width - 1);
    y0 = std::clamp(y0, 0, fb.height - 1);
    y1 = std::clamp(y1, 0, fb.height - 1);
    for (int y = y0; y <= y1; ++y) {
        bool border_y = (y - y0 < border_thick) || (y1 - y < border_thick);
        for (int x = x0; x <= x1; ++x) {
            bool border_x = (x - x0 < border_thick) || (x1 - x < border_thick);
            if (border_x || border_y) {
                fb.blendPixelPx(x, y, border, 0.95f);
            } else {
                fb.blendPixelPx(x, y, fill, fill_alpha);
            }
        }
    }
}

inline void drawHubHeart(render::MoebiusCanvas& fb, int x, int y, render::ColorRGB col, int scale = 2) {
    static const uint8_t kHeart[6] = {
        0b0110110,
        0b1111111,
        0b1111111,
        0b0111110,
        0b0011100,
        0b0001000
    };
    for (int r = 0; r < 6; ++r) {
        for (int c = 0; c < 7; ++c) {
            if ((kHeart[r] >> (6 - c)) & 1u) {
                for (int sy = 0; sy < scale; ++sy) {
                    for (int sx = 0; sx < scale; ++sx) {
                        fb.blendPixelPx(x + c * scale + sx + 1, y + r * scale + sy + 1, {18, 12, 22}, 0.85f);
                        fb.blendPixelPx(x + c * scale + sx,     y + r * scale + sy,     col, 1.0f);
                    }
                }
            }
        }
    }
}

// ============================================================================
// SANCTUARY POD & FLEET TELEMETRY STATE
// ============================================================================
struct SanctuaryPod {
    bool                          active           = false;
    SanctuaryPodRole              role             = SanctuaryPodRole::SANCTUARY_COMPANION;
    uint64_t                      linked_device_id = 0;
    uint32_t                      catalog_seed     = 1;
    uint8_t                       species_idx      = 0;
    uint8_t                       alt_idx          = 0;
    uint8_t                       pattern_variant  = 0;
    uint8_t                       evo_stage        = 2; // 0=BABY, 1=JUV, 2=ADULT, 3=ELDER*
    uint16_t                      evo_xp           = 420;
    uint8_t                       bond_hearts      = 3;
    uint8_t                       biome_variant    = 0;
    char                          name[20]         = "ODDLING";
    genetics::GenomeV1            genome{};
    genetics::MorphologyGraph     graph{};
    simulation::CreatureSimulation sim{};
    animation::RigPoseState       pose{};
    float                         home_x           = 160.0f;
    float                         hop_y            = 0.0f;
    float                         hop_vy           = 0.0f;
    float                         heart_timer      = 0.0f;
};

struct VaultSlotEntry {
    uint32_t magic           = 0;
    uint8_t  used            = 0;
    uint32_t catalog_seed    = 1;
    uint32_t markings_seed   = 0;
    uint8_t  palette_id      = 0;
    uint8_t  evo_stage       = 2;
    uint8_t  bond_hearts     = 3;
    uint8_t  biome_variant   = 0;
    char     name[20]        = "";
};

struct HubPeerSlotView {
    bool                   occupied       = false;
    uint64_t               device_id      = 0;
    uint64_t               creature_id    = 0;
    int8_t                 rssi_dbm       = -90;
    protocol::ProximityTier tier          = protocol::ProximityTier::UNSEEN;
    HubPeerConnectionState conn_state     = HubPeerConnectionState::OFFLINE_SNAPSHOT;
    uint32_t               catalog_seed   = 1;
    uint8_t                evo_stage      = 2;
    uint8_t                bond_hearts    = 3;
    uint8_t                biome_variant  = 0;
    uint8_t                assigned_pod   = 0xFF;
    char                   name[20]       = "CORES3 PEER";
};

// ============================================================================
// LARGE-SCALE TAB5 SANCTUARY & OBSERVATORY HUB ENGINE (1280x720)
// ============================================================================
class Tab5SanctuaryHub {
public:
    render::MoebiusCanvas      viewport_canvas; // 960x720 (3.0x HD Moebius Sanctuary)
    render::MoebiusCanvas      sidebar_canvas;  // 320x720 (Observatory & Hub Command Deck)
    worldgen::ProceduralWorld  world{};
    uint8_t                    sanctuary_biome  = 7; // Default: Coral Abyss Reef / Arzach

    SanctuaryPod               pods[kMaxSanctuaryPods]{};
    int                        selected_pod     = 0;
    int                        breed_partner_pod = 1;

    protocol::EspNowPeerNode   net_node;
    HubPeerSlotView            peer_views[4]{};
    int                        selected_peer    = 0;
    bool                       sim_fleet_active = true; // Simulated CoreS3 fleet when no external radio/peer in range (FR-TAB-007)
    uint32_t                   tx_frame_count   = 0;
    uint32_t                   rx_frame_count   = 0;

    // Interactive Sanctuary Toy Ball, Spore-Treats, Chorus, Noir Ink & Deep-Scan Inspector
    bool                       lod2_anatomy_scan = false;
    bool                       close_up_zoom     = false;
    bool                       noir_mode         = false;
    bool                       ota_update_requested  = false;
    bool                       sd_export_requested   = false;
    bool                       flip_screen_requested = false;
    char                       ota_status[48]    = "OTA READY: oddlings.vercel.app/firmware";
    bool                       treat_active      = false;
    float                      treat_x           = 160.0f;
    float                      treat_y           = 68.0f;
    float                      treat_phase       = 0.0f;

    bool                       ball_active       = true;
    float                      ball_x            = 196.0f;
    float                      ball_y            = 148.0f;
    float                      ball_vx           = 64.0f;
    float                      ball_vy           = -42.0f;
    float                      ball_angle        = 0.0f;

    bool                       chorus_active     = false;
    float                      chorus_timer      = 0.0f;

    // Command Deck Tabs: 0 = PODS & S3 RADAR, 1 = DNA SCULPTOR & BREEDER, 2 = 16-SLOT VAULT, 3 = SEED KEYPAD & NET
    int                        deck_tab          = 0;
    int                        vault_page        = 0;
    VaultSlotEntry             vault[kMaxVaultSlots]{};
    char                       seed_input_buf[10] = "46";
    char                       status_banner[48]  = "TAB5 SANCTUARY HUB READY - 4 PODS LIVE";
    uint32_t                   status_until_ms    = 0;
    float                      fps_smooth         = 60.0f;
    bool                       sidebar_dirty      = true;

    // High-speed PSRAM Background & Creature Compositor Cache for 60 FPS Tab5 Embedded Rendering
    render::MoebiusCanvas      creature_canvas;   // 480x360 (1.5x HD) fast animated creature & toy layer
    render::PixelBuffer        bg_cache_pixels;   // 960x720 (3.0x HD) cached static world + top/bottom HUD bars
    render::PixelBuffer        bg_cache_half;     // 480x360 (1.5x HD) cached static world for creature alpha-blending
    uint64_t                   bg_cache_key       = 0;
    bool                       viewport_full_dirty = true;
    int                        prev_dirty_vx0     = 0;
    int                        prev_dirty_vy0     = 56;
    int                        prev_dirty_vx1     = kTab5ViewportWidth - 1;
    int                        prev_dirty_vy1     = 662;
    int                        push_vx0           = 0;
    int                        push_vy0           = 0;
    int                        push_vx1           = kTab5ViewportWidth - 1;
    int                        push_vy1           = kTab5ViewportHeight - 1;
    render::PixelBuffer        dna_preview_cache;
    uint64_t                   dna_preview_key    = 0;

    Tab5SanctuaryHub(uint64_t hub_device_id = 0x5441423548554201ULL, uint32_t session_nonce = 0x7AB52026u)
        : viewport_canvas(kTab5ViewportWidth, kTab5ViewportHeight),
          sidebar_canvas(kTab5SidebarWidth, kTab5DisplayHeight),
          net_node(hub_device_id, 0x7AB50001ULL, session_nonce),
          creature_canvas(480, 360) {}

    void initDefaultSanctuary() {
        // Initialize Pod 0 as the signature Bell Medusa (#000001) centered in the main Sanctuary panel,
        // with Pods 1..3 populated for the sidebar Pods list, Vault, and DNA Hybrid Lab
        static const uint32_t kDefaultSeeds[4]  = {1u, 4u, 25u, 46u};
        static const uint8_t  kDefaultStages[4] = {2u, 2u, 2u, 2u};
        static const float    kHomeX[4]         = {160.0f, 160.0f, 160.0f, 160.0f};
        static const SanctuaryPodRole kRoles[4] = {
            SanctuaryPodRole::TAB5_RESIDENT,
            SanctuaryPodRole::VISITING_CORES3,
            SanctuaryPodRole::VISITING_CORES3,
            SanctuaryPodRole::SANCTUARY_COMPANION
        };

        for (int i = 0; i < kMaxSanctuaryPods; ++i) {
            loadPodByCatalogSeed(i, kDefaultSeeds[i], kDefaultStages[i], kRoles[i], kHomeX[i]);
        }

        // Load Sanctuary World (Biome 0: Sandstone Canyon)
        sanctuary_biome = 0;
        world = catalog::buildWorldForBiomeVariant(sanctuary_biome, 1u);

        // Pre-populate Vault Slots 1..4 with iconic seeds
        for (int s = 0; s < 4; ++s) {
            vault[s].magic         = 0x7AB5FA76u;
            vault[s].used          = 1u;
            vault[s].catalog_seed  = pods[s].catalog_seed;
            vault[s].markings_seed = pods[s].genome.appearance.markings_seed;
            vault[s].palette_id    = pods[s].genome.appearance.palette_id;
            vault[s].evo_stage     = pods[s].evo_stage;
            vault[s].bond_hearts   = pods[s].bond_hearts;
            vault[s].biome_variant = pods[s].biome_variant;
            std::snprintf(vault[s].name, sizeof(vault[s].name), "%.19s", pods[s].name);
        }

        // Initialize simulated CoreS3 fleet so the Hub works out-of-the-box and in standalone mode (FR-TAB-007)
        if (sim_fleet_active) {
            seedSimulatedCoreS3Fleet(1000);
        }
        sidebar_dirty = true;
    }

    void loadPodByCatalogSeed(
        int pod_idx,
        uint32_t seed_num,
        uint8_t stage,
        SanctuaryPodRole role,
        float home_x = -1.0f
    ) {
        if (pod_idx < 0 || pod_idx >= kMaxSanctuaryPods) return;
        SanctuaryPod& pod = pods[pod_idx];
        catalog::CatalogOddlingSpec spec{};
        catalog::synthesizeFromCatalogSeed(seed_num, spec);

        pod.active          = true;
        pod.role            = role;
        pod.catalog_seed    = spec.catalog_seed;
        pod.species_idx     = static_cast<uint8_t>(spec.species_idx);
        pod.alt_idx         = static_cast<uint8_t>(spec.alt_idx);
        pod.pattern_variant = spec.pattern_variant;
        pod.evo_stage       = stage & 3u;
        pod.evo_xp          = 480;
        pod.bond_hearts     = (pod_idx == 0) ? 4 : 3;
        pod.biome_variant   = spec.biome_variant;
        std::snprintf(pod.name, sizeof(pod.name), "%.19s", spec.title);
        pod.genome          = spec.genome;
        pod.graph           = spec.graph;
        static const float kDefaultHomes[4] = {160.0f, 160.0f, 160.0f, 160.0f};
        pod.home_x = (home_x > 0.0f) ? home_x : kDefaultHomes[pod_idx & 3];

        float gy = world.sampleGroundY(pod.home_x);
        if (gy <= 10.0f) gy = 185.0f;
        pod.sim.init(pod.genome);
        pod.sim.target_x = pod.home_x;
        animation::initRigPose(pod.pose, pod.graph, pod.home_x, gy);
        for (int step = 0; step < 24; ++step) {
            animation::stepProceduralAnimation(
                pod.pose, pod.graph, 0.033f,
                {0.0f, 0.0f}, {160.0f, 128.0f},
                false, 0.35f, gy
            );
        }
        pod.pose.camera_focus.x = 160.0f;
        if (pod_idx == selected_pod) {
            std::snprintf(seed_input_buf, sizeof(seed_input_buf), "%u", static_cast<unsigned>(pod.catalog_seed));
        }
        sidebar_dirty = true;
    }

    void seedSimulatedCoreS3Fleet(uint32_t now_ms) {
        struct SimPeerDef {
            uint64_t dev_id;
            uint64_t creat_id;
            uint32_t seed;
            uint8_t  stage;
            int8_t   rssi;
            const char* name;
            uint8_t  pod_slot;
            HubPeerConnectionState state;
        };
        static const SimPeerDef kSimFleet[3] = {
            {0xC0530001A4B2ULL, 0x1001ULL, 1u,  2u, -48, "S3-MEDUSA #000001", 0, HubPeerConnectionState::VISITING},
            {0xC0530002B8C4ULL, 0x1002ULL, 4u,  2u, -58, "S3-CRANE  #000004", 1, HubPeerConnectionState::VISITING},
            {0xC0530003D9E6ULL, 0x1003ULL, 31u, 3u, -66, "S3-CYCLOPS#000031", 0xFF, HubPeerConnectionState::NEARBY}
        };

        for (int i = 0; i < 3; ++i) {
            catalog::CatalogOddlingSpec spec{};
            catalog::synthesizeFromCatalogSeed(kSimFleet[i].seed, spec);

            protocol::StateSnapshotPayload snap{};
            snap.catalog_seed    = spec.catalog_seed;
            snap.markings_seed   = spec.genome.appearance.markings_seed;
            snap.seed_high       = spec.genome.seed128.high;
            snap.seed_low        = spec.genome.seed128.low;
            for (int f = 0; f < 5; ++f) snap.family_weights[f] = spec.genome.family_weights_bp[f];
            snap.palette_id      = spec.genome.appearance.palette_id;
            snap.hue_offset_deg  = static_cast<int8_t>(spec.genome.appearance.hue_offset_deg);
            snap.species_idx     = static_cast<uint8_t>(spec.species_idx);
            snap.alt_idx         = static_cast<uint8_t>(spec.alt_idx);
            snap.pattern_variant = spec.pattern_variant;
            snap.evo_stage       = kSimFleet[i].stage;
            snap.evo_xp          = 640;
            snap.bond_hearts     = 4;
            snap.biome_variant   = spec.biome_variant;
            snap.mood_state      = static_cast<uint8_t>(simulation::BehaviorState::APPROACH);
            snap.is_hub_node     = 0;
            snap.ownership_state = (kSimFleet[i].state == HubPeerConnectionState::VISITING) ? 1 : 0;
            std::snprintf(snap.name, sizeof(snap.name), "%.19s", kSimFleet[i].name);

            protocol::EspNowPeerNode s3_node(kSimFleet[i].dev_id, kSimFleet[i].creat_id, 0x5300u + i);
            uint8_t wire[protocol::MAX_WIRE_PACKET_BYTES];
            for (int sample = 0; sample < 3; ++sample) {
                size_t len = s3_node.encodeFrame(protocol::MessageType::STATE_SNAPSHOT, &snap, sizeof(snap), wire);
                net_node.receiveFrame(wire, len, kSimFleet[i].rssi, now_ms + sample * 50);
                rx_frame_count++;
            }
            if (kSimFleet[i].pod_slot < kMaxSanctuaryPods) {
                pods[kSimFleet[i].pod_slot].linked_device_id = kSimFleet[i].dev_id;
                pods[kSimFleet[i].pod_slot].role = SanctuaryPodRole::VISITING_CORES3;
            }
        }
        refreshPeerViews(now_ms + 200);
    }

    void refreshPeerViews(uint32_t now_ms) {
        int v_idx = 0;
        for (size_t i = 0; i < protocol::MAX_PEERS && v_idx < 4; ++i) {
            protocol::PeerEntry& p = net_node.peers[i];
            if (!p.active) continue;
            p.proximity.checkTimeout(now_ms);
            HubPeerSlotView& v = peer_views[v_idx++];
            v.occupied     = true;
            v.device_id    = p.device_id;
            v.creature_id  = p.creature_id;
            v.rssi_dbm     = static_cast<int8_t>(p.proximity.smoothed_rssi_dbm);
            v.tier         = p.proximity.current_tier;
            v.assigned_pod = 0xFF;

            for (int pod_i = 0; pod_i < kMaxSanctuaryPods; ++pod_i) {
                if (pods[pod_i].active && pods[pod_i].linked_device_id == p.device_id) {
                    v.assigned_pod = static_cast<uint8_t>(pod_i);
                    break;
                }
            }

            if (p.has_snapshot) {
                v.catalog_seed  = p.last_snapshot.catalog_seed;
                v.evo_stage     = p.last_snapshot.evo_stage & 3u;
                v.bond_hearts   = p.last_snapshot.bond_hearts;
                v.biome_variant = p.last_snapshot.biome_variant;
                std::snprintf(v.name, sizeof(v.name), "%.19s", p.last_snapshot.name);

                // If this CoreS3 peer is linked to a live Sanctuary Pod, mirror its snapshot seamlessly!
                if (v.assigned_pod < kMaxSanctuaryPods) {
                    SanctuaryPod& pod = pods[v.assigned_pod];
                    if (pod.catalog_seed != p.last_snapshot.catalog_seed ||
                        pod.genome.appearance.markings_seed != p.last_snapshot.markings_seed ||
                        pod.genome.appearance.palette_id != p.last_snapshot.palette_id) {
                        catalog::CatalogOddlingSpec spec{};
                        catalog::reconstructFromStateSnapshot(p.last_snapshot, spec);
                        pod.catalog_seed    = spec.catalog_seed;
                        pod.species_idx     = static_cast<uint8_t>(spec.species_idx);
                        pod.alt_idx         = static_cast<uint8_t>(spec.alt_idx);
                        pod.pattern_variant = spec.pattern_variant;
                        pod.genome          = spec.genome;
                        pod.graph           = spec.graph;
                        std::snprintf(pod.name, sizeof(pod.name), "%.19s", spec.title);
                    }
                    pod.evo_stage   = p.last_snapshot.evo_stage & 3u;
                    pod.bond_hearts = std::clamp<uint8_t>(p.last_snapshot.bond_hearts, 1, 4);
                }
            } else {
                v.catalog_seed = 1u;
                std::snprintf(v.name, sizeof(v.name), "S3-%04X", static_cast<unsigned>(p.device_id & 0xFFFFu));
            }

            if (v.tier == protocol::ProximityTier::UNSEEN) {
                v.conn_state = HubPeerConnectionState::OFFLINE_SNAPSHOT;
            } else if (v.assigned_pod < kMaxSanctuaryPods) {
                v.conn_state = (pods[v.assigned_pod].role == SanctuaryPodRole::LEASED_CORES3)
                    ? HubPeerConnectionState::TRANSFERRED
                    : HubPeerConnectionState::VISITING;
            } else if (p.trusted_paired) {
                v.conn_state = HubPeerConnectionState::CONNECTED;
            } else {
                v.conn_state = HubPeerConnectionState::NEARBY;
            }
        }
        for (; v_idx < 4; ++v_idx) {
            peer_views[v_idx] = HubPeerSlotView{};
        }
    }

    // Invite selected CoreS3 peer into a Sanctuary Pod (Pod 1..3) at 3.0x HD!
    bool invitePeerToSanctuaryPod(int peer_idx, int target_pod, uint8_t outWire[protocol::MAX_WIRE_PACKET_BYTES], size_t& outLen) {
        outLen = 0;
        if (peer_idx < 0 || peer_idx >= 4 || !peer_views[peer_idx].occupied) return false;
        if (target_pod <= 0 || target_pod >= kMaxSanctuaryPods) target_pod = 1;

        HubPeerSlotView& pv = peer_views[peer_idx];
        loadPodByCatalogSeed(target_pod, pv.catalog_seed, pv.evo_stage, SanctuaryPodRole::VISITING_CORES3);
        pods[target_pod].linked_device_id = pv.device_id;
        std::snprintf(pods[target_pod].name, sizeof(pods[target_pod].name), "%.19s", pv.name);
        pv.assigned_pod = static_cast<uint8_t>(target_pod);
        pv.conn_state   = HubPeerConnectionState::VISITING;
        selected_pod    = target_pod;

        protocol::HubCommandPayload cmd{};
        cmd.target_device_id = pv.device_id;
        cmd.action           = static_cast<uint8_t>(protocol::HubRemoteAction::REMOTE_INVITE_POD);
        cmd.pod_slot         = static_cast<uint8_t>(target_pod);
        std::snprintf(cmd.beam_name, sizeof(cmd.beam_name), "TAB5 HUB POD %d", target_pod + 1);
        outLen = net_node.encodeFrame(protocol::MessageType::EMOTE, &cmd, sizeof(cmd), outWire);
        tx_frame_count++;

        std::snprintf(status_banner, sizeof(status_banner), "INVITED %.12s TO POD %d!", pv.name, target_pod + 1);
        sidebar_dirty = true;
        return true;
    }

    // Atomic Ownership Lease Transfer between CoreS3 and Tab5 Sanctuary Hub (FR-TAB-005)
    bool togglePeerOwnershipLease(int peer_idx, uint8_t outWire[protocol::MAX_WIRE_PACKET_BYTES], size_t& outLen) {
        outLen = 0;
        if (peer_idx < 0 || peer_idx >= 4 || !peer_views[peer_idx].occupied) return false;
        HubPeerSlotView& pv = peer_views[peer_idx];
        int pod_slot = (pv.assigned_pod < kMaxSanctuaryPods) ? pv.assigned_pod : 1;

        bool returning_to_s3 = (pods[pod_slot].role == SanctuaryPodRole::LEASED_CORES3);
        if (!returning_to_s3) {
            loadPodByCatalogSeed(pod_slot, pv.catalog_seed, pv.evo_stage, SanctuaryPodRole::LEASED_CORES3);
            pods[pod_slot].linked_device_id = pv.device_id;
            std::snprintf(pods[pod_slot].name, sizeof(pods[pod_slot].name), "%.19s", pv.name);
            pv.assigned_pod = static_cast<uint8_t>(pod_slot);
            pv.conn_state   = HubPeerConnectionState::TRANSFERRED;
        } else {
            pods[pod_slot].role = SanctuaryPodRole::VISITING_CORES3;
            pv.conn_state       = HubPeerConnectionState::VISITING;
        }

        protocol::TransferLeasePayload ltx{};
        ltx.transaction_id   = 0x7AB50000ULL + (++tx_frame_count);
        ltx.creature_id      = pv.creature_id;
        ltx.source_device_id = returning_to_s3 ? net_node.device_id : pv.device_id;
        ltx.target_device_id = returning_to_s3 ? pv.device_id : net_node.device_id;
        ltx.state_revision   = tx_frame_count;
        ltx.checkpoint_crc32 = pods[pod_slot].genome.integrity_checksum;
        net_node.recordTransaction(ltx.transaction_id);
        outLen = net_node.encodeFrame(protocol::MessageType::TRANSFER_COMMIT, &ltx, sizeof(ltx), outWire);

        std::snprintf(status_banner, sizeof(status_banner),
                      returning_to_s3 ? "RETURNED LEASE TO CORES3!" : "PULLED LEASE TO TAB5 POD %d!",
                      pod_slot + 1);
        sidebar_dirty = true;
        return true;
    }

    // Beam Active Tab5 Sanctuary Pod (Seed + DNA Markings + Stage) directly to a CoreS3 unit!
    bool beamActivePodToPeer(int peer_idx, uint8_t outWire[protocol::MAX_WIRE_PACKET_BYTES], size_t& outLen) {
        outLen = 0;
        const SanctuaryPod& pod = pods[selected_pod];
        uint64_t target_dev = (peer_idx >= 0 && peer_idx < 4 && peer_views[peer_idx].occupied)
            ? peer_views[peer_idx].device_id
            : 0ULL;

        protocol::HubCommandPayload cmd{};
        cmd.target_device_id   = target_dev;
        cmd.action             = static_cast<uint8_t>(protocol::HubRemoteAction::REMOTE_BEAM_SEED);
        cmd.pod_slot           = static_cast<uint8_t>(selected_pod);
        cmd.evo_stage          = pod.evo_stage;
        cmd.biome_variant      = pod.biome_variant;
        cmd.beam_catalog_seed  = pod.catalog_seed;
        cmd.beam_markings_seed = pod.genome.appearance.markings_seed;
        cmd.beam_palette_id    = pod.genome.appearance.palette_id;
        std::snprintf(cmd.beam_name, sizeof(cmd.beam_name), "%.19s", pod.name);

        outLen = net_node.encodeFrame(protocol::MessageType::EMOTE, &cmd, sizeof(cmd), outWire);
        tx_frame_count++;
        if (peer_idx >= 0 && peer_idx < 4 && peer_views[peer_idx].occupied) {
            peer_views[peer_idx].catalog_seed = pod.catalog_seed;
            peer_views[peer_idx].evo_stage    = pod.evo_stage;
            std::snprintf(peer_views[peer_idx].name, sizeof(peer_views[peer_idx].name), "%.19s", pod.name);
        }
        std::snprintf(status_banner, sizeof(status_banner), "BEAMED #%06u TO CORES3!", static_cast<unsigned>(pod.catalog_seed));
        sidebar_dirty = true;
        return true;
    }

    // Send Remote Care Action (Feed / Toy Ball / Sing / +25 XP Blessing) to CoreS3 Fleet!
    size_t buildRemoteCareCommand(protocol::HubRemoteAction act, uint8_t outWire[protocol::MAX_WIRE_PACKET_BYTES]) {
        uint64_t target_dev = (selected_peer >= 0 && selected_peer < 4 && peer_views[selected_peer].occupied)
            ? peer_views[selected_peer].device_id
            : 0ULL;
        protocol::HubCommandPayload cmd{};
        cmd.target_device_id = target_dev;
        cmd.action           = static_cast<uint8_t>(act);
        cmd.pod_slot         = static_cast<uint8_t>(selected_pod);
        std::snprintf(cmd.beam_name, sizeof(cmd.beam_name), "TAB5 HUB CARE");
        tx_frame_count++;
        return net_node.encodeFrame(protocol::MessageType::EMOTE, &cmd, sizeof(cmd), outWire);
    }

    // Cross-Pod Sanctuary Breeding (Recombines Selected Pod x Partner Pod into a New Hybrid Seed!)
    uint32_t breedSanctuaryPods(int podA, int podB, int targetPod) {
        podA = std::clamp(podA, 0, kMaxSanctuaryPods - 1);
        podB = std::clamp(podB, 0, kMaxSanctuaryPods - 1);
        targetPod = std::clamp(targetPod, 0, kMaxSanctuaryPods - 1);
        if (podA == podB) podB = (podA + 1) % kMaxSanctuaryPods;

        uint32_t seedA = pods[podA].catalog_seed;
        uint32_t seedB = pods[podB].catalog_seed;
        uint32_t hybridSeed = 100000u + ((seedA * 7919u + seedB * 104729u + (++tx_frame_count) * 1543u) % 899999u);
        loadPodByCatalogSeed(targetPod, hybridSeed, 0u, SanctuaryPodRole::SANCTUARY_COMPANION);
        pods[targetPod].hop_vy = -95.0f;
        pods[targetPod].heart_timer = 1.6f;
        selected_pod = targetPod;

        std::snprintf(status_banner, sizeof(status_banner),
                      "SPLICED POD %d x POD %d -> #%06u!", podA + 1, podB + 1, static_cast<unsigned>(hybridSeed));
        sidebar_dirty = true;
        return hybridSeed;
    }

    // Step all 4 Sanctuary Pods, Multi-Creature Flock Socialization, Spore-Treats & Crystal Toy Ball!
    void stepSanctuary(float dt, uint32_t now_ms) {
        dt = std::clamp(dt, 0.004f, 0.08f);
        float inst_fps = 1.0f / std::max(0.001f, dt);
        fps_smooth = fps_smooth * 0.90f + inst_fps * 0.10f;

        if (sim_fleet_active) {
            for (size_t i = 0; i < protocol::MAX_PEERS; ++i) {
                if (net_node.peers[i].active) {
                    net_node.peers[i].proximity.last_seen_ms = now_ms;
                }
            }
        }
        refreshPeerViews(now_ms);
        worldgen::stepWorldParticles(world, dt);

        if (treat_active) {
            treat_phase += dt;
            treat_y = std::min(176.0f, treat_y + 28.0f * dt);
            treat_x += genetics::fastSin(treat_phase * 4.5f) * 8.0f * dt;
        }

        if (ball_active) {
            ball_vy = std::clamp(ball_vy + 180.0f * dt, -260.0f, 260.0f);
            ball_x += ball_vx * dt;
            ball_y += ball_vy * dt;
            ball_angle += ball_vx * 0.11f * dt;
            if (ball_x < 24.0f)  { ball_x = 24.0f;  ball_vx =  std::abs(ball_vx) * 0.90f; }
            if (ball_x > 296.0f) { ball_x = 296.0f; ball_vx = -std::abs(ball_vx) * 0.90f; }
            if (ball_y < 32.0f)  { ball_y = 32.0f;  ball_vy =  std::abs(ball_vy) * 0.85f; }
            if (ball_y > 192.0f) {
                ball_y = 192.0f;
                ball_vy = -std::max(84.0f, std::abs(ball_vy) * 0.82f);
            }
        }

        if (chorus_active) {
            chorus_timer -= dt;
            if (chorus_timer <= 0.0f) chorus_active = false;
        }

        // Step active Sanctuary Pod (on embedded ESP32-P4, only step the visible selected_pod for 4x faster physics!)
        for (int i = 0; i < kMaxSanctuaryPods; ++i) {
#ifdef ESP_PLATFORM
            if (i != selected_pod) continue;
#endif
            SanctuaryPod& pod = pods[i];
            if (!pod.active) continue;

            simulation::StimulusInput stim{};
            stim.peer_nearby = true;
            pod.sim.tick(stim);

            // Social wandering around each pod's home lane, with the selected/nearest pod catching Spore-Treats!
            float desired_x = pod.home_x + genetics::fastSin(pod.pose.time_sec * (0.55f + i * 0.14f) + i * 1.7f) * 10.0f;
            if (treat_active && i == selected_pod) {
                desired_x = treat_x;
                if (std::abs(pod.pose.world_pos.x - treat_x) < 26.0f && treat_y > 95.0f) {
                    treat_active = false;
                    pod.bond_hearts = std::min<uint8_t>(4, pod.bond_hearts + 1);
                    pod.hop_vy = -90.0f;
                    pod.heart_timer = 1.5f;
                    pod.evo_xp = std::min<uint16_t>(1000, pod.evo_xp + 25);
                    sidebar_dirty = true;
                }
            } else if (ball_active && std::abs(pod.pose.world_pos.x - ball_x) < 34.0f) {
                if (std::abs(pod.pose.world_pos.x - ball_x) < 24.0f &&
                    std::abs(pod.pose.world_pos.y - ball_y) < 34.0f &&
                    ball_vy > -20.0f) {
                    ball_vy = -152.0f;
                    ball_vx = (ball_x >= pod.pose.world_pos.x) ? 78.0f : -78.0f;
                    pod.hop_vy = -78.0f;
                    pod.heart_timer = 1.2f;
                }
            }
            desired_x = std::clamp(desired_x, 38.0f, 282.0f);

            if (std::abs(pod.hop_y) > 0.1f || std::abs(pod.hop_vy) > 0.5f) {
                pod.hop_vy += 310.0f * dt;
                pod.hop_y  += pod.hop_vy * dt;
                if (pod.hop_y >= 0.0f) {
                    pod.hop_y  = 0.0f;
                    pod.hop_vy = 0.0f;
                }
            }
            if (pod.heart_timer > 0.0f) {
                pod.heart_timer = std::max(0.0f, pod.heart_timer - dt);
            }

            genetics::Vec2 gaze = treat_active
                ? genetics::Vec2{treat_x, treat_y}
                : (ball_active ? genetics::Vec2{ball_x, ball_y} : genetics::Vec2{160.0f, 124.0f});

            float gy = world.sampleGroundY(pod.pose.world_pos.x) + pod.hop_y;
            bool zoom_this = (close_up_zoom && i == selected_pod);
            animation::stepProceduralAnimation(
                pod.pose, pod.graph, dt,
                {(desired_x - pod.pose.world_pos.x) * 1.65f, 0.0f},
                gaze,
                zoom_this,
                chorus_active ? 0.95f : 0.45f,
                gy
            );
            if (!zoom_this) {
                // Keep shared Sanctuary camera centered at virtual x=160 so all 4 pods occupy distinct horizontal lanes!
                pod.pose.camera_focus.x = 160.0f;
            }
        }
    }

    // Render the 960x720 3.0x HD Multi-Creature Moebius Sanctuary Viewport
    void renderSanctuaryViewport() {
        viewport_canvas.setClipRect(0, 0, kTab5ViewportWidth - 1, kTab5ViewportHeight - 1);
        SanctuaryPod& active_pod = pods[selected_pod];
        if (!close_up_zoom) {
            active_pod.pose.camera_focus.x = 160.0f;
        }
        animation::RigPoseState cam_pose = active_pod.pose;
        if (!close_up_zoom) {
            cam_pose.camera_zoom = 1.0f;
        }

        uint64_t cur_bg_key = 0x7AB5CA6E00000000ULL
            | (static_cast<uint64_t>(sanctuary_biome & 0xFFu) << 40)
            | (static_cast<uint64_t>(close_up_zoom ? 1u : 0u) << 39)
            | (static_cast<uint64_t>(noir_mode ? 1u : 0u) << 38)
            | (static_cast<uint64_t>(lod2_anatomy_scan ? 1u : 0u) << 37)
            | (static_cast<uint64_t>(ball_active ? 1u : 0u) << 36)
            | (static_cast<uint64_t>(chorus_active ? 1u : 0u) << 35)
            | (static_cast<uint64_t>(sim_fleet_active ? 1u : 0u) << 34)
            | (static_cast<uint64_t>(selected_pod & 3) << 32)
            | static_cast<uint64_t>(world.world_seed.low & 0xFFFFFFFFULL);

        bool rebuild_bg = (cur_bg_key != bg_cache_key)
                       || (bg_cache_pixels.size() != viewport_canvas.pixels.size())
                       || (bg_cache_half.size() != creature_canvas.pixels.size());

        if (rebuild_bg) {
            animation::RigPoseState static_pose = cam_pose;
            static_pose.time_sec = 0.0f; // Bake clean static world without transient particles
            render::renderMoebiusWorldCanvas(viewport_canvas, world, static_pose, 0, 0, kTab5ViewportWidth - 1, kTab5ViewportHeight - 1);

            // Downsample 960x720 static world into 480x360 bg_cache_half for fast anti-aliased creature compositing
            bg_cache_half.resize(creature_canvas.pixels.size());
            const uint8_t* __restrict vp_src = viewport_canvas.pixels.data();
            uint8_t* __restrict hf_dst = bg_cache_half.data();
            for (int hy = 0; hy < 360; ++hy) {
                const uint8_t* r0 = vp_src + static_cast<size_t>(hy * 2) * kTab5ViewportWidth * 3;
                const uint8_t* r1 = r0 + kTab5ViewportWidth * 3;
                uint8_t* drow = hf_dst + static_cast<size_t>(hy) * 480 * 3;
                for (int hx = 0; hx < 480; ++hx) {
                    int sx = hx * 6;
                    drow[hx * 3 + 0] = static_cast<uint8_t>((r0[sx + 0] + r0[sx + 3] + r1[sx + 0] + r1[sx + 3] + 2) >> 2);
                    drow[hx * 3 + 1] = static_cast<uint8_t>((r0[sx + 1] + r0[sx + 4] + r1[sx + 1] + r1[sx + 4] + 2) >> 2);
                    drow[hx * 3 + 2] = static_cast<uint8_t>((r0[sx + 2] + r0[sx + 5] + r1[sx + 2] + r1[sx + 5] + 2) >> 2);
                }
            }

            // Bake Top Sanctuary Header Bar (x = 12..948, y = 10..54) into static cache
            drawHubBox(viewport_canvas, 12, 10, 948, 54, {18, 16, 28}, 0.88f, {236, 204, 128}, 2);
            char topTitle[64];
            std::snprintf(topTitle, sizeof(topTitle), "TAB5 SANCTUARY | %d.%.16s",
                          static_cast<int>(sanctuary_biome + 1),
                          catalog::kBiomeNames[sanctuary_biome % catalog::kNumBiomeVariants]);
            drawHubText(viewport_canvas, 22, 24, topTitle, {255, 238, 168}, 2, 2);

            drawHubBox(viewport_canvas, 452, 16, 542, 48, noir_mode ? render::ColorRGB{74, 68, 60} : render::ColorRGB{34, 30, 48}, 0.92f, {248, 234, 192}, 2);
            drawHubText(viewport_canvas, 462, 26, noir_mode ? "NOIR:ON*" : "NOIR INK", {255, 246, 224}, 1, 2);

            drawHubBox(viewport_canvas, 548, 16, 650, 48, lod2_anatomy_scan ? render::ColorRGB{58, 118, 112} : render::ColorRGB{34, 30, 48}, 0.92f, {148, 242, 232}, 2);
            drawHubText(viewport_canvas, 560, 26, "LOD2 SCAN", {172, 252, 242}, 1, 2);

            drawHubBox(viewport_canvas, 656, 16, 746, 48, close_up_zoom ? render::ColorRGB{92, 68, 36} : render::ColorRGB{34, 30, 48}, 0.92f, {255, 218, 132}, 2);
            drawHubText(viewport_canvas, 668, 26, "ZOOM 2X", {255, 232, 158}, 1, 2);

            drawHubBox(viewport_canvas, 752, 16, 838, 48, {34, 30, 48}, 0.92f, {168, 224, 255}, 2);
            drawHubText(viewport_canvas, 764, 26, "BIOME +", {192, 234, 255}, 1, 2);

            drawHubBox(viewport_canvas, 844, 16, 940, 48, {68, 32, 48}, 0.92f, {255, 176, 164}, 2);
            drawHubText(viewport_canvas, 850, 26, "+SPLICE+", {255, 208, 196}, 1, 2);

            // Bake Bottom Sanctuary Interactive Care & Fleet Dock (x = 12..948, y = 664..710) into static cache
            drawHubBox(viewport_canvas, 12, 664, 948, 710, {18, 16, 28}, 0.88f, {142, 218, 212}, 2);
            static const char* kDockLabels[6] = {
                "FEED SANCTUARY", "TOY BALL: ON", "CHORUS SING", "BLESS +25 XP", "BEAM TO S3", "SIM S3 FLEET"
            };
            for (int b = 0; b < 6; ++b) {
                int bx0 = 20 + b * 154;
                int bx1 = bx0 + 146;
                bool active_btn = (b == 1 && ball_active) || (b == 2 && chorus_active) || (b == 5 && sim_fleet_active);
                render::ColorRGB fill = active_btn ? render::ColorRGB{44, 84, 82} : render::ColorRGB{32, 28, 46};
                render::ColorRGB bdr  = active_btn ? render::ColorRGB{164, 252, 238} : render::ColorRGB{128, 138, 164};
                drawHubBox(viewport_canvas, bx0, 670, bx1, 704, fill, 0.92f, bdr, 2);
                const char* lbl = (b == 1 && !ball_active) ? "TOY BALL: OFF" : kDockLabels[b];
                drawHubText(viewport_canvas, bx0 + 12, 681, lbl, {246, 242, 232}, 1, 2);
            }

            if (noir_mode) {
                for (size_t i = 0; i + 2 < viewport_canvas.pixels.size(); i += 3) {
                    int r = viewport_canvas.pixels[i + 0];
                    int g = viewport_canvas.pixels[i + 1];
                    int b = viewport_canvas.pixels[i + 2];
                    int luma = std::clamp(((77 * r + 150 * g + 29 * b) >> 8) + (((r - b) * 3) >> 3), 10, 250);
                    viewport_canvas.pixels[i + 0] = static_cast<uint8_t>(luma);
                    viewport_canvas.pixels[i + 1] = static_cast<uint8_t>(luma);
                    viewport_canvas.pixels[i + 2] = static_cast<uint8_t>(luma);
                }
            }

            bg_cache_pixels = viewport_canvas.pixels;
            bg_cache_key    = cur_bg_key;
            viewport_full_dirty = true;
            prev_dirty_vx0 = 0;
            prev_dirty_vy0 = 56;
            prev_dirty_vx1 = kTab5ViewportWidth - 1;
            prev_dirty_vy1 = 662;
        } else {
            viewport_full_dirty = false;
            // Restore only the previous frame's dirty bounding box from bg_cache_pixels (< 0.35 ms!)
            int rx0 = std::clamp(prev_dirty_vx0, 0, kTab5ViewportWidth - 1);
            int ry0 = std::clamp(prev_dirty_vy0, 56, 662);
            int rx1 = std::clamp(prev_dirty_vx1, 0, kTab5ViewportWidth - 1);
            int ry1 = std::clamp(prev_dirty_vy1, 56, 662);
            if (rx0 <= rx1 && ry0 <= ry1) {
                size_t row_bytes = static_cast<size_t>(rx1 - rx0 + 1) * 3;
                uint8_t* __restrict dst_base = viewport_canvas.pixels.data();
                const uint8_t* __restrict src_base = bg_cache_pixels.data();
                for (int y = ry0; y <= ry1; ++y) {
                    size_t off = (static_cast<size_t>(y) * kTab5ViewportWidth + rx0) * 3;
                    std::memcpy(dst_base + off, src_base + off, row_bytes);
                }
            }
        }

        // Compute tight bounding box in 480x360 (1.5x) creature_canvas coordinates
        genetics::Vec2 sp = render::worldToScreen(active_pod.pose.world_pos, active_pod.pose);
        int hcx = static_cast<int>(sp.x * 1.5f);
        int hcy = static_cast<int>(sp.y * 1.5f);
        int hrx = close_up_zoom ? 175 : 118;
        int hry_top = close_up_zoom ? 144 : 114;
        int hry_bot = close_up_zoom ? 148 : 132;

        int hx0 = hcx - hrx;
        int hx1 = hcx + hrx;
        int hy0 = hcy - hry_top;
        int hy1 = hcy + hry_bot;

        if (treat_active) {
            int tx_h = static_cast<int>(treat_x * 1.5f);
            int ty_h = static_cast<int>(treat_y * 1.5f);
            hx0 = std::min(hx0, tx_h - 18);
            hx1 = std::max(hx1, tx_h + 18);
            hy0 = std::min(hy0, ty_h - 18);
            hy1 = std::max(hy1, ty_h + 18);
        }
        if (ball_active) {
            int bx_h = static_cast<int>(ball_x * 1.5f);
            int by_h = static_cast<int>(ball_y * 1.5f);
            hx0 = std::min(hx0, bx_h - 22);
            hx1 = std::max(hx1, bx_h + 22);
            hy0 = std::min(hy0, by_h - 22);
            hy1 = std::max(hy1, static_cast<int>(198.0f * 1.5f) + 18);
        }
        hx0 = std::clamp(hx0, 0, 479);
        hx1 = std::clamp(hx1, 0, 479);
        hy0 = std::clamp(hy0, 28, 330);
        hy1 = std::clamp(hy1, 28, 330);

        // Restore hx0..hx1, hy0..hy1 in 480x360 creature_canvas from bg_cache_half
        {
            size_t h_row_bytes = static_cast<size_t>(hx1 - hx0 + 1) * 3;
            uint8_t* __restrict c_dst = creature_canvas.pixels.data();
            const uint8_t* __restrict c_src = bg_cache_half.data();
            for (int hy = hy0; hy <= hy1; ++hy) {
                size_t off = (static_cast<size_t>(hy) * 480 + hx0) * 3;
                std::memcpy(c_dst + off, c_src + off, h_row_bytes);
            }
        }
        creature_canvas.setClipRect(hx0, hy0, hx1, hy1);

        // Render active Oddling + selection halo + Spore-Treat + Crystal Toy Ball into 480x360 creature_canvas (4x fewer pixels!)
        if (active_pod.active) {
            float evo_scale = (active_pod.evo_stage == 2u) ? 1.0f : catalog::getEvolutionScaleForStage(active_pod.evo_stage);
            render::renderMoebiusCreatureCanvas(creature_canvas, active_pod.graph, active_pod.pose, evo_scale);
            if (!close_up_zoom) {
                creature_canvas.drawCircle(sp.x, sp.y + 20.0f, 20.0f, {255, 228, 132}, 0.42f, 1.4f);
            }
        }
        if (treat_active) {
            creature_canvas.drawCircle(treat_x, treat_y, 7.6f, {255, 228, 148}, 0.45f, 2.0f);
            creature_canvas.drawTaperedCapsule(treat_x - 1.5f, treat_y + 0.8f, 4.6f,
                                               treat_x + 1.5f, treat_y - 1.0f, 3.8f,
                                               {250, 152, 132}, {38, 30, 38}, 0.65f, 1.0f);
        }
        if (ball_active) {
            float h_factor = std::clamp(1.0f - (192.0f - ball_y) * 0.0045f, 0.35f, 1.0f);
            creature_canvas.drawCircle(ball_x, 198.0f, 7.5f * h_factor, {72, 56, 46}, 0.42f * h_factor, 1.2f);
            creature_canvas.drawCircle(ball_x, ball_y, 9.8f, {152, 246, 236}, 0.42f, 1.6f);
            creature_canvas.drawTaperedCapsule(ball_x - 0.6f, ball_y, 7.0f, ball_x + 0.6f, ball_y, 7.0f,
                                               {118, 218, 232}, {34, 28, 38}, 0.75f, 1.0f);
            float bx_s = genetics::fastCos(ball_angle) * 4.8f;
            float by_s = genetics::fastSin(ball_angle) * 4.8f;
            creature_canvas.drawInkLine(ball_x - bx_s, ball_y - by_s, ball_x + bx_s, ball_y + by_s, 1.6f, {252, 168, 118}, 0.92f);
            creature_canvas.drawCircle(ball_x - 2.2f, ball_y - 2.2f, 2.2f, {255, 252, 242}, 0.95f, 0.5f);
        }

        // Composite ONLY modified creature/toy pixels from 480x360 creature_canvas onto 960x720 viewport_canvas
        // (preserving 100% native 960x720 3.0x HD background pixels everywhere else!)
        {
            const uint8_t* __restrict c_pix = creature_canvas.pixels.data();
            const uint8_t* __restrict b_pix = bg_cache_half.data();
            uint8_t* __restrict v_pix = viewport_canvas.pixels.data();

            for (int hy = hy0; hy <= hy1; ++hy) {
                int vy0 = hy * 2;
                int vy1 = vy0 + 1;
                if (vy0 < 56 || vy1 > 662) continue;
                int hy_next = std::min(hy1, hy + 1);
                const uint8_t* crow = c_pix + static_cast<size_t>(hy) * 480 * 3;
                const uint8_t* crow_n = c_pix + static_cast<size_t>(hy_next) * 480 * 3;
                const uint8_t* brow = b_pix + static_cast<size_t>(hy) * 480 * 3;
                uint8_t* vrow0 = v_pix + static_cast<size_t>(vy0) * kTab5ViewportWidth * 3;
                uint8_t* vrow1 = v_pix + static_cast<size_t>(vy1) * kTab5ViewportWidth * 3;

                for (int hx = hx0; hx <= hx1; ++hx) {
                    int s = hx * 3;
                    uint8_t r00 = crow[s + 0], g00 = crow[s + 1], b00 = crow[s + 2];
                    if (r00 == brow[s + 0] && g00 == brow[s + 1] && b00 == brow[s + 2]) {
                        continue; // Untouched pixel: keep crisp 960x720 3.0x HD background!
                    }
                    int sn = (hx < hx1) ? (s + 3) : s;
                    uint8_t r10 = static_cast<uint8_t>((r00 + crow[sn + 0]) >> 1);
                    uint8_t g10 = static_cast<uint8_t>((g00 + crow[sn + 1]) >> 1);
                    uint8_t b10 = static_cast<uint8_t>((b00 + crow[sn + 2]) >> 1);
                    uint8_t r01 = static_cast<uint8_t>((r00 + crow_n[s + 0]) >> 1);
                    uint8_t g01 = static_cast<uint8_t>((g00 + crow_n[s + 1]) >> 1);
                    uint8_t b01 = static_cast<uint8_t>((b00 + crow_n[s + 2]) >> 1);
                    uint8_t r11 = static_cast<uint8_t>((r10 + r01) >> 1);
                    uint8_t g11 = static_cast<uint8_t>((g10 + g01) >> 1);
                    uint8_t b11 = static_cast<uint8_t>((b10 + b01) >> 1);

                    if (noir_mode) {
                        auto toNoir = [](uint8_t& r, uint8_t& g, uint8_t& b) {
                            int l = std::clamp(((77 * r + 150 * g + 29 * b) >> 8) + (((static_cast<int>(r) - static_cast<int>(b)) * 3) >> 3), 10, 250);
                            r = g = b = static_cast<uint8_t>(l);
                        };
                        toNoir(r00, g00, b00);
                        toNoir(r10, g10, b10);
                        toNoir(r01, g01, b01);
                        toNoir(r11, g11, b11);
                    }

                    int d = hx * 6;
                    vrow0[d + 0] = r00; vrow0[d + 1] = g00; vrow0[d + 2] = b00;
                    vrow0[d + 3] = r10; vrow0[d + 4] = g10; vrow0[d + 5] = b10;
                    vrow1[d + 0] = r01; vrow1[d + 1] = g01; vrow1[d + 2] = b01;
                    vrow1[d + 3] = r11; vrow1[d + 4] = g11; vrow1[d + 5] = b11;
                }
            }
        }

        int cur_vx0 = hx0 * 2;
        int cur_vy0 = std::max(56, hy0 * 2);
        int cur_vx1 = std::min(kTab5ViewportWidth - 1, hx1 * 2 + 1);
        int cur_vy1 = std::min(662, hy1 * 2 + 1);

        int px = static_cast<int>(sp.x * 3.0f);
        int py = static_cast<int>(sp.y * 3.0f);
        if (active_pod.heart_timer > 0.0f) {
            viewport_canvas.setClipRect(0, 56, kTab5ViewportWidth - 1, 662);
            drawHubHeart(viewport_canvas, px - 24, py - 110, {252, 134, 138}, 3);
            drawHubHeart(viewport_canvas, px + 8,  py - 122, {255, 208, 128}, 3);
            cur_vx0 = std::min(cur_vx0, std::max(0, px - 40));
            cur_vx1 = std::max(cur_vx1, std::min(kTab5ViewportWidth - 1, px + 48));
            cur_vy0 = std::min(cur_vy0, std::max(56, py - 135));
        }

        if (lod2_anatomy_scan) {
            viewport_canvas.setClipRect(0, 56, kTab5ViewportWidth - 1, 662);
            drawLod2AnatomyInspectorOverlay();
            cur_vx0 = std::min(cur_vx0, 16);
            cur_vx1 = std::max(cur_vx1, kTab5ViewportWidth - 16);
            cur_vy0 = std::min(cur_vy0, 60);
            cur_vy1 = std::max(cur_vy1, 650);
        }
        viewport_canvas.setClipRect(0, 0, kTab5ViewportWidth - 1, kTab5ViewportHeight - 1);

        if (viewport_full_dirty) {
            push_vx0 = 0;
            push_vy0 = 0;
            push_vx1 = kTab5ViewportWidth - 1;
            push_vy1 = kTab5ViewportHeight - 1;
        } else {
            push_vx0 = std::min(prev_dirty_vx0, cur_vx0);
            push_vy0 = std::min(prev_dirty_vy0, cur_vy0);
            push_vx1 = std::max(prev_dirty_vx1, cur_vx1);
            push_vy1 = std::max(prev_dirty_vy1, cur_vy1);
        }
        prev_dirty_vx0 = cur_vx0;
        prev_dirty_vy0 = cur_vy0;
        prev_dirty_vx1 = cur_vx1;
        prev_dirty_vy1 = cur_vy1;
    }

    void drawLod2AnatomyInspectorOverlay() {
        const SanctuaryPod& pod = pods[selected_pod];
        genetics::Vec2 sp = render::worldToScreen(pod.pose.world_pos, pod.pose);
        int cx = static_cast<int>(sp.x * 3.0f);
        int cy = static_cast<int>(sp.y * 3.0f);

        // Draw callout lines and diagnostic cards around the selected Oddling!
        viewport_canvas.drawCircle(sp.x, sp.y, 34.0f, {142, 252, 238}, 0.55f, 1.0f);
        viewport_canvas.drawInkLine(sp.x - 18.0f, sp.y - 12.0f, sp.x - 58.0f, sp.y - 36.0f, 1.2f, {142, 252, 238}, 0.92f);
        viewport_canvas.drawInkLine(sp.x + 18.0f, sp.y - 8.0f,  sp.x + 58.0f, sp.y - 32.0f, 1.2f, {255, 224, 136}, 0.92f);

        int lx = std::clamp(cx - 310, 20, kTab5ViewportWidth - 320);
        int ly = std::clamp(cy - 145, 68, kTab5ViewportHeight - 210);
        drawHubBox(viewport_canvas, lx, ly, lx + 256, ly + 88, {16, 22, 34}, 0.92f, {142, 252, 238}, 2);

        char line1[36], line2[36], line3[36];
        auto tr = genetics::unpackChimeraTraits(pod.genome.appearance.markings_seed);
        std::snprintf(line1, sizeof(line1), "MORPH NODES: %u | EYES: %u",
                      static_cast<unsigned>(pod.graph.component_count), static_cast<unsigned>(pod.pose.eye_count));
        std::snprintf(line2, sizeof(line2), "CHA:%u DOR:%u EYE:%u PAT:%u",
                      static_cast<unsigned>(tr.chassis_id), static_cast<unsigned>(tr.dorsal_part),
                      static_cast<unsigned>(tr.eye_style), static_cast<unsigned>(pod.pattern_variant));
        std::snprintf(line3, sizeof(line3), "CRC32: %08X | %s",
                      static_cast<unsigned>(pod.genome.integrity_checksum),
                      catalog::getEvolutionStageFullName(pod.evo_stage));
        drawHubText(viewport_canvas, lx + 10, ly + 12, line1, {164, 252, 242}, 1, 2);
        drawHubText(viewport_canvas, lx + 10, ly + 36, line2, {255, 232, 156}, 1, 2);
        drawHubText(viewport_canvas, lx + 10, ly + 60, line3, {228, 224, 242}, 1, 2);
    }

    // Render the Right-Hand 320x720 Observatory & Hub Command Deck
    void renderObservatorySidebar() {
        sidebar_canvas.setClipRect(0, 0, kTab5SidebarWidth - 1, kTab5DisplayHeight - 1);
        drawHubBox(sidebar_canvas, 0, 0, kTab5SidebarWidth - 1, kTab5DisplayHeight - 1, {20, 18, 30}, 1.0f, {218, 186, 118}, 3);

        // Top Observatory Banner (y = 8..48)
        drawHubBox(sidebar_canvas, 8, 8, 311, 48, {32, 28, 48}, 0.96f, {248, 214, 128}, 2);
        drawHubText(sidebar_canvas, 18, 14, "TAB5 OBSERVATORY HUB", {255, 236, 164}, 2, 2);
        char subHdr[42];
        std::snprintf(subHdr, sizeof(subHdr), "%.36s", status_banner);
        drawHubText(sidebar_canvas, 18, 32, subHdr, {158, 246, 236}, 1, 1);

        // 4 Command Deck Tab Buttons (y = 54..90)
        static const char* kTabNames[4] = {"1:PODS", "2:DNA", "3:VAULT", "4:SEED"};
        for (int t = 0; t < 4; ++t) {
            int tx0 = 8 + t * 77;
            int tx1 = tx0 + 73;
            bool sel = (deck_tab == t);
            drawHubBox(sidebar_canvas, tx0, 54, tx1, 90,
                       sel ? render::ColorRGB{68, 54, 92} : render::ColorRGB{28, 24, 40}, 0.96f,
                       sel ? render::ColorRGB{255, 224, 132} : render::ColorRGB{112, 104, 128}, 2);
            drawHubText(sidebar_canvas, tx0 + 10, 66, kTabNames[t],
                        sel ? render::ColorRGB{255, 238, 168} : render::ColorRGB{204, 198, 218}, 1, 2);
        }

        if (deck_tab == 0) {
            renderDeckTabPodsAndRadar();
        } else if (deck_tab == 1) {
            renderDeckTabDnaSculptor();
        } else if (deck_tab == 2) {
            renderDeckTabVault();
        } else {
            renderDeckTabSeedAndNet();
        }
        if (noir_mode) {
            for (size_t i = 0; i + 2 < sidebar_canvas.pixels.size(); i += 3) {
                int r = sidebar_canvas.pixels[i + 0];
                int g = sidebar_canvas.pixels[i + 1];
                int b = sidebar_canvas.pixels[i + 2];
                int luma = std::clamp(((77 * r + 150 * g + 29 * b) >> 8) + (((r - b) * 3) >> 3), 10, 250);
                sidebar_canvas.pixels[i + 0] = static_cast<uint8_t>(luma);
                sidebar_canvas.pixels[i + 1] = static_cast<uint8_t>(luma);
                sidebar_canvas.pixels[i + 2] = static_cast<uint8_t>(luma);
            }
        }
        sidebar_dirty = false;
    }

    void renderDeckTabPodsAndRadar() {
        // Section 1: 4 Sanctuary Habitat Pod Cards (y = 96..348)
        drawHubText(sidebar_canvas, 12, 98, "SANCTUARY HABITAT PODS (TAP TO SELECT)", {255, 228, 152}, 1, 2);
        for (int i = 0; i < kMaxSanctuaryPods; ++i) {
            const SanctuaryPod& pod = pods[i];
            int py0 = 118 + i * 56;
            int py1 = py0 + 52;
            bool sel = (i == selected_pod);
            render::ColorRGB fill = sel ? render::ColorRGB{44, 38, 66} : render::ColorRGB{28, 24, 40};
            render::ColorRGB bdr  = sel ? render::ColorRGB{255, 224, 128}
                                        : ((pod.role == SanctuaryPodRole::VISITING_CORES3 || pod.role == SanctuaryPodRole::LEASED_CORES3)
                                            ? render::ColorRGB{132, 238, 226}
                                            : render::ColorRGB{108, 102, 126});
            drawHubBox(sidebar_canvas, 10, py0, 309, py1, fill, 0.95f, bdr, 2);

            char line1[40], line2[40];
            std::snprintf(line1, sizeof(line1), "POD %d [%s] #%06u",
                          i + 1, platforms::tab5::podRoleLabel(pod.role), static_cast<unsigned>(pod.catalog_seed));
            std::snprintf(line2, sizeof(line2), "%.16s | %s",
                          pod.name, catalog::getEvolutionStageShortName(pod.evo_stage));
            drawHubText(sidebar_canvas, 18, py0 + 8,  line1, sel ? render::ColorRGB{255, 238, 168} : render::ColorRGB{172, 246, 236}, 1, 2);
            drawHubText(sidebar_canvas, 18, py0 + 28, line2, {242, 238, 228}, 1, 2);

            // 4 Bond Hearts on the right of each Pod card
            for (int h = 0; h < 4; ++h) {
                render::ColorRGB hcol = (h < pod.bond_hearts) ? render::ColorRGB{252, 134, 136} : render::ColorRGB{76, 68, 88};
                drawHubHeart(sidebar_canvas, 234 + h * 17, py0 + 28, hcol, 2);
            }
        }

        // Section 2: Live ESP-NOW CoreS3 Fleet Radar (y = 348..568)
        drawHubText(sidebar_canvas, 12, 350, "ESP-NOW CORES3 FLEET RADAR (2.4GHZ CH1)", {156, 248, 236}, 1, 2);
        for (int p = 0; p < 3; ++p) {
            int ry0 = 370 + p * 64;
            int ry1 = ry0 + 58;
            const HubPeerSlotView& pv = peer_views[p];
            bool sel = (p == selected_peer);
            render::ColorRGB bdr = sel ? render::ColorRGB{152, 252, 238} : render::ColorRGB{96, 108, 128};
            drawHubBox(sidebar_canvas, 10, ry0, 309, ry1,
                       sel ? render::ColorRGB{32, 48, 58} : render::ColorRGB{24, 28, 40}, 0.95f, bdr, 2);

            if (pv.occupied) {
                char pline1[42], pline2[42];
                std::snprintf(pline1, sizeof(pline1), "%.15s | %d DBM [%s]",
                              pv.name, static_cast<int>(pv.rssi_dbm), protocol::proximityTierName(pv.tier));
                std::snprintf(pline2, sizeof(pline2), "STATE: %s | SEED #%06u",
                              platforms::tab5::hubPeerStateLabel(pv.conn_state),
                              static_cast<unsigned>(pv.catalog_seed));
                drawHubText(sidebar_canvas, 18, ry0 + 10, pline1, {255, 238, 168}, 1, 2);
                drawHubText(sidebar_canvas, 18, ry0 + 32, pline2, {168, 246, 236}, 1, 2);
            } else {
                drawHubText(sidebar_canvas, 18, ry0 + 20, "SCANNING FOR CORES3 PEER ON CH 1...", {148, 144, 166}, 1, 2);
            }
        }

        // Section 3: One-Tap Hub <-> CoreS3 Fleet Action Deck (y = 568..710)
        drawHubBox(sidebar_canvas, 10, 568, 156, 612, {38, 76, 74}, 0.95f, {152, 248, 236}, 2);
        drawHubText(sidebar_canvas, 20, 582, "INVITE TO POD", {248, 252, 246}, 1, 2);

        drawHubBox(sidebar_canvas, 162, 568, 309, 612, {74, 52, 34}, 0.95f, {255, 218, 132}, 2);
        drawHubText(sidebar_canvas, 174, 582, "LEASE / RETURN", {255, 238, 168}, 1, 2);

        drawHubBox(sidebar_canvas, 10, 618, 156, 662, {68, 36, 62}, 0.95f, {252, 178, 232}, 2);
        drawHubText(sidebar_canvas, 20, 632, "BEAM POD TO S3", {255, 224, 246}, 1, 2);

        drawHubBox(sidebar_canvas, 162, 618, 309, 662, {36, 58, 84}, 0.95f, {168, 218, 255}, 2);
        drawHubText(sidebar_canvas, 172, 632, "REMOTE CARE S3", {218, 238, 255}, 1, 2);

        drawHubBox(sidebar_canvas, 10, 668, 309, 710, {56, 38, 72}, 0.95f, {255, 214, 138}, 2);
        char breedLbl[40];
        std::snprintf(breedLbl, sizeof(breedLbl), "+ SPLICE POD %d x POD %d HYBRIDS +",
                      selected_pod + 1, ((selected_pod + 1) % kMaxSanctuaryPods) + 1);
        drawHubText(sidebar_canvas, 24, 681, breedLbl, {255, 236, 168}, 1, 2);
    }

    void renderDeckTabDnaSculptor() {
        const SanctuaryPod& pod = pods[selected_pod];
        const SanctuaryPod& partner = pods[(selected_pod + 1) % kMaxSanctuaryPods];
        auto tr = genetics::unpackChimeraTraits(pod.genome.appearance.markings_seed);

        drawHubText(sidebar_canvas, 12, 98, "VISUAL DNA SPLICER & HYBRID LAB", {255, 228, 152}, 1, 2);

        // Live Visual Creature Trio Preview (Parent A x Parent B -> Hybrid Offspring!) at y = 118..264
        uint64_t cur_dna_key = (static_cast<uint64_t>(pod.catalog_seed) << 32)
                             ^ (static_cast<uint64_t>(partner.catalog_seed) << 12)
                             ^ static_cast<uint64_t>(pod.genome.appearance.markings_seed)
                             ^ (static_cast<uint64_t>(pod.genome.appearance.palette_id) << 8)
                             ^ 0xD4A1AB00ULL;
        constexpr int kPrevW = 300;
        constexpr int kPrevH = 147;
        if (cur_dna_key == dna_preview_key && dna_preview_cache.size() == static_cast<size_t>(kPrevW * kPrevH * 3)) {
            for (int y = 0; y < kPrevH; ++y) {
                std::memcpy(&sidebar_canvas.pixels[((118 + y) * kTab5SidebarWidth + 10) * 3],
                            &dna_preview_cache[y * kPrevW * 3],
                            kPrevW * 3);
            }
        } else {
            drawHubBox(sidebar_canvas, 10, 118, 309, 264, {34, 30, 48}, 0.98f, {248, 214, 128}, 2);
            sidebar_canvas.setClipRect(12, 120, 307, 262);

            uint32_t previewHybridSeed = 100000u + ((pod.catalog_seed * 7919u + partner.catalog_seed * 104729u + 1543u) % 899999u);
            catalog::CatalogOddlingSpec hybSpec{};
            catalog::synthesizeFromCatalogSeed(previewHybridSeed, hybSpec);

            const genetics::MorphologyGraph* trioGraphs[3] = { &pod.graph, &partner.graph, &hybSpec.graph };
            const float trioX[3] = { 58.0f, 156.0f, 256.0f };
            for (int c_i = 0; c_i < 3; ++c_i) {
                animation::RigPoseState mini_pose{};
                bool is_fl = (trioGraphs[c_i]->primary_family == genetics::Family::FLOATER);
                float base_y = is_fl ? 168.0f : 196.0f;
                animation::initRigPose(mini_pose, *trioGraphs[c_i], trioX[c_i], base_y + trioGraphs[c_i]->standing_clearance_y);
                mini_pose.camera_focus = {160.0f, 128.0f};
                mini_pose.camera_zoom  = 1.0f;
                mini_pose.world_pos    = {trioX[c_i], base_y};
                render::renderMoebiusCreatureCanvas(sidebar_canvas, *trioGraphs[c_i], mini_pose, (c_i == 2) ? 0.50f : 0.44f);
            }
            sidebar_canvas.setClipRect(0, 0, kTab5SidebarWidth - 1, kTab5DisplayHeight - 1);
            drawHubText(sidebar_canvas, 18,  124, "PARENT A", {164, 248, 236}, 1, 1);
            drawHubText(sidebar_canvas, 104, 182, "X",        {255, 224, 132}, 2, 2);
            drawHubText(sidebar_canvas, 126, 124, "PARENT B", {164, 248, 236}, 1, 1);
            drawHubText(sidebar_canvas, 198, 182, ">",        {255, 224, 132}, 2, 2);
            drawHubText(sidebar_canvas, 224, 124, "HYBRID",   {255, 232, 148}, 1, 1);

            dna_preview_cache.resize(static_cast<size_t>(kPrevW * kPrevH * 3));
            for (int y = 0; y < kPrevH; ++y) {
                std::memcpy(&dna_preview_cache[y * kPrevW * 3],
                            &sidebar_canvas.pixels[((118 + y) * kTab5SidebarWidth + 10) * 3],
                            kPrevW * 3);
            }
            dna_preview_key = cur_dna_key;
        }

        // DNA Sculptor Trait Buttons (y = 270..656)
        struct TraitBtn { int y0; int y1; char label[38]; render::ColorRGB bdr; };
        TraitBtn btns[7]{};
        std::snprintf(btns[0].label, sizeof(btns[0].label), "< SPECIES %d/16: %.14s >", pod.species_idx + 1, catalog::kPresets[pod.species_idx * 3].name);
        std::snprintf(btns[1].label, sizeof(btns[1].label), "CURATED ALTERNATE: ALT %d / 3", pod.alt_idx + 1);
        std::snprintf(btns[2].label, sizeof(btns[2].label), "SURFACE PATTERN:   VAR %d / 6", pod.pattern_variant + 1);
        std::snprintf(btns[3].label, sizeof(btns[3].label), "DORSAL CREST PART: %u / 8", static_cast<unsigned>(tr.dorsal_part + 1));
        std::snprintf(btns[4].label, sizeof(btns[4].label), "EYE MORPHOLOGY:    STYLE %u / 7", static_cast<unsigned>(tr.eye_style + 1));
        std::snprintf(btns[5].label, sizeof(btns[5].label), "COLOR PALETTE:     PAL %u / 16", static_cast<unsigned>((pod.genome.appearance.palette_id & 0xFu) + 1));
        std::snprintf(btns[6].label, sizeof(btns[6].label), "EVOLUTION STAGE:   %s", catalog::getEvolutionStageFullName(pod.evo_stage));

        for (int b = 0; b < 7; ++b) {
            int by0 = 272 + b * 54;
            int by1 = by0 + 46;
            drawHubBox(sidebar_canvas, 10, by0, 309, by1, {32, 28, 48}, 0.95f, {168, 214, 242}, 2);
            drawHubText(sidebar_canvas, 18, by0 + 14, btns[b].label, {252, 244, 228}, 1, 2);
        }

        drawHubBox(sidebar_canvas, 10, 656, 309, 708, {68, 36, 56}, 0.95f, {255, 218, 132}, 2);
        char seedCode[40];
        std::snprintf(seedCode, sizeof(seedCode), "DNA SEED #%06u - BEAM TO CORES3", static_cast<unsigned>(pod.catalog_seed));
        drawHubText(sidebar_canvas, 20, 674, seedCode, {255, 238, 168}, 1, 2);
    }

    void renderDeckTabVault() {
        drawHubText(sidebar_canvas, 12, 98, "16-SLOT SANCTUARY VAULT & SD EXPORT", {255, 228, 152}, 1, 2);
        int base = (vault_page & 3) * 4;
        for (int i = 0; i < 4; ++i) {
            int slot = base + i;
            const VaultSlotEntry& v = vault[slot];
            int vy0 = 122 + i * 94;
            int vy1 = vy0 + 84;
            drawHubBox(sidebar_canvas, 10, vy0, 309, vy1, {28, 24, 42}, 0.95f,
                       v.used ? render::ColorRGB{152, 242, 232} : render::ColorRGB{92, 86, 112}, 2);
            char hdr[38];
            if (v.used) {
                std::snprintf(hdr, sizeof(hdr), "SLOT #%02d: #%06u %.12s [%s]",
                              slot + 1, static_cast<unsigned>(v.catalog_seed), v.name,
                              catalog::getEvolutionStageShortName(v.evo_stage));
            } else {
                std::snprintf(hdr, sizeof(hdr), "SLOT #%02d: [EMPTY SANCTUARY SLOT]", slot + 1);
            }
            drawHubText(sidebar_canvas, 18, vy0 + 10, hdr, v.used ? render::ColorRGB{255, 238, 168} : render::ColorRGB{164, 158, 182}, 1, 2);

            drawHubBox(sidebar_canvas, 18,  vy0 + 38, 154, vy0 + 74, {42, 74, 68}, 0.95f, {152, 248, 236}, 2);
            drawHubText(sidebar_canvas, 34, vy0 + 48, "SAVE POD HERE", {242, 252, 246}, 1, 2);

            drawHubBox(sidebar_canvas, 164, vy0 + 38, 300, vy0 + 74, {68, 52, 34}, 0.95f, {255, 218, 132}, 2);
            drawHubText(sidebar_canvas, 180, vy0 + 48, "LOAD TO POD", {255, 238, 168}, 1, 2);
        }

        // Page selector, Wireless OTA & SD export buttons (y = 508..708)
        drawHubBox(sidebar_canvas, 10, 508, 309, 556, {34, 30, 52}, 0.95f, {218, 198, 255}, 2);
        char pgStr[38];
        std::snprintf(pgStr, sizeof(pgStr), "< VAULT PAGE %d / 4 (SLOTS %d..%d) >", (vault_page & 3) + 1, base + 1, base + 4);
        drawHubText(sidebar_canvas, 22, 524, pgStr, {234, 222, 255}, 1, 2);

        drawHubBox(sidebar_canvas, 10, 566, 309, 630, {32, 62, 58}, 0.95f, {152, 248, 236}, 2);
        drawHubText(sidebar_canvas, 18, 578, "WIRELESS OTA UPDATE (VERCEL)", {176, 252, 242}, 1, 2);
        drawHubText(sidebar_canvas, 18, 600, ota_status, {248, 244, 232}, 1, 1);

        drawHubBox(sidebar_canvas, 10, 640, 309, 706, {64, 42, 34}, 0.95f, {255, 214, 132}, 2);
        drawHubText(sidebar_canvas, 24, 665, "EXPORT TAB5 HUB .BIN FOR M5LAUNCHER", {255, 236, 164}, 1, 2);
    }

    void renderDeckTabSeedAndNet() {
        drawHubText(sidebar_canvas, 12, 98, "6-DIGIT SEED KEYPAD (#000001-#999999)", {255, 228, 152}, 1, 2);

        // Seed Display Box (y = 122..172)
        drawHubBox(sidebar_canvas, 10, 122, 309, 172, {14, 12, 22}, 0.98f, {255, 224, 128}, 2);
        char sdisp[32];
        std::snprintf(sdisp, sizeof(sdisp), "SEED # %s_", seed_input_buf);
        drawHubText(sidebar_canvas, 24, 138, sdisp, {164, 252, 238}, 2, 2);

        // 4x3 Numeric Keypad (y = 182..454)
        static const char* kKeys[12] = {
            "1", "2", "3",
            "4", "5", "6",
            "7", "8", "9",
            "CLR", "0", "LOAD!"
        };
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 3; ++c) {
                int k = r * 3 + c;
                int kx0 = 10 + c * 101;
                int kx1 = kx0 + 95;
                int ky0 = 182 + r * 66;
                int ky1 = ky0 + 58;
                render::ColorRGB fill = (k == 11) ? render::ColorRGB{38, 86, 78}
                                      : ((k == 9) ? render::ColorRGB{78, 36, 44} : render::ColorRGB{34, 30, 50});
                drawHubBox(sidebar_canvas, kx0, ky0, kx1, ky1, fill, 0.95f, {168, 188, 218}, 2);
                drawHubText(sidebar_canvas, kx0 + (k >= 9 ? 18 : 38), ky0 + 20, kKeys[k], {255, 244, 224}, 2, 2);
            }
        }

        // Live ESP32-P4 + ESP32-C6 Bridge Diagnostics Card (y = 460..638)
        drawHubBox(sidebar_canvas, 10, 460, 309, 638, {22, 20, 34}, 0.96f, {142, 236, 228}, 2);
        drawHubText(sidebar_canvas, 18, 470, "TAB5 <-> CORES3 BRIDGE TELEMETRY", {164, 252, 238}, 1, 2);

        char t1[40], t2[40], t3[40], t4[40], t5[40];
        std::snprintf(t1, sizeof(t1), "DISPLAY: 1280x720 (3.0X HD VIEWPORT)");
        std::snprintf(t2, sizeof(t2), "ESP-NOW CH 1: TX %u | RX %u PKTS",
                      static_cast<unsigned>(tx_frame_count), static_cast<unsigned>(rx_frame_count));
        std::snprintf(t3, sizeof(t3), "REPLAY DROPS: %u | AUTH DROPS: %u",
                      static_cast<unsigned>(net_node.duplicate_drops), static_cast<unsigned>(net_node.auth_failures));
        std::snprintf(t4, sizeof(t4), "SANCTUARY FPS: %.1f | PSRAM: 32 MB", fps_smooth);
        std::snprintf(t5, sizeof(t5), "OTA: %.31s", ota_status);
        drawHubText(sidebar_canvas, 18, 498, t1, {242, 238, 228}, 1, 2);
        drawHubText(sidebar_canvas, 18, 524, t2, {255, 232, 156}, 1, 2);
        drawHubText(sidebar_canvas, 18, 550, t3, {188, 224, 255}, 1, 2);
        drawHubText(sidebar_canvas, 18, 576, t4, {168, 248, 198}, 1, 2);
        drawHubText(sidebar_canvas, 18, 604, t5, {255, 218, 168}, 1, 2);

        // Wireless HTTPS OTA Button (y = 646..708)
        drawHubBox(sidebar_canvas, 10, 646, 309, 708, {36, 68, 62}, 0.95f, {152, 248, 236}, 2);
        drawHubText(sidebar_canvas, 20, 668, "CHECK WIRELESS OTA (VERCEL)", {242, 255, 246}, 1, 2);
    }

    // Handle 1280x720 Capacitive Touch Input across Viewport (0..959) and Command Deck (960..1279)
    bool handleTouch1280x720(int tx, int ty, uint8_t outWire[protocol::MAX_WIRE_PACKET_BYTES], size_t& outLen) {
        outLen = 0;
        tx = std::clamp(tx, 0, kTab5DisplayWidth - 1);
        ty = std::clamp(ty, 0, kTab5DisplayHeight - 1);

        if (tx < kTab5ViewportWidth) {
            // Left 960x720 3.0x HD Sanctuary Viewport Touch
            if (ty <= 56) {
                if (tx < 448) {
                    flip_screen_requested = true;
                    bg_cache_key = 0;
                    std::snprintf(status_banner, sizeof(status_banner), "SCREEN ROTATION FLIPPED 180 DEG");
                } else if (tx >= 452 && tx <= 542) {
                    noir_mode = !noir_mode;
                    std::snprintf(status_banner, sizeof(status_banner), noir_mode ? "NOIR INK CROSSHATCH ENABLED" : "COLOR MOEBIUS MODE RESTORED");
                } else if (tx >= 548 && tx <= 650) {
                    lod2_anatomy_scan = !lod2_anatomy_scan;
                } else if (tx >= 656 && tx <= 746) {
                    close_up_zoom = !close_up_zoom;
                } else if (tx >= 752 && tx <= 838) {
                    sanctuary_biome = (sanctuary_biome + 1u) % catalog::kNumBiomeVariants;
                    world = catalog::buildWorldForBiomeVariant(sanctuary_biome, pods[selected_pod].catalog_seed);
                } else if (tx >= 844) {
                    breedSanctuaryPods(selected_pod, (selected_pod + 1) % kMaxSanctuaryPods, (selected_pod + 2) % kMaxSanctuaryPods);
                }
                sidebar_dirty = true;
                return true;
            }
            if (ty >= 664) {
                int btn = (tx - 20) / 154;
                if (btn == 0) {
                    treat_active = true;
                    treat_x = pods[selected_pod].pose.world_pos.x;
                    treat_y = 42.0f;
                    treat_phase = 0.0f;
                    outLen = buildRemoteCareCommand(protocol::HubRemoteAction::REMOTE_FEED_TREAT, outWire);
                } else if (btn == 1) {
                    ball_active = !ball_active;
                    outLen = buildRemoteCareCommand(protocol::HubRemoteAction::REMOTE_TOGGLE_BALL, outWire);
                } else if (btn == 2) {
                    chorus_active = true;
                    chorus_timer = 4.5f;
                    for (int i = 0; i < kMaxSanctuaryPods; ++i) {
                        pods[i].hop_vy = -82.0f;
                        pods[i].heart_timer = 1.6f;
                    }
                    outLen = buildRemoteCareCommand(protocol::HubRemoteAction::REMOTE_SING_CHIME, outWire);
                } else if (btn == 3) {
                    pods[selected_pod].evo_stage = (pods[selected_pod].evo_stage + 1u) & 3u;
                    pods[selected_pod].hop_vy = -90.0f;
                    pods[selected_pod].heart_timer = 1.8f;
                    outLen = buildRemoteCareCommand(protocol::HubRemoteAction::REMOTE_BLESS_XP, outWire);
                } else if (btn == 4) {
                    beamActivePodToPeer(selected_peer, outWire, outLen);
                } else if (btn == 5) {
                    sim_fleet_active = !sim_fleet_active;
                    if (sim_fleet_active) seedSimulatedCoreS3Fleet(2000);
                }
                sidebar_dirty = true;
                return true;
            }

            // Direct touch inside the 960x720 world (mapped to 320x240 virtual coordinates via / 3.0f)
            float wx = tx / 3.0f;
            float wy = ty / 3.0f;
            int best_pod = selected_pod;
            float best_d2 = 999999.0f;
            for (int i = 0; i < kMaxSanctuaryPods; ++i) {
                if (!pods[i].active) continue;
                float dx = pods[i].pose.world_pos.x - wx;
                float dy = pods[i].pose.world_pos.y - wy;
                float d2 = dx * dx + dy * dy;
                if (d2 < best_d2) {
                    best_d2 = d2;
                    best_pod = i;
                }
            }
            if (best_d2 < 46.0f * 46.0f) {
                selected_pod = best_pod;
                pods[selected_pod].hop_vy = -85.0f;
                pods[selected_pod].heart_timer = 1.5f;
                std::snprintf(seed_input_buf, sizeof(seed_input_buf), "%u", static_cast<unsigned>(pods[selected_pod].catalog_seed));
                sidebar_dirty = true;
            } else if (wy < 105.0f) {
                treat_active = true;
                treat_x = std::clamp(wx, 38.0f, 282.0f);
                treat_y = std::clamp(wy, 28.0f, 145.0f);
                treat_phase = 0.0f;
            } else {
                pods[selected_pod].home_x = std::clamp(wx, 42.0f, 278.0f);
                pods[selected_pod].hop_vy = -65.0f;
            }
            return true;
        }

        // Right 320x720 Observatory Command Deck Touch (lx = 0..319)
        int lx = tx - kTab5SidebarX;
        if (ty >= 54 && ty <= 90) {
            deck_tab = std::clamp((lx - 8) / 77, 0, 3);
            sidebar_dirty = true;
            return true;
        }

        if (deck_tab == 0) {
            if (ty >= 118 && ty < 342) {
                int pod_i = std::clamp((ty - 118) / 56, 0, kMaxSanctuaryPods - 1);
                selected_pod = pod_i;
                pods[pod_i].hop_vy = -75.0f;
                std::snprintf(seed_input_buf, sizeof(seed_input_buf), "%u", static_cast<unsigned>(pods[pod_i].catalog_seed));
                sidebar_dirty = true;
            } else if (ty >= 370 && ty < 562) {
                selected_peer = std::clamp((ty - 370) / 64, 0, 2);
                sidebar_dirty = true;
            } else if (ty >= 568 && ty <= 612) {
                if (lx < 158) {
                    invitePeerToSanctuaryPod(selected_peer, (selected_pod == 0) ? 1 : selected_pod, outWire, outLen);
                } else {
                    togglePeerOwnershipLease(selected_peer, outWire, outLen);
                }
            } else if (ty >= 618 && ty <= 662) {
                if (lx < 158) {
                    beamActivePodToPeer(selected_peer, outWire, outLen);
                } else {
                    outLen = buildRemoteCareCommand(protocol::HubRemoteAction::REMOTE_BLESS_XP, outWire);
                }
            } else if (ty >= 668) {
                breedSanctuaryPods(selected_pod, (selected_pod + 1) % kMaxSanctuaryPods, selected_pod);
            }
        } else if (deck_tab == 1) {
            SanctuaryPod& pod = pods[selected_pod];
            auto tr = genetics::unpackChimeraTraits(pod.genome.appearance.markings_seed);
            if (ty >= 272 && ty < 650) {
                int row = (ty - 272) / 54;
                if (row == 0) {
                    int delta = (lx < 160) ? -1 : 1;
                    int next_sp = (pod.species_idx + delta + catalog::kNumPrimarySpecies) % catalog::kNumPrimarySpecies;
                    uint32_t seed = static_cast<uint32_t>(next_sp * 3 + pod.alt_idx + 1);
                    loadPodByCatalogSeed(selected_pod, seed, pod.evo_stage, pod.role, pod.home_x);
                } else if (row == 1) {
                    int next_alt = (pod.alt_idx + 1) % catalog::kNumAlternates;
                    uint32_t seed = static_cast<uint32_t>(pod.species_idx * 3 + next_alt + 1);
                    loadPodByCatalogSeed(selected_pod, seed, pod.evo_stage, pod.role, pod.home_x);
                } else if (row == 2) {
                    pod.pattern_variant = (pod.pattern_variant + 1u) % 6u;
                } else if (row == 3) {
                    tr.dorsal_part = (tr.dorsal_part + 1u) % 8u;
                } else if (row == 4) {
                    tr.eye_style = (tr.eye_style + 1u) % 7u;
                } else if (row == 5) {
                    pod.genome.appearance.palette_id = (pod.genome.appearance.palette_id + 1u) & 0xFu;
                    pod.graph.palette_id = pod.genome.appearance.palette_id;
                } else if (row == 6) {
                    pod.evo_stage = (pod.evo_stage + 1u) & 3u;
                }
                if (row >= 2 && row <= 5) {
                    pod.genome.appearance.markings_seed = genetics::packChimeraMarkingsSeed(tr);
                    pod.graph.markings_seed = pod.genome.appearance.markings_seed;
                    pod.catalog_seed = catalog::encodeDnaSculptorCatalogSeed(
                        pod.species_idx * 3 + pod.alt_idx,
                        pod.pattern_variant,
                        pod.genome.appearance.markings_seed,
                        pod.genome.appearance.palette_id
                    );
                }
                sidebar_dirty = true;
            } else if (ty >= 656) {
                beamActivePodToPeer(selected_peer, outWire, outLen);
            }
        } else if (deck_tab == 2) {
            if (ty >= 122 && ty < 498) {
                int slot = (vault_page & 3) * 4 + std::clamp((ty - 122) / 94, 0, 3);
                if (lx < 158) {
                    VaultSlotEntry& v = vault[slot];
                    v.magic         = 0x7AB5FA76u;
                    v.used          = 1u;
                    v.catalog_seed  = pods[selected_pod].catalog_seed;
                    v.markings_seed = pods[selected_pod].genome.appearance.markings_seed;
                    v.palette_id    = pods[selected_pod].genome.appearance.palette_id;
                    v.evo_stage     = pods[selected_pod].evo_stage;
                    v.bond_hearts   = pods[selected_pod].bond_hearts;
                    v.biome_variant = pods[selected_pod].biome_variant;
                    std::snprintf(v.name, sizeof(v.name), "%.19s", pods[selected_pod].name);
                    std::snprintf(status_banner, sizeof(status_banner), "SAVED #%06u TO VAULT %d!", static_cast<unsigned>(v.catalog_seed), slot + 1);
                } else if (vault[slot].used) {
                    loadPodByCatalogSeed(selected_pod, vault[slot].catalog_seed, vault[slot].evo_stage, pods[selected_pod].role, pods[selected_pod].home_x);
                }
                sidebar_dirty = true;
            } else if (ty >= 508 && ty <= 556) {
                vault_page = (vault_page + ((lx < 160) ? 3 : 1)) & 3;
                sidebar_dirty = true;
            } else if (ty >= 566 && ty <= 630) {
                ota_update_requested = true;
                std::snprintf(ota_status, sizeof(ota_status), "OTA CHECK QUEUED...");
                sidebar_dirty = true;
            } else if (ty >= 640 && ty <= 706) {
                sd_export_requested = true;
                sidebar_dirty = true;
            }
        } else if (deck_tab == 3) {
            if (ty >= 182 && ty <= 446) {
                int r = std::clamp((ty - 182) / 66, 0, 3);
                int c = std::clamp((lx - 10) / 101, 0, 2);
                int k = r * 3 + c;
                size_t cur_len = std::strlen(seed_input_buf);
                if (k == 9) {
                    seed_input_buf[0] = '\0';
                } else if (k == 11) {
                    uint32_t val = static_cast<uint32_t>(std::strtoul(seed_input_buf, nullptr, 10));
                    if (val < 1u) val = 1u;
                    loadPodByCatalogSeed(selected_pod, val, pods[selected_pod].evo_stage, pods[selected_pod].role, pods[selected_pod].home_x);
                } else {
                    char digit = (k == 10) ? '0' : static_cast<char>('1' + k);
                    if (cur_len < 6) {
                        seed_input_buf[cur_len] = digit;
                        seed_input_buf[cur_len + 1] = '\0';
                    }
                }
                sidebar_dirty = true;
            } else if (ty >= 646 && ty <= 708) {
                ota_update_requested = true;
                std::snprintf(ota_status, sizeof(ota_status), "OTA CHECK QUEUED...");
                sidebar_dirty = true;
            }
        }
        return true;
    }

    // Composite the 960x720 Sanctuary Viewport + 320x720 Command Deck into a full 1280x720 RGB888 buffer
    void compositeFrame1280x720(render::PixelBuffer& out1280x720) {
        out1280x720.resize(static_cast<size_t>(kTab5DisplayWidth) * kTab5DisplayHeight * 3);
        const uint8_t* vp = viewport_canvas.pixels.data();
        const uint8_t* sb = sidebar_canvas.pixels.data();
        uint8_t* dst = out1280x720.data();

        for (int y = 0; y < kTab5DisplayHeight; ++y) {
            uint8_t* row_dst = dst + static_cast<size_t>(y) * kTab5DisplayWidth * 3;
            std::memcpy(row_dst, vp + static_cast<size_t>(y) * kTab5ViewportWidth * 3, kTab5ViewportWidth * 3);
            std::memcpy(row_dst + kTab5ViewportWidth * 3, sb + static_cast<size_t>(y) * kTab5SidebarWidth * 3, kTab5SidebarWidth * 3);
        }
    }
};

} // namespace tab5hub
} // namespace oddling
