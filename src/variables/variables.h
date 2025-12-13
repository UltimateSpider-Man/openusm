#pragma once

#include "variable.h"
#include "../nlPlatformEnum.h"

#include <windows.h>

#include <dsound.h>

// @todo: global config
struct GameConfig {
	bool DebugMode = false;

	bool WindowedMode = false;

	bool NoLoadScreen = false;
};

extern GameConfig g_config;



struct string_hash;
struct Input;
struct InputSettings;
struct PolytubeCustomMaterial;

inline constexpr bool STANDALONE_SYSTEM = 0;

extern PolytubeCustomMaterial *& webline_texture;

inline Var<float> g_strafe_mult {0x00921B70};

inline Var<float> g_move_mult {0x00921B6C};

inline Var<float> sluggish_mix {0x00959E44};

inline Var<float> med_mix {0x00959E50};

inline Var<float> fast_mix {0x00959E38};

inline Var<float> pronto_mix {0x00959E40};

inline Var<float> slow_mix {0x00959E4C};

inline Var<float> nglIFLSpeed {0x0093BC78};

inline Var<float> s_camera_target_radius_factor {0x0087EBC4};

extern Var<float> flt_87EBD4;

extern Var<float> flt_86F860;

extern Var<bool> byte_959561;

extern Var<int> dword_975308;

extern Var<int> dword_975314;

extern Var<int> dword_97530C;

extern Var<bool> byte_971F9C;

extern Var<bool> s_freeze_game_time;

extern Var<bool> g_generating_vtables;

extern Var<bool> byte_965C20;

inline auto & g_platform = var<_nlPlatformEnum>(0x0095C1E4);

extern Var<bool> g_distance_clipping_enabled;

extern Var<int> g_distance_clipping;

extern Var<int> g_disable_occlusion_culling;

extern Var<int> globalTextLanguage;

extern Var<HWND> g_appHwnd;

extern Var<bool> g_is_the_packer;

extern Var<int> g_TOD;

extern Var<char *[14]> dword_965C24;

extern Var<char[1024]> g_scene_name;

extern Var<bool> bExit;

extern Var<bool> byte_965BF9;

extern Var<HCURSOR> hCursor;

extern Var<HANDLE> hEvent;

extern Var<HANDLE> hObject;

extern Var<bool> byte_965BF5;
extern Var<bool> byte_965BF6;

struct IDirect3DDevice9;
extern Var<IDirect3DDevice9 *> g_Direct3DDevice;

extern IDirectSound8 *& g_directSound;



extern Var<float> g_tan_half_fov_ratio;

extern Var<int> dword_922908;

extern Var<bool> byte_922994;

extern Var<char> byte_965C21;

extern Var<char> byte_965BF8;

extern Var<int> g_cx;
extern Var<int> g_cy;

extern Var<HWND> g_hWnd;

extern Var<char *> dword_95C730;
extern Var<char *> dword_95C72C;

extern Var<int> dword_91E1D8;

extern Var<bool> g_indoors;

extern Var<bool> g_player_shadows_enabled;
extern Var<char> g_enable_stencil_shadows;

extern Var<int> g_cur_shadow_target;

extern Var<string_hash> bip01_l_calf;
extern Var<string_hash> bip01_r_calf;
extern Var<string_hash> bip01_pelvis;
extern Var<string_hash> bip01_head;
extern Var<string_hash> bip01_spine;

extern Var<char[260]> byte_9659B8;

extern Var<int> nWidth;
extern Var<int> nHeight;

extern Var<float> flt_965BDC;

extern Var<char> byte_95C718;
extern Var<int> dword_95C2F8;

extern Var<bool> ChromeEffect;

extern Var<int> g_Windowed;

struct IDirectSound8;

extern Var<float> flt_88E518;

extern Var<float> flt_88E51C;

extern Var<IDirectSound8 *> pUnkOuter;

extern Var<bool> g_master_clock_is_up;

extern Var<bool> byte_975468;

extern Var<bool> cam_target_locked;

extern Var<bool> EnableShader;

inline constexpr uint32_t RESOURCE_VERSION_INVALID = 0xFFFFFFFF;

inline constexpr uint32_t RESOURCE_PACK_VERSION = 0xE;
inline constexpr uint32_t RESOURCE_ENTITY_MASH_VERSION = 0x24D;
inline constexpr uint32_t RESOURCE_NONENTITY_MASH_VERSION = 0x12D;
inline constexpr uint32_t RESOURCE_AUTO_MASH_VERSION = 0x249;
inline constexpr uint32_t RESOURCE_RAW_MASH_VERSION = 0x115;
