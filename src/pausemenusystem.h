#pragma once

#include "femenusystem.h"

#include "femanager.h"
#include "variable.h"

struct PanelFile;
struct menu_nav_bar;
struct pause_menu_save_load_display;

struct PauseMenuSystem : FEMenuSystem {
    PanelFile *field_2C;
    menu_nav_bar *field_30;
    pause_menu_save_load_display *field_34;
    int field_38;

    //0x00647E50
    PauseMenuSystem(font_index a2);
	
void	PauseMenuSystem2(font_index a2);

    //0x0060C160
    bool IsDialogActivated();

    //0x006430F0
    void LoadAll();
	
	void Activate(int a2, bool a3);

    //0x0060BEE0
    void Deactivate();

    void SetTransition(int a1);

    //0x0060BF10
    //virtual
    void Draw();

    //0x0062F0C0
    //virtual
    void Update(Float a2);

    //0x00618FC0
    //virtual
    void OnButtonPress(int a2, int a3);

    //virtual
    void UpdateButtonPresses();
};

extern void PauseMenuSystem_patch();



extern PauseMenuSystem *& pause_menu_system_ptr;