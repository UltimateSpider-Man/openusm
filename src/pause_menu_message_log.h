#pragma once

#include "femenu.h"

struct pause_menu_message_log : FEMenu {
    int field_2C[53];

    //0x00611BD0
    pause_menu_message_log(FEMenuSystem *a2, int a3, int a4);

    //virtual
    void _Load();
	
	// 0x00611D20
void Update(float a2);

// 0x00611D50
int get_title(int arg0);

// 0x00611DE0
int get_body(int arg0);

// 0x0061F8C0
void update_selected(int a6);

// 0x0061FCC0
void OnUp(int out);

// 0x0061FE80
void OnDown(int out);

// 0x00620040
void OnButtonRelease(int a2, int a3);

// 0x006200B0
void OnStart(int a2);

// 0x00631A10
void OnActivate();

// 0x00631B50
void OnDeactivate(FEMenu *a2);

// 0x00631B70
void OnCross(int out);


};

extern void pause_menu_message_log_patch();
