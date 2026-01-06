#include "pause_menu_status.h"

#include "common.h"
#include "func_wrapper.h"
#include "memory.h"
#include "utility.h"
#include "trace.h"
#include "panelquad.h"
#include "pausemenusystem.h"
#include "pause_menu_goals.h"
#include "pause_menu_awards.h"
#include "pause_menu_game.h"
#include "panelfile.h"


VALIDATE_SIZE(pause_menu_status, 0x130);

pause_menu_status::pause_menu_status(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 0, 0) {
		    // Initialize field_2C array (30 elements)
    for (int i = 0; i < 30; ++i) {
        field_2C[i] = 0;
    }
    
    // Initialize field_A4 array (14 elements)
    for (int i = 0; i < 14; ++i) {
        field_A4[i] = 0;
    }
    
    // Create and initialize pause_menu_awards
    field_F0 = new pause_menu_awards();
    field_F0->Init();
    
    // Create and initialize pause_menu_game
    field_F4 = new pause_menu_game();
    field_F4->Init();
    
    // Create and initialize pause_menu_goals
    field_F8 = new pause_menu_goals(a2, a3, a4);
    field_F8->Init();
    THISCALL(0x0060FF30, this, a2, a3, a4);
}



void pause_menu_status::OnTriangle(int a2) {
    THISCALL(0x0061D3F0, this, a2);
}

void pause_menu_status::_Load() {
    TRACE("pause_menu_status::Load");

    if constexpr (1)
    {
        auto *v2 = this->field_E8;
        this->field_12C = 0;
        this->field_12D = 0;
        auto *v3 = v2->field_2C;
        this->field_2C[0] = v3->GetPQ("pm_all_back_01");
        this->field_2C[1] = v3->GetPQ("pm_all_back_02");
        this->field_2C[2] = v3->GetPQ("pm_all_back_02a");
        this->field_2C[3] = v3->GetPQ("pm_all_back_02b");
        this->field_2C[4] = v3->GetPQ("pm_all_back_03");
        this->field_2C[5] = v3->GetPQ("pm_all_back_04");
        this->field_2C[6] = v3->GetPQ("pm_all_back_05");
        this->field_2C[7] = v3->GetPQ("pm_all_detail_02");
        this->field_2C[8] = v3->GetPQ("pm_all_detail_03");
        this->field_2C[9] = v3->GetPQ("pm_all_detail_04");
        this->field_2C[10] = v3->GetPQ("pm_all_detail_05");
        this->field_2C[11] = v3->GetPQ("pm_all_detail_06");
        this->field_2C[12] = v3->GetPQ("pm_all_detail_07");
        this->field_2C[13] = v3->GetPQ("pm_all_detail_08");
        this->field_2C[14] = v3->GetPQ("pm_all_detail_09");
        this->field_2C[15] = v3->GetPQ("pm_all_detail_10");
        this->field_2C[16] = v3->GetPQ("pm_all_detail_11");
        this->field_2C[17] = v3->GetPQ("pm_all_box_01");
        this->field_2C[18] = v3->GetPQ("pm_all_box_02");
        this->field_2C[19] = v3->GetPQ("pm_all_box_03");
        this->field_2C[20] = v3->GetPQ("pm_all_icon");
        this->field_2C[21] = v3->GetPQ("pm_status_explanation_box_01");
        this->field_2C[22] = v3->GetPQ("pm_status_explanation_box_02");
        this->field_2C[23] = v3->GetPQ("pm_status_hilite_text");
        this->field_2C[24] = v3->GetPQ("pm_status_hilite_text_01");
        this->field_90[0] = v3->GetPQ("pm_scroll_arrow_down");
        this->field_90[1] = v3->GetPQ("pm_scroll_arrow_up");
        this->field_90[2] = v3->GetPQ("pm_scroll_bar_01");
        this->field_90[3] = v3->GetPQ("pm_scroll_bar_02");
        this->field_90[4] = v3->GetPQ("pm_scroll_spider_icon");
        this->field_A4[0] = v3->GetTextPointer("pm_header_text_AWARDS");
        this->field_A4[1] = v3->GetTextPointer("pm_status_explanation_box_text_BLANK");
        this->field_A4[2] = v3->GetTextPointer("pm_status_text_left_01_BLANK");
        this->field_A4[3] = v3->GetTextPointer("pm_status_text_left_02_BLANK");
        this->field_A4[4] = v3->GetTextPointer("pm_status_text_left_03_BLANK");
        this->field_A4[5] = v3->GetTextPointer("pm_status_text_left_04_BLANK");
        this->field_A4[6] = v3->GetTextPointer("pm_status_text_left_05_BLANK");
        this->field_A4[7] = v3->GetTextPointer("pm_status_text_left_06_BLANK");
        this->field_A4[8] = v3->GetTextPointer("pm_status_text_right_01_BLANK");
        this->field_A4[9] = v3->GetTextPointer("pm_status_text_right_02_BLANK");
        this->field_A4[10] = v3->GetTextPointer("pm_status_text_right_03_BLANK");
        this->field_A4[11] = v3->GetTextPointer("pm_status_text_right_04_BLANK");
        this->field_A4[12] = v3->GetTextPointer("pm_status_text_right_05_BLANK");
        this->field_A4[13] = v3->GetTextPointer("pm_status_text_right_06_BLANK");

        auto *v4 = mem_alloc(sizeof(PanelQuad));
        auto *v5 = new (v4) PanelQuad {};
        auto *v6 = this->field_2C[21];
        this->field_DC = v5;
        v5->CopyFrom(v6);

        float v34[4] {};
        float v30[4] {};
        this->field_DC->GetPos(v34, v30);

        v30[0] = v30[0] - 50.f;
        v30[1] = v30[1] - 70.f;
        v30[2] = v30[2] - 50.f;
        v30[3] = v30[3] - 70.f;

        this->field_DC->SetPos(v34, v30);

        auto *v9 = mem_alloc(sizeof(PanelQuad));
        auto *v10 = new (v9) PanelQuad {};
        auto *v11 = this->field_2C[22];
        this->field_E0 = v10;
        v10->CopyFrom(v11);
        this->field_E0->GetPos(v34, v30);

        v30[0] = v30[0] - 50.f;
        v30[1] = v30[1] - 70.f;
        v30[2] = v30[2] - 50.f;
        v30[3] = v30[3] - 70.f;

        this->field_E0->SetPos(v34, v30);
        int v27 = this->field_A4[1]->GetX();
        int v26 = this->field_A4[1]->GetY();
        auto *v14 = mem_alloc(sizeof(FEText));

        float a5 = v26 - 45.f;
        float a4 = v27;
        auto *v15 = new (v14) FEText {
                static_cast<font_index>(1),
                static_cast<global_text_enum>(292),
                a4,
                a5,
                5,
                static_cast<panel_layer>(1),
                1.0,
                16,
                0,
                color32 {0xFFC8C8C8}};

        auto *v16 = this->field_90[4];
        this->field_E4 = v15;
        this->field_120 = v16->GetCenterY();
        auto *v18 = v3->GetPQ("pm_scroll_spider_icon_end_marker");
        this->field_124 = v18->GetCenterY();
        auto *v19 = this->field_A4[2];
        auto v28 = this->field_2C[23]->GetCenterY();
        auto v20 = v19->GetY();
        auto *v21 = this->field_2C[24];
        auto *v22 = this->field_A4[2];
        this->field_118 = v28 - v20;
        auto v29 = v21->GetCenterY();
        auto v23 = v22->GetY();
        this->field_128 = 0;
        this->field_11C = v29 - v23;
        this->update_selected();
    }
    else
    {
        THISCALL(0x0063B890, this);
    }
}

mString *pause_menu_status::get_element_desc(mString *out, int a3)
{
    TRACE("pause_menu_status::get_element_desc");

    mString *result;
    if constexpr (1)
    {
        auto v3 = this->field_EC;
        if ( v3 != 0 )
        {
            auto v4 = v3 - 1;
            if ( v4 != 0 )
            {
                if ( v4 == 1 )
                    *out = this->field_F8->get_element_desc(a3);
                else
                    *out = this->field_F0->field_0;

                result = out;
            }
            else
            {
                *out = this->field_F4->field_160[a3];
                result = out;
            }
        }
        else
        {
            *out = this->field_F0->field_140[a3];
            result = out;
        }
    }
    else
    {
        THISCALL(0x00610290, this, out, a3);
        result = out;
    }

    return result;
}

void pause_menu_status::update_selected() {
    TRACE("pause_menu_status::update_selected");

    THISCALL(0x0061CC30, this);
}


pause_menu_awards pause_menu_status::get_element_value(int index) {
    THISCALL(0x006101B0, this, index);
}

void pause_menu_status_patch()
{
    {
        FUNC_ADDRESS(address, &pause_menu_status::_Load);
        set_vfunc(0x008940B0, address);
    }

    {
        FUNC_ADDRESS(address, &pause_menu_status::get_element_desc);
       // REDIRECT(0x0061CD64, address);
    }
	
	
	    {
	FUNC_ADDRESS(address, &pause_menu_status::update_selected);
    REDIRECT(0x0061D235, address);
    REDIRECT(0x0061D3CF, address);
    REDIRECT(0x0061D415, address);
    REDIRECT(0x0061D4CF, address);
    REDIRECT(0x0062AB0D, address);
    REDIRECT(0x0062AB7D, address);
    REDIRECT(0x0062AE0C, address);
    REDIRECT(0x0062AE31, address);
    REDIRECT(0x0062AE5D, address);
    REDIRECT(0x0062AE77, address);
    REDIRECT(0x0063BE64, address);
	
	    }
		
			    {
	FUNC_ADDRESS(address, &pause_menu_status::get_element_value);
  //  REDIRECT(0x0061CCAF, address);
	
	    }
		
}
