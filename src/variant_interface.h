#pragma once

#include "generic_interface.h"
#include "mashable_vector.h"
#include "string_hash.h"

struct conglomerate;
struct variant_info
{
    string_hash hash;
    unsigned __int16 field_4;
    unsigned __int16 field_6;
    unsigned __int8* parts;
    char* ifl_frames;
};

struct nglMeshFile;
struct nglMesh;
struct nglMorphSet;
struct variant_speaker_id_set;

struct variant_interface : generic_interface {
    conglomerate *field_4;
    bool field_8;
    mashable_vector<variant_info> variants;
    mashable_vector<variant_speaker_id_set> field_14;
    variant_info *field_1C;
    nglMesh *field_20;
    nglMorphSet *field_24;
    nglMeshFile *field_28;
    int field_2C;
    int field_30;
    int field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
    int field_50;
    int field_54;

    variant_interface(conglomerate *);

    variant_info *get_random_variant();

    void apply_variant(string_hash a2);

    void apply_variant(variant_info *info);

};

extern void variant_interface_patch();
