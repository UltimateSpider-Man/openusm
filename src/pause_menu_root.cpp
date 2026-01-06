#include "pause_menu_root.h"

#include "pause_menu_goals.h"
#include "comic_panels.h"
#include "common.h"
#include "entity_base.h"
#include "fe_health_widget.h"
#include "fetext.h"
#include "femanager.h"
#include "femenusystem.h"
#include "igofrontend.h"
#include "mstring.h"
#include "panelquad.h"
#include "pausemenusystem.h"
#include "panelfile.h"
#include "utility.h"
#include "trace.h"
#include "vtbl.h"
#include "wds.h"
#include "movie_manager.h"
#include "main.h"

#include "game.h"

#include "FileUSM.h"

#include "fe_menu_nav_bar.h"

#include "femultilinetext.h"
#include "mission_manager.h"

#include "variables.h"

#include "sound_instance_id.h"

#include "cursor.h"

#include "script_object.h"




#include "string_hash.h"


VALIDATE_SIZE(pause_menu_root, 0x100u);

VALIDATE_SIZE(pause_menu_root2, 0x100u);


pause_menu_root::pause_menu_root(FEMenuSystem *a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 8, 0) {
    this->field_AC = a2;

    this->field_A0 = nullptr;
    this->field_B0 = 0;
    this->field_B4 = 0;
    this->field_30 = 0;
    this->field_2C = 0;
    this->field_2D = 0;
}



void pause_menu_root::_Load()
{
    TRACE("pause_menu_root::Load");

    if constexpr (1)
    {
        auto *v2 = bit_cast<PauseMenuSystem *>(this->field_AC)->field_2C;

        this->field_3C[0] = v2->GetPQ("pm_splash_back_01a");
        this->field_3C[1] = v2->GetPQ("pm_splash_back_01b");
        this->field_3C[2] = v2->GetPQ("pm_splash_back_02a");
        this->field_3C[3] = v2->GetPQ("pm_splash_back_02b");
        this->field_3C[4] = v2->GetPQ("pm_splash_back_03a");
        this->field_3C[5] = v2->GetPQ("pm_splash_back_03b");
        this->field_3C[6] = v2->GetPQ("pm_splash_back_stub_01");
        this->field_3C[7] = v2->GetPQ("pm_splash_back_stub_02");
        this->field_3C[8] = v2->GetPQ("pm_splash_icon");
		this->field_3C[9] = v2->GetPQ("pm_splash_back_venom");

        this->field_68 = v2->GetPQ("pm_splash_dialog_box_01");
        this->field_6C = v2->GetPQ("pm_splash_dialog_box_02");
        this->field_64 = v2->GetPQ("pm_splash_hilite_text");
        this->field_64 = v2->GetPQ("pm_splash_hilite_text_01");
        this->field_70 = v2->GetPQ("pm_splash_back_04");
		this->field_74 = v2->GetPQ("pm_splash_back_venom");
        this->field_78[0] = v2->GetTextPointer("pm_splash_text_01");
        this->field_78[1] = v2->GetTextPointer("pm_splash_text_02");
        this->field_78[2] = v2->GetTextPointer("pm_splash_text_03");
        this->field_78[3] = v2->GetTextPointer("pm_splash_text_04");
        this->field_78[4] = v2->GetTextPointer("pm_splash_text_05");
        this->field_78[5] = v2->GetTextPointer("pm_splash_text_06");
        this->field_78[6] = v2->GetTextPointer("pm_splash_text_07");
        this->field_78[7] = v2->GetTextPointer("pm_splash_text_08");
        this->field_78[8] = v2->GetTextPointer("pm_splash_text_09");
		this->field_78[9] = v2->GetTextPointer("pm_splash_text_10");

        this->field_A0 = v2->GetTextPointer("pm_splash_text_GAMEPAUSED");
        this->field_A0 = v2->GetTextPointer("pm_splash_dialog_box_text_BODY");
        this->field_A4 = v2->GetTextPointer("pm_splash_dialog_box_text_NOWAY");

        this->field_A8 = v2->GetTextPointer("pm_splash_dialog_box_text_OKAY");
        for (auto i = 0u; i < 10u; ++i)
        {
            this->field_3C[i]->TurnOn(true);
        }

        this->field_78[0]->SetShown(true);
        this->field_78[0]->SetNoFlash(color32 {0xFFE6D03F});
        this->field_78[0]->SetScale(1.2, 1.2);
		
		

        for (auto i = 0u; i < 10u; ++i) 
        {
            auto *v6 = this->field_78[i + 1];
            v6->SetShown(true);
            v6->SetNoFlash(color32 {0xFFC87238});
        }

        this->field_68->TurnOn(1);
        this->field_6C->TurnOn(1);
        this->field_64->TurnOn(1);
        this->field_64->TurnOn(1);
        this->field_A0->SetShown(true);
        this->field_A0->SetText(static_cast<global_text_enum>(253));
        this->field_A0->SetNoFlash(color32 {0xFFC8C8C8});
        this->field_A0->SetShown(1);
        this->field_A0->SetText(static_cast<global_text_enum>(271));
        this->field_A0->SetNoFlash(color32 {0xFFC8C8C8});
        this->field_A4->SetShown(1);
        this->field_A4->SetText(static_cast<global_text_enum>(254));
        this->field_A4->SetNoFlash(color32 {0xFFC87238});
        this->field_A8->SetShown(1);
        this->field_A8->SetText(static_cast<global_text_enum>(255));
        this->field_A8->SetNoFlash(color32 {0xFFC87238});

        this->field_78[7]->SetText(static_cast<global_text_enum>(265));
		this->field_78[8]->SetText(static_cast<global_text_enum>(261));
        this->field_78[0]->SetText(static_cast<global_text_enum>(275));
        this->field_78[1]->SetText(static_cast<global_text_enum>(260));
        this->field_78[2]->SetText(static_cast<global_text_enum>(258));
        this->field_78[3]->SetText(static_cast<global_text_enum>(259));
        this->field_78[4]->SetText(static_cast<global_text_enum>(273));
        this->field_78[5]->SetText(static_cast<global_text_enum>(263));
        this->field_78[9]->SetText(static_cast<global_text_enum>(92));
        this->field_78[6]->SetText(static_cast<global_text_enum>(297));

        auto v8 = this->field_78[0]->GetX();
        auto v9 = this->field_78[0]->GetY();
        this->field_64->GetPos(this->field_B8, this->field_C8);
        this->field_64->GetPos(this->field_D8, this->field_E8);

        for (auto i = 0u; i < 4u; ++i)
        {
            this->field_B8[i] = this->field_B8[i] - v8;
            this->field_C8[i] = this->field_C8[i] - v9;
            this->field_D8[i] = this->field_D8[i] - v8;
            this->field_E8[i] = this->field_E8[i] - v9;
        }

        this->field_F8 = false;
    }
    else
    {
        THISCALL(0x0063B2E0, this);
    }
}



void pause_menu_root2::update_selected()
{
    int prev_index = this->field_B4;
    int curr_index = this->field_B0;

    // Select current item
    FEText *curr_text;
    if (curr_index == 10) {
        curr_text = this->field_AC->field_30->text_box;
    } else {
        curr_text = this->field_78[curr_index];
    }
	//    curr_text->SetNoFlash(color32 {200, 200, 200, 255});
	curr_text->SetScale(1.0f,1.0f);

    this->field_B4 = this->field_B0;

    // Deselect previous item
    FEText *prev_text;
	menu_nav_bar *field_30;
    if (prev_index == 10) {
        prev_text = this->field_AC->field_30->text_box;
    } else {
        prev_text = this->field_78[prev_index];
		
    }
	//prev_text->SetNoFlash(color32 {63, 208, 230, 255});
    prev_text->SetScale(1.0f, 1.0f);
	
	
	
}

void pause_menu_root2::OnDown(int a2)
{
    if (this->field_30 && this->field_2C && this->field_F8) {
        return;
    }
    if (this->field_B0 == 10) {
    if (++this->field_B0 >= 10) {
        this->field_B0 = 0;
    }

    if (this->field_B0 == 10) {
        if (mission_manager::s_inst->is_story_active()) {
            if (++this->field_B0 >= 10) {
                this->field_B0 = 0;
            }
        }
    }
	    }

    this->update_selected();

    static bool initialized = false;
    if (!initialized) {
        initialized = true;
    }
	

	static string_hash sfx_id_hash{"fe_ps_udscroll"};

        [[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
		
		    THISCALL(0x0061BE10, this, a2);
}

void pause_menu_root2::OnUp(int a2)
{
	    sp_log("pause_menu_root::OnUp(): %d", a2);

    if (this->field_30 || this->field_2C || this->field_F8) {
        return;
    }
   if (this->field_B0 == 10) {
    if (--this->field_B0 < 0) {
        this->field_B0 = 10;
    }

    if (this->field_B0 == 10) {
        if (mission_manager::s_inst->is_story_active()) {
            if (--this->field_B0 < 0) {
                this->field_B0 = 10;
            }
        }
    }
    }
    this->update_selected();
	


    static bool initialized = false;
    if (!initialized) {
        initialized = true;
			
    }

	static string_hash sfx_id_hash{"fe_ps_udscroll"};

        [[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
	
	    THISCALL(0x0061BD00, this, a2);
}



void sub_648F40() {
    CDECL_CALL(0x00648F40);
}

void pause_menu_root2::OnDeactivate(FEMenu *a2)
{
    this->field_28 &= ~0x80;
}

void pause_menu_root::Update(Float a2) {
    if constexpr (1) {
        if (this->field_2D) {
            sub_648F40();
        }

        FEMenu::Update(a2);
        if (this->field_30) {
            this->update_switching_heroes();
        }

        if (this->field_2C) {
            if (!mission_stack_manager::s_inst->waiting_for_push_or_pop()) {
                auto *v3 = this->field_AC;
                this->field_2C = false;

                v3->MakeActive(9);

                comic_panels::game_play_panel()->field_67 = true;
            }
        }
    } else {
        THISCALL(0x006490A0, this, a2);
    }
}

void  sub_582AD0()
{

CDECL_CALL(0x00582AD0);
  
}

void pause_menu_root2::sub_62A840()
{
    menu_nav_bar *nav_bar = this->field_AC->field_30;

    nav_bar->field_4 = "Source";
    nav_bar->field_28 = 0;
    nav_bar->AddButtons(menu_nav_bar::button_type{15},
                                   menu_nav_bar::button_type{17},
                                   static_cast<global_text_enum>(3));



    mString text;

    text = mString(nav_bar->field_4.guts);
    nav_bar->text_box->SetTextNoLocalize(text);

    text = mString(get_msg(g_fileUSM(), "Resume"));
    nav_bar->text_box->SetTextNoLocalize(text);
}

void pause_menu_root2::OnActivate()
{
	    if constexpr (0) {
    sub_62A840();

    this->field_28 |= 0x80;
    this->field_30 = 0;
    this->field_2C = false;
    this->field_2A = -1;
   this->field_FC = mission_manager::s_inst->is_story_active() ? 1 : 0;

    color32 deselected_color{200, 200, 200, 255};
    FEText *text_box = this->field_AC->field_30->text_box;
    text_box->SetNoFlash(deselected_color);
    text_box->SetScale(1.0f, 1.0f);

    sub_582AD0();
    g_cursor()->sub_5A6790();
    g_cursor()->sub_5A67D0(305, 420, 385, 445);
  
  THISCALL(0x006302D0, this);
}

}



void pause_menu_root::update_switching_heroes() {
    int v2 = this->field_30;
    if (v2 == 4) {
        g_world_ptr->remove_player(g_world_ptr->num_players - 1);
    } else if (v2 == 2) {
        int v3;
        if (this->field_34) {
            g_world_ptr->add_player(mString{"venom"});

            v3 = 4;
        } else {
            g_world_ptr->add_player(mString{"ultimate_spiderman"});

            v3 = 0;
        }

        auto *v4 = g_femanager.IGO->hero_health;
        if (v4->field_0[v3] != nullptr)
        {
            v4->field_30 = g_world_ptr->get_hero_ptr(0)->my_handle.field_0;
            v4->field_38 = v3;
            v4->UpdateMasking();
            v4->clear_bars();
        }

        v4->SetShown(this->field_38);
    }

    --this->field_30;
}


 void pause_menu_root::run_script()
{
    if (!this->field_F9)
    {
        // "Yes" selected
        int selection = this->field_B0;

        if (selection == 9)
        {
            // Initiate hero switch
          //  this->field_F8 = is_spidey;

            string_hash toggle_hash;
            toggle_hash.initialize(0, "start_to_level_s01(debug_menu_entry)", 0);
			    auto* v1 = script::gsoi()->parent;
			int function = script::find_function(toggle_hash, v1, 0);
			script::new_thread(function, reinterpret_cast<script_instance*>(script::gsoi()));
    

            script::sub_5028B0(toggle_hash, script::gsoi());
            script::exec_thread(0);
			
			    auto* v5 = this->field_AC;
    if (v5->m_index >= 0) {
        v5->MakeActive(0);
        comic_panels::game_play_panel()->field_67 = v5->m_count;
    }
			
pause_menu_system_ptr->Deactivate();


        }

static string_hash sfx_id_hash{"FE_PS_ACCEPT"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
}

    } 

void pause_menu_root::handle_objectives()
{
    auto *menu_system = this->field_AC;
	pause_menu_goals* v1;
	v1->Init();
    //*reinterpret_cast<std::uint32_t *>(*(menu_system->base.m_id + 1) + 72) = 5;
    menu_system->MakeActive(1);

    if (!menu_system->m_count)
    {
        comic_panels::game_play_panel()->field_67 = 0;
    }

static string_hash sfx_id_hash{"FE_PS_ACCEPT"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);

}	


void pause_menu_root::OnCross(int a2)
{
    int type = this->field_B0;
	  
static string_hash sfx_id_hash{"FE_PS_ACCEPT"};
	                if (type == 0)
		        THISCALL(0x00630460,this, a2);
				    if (type == 1)
		        THISCALL(0x00630460,this, a2);
				    if (type == 2)
		        THISCALL(0x00630460,this, a2);
				    if (type == 3)
		        THISCALL(0x00630460,this, a2);
				    if (type == 4)
		        THISCALL(0x00630460,this, a2);
				    if (type == 5)
		        THISCALL(0x00630460,this, a2);
			        if (type == 6)
                movie_manager::load_and_play_movie("credits", nullptr, false);
				    if (type == 7)
		        THISCALL(0x00630460,this, a2);
				    if (type == 8)
			    THISCALL(0x00630460,this, a2);
					if (type == 9)
        [[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
                       run_script();


}

void pause_menu_root::sub_61C520()
{

	
	
	 THISCALL(0x0061C520,this);
}




void pause_menu_root_patch() {

    {
        FUNC_ADDRESS(address, &pause_menu_root::_Load);
        set_vfunc(0x00893F48, address);
    }
    {
        FUNC_ADDRESS(address, &pause_menu_root::OnCross);
        set_vfunc(0x00893F84, address);
    }
    {
        FUNC_ADDRESS(address, &pause_menu_root::Update);
        set_vfunc(0x00893F58, address);
    }
	{
        FUNC_ADDRESS(address, &pause_menu_root2::OnUp);
        set_vfunc(0x00893F74, address);
    }
    {
        FUNC_ADDRESS(address, &pause_menu_root2::OnDown);
        set_vfunc(0x00893F78, address);
    }
	{
        FUNC_ADDRESS(address, &pause_menu_root2::OnActivate);
        set_vfunc(0x00893F64, address);
    }
	{
        FUNC_ADDRESS(address, &pause_menu_root2::OnDeactivate);
        set_vfunc(0x00893F68, address);
    }
	{
        FUNC_ADDRESS(address, &pause_menu_root2::OnDeactivate);
        set_vfunc(0x00893F68, address);
    }
	{
	    FUNC_ADDRESS(address, &pause_menu_root::update_switching_heroes);
	    REDIRECT(0x006490C4, address);
    }

    return;


	
	

}


pause_menu_root *& pause_menu_root_ptr = var<pause_menu_root*>(0x0965C21);


pause_menu_root2 *& pause_menu_root2_ptr = var<pause_menu_root2*>(0x0965C21);