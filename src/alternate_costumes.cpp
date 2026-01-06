#include "alternate_costumes.h"

#include "camera.h"
#include "comic_panels.h"
#include "common.h"
#include "cursor.h"
#include "cut_scene_player.h"
#include "entity.h"
#include "fe_menu_nav_bar.h"
#include "femenusystem.h"
#include "fileusm.h"
#include "func_wrapper.h"
#include "game.h"
#include "marky_camera.h"
#include "mission_stack_manager.h"
#include "oldmath_po.h"
#include "panelfile.h"
#include "resource_manager.h"
#include "script_manager.h"
#include "us_lighting.h"
#include "utility.h"
#include "variables.h"
#include "wds.h"

#include "comic_panels.h"

VALIDATE_SIZE(ptr_PanelQuad, 12u);
VALIDATE_SIZE(ptr1_PanelQuad, 8u);

VALIDATE_SIZE(alternate_costumes, 0x16C);
VALIDATE_OFFSET(alternate_costumes, field_C4, 0xC4);

static Var<char *[1]> off_937798 { 0x00937798 };

alternate_costumes::alternate_costumes(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x00625D70, this, a2, a3, a4);
}

void alternate_costumes::Init() {
    THISCALL(0x00646790, this);
}

void alternate_costumes::sub_640BC0(int a2) {
    THISCALL(0x00640BC0, this, a2);
}

void alternate_costumes::set_info() {
    THISCALL(0x00614B60, this);
}

void alternate_costumes::sub_640740(int idx) {
    if constexpr (1) {
        if (this->field_C4[idx].field_8) {
            mString a3{off_937798()[idx]};

            auto *v3 = g_cut_scene_player();
            auto *v4 = v3->field_7C;
            if (v4 != nullptr) {
                ::operator delete(v4);
            }

            v3->field_7C = nullptr;
            v3->field_80 = 0;
            v3->field_84 = 0;
            g_world_ptr->ent_mgr.release_entity(this->field_150);
            mString a1 = "ch_vwr_" + a3;

            if (mission_stack_manager::s_inst->is_pack_pushed(a1)) {
                mission_stack_manager::s_inst->pop_mission_pack_immediate(a1, a1);
            }
        }
    } else {
        THISCALL(0x00640740, this, idx);
    }
}

void alternate_costumes::sub_640510(int idx) {
    THISCALL(0x00640510, this, idx);
}

bool alternate_costumes::sub_614AC0(int a1) {
    mString v4{"gv_unlockable_costumes"};

    auto *v2 = (float *) script_manager::get_game_var_address(v4, nullptr, nullptr);

    return (v2[a1] == 1.0f);
}




void alternate_costumes::onActivate() {
    sp_log("alternate_costumes::onActivate:");

    if constexpr (0) {
        mString v32{"unlockables_chars_costumes"};

        mission_stack_manager::s_inst->push_mission_pack_immediate(v32, v32);
        auto *v2 = resource_manager::get_best_context(RESOURCE_PARTITION_MISSION);
        resource_manager::push_resource_context(v2);

        if constexpr (0) {
            //auto &Init = get_vfunc(this->m_vtbl, 0x0);

            //Init(this);
        } else {
            this->Init();
        }

        resource_manager::pop_resource_context();
        auto *v3 = this->field_34;

        v3->field_4 = "";

        v3->field_28 = 0;
        this->field_34->AddButtons(menu_nav_bar::button_type{15},
                                   menu_nav_bar::button_type{17},
                                   static_cast<global_text_enum>(3));
        auto *v4 = this->field_34;
        auto *v5 = v4->field_4.c_str();

        {
            mString v23{v5};
            auto str = *bit_cast<FEText::string *>(&v23);

            v4->text_box->SetTextNoLocalize(str);
        }

        {
            auto *v6 = get_msg(g_fileUSM(), "RESUME");

            mString v23{v6};
            auto str = *bit_cast<FEText::string *>(&v23);
            v4->text_box->SetTextNoLocalize(str);
        }

        auto *v7 = g_world_ptr->get_hero_ptr(0);
        if (v7 != nullptr) {
            this->field_158 = v7->get_abs_position();
        } else {
            auto *v8 = g_game_ptr->get_current_view_camera(0);

            this->field_158 = v8->get_abs_position();
        }

        this->field_158[1] = 350.0;
        auto *panel = comic_panels::acquire_panel("Viewer");

        this->field_14C = panel;
        panel->add_camera_component("MARKY_CAM", true, 2);
        this->field_14C->field_4C = g_cut_scene_player();

        float v24, v25;
        {
            auto *vtbl = bit_cast<fastcall_call(*)[1]>(this->field_4C[20]->m_vtbl);
            void (__fastcall *GetCenterPos)(void *, void*, float *, float *) = CAST(GetCenterPos, (*vtbl)[42]);
            GetCenterPos(this->field_4C[20], nullptr, &v25, &v24);
        };

        auto *v11 = this->field_4C[20];

        auto v12 = v11->GetMax();
        auto v13 = v11->GetMin();

        v12[0] = v13[0] - v12[0];
        v12[1] = v13[1] - v12[1];
        auto a2 = v12;

        auto *v16 = this->field_14C;
        v16->set_size(a2);

        vector3d v31;
        v31[0] = v25;
        v31[1] = 480.0 - v24;
        v31[2] = 10.0;
        v16->set_loc(v31);
        v16->field_68 = true;

        if (!g_world_ptr->field_28.field_48)
        {
            g_game_ptr->enable_marky_cam(true, true, -1000.0, 0.0);
            auto *v18 = g_world_ptr->field_28.field_44;
            auto *vtbl = bit_cast<fastcall_call(*)[1]>(v18->m_vtbl);
            void (__fastcall *sync)(void *, void *, void *) = CAST(sync, (*vtbl)[165]);

            auto *v20 = g_game_ptr->get_current_view_camera(0);
            sync(v18, nullptr, v20);
        }

        this->sub_640510(this->field_114);
        this->field_154 = 0.0;
        this->field_168 = g_TOD();
        us_lighting_switch_time_of_day(0);

        bit_cast<uint8_t *>(&this->field_28)[0] |= 0x80u;

        this->field_2A = -1;
        g_cursor()->sub_5A6790();
        g_cursor()->sub_5A67D0(275, 420, 355, 445);
        auto v21 = 40;
        auto v22 = 6;
        do {
            g_cursor()->sub_5A67D0(50, v21, 98, v21 + 50);
            v21 += 54;
            --v22;
        } while (v22);

    } else {
        THISCALL(0x00642340, this);
    }
}

void alternate_costumes::OnDeactivate(FEMenu *a2) {
    sp_log("alternate_costumes::OnDeactivate:");
    THISCALL(0x00614A30, this, a2);
}

void alternate_costumes::OnTriangle(int a2) {
    sp_log("alternate_costumes::OnTriangle:");
    THISCALL(0x00640BC0, this, a2);
}



void alternate_costumes::sub_614C60(int a2) {
    for (int i = 0; i < 6; ++i) {
        auto& entry = this->field_11C[i];
        int field_4 = entry.field_4;
        
        if (field_4 == 0) {
            field_4 = (a2 >= 0) ? 1 : 2;
        }
        else if (field_4 == 1) {
            field_4 = (a2 >= 0) ? 0 : 2;
        }
        else if (field_4 == 2) {
            field_4 = (a2 >= 0) ? 1 : 0;
        }
        
        entry.field_4 = field_4;
        
        nglTexture* texture;
        if (field_4 == 0) {
            texture = this->field_A8;
        }
        else if (field_4 == 1) {
            texture = this->field_B0;
        }
        else {
            texture = this->field_B8;
        }
        
        entry.field_0->SetTexture(texture);
		
		THISCALL(0x00614C60, this, a2);
    }
}
void alternate_costumes::update_selected(int a2)
{
    if constexpr (0)
	{
        for (auto &p : this->field_C4) {
            auto v7 = (float) (a2 * this->field_10C);

            auto vtbl = bit_cast<fastcall_call(*)[1]>(p.field_0->m_vtbl);
            void (__fastcall *field_C0)(void *, void *, Float, Float) = CAST(field_C0, (*vtbl)[48]);

            field_C0(p.field_0, nullptr, 0.0, v7);
        }

        int v6;

        auto v5 = this->field_114;
        if (a2 >= 0) {
            v6 = (v5 + 5) % 6;
        } else {
            v6 = (v5 + 4) % 6;
        }

        auto v8 = (float) -(a2 * this->field_110);

        auto *panel = this->field_C4[v6].field_0;

        auto vtbl = bit_cast<fastcall_call(*)[1]>(panel->m_vtbl);
        void (__fastcall *field_C0)(void *, void *, Float, Float) = CAST(field_C0, (*vtbl)[48]);
        field_C0(panel, nullptr, 0.0, v8);

        this->sub_640740(this->field_118);
        this->sub_640510(this->field_114);
        this->sub_614C60(a2);
        this->set_info();
        this->field_118 = this->field_114;
    } else {
        THISCALL(0x00640840, this, a2);
    }
}

void alternate_costumes::OnUp(int a2) {
    sp_log("alternate_costumes::OnUp:");
    THISCALL(0x00640950, this, a2);
}

void alternate_costumes::OnDown(int a2) {
    sp_log("alternate_costumes::OnDown:");
    THISCALL(0x00640A00, this, a2);
}

void alternate_costumes_patch() {
    {
        FUNC_ADDRESS(address, &alternate_costumes::update_selected);
        REDIRECT(0x0064097A, address);
        REDIRECT(0x00640A34, address);
    }

    {
        FUNC_ADDRESS(address, &alternate_costumes::sub_640740);
        REDIRECT(0x0064090A, address);
    }
	
    {
	FUNC_ADDRESS(address, &alternate_costumes::sub_614C60);
	REDIRECT(0x00640920, address);
    }

    {
        FUNC_ADDRESS(address, &alternate_costumes::sub_640510);
        REDIRECT(0x006425BD, address);
        REDIRECT(0x00640918, address);
    }

    {
        FUNC_ADDRESS(address, &alternate_costumes::sub_640BC0);
        set_vfunc(0x00894B78, address);
    }

    auto addr = 0x00894B6C;

    {
        FUNC_ADDRESS(address, &alternate_costumes::onActivate);
        set_vfunc(addr, address);
    }

    addr += 0x4;

    {
        FUNC_ADDRESS(address, &alternate_costumes::OnDeactivate);
        set_vfunc(addr, address);
    }

    addr += 0x8;

    {
        FUNC_ADDRESS(address, &alternate_costumes::OnTriangle);
        set_vfunc(addr, address);
    }

    addr += 0x4;

    {
        FUNC_ADDRESS(address, &alternate_costumes::OnUp);
        set_vfunc(addr, address);
    }

    addr += 0x4;

    {
        FUNC_ADDRESS(address, &alternate_costumes::OnDown);
        set_vfunc(addr, address);
    }
	
}
