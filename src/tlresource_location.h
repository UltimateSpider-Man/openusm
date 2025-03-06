#pragma once

#include "string_hash.h"

#include <cstdint>

struct generic_mash_header;
struct generic_mash_data_ptrs;

enum tlresource_type
{
    TLRESOURCE_TYPE_NONE = 0,
    TLRESOURCE_TYPE_TEXTURE = 1,
    TLRESOURCE_TYPE_MESH_FILE = 2,
    TLRESOURCE_TYPE_MESH = 3,
    TLRESOURCE_TYPE_MORPH_FILE = 4,
    TLRESOURCE_TYPE_MORPH = 5,
    TLRESOURCE_TYPE_MATERIAL_FILE = 6,
    TLRESOURCE_TYPE_MATERIAL = 7,
    TLRESOURCE_TYPE_ANIM_FILE = 8,
    TLRESOURCE_TYPE_ANIM = 9,
    TLRESOURCE_TYPE_SCENE_ANIM = 10,
    TLRESOURCE_TYPE_SKELETON = 11,
    TLRESOURCE_TYPE_Z = 12
};

struct tlresource_location {
    string_hash name;
    uint32_t m_type;
    char *field_8;

    uint32_t get_size();

    tlresource_type get_type() const;

    //0x0050EBE0
    void un_mash(generic_mash_header *, void *, generic_mash_data_ptrs *);
};

#include <map>

struct Mod {
    tlresource_type Type;
    std::vector<uint8_t> Data;
};
static std::map<uint32_t, Mod> Mods;

[[maybe_unused]] static bool hasMod(uint32_t hash) {
    return Mods.find(hash) != Mods.end();
}

[[maybe_unused]] static Mod* getMod(uint32_t hash) {
    auto it = Mods.find(hash);
    if (it != Mods.end())
        return &it->second;
    return nullptr;
}
