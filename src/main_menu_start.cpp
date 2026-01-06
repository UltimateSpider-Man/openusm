#include "main_menu_start.h"

#include "common.h"
#include "func_wrapper.h"

#include "panelquad.h"

VALIDATE_SIZE(main_menu_start, 0x130);

main_menu_start::main_menu_start(FrontEndMenuSystem *a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 8, 0) {
    this->field_128 = 0;
    this->field_12C = a2;
    this->field_120 = 0.0;
    this->field_124 = 0.0;
    this->field_12A = 0;
    this->field_128 = 0;
}

void main_menu_start::Update(Float a2)
{

			//movie_manager::load_and_play_movie("attract", nullptr, true);


     THISCALL(0x00636AA0, this, a2);
}

void main_menu_start::Init()
{
    PanelFile *panel_file = this->field_12C->field_7C;

    auto load_pq = [panel_file](const char *name) -> PanelQuad* {
        PanelQuad *pq = panel_file->GetPQ(name);
        pq->TurnOn(false);
        return pq;
    };

    this->pq_bkg_city           = load_pq("mm_bkg_city");
    this->pq_bkg_detail01       = load_pq("mm_bkg_detail01");
    this->pq_bkg_detail02       = load_pq("mm_bkg_detail02");
    this->pq_bkg_grey_a01       = load_pq("mm_bkg_grey_a01");
    this->pq_bkg_grey_a02       = load_pq("mm_bkg_grey_a02");
    this->pq_logo_main          = load_pq("mm_logo_main");
    this->pq_bkg_city01         = load_pq("mm_bkg_city01");
    this->pq_bkg_city02         = load_pq("mm_bkg_city02");
    this->pq_bkg_grey_a03       = load_pq("mm_bkg_grey_a03");
    this->pq_bkg_grey_a04       = load_pq("mm_bkg_grey_a04");
    this->pq_bkg_grey_a05       = load_pq("mm_bkg_grey_a05");
    this->pq_bkg_grey_a06       = load_pq("mm_bkg_grey_a06");
    this->pq_bkg_grey_a07       = load_pq("mm_bkg_grey_a07");
    this->pq_bkg_grey_a08       = load_pq("mm_bkg_grey_a08");
    this->pq_bkg_grey_a09       = load_pq("mm_bkg_grey_a09");
    this->pq_pre_main_back_b    = load_pq("mm_pre_main_back_b");
    this->pq_pre_main_screen    = load_pq("mm_pre_main_screen");
    this->pq_pre_main_screen_0  = load_pq("mm_pre_main_screen_0");
    this->pq_pre_main_screen_1  = load_pq("mm_pre_main_screen_1");
    this->pq_pre_main_screen_2  = load_pq("mm_pre_main_screen_2");
    this->pq_pre_main_spider    = load_pq("mm_pre_main_spider");
    this->pq_bkg_white01        = load_pq("mm_bkg_white01");
    this->pq_bkg_detail_lig     = load_pq("mm_bkg_detail_lig");
    this->pq_bkg_detail_lig_0   = load_pq("mm_bkg_detail_lig_0");
    this->pq_bkg_detail_lig_1   = load_pq("mm_bkg_detail_lig_1");
    this->pq_bkg_detail_lig_2   = load_pq("mm_bkg_detail_lig_2");
    this->pq_bkg_detail_lig_3   = load_pq("mm_bkg_detail_lig_3");
    this->pq_bkg_detail_lig_4   = load_pq("mm_bkg_detail_lig_4");
    this->pq_bkg_detail_dar     = load_pq("mm_bkg_detail_dar");
    this->pq_bkg_detail_dar_0   = load_pq("mm_bkg_detail_dar_0");
    this->pq_bkg_detail_dar_1   = load_pq("mm_bkg_detail_dar_1");
    this->pq_bkg_detail_dar_2   = load_pq("mm_bkg_detail_dar_2");
    this->pq_bkg_detail_dar_3   = load_pq("mm_bkg_detail_dar_3");
    this->pq_bkg_detail_dar_4   = load_pq("mm_bkg_detail_dar_4");
    this->pq_mainmenu_box_h     = load_pq("mm_mainmenu_box_h");
    this->pq_mainmenu_box_h_0   = load_pq("mm_mainmenu_box_h_0");
    this->pq_loading_bar01      = load_pq("mm_loading_bar01");
    this->pq_loading_bar02      = load_pq("mm_loading_bar02");
    this->pq_loading_bar_ga     = load_pq("mm_loading_bar_ga");

    // Load text elements
    this->text_mainmenu = panel_file->GetTextPointer("mm_mainmenu_text");
    if (this->text_mainmenu) {
        this->text_mainmenu->SetShown(false);
    }

    this->text_mainmenu_0 = panel_file->GetTextPointer("mm_mainmenu_text_0");
    if (this->text_mainmenu_0) {
        this->text_mainmenu_0->SetShown(false);
    }

    // Load animation/position data from panel file sub-structure

}

