#include "nal_anim_controller.h"

#include "actor.h"
#include "als_nal_meta_anim.h"
#include "als_meta_anim_base.h"
#include "als_animation_logic_system.h"
#include "base_state_machine.h"
#include "common.h"
#include "conglom.h"
#include "func_wrapper.h"
#include "memory.h"
#include "nal_instance.h"
#include "nal_skeleton.h"
#include "nal_system.h"
#include "oldmath_po.h"
#include "osassert.h"
#include "trace.h"
#include "traffic.h"
#include "quaternion.h"
#include "variables.h"
#include "vtbl.h"

#include <cassert>

VALIDATE_SIZE(nal_anim_controller, 0x54);

nalAnimClass<nalAnyPose>::nalInstanceClass *nal_anim_controller::scene_anim_client::CreateInstance(nalAnimClass<nalAnyPose> *a2)
{
    TRACE("nal_anim_controller::scene_anim_client::CreateInstance");

    if constexpr (1)
    {
        auto *v2 = this->field_4;
        auto *Skeleton = a2->GetSkeleton();
        auto v9 = Skeleton->GetAnimTypeName();

        if ( !v2->is_same_animtype(v9) )
        {
            auto v4 = this->field_4->field_8->GetAnimTypeName();
            auto *char_anim_type = v4.to_string();
            auto Name = this->field_4->field_8->GetName();
            auto *char_skel_name = Name.to_string();
            auto *v6 = a2->GetSkeleton();
            auto v7 = v6->GetAnimTypeName();
            auto *anim_type = v7.to_string();
            auto &v8 = a2->field_8;
            auto *anim_name = v8.to_string();
            error("Attempted to play an animation from a cutscene (anim name: \"%s\") of animtype \"%s\" on a character skeleton \"%s"
            "\" of animtype \"%s\". They are not compatible. Please have this scene animation altered to use the correct character's skeleton.",
            anim_name,
            anim_type,
            char_skel_name,
            char_anim_type);
        }

        this->field_8 = 0;
        return static_cast<nalAnimClass<nalAnyPose>::nalInstanceClass *>(a2->CreateInstance(v2->field_8));
    }
    else
    {
        return (nalAnimClass<nalAnyPose>::nalInstanceClass *) THISCALL(0x00492890, this, a2);
    }
}

int nal_anim_controller::scene_anim_client::Advance(
    nalAnimClass<nalAnyPose>::nalInstanceClass *a2, Float a3, Float a4, Float a5, Float a6)
{
    if constexpr (0)
    {}
    else
    {
        return THISCALL(0x0049C090, this, a2, a3, a4, a5, a6);
    }
}

void nal_anim_controller::scene_anim_client::Render(
        nalAnimClass<nalAnyPose>::nalInstanceClass *,
        Float )
{
    TRACE("nal_anim_controller::scene_anim_client::Render");

    byte_959561() = true;
    this->field_4->get_matrix_data_from_pose(this->field_4->field_40);
    byte_959561() = false;
}

nal_anim_controller::nal_anim_controller(actor *a2,
                                         nalBaseSkeleton *a3,
                                         unsigned int a4,
                                         const als::als_meta_anim_table_shared *a5) {
    THISCALL(0x0049BCF0, this, a2, a3, a4, a5);
}

void * nal_anim_controller::operator new(size_t size)
{
    return mem_alloc(size);
}

double nal_anim_controller::_get_base_anim_speed()
{
    TRACE("nal_anim_controller::get_base_anim_speed");

    auto *v1 = this->my_player.field_14[0];
    if ( v1 != nullptr ) {
        return v1->field_4;
    }

    return 0.0;
}

void nal_anim_controller::_set_base_anim_time_in_sec(Float a2)
{
    TRACE("nal_anim_controller::set_base_anim_time_in_sec");

    auto *v2 = this->my_player.field_14[0];
    if ( v2 != nullptr )
    {
        this->my_player.field_14[0]->field_18 = a2 * v2->field_0->field_8;
        this->my_player.field_14[0]->field_1C = this->my_player.field_14[0]->field_18;
    }
}

double nal_anim_controller::_get_anim_speed(Float priority)
{
    TRACE("nal_anim_controller::get_anim_speed");

    assert(my_player.IsAnimActive(priority) && "Cannot get speed of an inactive animation.");

    auto *v2 = this->my_player.Advance(priority);
    if ( v2 != nullptr ) {
        return v2->field_4;
    }

    return 0.0;
}

void nal_anim_controller::_set_base_anim_speed(Float speed)
{
    TRACE("nal_anim_controller::set_base_anim_speed");

    assert(speed >= 0.0f && "Animation speed must be 0 or more. Negative speeds are not supported.");

    auto *v2 = this->my_player.field_14[0];
    if ( v2 != nullptr ) {
        v2->field_4 = speed;
    }
}

void nal_anim_controller::_frame_advance(Float a2, bool a3, bool a4)
{
    TRACE("nal_anim_controller::frame_advance");

    if constexpr (1) {
        if ( !traffic::is_unanimated_car(this->field_4) 
            && !this->field_4->is_flagged(0x40000000u)
            && !this->field_50 )
        {
            this->my_player.sub_4B06A0(a2);
            this->my_player.sub_4B0860(this->field_40);
            if ( this->field_4->is_visible() && !a3 )
            {
                static tlFixedString stru_959A24 {"green_goblin"};

                if ( this->field_8->field_8 == stru_959A24 ) {
                    byte_959561() = true;
                }

                this->get_matrix_data_from_pose(this->field_40);
                byte_959561() = false;
            }
        }
    } else {
        void (__fastcall *func)(void *, void *, Float, bool, bool) = CAST(func, 0x004A6110);
        func(this, nullptr, a2, a3, a4);
    }
}

bool nal_anim_controller::scene_animation_playing() const
{
    return this->field_50;
}

bool nal_anim_controller::is_anim_active(Float a1)
{
    TRACE("nal_anim_controller::is_anim_active");

    return this->my_player.Advance(a1) != nullptr;
}

double nal_anim_controller::_get_base_anim_time_in_sec() const
{
    TRACE("nal_anim_controller::get_base_anim_time_in_sec");

    if constexpr (0) {
        auto *v3 = this->my_player.field_14[0];
        auto *v4 = (v3 != nullptr ? v3->field_0->field_10 : nullptr);
        if ( (v4->field_34 & 1) != 0 )
        {
            auto *v5 = this->my_player.field_14[0];
            double v6 = (v5 != nullptr ? v5->field_18 : 0.0);
            auto *v7 = this->my_player.field_14[0];
            auto v8 = v6 - static_cast<int>(v6);
            if ( v7 != nullptr ) {
                return v8 * v7->field_0->field_10->field_38;
            } else {
                assert(0);
                //return v8 * MEMORY[0x38];
            }
        }

        auto result = this->get_total_base_anim_time_in_sec();
        auto *v9 = this->my_player.field_14[0];
        nalAnimClass<nalAnyPose> *v10 = (v9 != nullptr ? v9->field_0->field_10 : nullptr);
        if ( result > v10->field_38 )
        {
            auto *v11 = this->my_player.field_14[0];
            if ( v11 != nullptr ) {
                return v11->field_0->field_10->field_38;
            } else {
                assert(0);
                //return MEMORY[0x38];
            }
        }

        return result;
    } else {
        double (__fastcall *func)(const void *) = CAST(func, 0x00497E50);
        return func(this);
    }
}


void nal_anim_controller::get_curr_po_offset(po &a2)
{
    THISCALL(0x00497FC0, this, &a2);
}

void nal_anim_controller::get_matrix_data_from_pose(nalAnyPose &arg0)
{
    TRACE("nal_anim_controller::get_matrix_data_from_pose");

    {
        auto *skel = *bit_cast<nalComp::nalCompSkeleton **>(this->field_40.field_0);
        //sp_log("0x%08X", skel->m_vtbl);
    }

    if constexpr (0) {
        auto *v3 = (conglomerate *)this->field_4;
        if ( v3->is_a_conglomerate() )
        {
            auto *skel = this->field_40.field_0->field_0;
            struct {
                char field_0[0x18];
                void (__fastcall *VirtualGetBoneMatrices)(void *, void *, const nalBasePose *, nalMatrix4x4 *);
            } * vtbl = CAST(vtbl, skel->m_vtbl);
            vtbl->VirtualGetBoneMatrices(skel, nullptr,
                this->field_40.field_0,
                bit_cast<nalMatrix4x4 *>(v3->all_model_po.m_data));

            v3->sub_4D0E00();
            if ( this->scene_animation_playing() )
            {
                auto new_parent = *v3->all_model_po.m_data;
                assert(new_parent.get_z_facing().is_normal() && "NAL Pelvis Matrix is bad.");

                this->field_4->set_abs_po(new_parent);
                auto *child = v3->get_first_child();
                auto v32 = new_parent.inverse();
                for ( ; child != nullptr; child = child->field_28 )
                {
                    if ( child->has_model_po() )
                    {
                        po new_po {};
                        auto *v6 = child->get_model_po();

                        const void * a2[2] {
                            v32,
                            v6
                        };

                        new_po.m.sub_415A30(a2);

                        child->set_abs_po(new_po);

                        child->get_abs_po();
                    }
                }

                v3->get_abs_po();
                v3->get_rel_po();
            }
            else if ( v3->get_my_als() == nullptr )
            {
                vector3d a2_12 = v3->get_abs_position();
                po a2_28 {};
                this->get_curr_po_offset(a2_28);

                ptr_to_po a2;
                a2.m_abs_po = &v3->get_rel_po();
                a2.m_rel_po = &a2_28;
                a2_28.m.sub_415A30(&a2);
                a2_28.sub_48D840();
                this->field_4->set_abs_po(a2_28);

                vector3d v17 = v3->get_abs_position() - a2_12;
                this->field_4->set_frame_delta_trans(v17, 0.033333302);
            }
        }
        else
        {
            nalPositionOrientation v30 {};
            auto *skel = arg0.field_0->field_0;
            struct {
                char field_0[0x1C];
                void (__fastcall *VirtualGetTrajectoryUpdate)(void *, void *, const nalBasePose *, nalPositionOrientation *);
            } * vtbl = CAST(vtbl, skel->m_vtbl);
            vtbl->VirtualGetTrajectoryUpdate(skel, nullptr,
                arg0.field_0,
                &v30);
            if ( this->field_50
                || ((static_cast<nalGeneric::nalGenericAnim *>(this->get_base_layer_anim_ptr())->field_34 & 2) != 0) )
            {
                quaternion a2_12 {v30.arr[3], v30.arr[0], v30.arr[1], v30.arr[2]};
                vector3d v31 {};
                po v32 {v31, a2_12, 1.0};

                this->field_4->set_abs_po(v32);
            }
            else
            {
                vector3d a2_12 = this->field_4->get_abs_position();
                po a2_28 {};
                this->get_curr_po_offset(a2_28);
                auto *my_rel_po = &this->field_4->get_rel_po();

                ptr_to_po a2;
                a2.m_rel_po = &a2_28;
                a2.m_abs_po = my_rel_po;
                a2_28.m.sub_415A30(&a2);
                a2_28.sub_48D840();
                this->field_4->set_abs_po(a2_28);
                vector3d v17 = this->field_4->get_abs_position() - a2_12;
                this->field_4->set_frame_delta_trans(v17, 0.033);
            }
        }
    } else {
        THISCALL(0x004A8A60, this, &arg0);
    }
}

double nal_anim_controller::get_anim_time_in_sec(Float a2)
{
    TRACE("nal_anim_controller::get_anim_time_in_sec");

    if constexpr (1) {
        double (__fastcall *func)(void *, void *, Float) = CAST(func, get_vfunc(m_vtbl, 0x34));
        return func(this, nullptr, a2);
    } else {
        double (__fastcall *func)(void *, void *, Float) = CAST(func, 0x0049BE60);
        return func(this, nullptr, a2);
    }
}

double nal_anim_controller::get_total_base_anim_time_in_sec() const
{
    auto *v1 = this->my_player.field_14[0];
    auto v2 = (v1 != nullptr ? v1->field_18 : 0.0);

    auto *v3 = this->my_player.field_14[0];
    if ( v3 != nullptr ) {
        return v2 * v3->field_0->field_10->field_38;
    } else {
        assert(0);
        //return v2 * MEMORY[0x38];
    }
}

void *nal_anim_controller::get_base_layer_anim_ptr()
{
    TRACE("nal_anim_controller::get_base_layer_anim_ptr");

    int *v1 = *bit_cast<int **>(this->my_player.field_14);
    if ( v1 != nullptr ) {
        return (void *)v1[4];
    } else {
        return nullptr;
    }
}

void *nal_anim_controller::std_play_method::CreateInstance(
        nalAnimClass<nalAnyPose> *a1,
        nalBaseSkeleton *a2,
        void *pParameter)
{
    TRACE("nal_anim_controller::std_play_method::CreateInstance");

    if constexpr (0) {
        if ( nalAnimPtrCast<als::als_nal_meta_anim>(a1) != nullptr ) {
            return a1->VirtualCreateInstance(a2);
        }

        assert(pParameter != nullptr && "Must have a non-null parameter for meta anim playing.");

        auto *v4 = (conglomerate *)*(bit_cast<DWORD *>(pParameter) + 1);
        als::animation_logic_system *my_als = nullptr;
        als::base_state_machine *als_layer_internal = nullptr;
        if ( v4->is_a_conglomerate() )
        {
            if ( v4->get_my_als() != nullptr )
            {
                auto *my_als = v4->get_my_als();
                als_layer_internal = my_als->get_als_layer_internal(*static_cast<als::layer_types *>(pParameter));
            }
        }

        auto *v8 = nalAnimPtrCast<als::als_nal_meta_anim>(a1);

        void * (__fastcall *func)(void *, void *,
                    nalBaseSkeleton *,
                    als::als_nal_meta_anim *,
                    als::animation_logic_system *,
                    als::state_machine *) = CAST(func, get_vfunc(v8->field_40->m_vtbl, 0x2C));
        return func(v8->field_40, nullptr,
                    a2,
                    v8,
                    my_als,
                    als_layer_internal);

        /*
        return v8->create_anim_inst(
                                   a2,
                                   my_als,
                                   als_layer_internal);
        */
    } else {
        return (void *) THISCALL(0x004A6050, this, a1, a2, pParameter);
    }

}

void nal_anim_controller_patch()
{
    {
        FUNC_ADDRESS(address, &nal_anim_controller::scene_anim_client::CreateInstance);
        set_vfunc(0x00880B04, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::scene_anim_client::Render);
        set_vfunc(0x00880B0C, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::get_base_layer_anim_ptr);
        SET_JUMP(0x00497FB0, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::is_anim_active);
        //SET_JUMP(0x0049BE40, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::std_play_method::CreateInstance);
        set_vfunc(0x00880B90, address);
        set_vfunc(0x00880BA8, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::_get_base_anim_time_in_sec);
        set_vfunc(0x00880D88, address);
        set_vfunc(0x00880F18, address);
        set_vfunc(0x00880FC0, address);
        set_vfunc(0x00881068, address);
        set_vfunc(0x00881110, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::_get_base_anim_speed);
        //SET_JUMP(0x00497F70, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::_set_base_anim_time_in_sec);
        SET_JUMP(0x00497F40, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::_get_anim_speed);
        SET_JUMP(0x0049BFE0, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::_set_base_anim_speed);
        SET_JUMP(0x00497F90, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::_frame_advance);
        set_vfunc(0x00880DC8, address);
        set_vfunc(0x00880F58, address);
        set_vfunc(0x00881000, address);
        set_vfunc(0x008810A8, address);
        set_vfunc(0x00881150, address);
    }

    {
        FUNC_ADDRESS(address, &nal_anim_controller::get_anim_time_in_sec);
        set_vfunc(0x00880D8C, address);
        set_vfunc(0x00880F1C, address);
        set_vfunc(0x00880FC4, address);
        set_vfunc(0x0088106C, address);
        set_vfunc(0x00881114, address);
    }
}
