#pragma once

#include "ai_param_types.h"
#include "float.hpp"
#include "mash.h"
#include "mvector.h"
#include "string_hash.h"
#include "variance_variable.h"
#include "vector3d.h"
#include "entity_base.h"
#include "entity.h"
#include "entity_base_vhandle.h"

#include <variant>

struct entity_base_vhandle;

namespace ai {

struct param_block {

    struct param_data {
    public:
        union U {
            int i;
            float f;
            string_hash hash;
            char *str;
            vector3d *vec3;
            variance_variable<float> *float_variance;
            entity_base_vhandle *ent;
            void *ptr;
        } m_union = {};
        param_types my_type;
        string_hash m_name;

    public:

        param_data();

        ~param_data();

        void operator delete(void *ptr, size_t size);

        void initialize(mash::allocation_scope a2);

        void finalize(mash::allocation_scope );

        void destruct_mashed_class();

        string_hash get_name() const {
            return m_name;
        }

        void custom_unmash(mash_info_struct *a2, void *a3);

        void unmash(mash_info_struct *a1, void *a3);

        int get_data_int();

        float get_data_float() const;

        //0x006BD200
        string_hash get_data_hash() const;

        const char * get_data_fixedstring() const;

        vector3d * get_data_vector3d() const;

        variance_variable<float> * get_data_float_variance() const;

        void * get_data_pointer() const;

        int get_data_type() const {
            return this->my_type;
        }
        const char* get_data_string()
        {
            assert(my_type == PT_FIXED_STRING);

            return this->m_union.str;
        }

        entity_base_vhandle* get_data_entity()
        {
            assert(my_type == PT_ENTITY);

            return this->m_union.ent;
        }

        mString get_value_in_string_form()
        {
            mString result{};
            switch (this->my_type)
            {
            case PT_FLOAT: {
                auto data_float = this->get_data_float();
                result = mString{ 0, "%.1f", data_float };
                break;
            }
            case PT_INTEGER: {
                auto data_int = this->get_data_int();
                result = mString{ 0, "%d", data_int };
                break;
            }
            case PT_STRING_HASH: {
                auto v22 = this->get_data_hash();
                auto* v4 = v22.to_string();
                result = mString{ v4 };
                break;
            }
            case PT_FIXED_STRING: {
                auto* v5 = this->get_data_fixedstring();
                result = mString{ v5 };
                break;
            }
            case PT_VECTOR_3D: {
                auto v17 = this->get_data_vector3d()->z;
                auto v16 = this->get_data_vector3d()->y;
                auto v6 = this->get_data_vector3d()->x;
                result = mString{ 0, "%.1f %.1f %.1f", v6, v16, v17 };
                break;
            }
            case PT_FLOAT_VARIANCE: {
                auto v7 = *this->get_data_float_variance();
                auto v18 = v7.field_4;
                auto v9 = v7.field_0;
                result = mString{ 0, "%.1f %.1f", v9, v18 };
                break;
            }
            case PT_ENTITY: {
                mString v14{};
                auto* v10 = this->get_data_entity();
                if (v10->get_volatile_ptr() != nullptr)
                {
                    auto* ent = v10->get_volatile_ptr();
                    auto id = ent->get_id();
                    auto* v27 = id.to_string();
                    v14 = mString{ 0, "%s", v27 };
                }
                else
                {
                    v14 = mString{ 0, "%s", "<NULL>" };
                }

                result = v14;
                break;
            }
            case PT_POINTER: {
                result = mString{ "<NULL>" };
                break;
            }
            default:
                assert(0 && "Unhandled AI::param_block::param_data::param_type type!");
                return result;
            }

            return result;
        }


        void set_data_float(float a2);

        //0x006C86C0
        void set_data_float_variance(const variance_variable<float> &a2);

        void set_data_vector3d(const vector3d &a2);

        void set_data_entity(entity_base_vhandle &a2);

        void set_data_fixedstring(char *);

        void set_data_pointer(void *a2);

        bool deep_compare(const param_block::param_data *a2) const;
    };

    struct param_data_array {
        mVector<param_block::param_data> field_0;
        int field_14;

        ~param_data_array();

        void destruct_mashed_class();

        void finalize(mash::allocation_scope )
        {
            this->field_14 = 0;
        }

        //0x006CD450
        param_data *common_find_data(string_hash a2);

        void unmash(mash_info_struct *, void *);
    };

    int field_0;
    param_data_array *param_array;
    char field_8;
    char pad[3];

    param_block();

    ~param_block();

    void finalize(mash::allocation_scope a3);

    //0x006D56B0
    void unmash(mash_info_struct *a1, void *a3);

    //0x006D6710
    void add_param(
        string_hash a2,
        param_types a3,
        const void *a4,
        string_hash a5);

    //0x006CE130
    bool does_parameter_exist(string_hash a2) const;

    bool does_parameter_match(const param_block::param_data *a2) const;

    void destruct_mashed_class();

    int get_parameter_data_type(string_hash a2) const;

    //0x006CDCE0
    string_hash get_pb_hash(string_hash a3) const;

    //0x006CDDF0
    variance_variable<float> *get_pb_float_variance(string_hash a2);

    //0x006CDD80
    vector3d *get_pb_vector3d(string_hash a2);

    //0x006CDD40
    const char *get_pb_fixedstring(string_hash a2) const;

    void set_pb_float(string_hash a2, Float a3, bool a4);

    //0x006CDC60
    float get_pb_float(string_hash a2);

    //0x006CDCA0
    int get_pb_int(string_hash a2);

    //0x006CDF50
    int get_optional_pb_int(string_hash a2, const int &a3, bool *a4) const;

    //0x006CDF10
    float get_optional_pb_float(string_hash a2, const float &a3, bool *a4) const;
};
} // namespace ai

extern void param_block_patch();
