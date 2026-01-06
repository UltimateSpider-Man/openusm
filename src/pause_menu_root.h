#pragma once

#include "femenu.h"

#include "float.hpp"
#include "panelanim.h"
#include "panelanimfile.h"
#include "string_hash.h"
#include "panelquad.h"

#include "func_wrapper.h"

#include "script.h"

#include "wds.h"

#include "ai_player_controller.h"

#include "cursor.h"

struct FEMenuSystem;
struct FEMenuSystem2;
struct PanelQuad;
struct FEText;




struct pause_menu_root : FEMenu {
    bool field_2C;
    char field_2D;
    int field_30;
    int field_34;
    int field_38;
    PanelQuad *field_3C[9];
    PanelQuad *field_60;
    PanelQuad *field_64;
    PanelQuad *field_68;
    PanelQuad *field_6C;
    PanelQuad *field_70;
    PanelQuad *field_74;
    FEText *field_78[10];
    FEText *field_A0;
    FEText *field_A4;
    FEText *field_A8;
    FEMenuSystem *field_AC;
    int field_B0;
    int field_B4;
    float field_B8[4];
    float field_C8[4];
    float field_D8[4];
    float field_E8[4];
	bool field_F8;
    int field_F9;

    //0x0060E590
    pause_menu_root(FEMenuSystem *a2, int a3, int a4);

    //0x0063B2E0
    //virtual
    void _Load();

    //0x00641BF0
    void update_switching_heroes();


    //0x006490A0
    //virtual
    void Update(Float a2);
	
	void OnCross(int a2);
	
	void sub_61C520();
	
	void run_script();
	
	
	void handle_objectives();


	

};

struct pause_menu_root2 : FEMenu {
    bool field_2C;
    char field_2D;
    int field_30;
    int field_34;
    int field_38;
    PanelQuad *field_3C[9];
    PanelQuad *field_60;
    PanelQuad *field_64;
    PanelQuad *field_68;
    PanelQuad *field_6C;
    PanelQuad *field_70;
    PanelQuad *field_74;
    FEText *field_78[10];
    FEText *field_A0;
    FEText *field_A4;
    FEText *field_A8;
    FEMenuSystem2 *field_AC;
    int field_B0;
    int field_B4;
    float field_B8[4];
    float field_C8[4];
    float field_D8[4];
    float field_E8[4];
    bool field_F8;
    int field_FC;
	
	    //0x0061BD00
    //virtual
    void OnUp(int a2);

    //0x0061BE10
    //virtual
    void OnDown(int a2);
	
	void update_selected();
	
	
		void OnActivate();
		
		
		void sub_62A840();
		
		void OnDeactivate(FEMenu *a2);
	
	};
	
	

extern void pause_menu_root_patch();

extern pause_menu_root *& pause_menu_root_ptr;




extern pause_menu_root2 *& pause_menu_root2_ptr;
