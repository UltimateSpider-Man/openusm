#pragma once

#include "singleton.h"

#include "mstring.h"

#include <optional>

struct os_developer_options : singleton {

    enum strings_t {
        SOUND_LIST = 0,
        SCENE_NAME = 1,
        HERO_NAME = 2,
        GAME_TITLE = 3,
        GAME_LONG_TITLE = 4,
        SAVE_GAME_DESC = 5,
        VIDEO_MODE = 6,
        GFX_DEVICE = 7,
        FORCE_DEBUG_MISSION = 8,
        FORCE_LANGUAGE = 9,
        SKU = 10,
        CONSOLE_EXEC = 11,
        HERO_START_DISTRICT = 12,
        DEBUG_ENTITY_NAME = 13,

    };

	enum flags_t {};

	enum ints_t {};

    bool m_flags[150];
    mString m_strings[14];
    int m_ints[76];
    mString field_2AC;

    //0x005B8700
    os_developer_options();

    //0x005E2CB0
    //virtual
    ~os_developer_options();
	
	void set_hero_name(const mString& hero_name);

	void toggle_flag(flags_t a2);

    //0x005B87E0
    char get_flag(flags_t a2) const;

    //0x005C2F20
    char get_flag(const mString &a2) const;

    //0x005B88A0
    int get_flag_from_name(const mString &a1) const;

    //0x005B8830
    int get_int(ints_t a2) const;

    //0x005C2F60
    int get_int(const mString &a2) const;

    //0x005B8950
    int get_int_from_name(const mString &a1) const;

    //0x005B8810
    void set_int(int idx, int a3);

    //0x005C2F40
    void set_int(const mString &a2, int a3);

    //0x005B87D0
    void set_flag(int a2, bool a3);

    //0x005C3150
    mString *get_hero_name() const;

    //0x005C2F00
    void set_flag(const mString &a2, bool a3);

    //0x005B8860
    std::optional<mString> get_string(strings_t a2) const;

    //0x005C2FB0
    std::optional<mString> get_string(const mString &a1) const;

    //0x005B8A00
    strings_t get_string_from_name(const mString &a1) const;

    //0x005C2F80
    void set_string(const mString &a2, const mString &a3);

    //0x005B8840
    void set_string(strings_t a2, const mString &a3);

    //0x005B23E0
    static void os_developer_init();

    static os_developer_options *& instance;
};

using int_names_t = const char *[76];
extern int_names_t & int_names;

using flag_names_t = const char *[150];
extern flag_names_t & flag_names;

using string_names_t = const char *[14];
extern string_names_t & string_names;

using flag_defaults_t = BOOL[150];
inline flag_defaults_t & flag_defaults = var<flag_defaults_t>(0x00936678);

extern void os_developer_options_patch();
