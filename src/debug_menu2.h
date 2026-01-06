#pragma once

#include "mstring.h"
#include "fixed_pool.h"
#include "float.hpp"

namespace ai {
struct ai_core;
}

struct debug_menu_entry2;
struct script_instance;

struct debug_menu2
{
    enum class sort_mode_t {
        undefined = 0,
        ascending = 1,
        descending = 2,
    };

    debug_menu_entry2 *first;
    debug_menu_entry2 *last;
    debug_menu_entry2 *highlighted;
    mString field_C;
    sort_mode_t m_sort_mode;


    debug_menu2(const char *a1, sort_mode_t sort_mode);

    debug_menu2(const mString &a1, sort_mode_t a2);

    ~debug_menu2();

    void * operator new(size_t size);

    void operator delete(void *);

    debug_menu2 *find_submenu_parent(debug_menu2 *);

    void activate_parent();

    void render(int x, int y);

    void do_frame_advance(Float a2);

    void add_entry(debug_menu2 *a2);

    void add_entry(debug_menu_entry2 *e);

    void remove_entry(debug_menu_entry2 *e);

    mString &sub_B7E660();
	


    static void hide();

    static void show();

    static void grab_focus();

    static void release_focus();

    static void render_active();

    static void gather_input(Float a1);

    static void frame_advance(Float a1);

    static debug_menu2 *script_menu2;

    static debug_menu2 *active_menu2;

    static debug_menu2 *root_menu2;

    static inline int menu_height = 0;

    static inline int menu_width = 0;

    static inline bool has_focus = false;
    
    static inline bool physics_state_on_exit = true;

    static inline bool had_menu_this_frame = false;

    static inline bool previous_input_state[10]{};

    static inline bool virtual_input_state[10]{};

    static inline bool current_input_state[10]{};

    static inline bool virtual_input_repeating[10]{};

    static inline float input_state_timer[10]{};

    static fixed_pool pool;
};

enum class ValueType : uint16_t
{
    UNDEFINED = 0,
    FLOAT = 1,
    POINTER_FLOAT = 2,
    BOOL = 3,
    POINTER_BOOL = 4,
    INT = 5,
    POINTER_INT = 6,
    POINTER_MENU = 7,
};

struct debug_menu_entry2 {
    union {
        float fval;
        float *p_fval;
        bool bval;
        bool *p_bval;
        int ival;
        int *p_ival;
        debug_menu2 *p_menu;
    } m_value;
    void (*m_game_flags_handler)(debug_menu_entry2 *);
    mString (*render_callback)(debug_menu_entry2 *);
    void (__cdecl *field_C)(debug_menu_entry2 *, void *);
    void (__cdecl *frame_advance_callback)(debug_menu_entry2 *);
    script_instance *field_14;
    int field_18;
    unsigned short m_id;
    ValueType value_type;
    struct {
        float m_min_value;
        float m_max_value;
        float m_step_size;
        float m_step_scale;
    } field_20;
    debug_menu_entry2 *prev;
    debug_menu_entry2 *next;
    void *m_data;
    mString m_name;
    bool m_value_initialized;

    debug_menu_entry2(const mString &a1);

    debug_menu_entry2(debug_menu2 *submenu);

    ~debug_menu_entry2();

    void * operator new(size_t );

    void operator delete(void *) {}

    auto get_value_type() const
    {
        return value_type;
    }

    void *get_data()
    {
        return this->m_data;
    }

    void set_data(void *a1)
    {
        this->m_data = a1;
    }

    unsigned get_id() const
    {
        return this->m_id;
    }

    void set_submenu(debug_menu2 *a2);

    void set_frame_advance_cb(void (*a2)(debug_menu_entry2 *))
    {
        this->frame_advance_callback = a2;
    }

    debug_menu2 *remove_menu();

    int render(int a1, int a2, bool a3);

    bool set_script_handler(script_instance *inst, const mString &a3);

    void set_render_cb(mString (*a2)(debug_menu_entry2 *) );

    void set_game_flags_handler(void (*a2)(debug_menu_entry2 *));

    void set_id(short a2);

    bool is_value_initialized() const;

    void set_value_initialized(bool a2) {
        m_value_initialized = a2;
    }

    void set_min_value(float a2);

    void set_max_value(float a2);

    void set_val(Float a1, bool a2);

    double get_val();

    void set_fval(float a2, bool a3);

    double get_fval();

    bool set_bval(bool, bool);

    void set_bval(bool a2);

    bool get_bval();

    int set_ival(int a2, bool a3);

    void set_ival(int a2);

    void set_p_ival(int *);

    void set_pt_bval(bool *a2);

    void set_pt_fval(float *a2);

    void set_fl_values(const float *a2);

    int get_ival();

    const mString & get_name() const;

    void on_select(float a2);

    void on_change(float a3, bool a4);

    static fixed_pool pool;
};

extern mString entry_render_callback_default(debug_menu_entry2 *a2);

extern auto create_menu2(const mString &str, debug_menu2::sort_mode_t sort_mode) -> debug_menu2*;

extern auto create_menu2(const char *str, debug_menu2::sort_mode_t sort_mode) -> debug_menu2*;

extern auto create_menu_entry2(const mString &str) -> debug_menu_entry2*;

extern auto create_menu_entry2(debug_menu2 *menu) -> debug_menu_entry2*;

extern void _populate_missions();

extern debug_menu_entry2 *g_debug_camera_entry2;
