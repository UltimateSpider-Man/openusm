#include "os_developer_options.h"

#include "common.h"
#include "func_wrapper.h"
#include "log.h"

#include "trace.h"
#include "utility.h"
#include "variables.h"

#include <cassert>
#include <cstdio>
#include <string.h>

VALIDATE_OFFSET(os_developer_options, m_strings, 0x9C);

VALIDATE_SIZE(os_developer_options, 0x2BC);

#ifndef TEST_CASE
os_developer_options *& os_developer_options::instance = var<os_developer_options *>(0x0096858C);

int_names_t & int_names = var<const char *[76]>(0x00936940);

flag_names_t & flag_names = var<const char *[150]>(0x00936420);

string_names_t & string_names = var<const char *[14]>(0x009368D0);

static auto & int_defaults = var<int[76]>(0x00936A70);

static auto & string_defaults = var<const char *[8]>(0x00936908);

#else
static os_developer_options *g_instance {};
os_developer_options *& os_developer_options::instance {g_instance};

static const char *g_int_names[76]{"DIFFICULTY",
                                   "CAMERA_STYLE",
                                   "CAMERA_STATE",
                                   "CAMERA_FOV",
                                   "FOG_RED",
                                   "FOG_GREEN",
                                   "FOG_BLUE",
                                   "FOG_DISTANCE",
                                   "BIT_DEPTH",
                                   "MONKEY_MODE",
                                   "RANDOM_SEED",
                                   "FORCE_WIN",
                                   "CONTROLLER_TYPE",
                                   "FRAME_LOCK",
                                   "FRAME_LIMIT",
                                   "SWING_DEBUG_TRAILS",
                                   "SOAK_SMOKE",
                                   "FAR_CLIP_PLANE",
                                   "POI_DISPLAY_TYPE",
                                   "STORY_MISSION",
                                   "EXEC_DELAY",
                                   "RUN_LENGTH",
                                   "PC_WINDOW_TOP",
                                   "PC_WINDOW_LEFT",
                                   "PC_WINDOW_WI DTH",
                                   "PC_WINDOW_HEIGHT",
                                   "ALLOW_SCREENSHOT",
                                   "AMALGA_REFRESH_INTERVAL",
                                   "ENABLE_LONG_MALOR_ASSERTS",
                                   "GOD_MODE",
                                   "PCLISTBUFFER",
                                   "PCSCRATCHBUFFER",
                                   "PCSCRATCHI NDEXBUFFER",
                                   "PCSCRATCHVERTEXBUFFER",
                                   "NAL_HEAP_SIZE",
                                   "ASSERT_BOX_MARGIN",
                                   "ASSERT_TEXT_MARGIN",
                                   "ASSERT_FONT_PCT_X",
                                   "ASSERT_FONT_PCT_Y",
                                   "STREAMER_INFO_FONT_PCT",
                                   "DEBUG_INFO_FONT_PCT",
                                   "PITCH_FACTOR",
                                   "BANK_FACTOR",
                                   "SWING_INTERPOLATION_TIME",
                                   "BOTH_HANDS_INTERPOLATION_TIME",
                                   "MEM_DUMP_FRAME",
                                   "HERO_START_X",
                                   "HERO_START_Y",
                                   "HERO_START_Z",
                                   "SHOW_SOUND_INFO",
                                   "SHOW_VOICE_BOX_INFO",
                                   "DEBUG_CAMERA_PITCH_MULTIPLIER",
                                   "DEBUG_CAMERA_YAW_MULTIPLIER",
                                   "DEBUG_CAMERA_MOVE_MULTIPLIER",
                                   "DEBUG_CAMERA_STRAFE_MULTIPLIER",
                                   "TAM_SCALE_MIN_DISTANCE",
                                   "TAM_SCALE_MAX_ DISTANCE",
                                   "TAM_SCALE_MIN_PERCENT",
                                   "THUG_HEALTH_UI_SCALE_MIN_DISTANCE",
                                   "THUG_H EALTH_UI_SCALE_MAX_DISTANCE",
                                   "THUG_HEALTH_UI_SCALE_MIN_PERCENT",
                                   "TARGETING_RET ICLE_SCALE_MIN_DISTANCE",
                                   "TARGETING_RETICLE_SCALE_MAX_DISTANCE",
                                   "TARGETING_RET ICLE_SCALE_MIN_PERCENT",
                                   "HIRES_SCREENSHOT_X",
                                   "HIRES_SCREENSHOT_Y",
                                   "TIME_OF_DAY",
                                   "MINI_MAP_ZOOM",
                                   "RTDT_REPLAY_BUFFER_SIZE",
                                   "TIMER_WIDGET_TIME_DELTA_PERCENT",
                                   "DEBUG_PARTICLE_LEVEL",
                                   "DEBUG_PARTICLE_MEMORY",
                                   "MAX_AEPS_ENTITIES",
                                   "MAX_AE PS_SPAWNERS",
                                   "MAX_AEPS_EMITTERS",
                                   "MAX_AEPS_PARTICLES"};
int_names_t & int_names { g_int_names };

static const char *g_flag_names[150]{"CD_ONLY",
                                     "ENVMAP_TOOL",
                                     "NO_CD",
                                     "CHATTY_LOAD",
                                     "WINDOW_DEFAULT",
                                     "SHOW_FPS",
                                     "SHOW_STREAMER_INFO",
                                     "SHOW_STREAMER_SPAM",
                                     "SHOW_RESOURCE_SPAM",
                                     "SHOW_MEMORY_INFO",
                                     "SHOW_SPIDEY_SPEED",
                                     "TRACE_MISSION_MANAGER",
                                     "DUMP_MISSION_HEAP",
                                     "CAMERA_CENTRIC_STREAMER",
                                     "RENDER_LOWLODS",
                                     "LOAD_STRING_HASH_DICTIONARY",
                                     "LOG_RUNTIME_STRING_HASHES",
                                     "SHOW_WATERMARK_VELOCITY",
                                     "SHOW_STATS",
                                     "ENABLE_ZOOM_MAP",
                                     "SHOW_DEBUG_INFO",
                                     "SHOW_PROFILE_INFO",
                                     "SHOW_LOCOMOTION_INFO",
                                     "GRAVITY",
                                     "TEST_MEMORY_LEAKS",
                                     "HALT_ON_ASSERTS",
                                     "SCREEN_ASSERTS",
                                     "NO_ANIM_WARNINGS",
                                     "PROFILING_ON",
                                     "MONO_AUDIO",
                                     "NO_MESSAGES",
                                     "LOCK_STEP",
                                     "TEXTURE_DUMP",
                                     "DISABLE_SOUND_WARNINGS",
                                     "DISABLE_SOUND_DEBUG_OUTPUT",
                                     "DELETE_UNUSED_SOUND_BANKS_ON_PACK",
                                     "LOCKED_HERO",
                                     "FOG_OVERR IDE",
                                     "FOG_DISABLE",
                                     "MOVE_EDITOR",
                                     "AI_PATH_DEBUG",
                                     "AI_PATH_COLOR",
                                     "AI_CRITTER_ACTIVITY",
                                     "AI_PATH_COLOR_CRITTER",
                                     "AI_PATH_COLOR_HERO",
                                     "NO_PARTICLES",
                                     "NO_PARTICLE_PUMP",
                                     "SHOW_NORMALS",
                                     "SHOW_SHADOW_BALL",
                                     "SHOW_LIGHTS",
                                     "SHOW_PLRCTRL",
                                     "SHOW_PSX_INFO",
                                     "FLAT_SHADE",
                                     "INTERFACE_DISABLE",
                                     "WIDGET_TOOLS",
                                     "LIGHTING",
                                     "FAKE_POINT_LIGHTS",
                                     "BSP_SPRAY_PAINT",
                                     "CAMERA_EDITOR",
                                     "IGNORE_RAMPING",
                                     "POINT_SAMPLE",
                                     "DISTANCE_FADING",
                                     "OVERRIDE_CONTROLLER_OPTIONS",
                                     "DISABLE_MOUSE_PLAYER_CONTROL",
                                     "NO_MOVIES",
                                     "XBOX_USER_CAM",
                                     "NO_LOAD_SCREEN",
                                     "EXCEPTION_HANDLER",
                                     "NO_EXCEPTION_HANDLER",
                                     "NO_CD_CHECK",
                                     "NO_LOAD_METER",
                                     "NO_MOVIE_BUFFER_WARNING",
                                     "LIMITED_EDITION_DISC",
                                     "NEW_COMBAT_LOCO",
                                     "LEVEL_WARP",
                                     "SMOKE_TEST",
                                     "SMOKE_TEST_LEVEL",
                                     "COMBO_TESTER",
                                     "DROP _SHADOWS_ALWAYS_RAYCAST",
                                     "DISABLE_DROP_SHADOWS",
                                     "DISABLE_HIRES_SHADOWS",
                                     "DISABLE_STENCIL_SHADOWS",
                                     "DISABLE_COLORVOLS",
                                     "DISABLE_RENDER_ENTS",
                                     "DISABLE_FULLSCREEN_BLUR",
                                     "FORCE_TIGHTCRAWL",
                                     "FORCE_NONCRAWL",
                                     "SHOW_DEBUG_TEXT",
                                     "SHOW_STYLE_POINTS",
                                     "CAMERA_MOUSE_MODE",
                                     "USERCAM_ON_CONTROLLER2",
                                     "DISABLE_ANCHOR_RETICLE_RENDERING",
                                     "SHOW_ANCHOR_LINE",
                                     "FREE_SPIDER_REFLEXES",
                                     "SHOW_BAR_OF_SHAME",
                                     "SHOW_ENEMY_HEALTH_WIDGETS",
                                     "ALLOW_IGC_PAUSE",
                                     "SHOW_OBBS",
                                     "SHOW_DISTRICTS",
                                     "SHOW_CHUCK_DEBUGGER",
                                     "CHUCK_OLD_FASHIONED",
                                     "CHUCK_DISABLE_BREAKPOINTS",
                                     "SHOW_AUDIO_BOXES",
                                     "DISABLE_SOUNDS",
                                     "SHOW_TERRAIN_INFO",
                                     "DISABLE_AUDIO_BOXES",
                                     "NSL_OLD_FASHIONED",
                                     "SHOW_MASTER_CLOCK",
                                     "LOAD_GRADIENTS",
                                     "BONUS_LEVELS_AVAILABLE",
                                     "COMBAT_DISPLAY",
                                     "COMBAT_DEBUGGER",
                                     "ALLOW_ERROR_POPUPS",
                                     "ALLOW_WARNING_POPUPS",
                                     "OUTPUT_WARNING_DISABLE",
                                     "OUTPUT_ASSERT_DISABLE",
                                     "ASSERT_ON_WARNING",
                                     "ALWAYS_ACTIVE",
                                     "FORCE_PROGRESSION",
                                     "LINK",
                                     "WAIT_FOR_LINK",
                                     "SHOW_END_OF_PACK",
                                     "LIVE_IN_GLASS_HOUSE",
                                     "SHOW_GLASS_HOUSE",
                                     "DISABLE_RACE_PREVIEW",
                                     "FREE_MINI_MAP",
                                     "SHOW_LOOPING_ANIM_WARNINGS",
                                     "SHOW_PERF_INFO",
                                     "COPY_ERROR_TO_CLIPBOARD",
                                     "BOTH_HANDS_UP_REORIENT",
                                     "SHOW_CAMERA_PROJECTION",
                                     "OLD_DEFAULT_CONTROL_SETTINGS",
                                     "IGC_SPEED_CONTROL",
                                     "RTDT_ENABLED",
                                     "ENABLE_DECALS",
                                     "AUTO_STICK_TO_WALLS",
                                     "ENABLE_PEDESTRIANS",
                                     "CAMERA_INVERT_LOOKAROUND",
                                     "CAMERA_INVERT_LOOKAROUND_X",
                                     "CAMERA_INVERT_LOOKAROUND_Y",
                                     "FORCE_COMBAT_CAMERA_OFF",
                                     "DISPLAY_THOUGHT_BUBBLES",
                                     "ENABLE_DEBUG_LOG",
                                     "ENABLE_LONG_CALLSTACK",
                                     "RENDER_FE_UI",
                                     "LOCK_INTERIORS",
                                     "MEMCHECK_ON_LOAD",
                                     "DISPLAY_ALS_USAGE_PROFILE",
                                     "ENABLE_FPU_EXCEPTION_HANDLING",
                                     "UNLOCK_ALL_UNLOCKABLES"};
flag_names_t & flag_names { g_flag_names };

static const char *g_string_names[14]{};
string_names_t & string_names { g_string_names };

static const int g_flag_defaults[150]{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
                                      1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                                      0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                                      0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                                      0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                                      0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0};
static flag_defaults_t & flag_defaults {g_flag_defaults};

static int g_int_defaults[76] {2,    9,    0,   90,  128, 128, 128,   0,   16,   0,    0,
                              0,    2,    0,   1,   0,   0,   10000, 2,   0,    0,    -1,
                              -1,   -1,   640, 480, 0,   5,   0,     0,   8192, 1024, 65535,
                              2048, 1024, 15,  2,   100, 100, 100,   100, 100,  100,  100,
                              100,  -1,   0,   0,   0,   0,   0,     20,  20,   20,   20,
                              1,    11,   50,  1,   11,  25,  1,     11,  25,   1920, 1440,
                              -1,   200,  5,   100, 0,   0,   0,     0,   0,    0};
static auto & int_defaults{g_int_defaults};

static const char *g_string_defaults[8]{"spidey.snd",
                                        "city_arena",
                                        "ultimate_spiderman",
                                        "Spider-Man",
                                        "Spider-Man",
                                        "Spider-Man saved game",
                                        "640x480",
                                        "display"

};
static auto & string_defaults {g_string_defaults};

#endif

void os_developer_options::set_hero_name(const mString& hero_name) {
    set_hero_name(hero_name.c_str());
}

void os_developer_options::toggle_flag(os_developer_options::flags_t a2)
{
  	this->m_flags[a2] = !this->m_flags[a2];
}

char os_developer_options::get_flag(os_developer_options::flags_t a2) const
{
    return this->m_flags[a2];
}

int os_developer_options::get_int(os_developer_options::ints_t a2) const
{
    return this->m_ints[a2];
}

void os_developer_options::set_int(int idx, int a3) {
    this->m_ints[idx] = a3;
}

void os_developer_options::set_int(const mString &a2, int a3) {
    this->m_ints[this->get_int_from_name(a2)] = a3;
}

void os_developer_options::set_flag(int a2, bool a3) {
    this->m_flags[a2] = a3;
}

void os_developer_options::set_flag(const mString &a2, bool a3) {
    if constexpr (1) {
        this->m_flags[this->get_flag_from_name(a2)] = a3;
    }
    else
    {
        THISCALL(0x005C2F00, this, &a2, a3);
    }
}

os_developer_options::os_developer_options()
{
    {
        bool *v3 = this->m_flags;
        const int *v4 = flag_defaults;
#if 0
        char v5;
        do {
            v5 = (*(uint32_t *) v4 != 0);
            v4 += 4;
            *v3++ = v5;
        } while ((int) v4 < (int) string_names);
#else
        std::copy(v4, v4 + 150, v3);
#endif
    }

    for (uint32_t i{0}; i < 76u; ++i) {
        this->m_ints[i] = int_defaults[i];
    }

    {
        mString *v2 = this->m_strings;
        const char **v8 = string_defaults;
        do {
            *v2 = *v8;
            ++v8;
            ++v2;
        } while ((int) v8 < (int) int_names);
    }
}

os_developer_options::~os_developer_options() {
    if constexpr (0) {
    } else {
        THISCALL(0x005E2CB0, this);
    }
}

mString *os_developer_options::get_hero_name() const
{
    if constexpr (1)
	{
        static mString result {"HERO"};

        result = this->m_strings[2];
        return &result;
    } else {
        return (mString *) THISCALL(0x005C3150, this);
    }
}

int os_developer_options::get_flag_from_name(const mString &a1) const
{
    auto func = [&a1](const char *v2) {
        return (_strcmpi(v2, a1.c_str()) == 0);
    };

    auto it = std::find_if(std::begin(flag_names), std::end(flag_names), func);

    size_t v3 = std::distance(std::begin(flag_names), it);
    if (v3 == std::size(flag_names)) {
        mString out = "Nonexistent option flag " + a1;
        sp_log("%s", out.c_str());
    }

    return v3;
}

char os_developer_options::get_flag(const mString &a2) const
{
	TRACE("os_developer_options::get_flag");

    if constexpr (1) {
        return this->m_flags[this->get_flag_from_name(a2)];
    } else {
		char (__fastcall *func)(const void *, void *, const mString *) = CAST(func, 0x005C2F20);
        return func(this, nullptr, &a2);
    }
}

os_developer_options::strings_t os_developer_options::get_string_from_name(const mString &a1) const
{
    auto func = [&a1](const char *v2) {
        return (_strcmpi(v2, a1.c_str()) == 0);
    };

    auto it = std::find_if(std::begin(string_names), std::end(string_names), func);

    auto v3 = (strings_t) std::distance(std::begin(string_names), it);
    if (v3 == std::size(string_names)) {
        mString out = "Nonexistent string " + a1;
    }

    return v3;
}

void os_developer_options::set_string(const mString &a2, const mString &a3) {
    strings_t v4 = this->get_string_from_name(a2);

    this->set_string(v4, a3);
}

void os_developer_options::set_string(strings_t a2, const mString &a3) {
    this->m_strings[a2] = a3;
}

std::optional<mString> os_developer_options::get_string(os_developer_options::strings_t a2) const
{
    if (a2 < strings_t::SOUND_LIST || a2 > strings_t::DEBUG_ENTITY_NAME) {
        return {};
    } else {
        auto a3 = this->m_strings[a2];
        return a3;
    }
}

std::optional<mString> os_developer_options::get_string(const mString &a1) const
{

    auto v4 = this->get_string_from_name(a1);
    if (v4 < strings_t::SOUND_LIST || v4 > strings_t::DEBUG_ENTITY_NAME) {
        return {};
    } else {
        mString str = this->m_strings[v4];
        return str;
    }
}

int os_developer_options::get_int(const mString &a2) const
{
    return this->m_ints[os_developer_options::get_int_from_name(a2)];
}

int os_developer_options::get_int_from_name(const mString &a1) const
{
    auto func = [&a1](const char *v2) -> bool {
        return (_strcmpi(v2, a1.c_str()) == 0);
    };

    auto it = std::find_if(std::begin(int_names), std::end(int_names), func);

    size_t v3 = std::distance(std::begin(int_names), it);
    if (v3 == std::size(int_names)) {
        mString out = "Nonexistent int " + a1;
        sp_log("%s", out.c_str());
    }

    return v3;
}

void os_developer_options::os_developer_init() {
    instance = new os_developer_options{};
}

void os_developer_options_patch()
{
    int (os_developer_options::*func)(os_developer_options::ints_t ) const = &os_developer_options::get_int;

    FUNC_ADDRESS(address, func);
    REDIRECT(0x0052B5F1, address);

    {
        char (os_developer_options::*func)(os_developer_options::flags_t a2) const = &os_developer_options::get_flag;
        FUNC_ADDRESS(address, func);
        SET_JUMP(0x005B87E0, address);
    }

    {
        FUNC_ADDRESS(address, &os_developer_options::get_flag_from_name);
        SET_JUMP(0x005B88A0, address);
    }
}
