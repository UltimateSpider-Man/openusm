#pragma once

#include "color32.h"
#include "signaller.h"

#include <list.hpp>

struct region;
struct generic_mash_header;
struct generic_mash_data_ptrs;
struct time_interface;
struct collision_geometry;
struct light_manager;
struct terrain;
struct nglMesh;

template<typename T, uint32_t N>
struct fixed_vector;

struct entity : signaller {
    using base_type = vhandle_type<signaller>;

    static constexpr inline auto FIXED_REGIONS_ARRAY_SIZE = 2;

    region *regions[FIXED_REGIONS_ARRAY_SIZE];
    fixed_vector<region *, 7> *extended_regions;
    collision_geometry *colgeom;
    time_interface *field_58;
    int field_5C;
    int field_60;
    int field_64;

    entity() = default;

    //0x004F9180
    entity(const string_hash &a2, uint32_t a3);

    bool is_a_car() const {
        return this->is_flagged(0x800u);
    }

    static void destroy_static_entity_pointers();

    bool is_in_limbo() const;

    void randomize_position(const vector3d &a2, Float a3, Float a4, Float a5);

    void update_proximity_maps();

    ////// VIRTUAL FUNCTIONS: START //////

    ~entity();

    //0x004C0780
    float get_visual_radius();

    //0x004E1BC0
    /* virtual */ vector3d get_visual_center();

    //0x004CB9B0
    void un_mash(generic_mash_header *a2, void *a3, generic_mash_data_ptrs *a4);

    void clear_region(region *r, int i_know_what_i_am_doing);

    entity *compute_sector(terrain *, bool, entity *);

    void force_region_hack(region *a2);

    void force_region(region *a2);

    void force_current_region();

    //0x004CB7F0
    void unforce_regions();

    void force_regions(entity *ent);

    void update_ai_proximity_map_recursive();

    void set_family_visible(bool a2);

    //0x004C0850
    bool is_renderable() const;

    bool possibly_collide();

    bool possibly_camera_collide();

    bool possibly_walkable() const;

    bool sub_4C08E0();

    //0x004C0810
    void region_update_poss_collide();

    void frame_advance(Float);

    bool is_still_visible();

    void render(Float);

    nglMesh *get_mesh();

    bool has_mesh();

    void suspend(bool);

    void unsuspend(bool);

    //virtual
    void set_render_color(color32);

    color32 get_render_color() const;

    void set_render_alpha_mod(Float);

    float get_render_alpha_mod() const;

    void set_render_scale(const vector3d &);

    vector3d get_render_scale() const;

    void set_render_zbias(Float);

    float get_render_zbias() const;

    light_manager *get_light_set() const;

    bool can_be_a_lego();

    bool can_be_a_conglom_clone();

    bool is_a_dynamic_conglomerate_clone();

    void set_collisions_active(bool, bool);

    bool are_character_collisions_active();

    void set_character_collisions_active(bool a2);

    void set_terrain_collisions_active(bool a2);

    float get_age();

    void set_age(Float a2);
	

    void set_recursive_age(Float a2);

    ////// VIRTUAL FUNCTIONS: END //////

    bool is_visible2() const {
        return (this->field_4 & 0x200) != 0;
    }

    //0x004CB5C0
    bool is_in_region(const region *a2) const;

    //0x004F52C0
    void add_me_to_region(region *r);

    collision_geometry *get_colgeom() const;

    /* virtual */ float get_colgeom_radius() const;

    //virtual
    vector3d get_colgeom_center() const;

    //0x004F5390
    void remove_me_from_region(region *r);

    //0x004DBA10
    void create_time_ifc();

    //0x004F5510
    region *update_regions(region **a2, int a3);

    //0x004CB750
    void remove_from_regions();

    //0x004CB670
    bool is_indoors();

    //0x004C0760
    region *get_primary_region() const;

    //0x004C0970
    bool match_search_flags(int a2);

    //0x004D67D0
    static int find_entities(int a1);

    static inline int & visit_key = var<int>(0x0095A6E4);

    static inline int & visit_key2 = var<int>(0x0095A6E8);

    static inline int & visit_key3 = var<int>(0x0095A6EC);

    static inline _std::list<entity *> *& found_entities = var<_std::list<entity *> *>(0x0095A6E0);
};

extern void entity_patch();



