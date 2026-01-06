#include "pause_menu_message_log.h"

#include "common.h"
#include "func_wrapper.h"
#include "utility.h"
#include "trace.h"

VALIDATE_SIZE(pause_menu_message_log, 0x100u);

pause_menu_message_log::pause_menu_message_log(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x00611BD0, this, a2, a3, a4);
}

void pause_menu_message_log::_Load()
{
    TRACE("pause_menu_message_log::Load");

    THISCALL(0x0063D560, this);
}

void pause_menu_message_log::update_selected(int a6) {
    THISCALL(0x0061F8C0, this, a6);
}


void pause_menu_message_log::OnUp(int out) {
    THISCALL(0x0061FCC0, this, out);
}


void pause_menu_message_log::OnDown(int out) {
    THISCALL(0x0061FE80, this, out);
}


void pause_menu_message_log::OnButtonRelease(int a2, int a3) {
    THISCALL(0x00620040, this, a2, a3);
}


void pause_menu_message_log::OnStart(int a2) {
    THISCALL(0x006200B0, this, a2);
}


void pause_menu_message_log::OnActivate() {
    THISCALL(0x00631A10, this);
}


void pause_menu_message_log::OnDeactivate(FEMenu *a2) {
    THISCALL(0x00631B50, this, a2);
}


void pause_menu_message_log::OnCross(int out) {
    THISCALL(0x00631B70, this, out);
}


int pause_menu_message_log::get_title(int arg0) {
    return (int)THISCALL(0x00611D50, this, arg0);
}


int pause_menu_message_log::get_body(int arg0) {
    return (int)THISCALL(0x00611DE0, this, arg0);
}




void pause_menu_message_log_patch()
{
    {
        FUNC_ADDRESS(address, &pause_menu_message_log::_Load);
        set_vfunc(0x0008943E8, address);
    }
    {
    FUNC_ADDRESS(address, &pause_menu_message_log::update_selected);
    REDIRECT(0x0061FE60, address);
    REDIRECT(0x0062001B, address);
    REDIRECT(0x006200D1, address);
    REDIRECT(0x0062052D, address);
    REDIRECT(0x00620561, address);
    REDIRECT(0x0062059E, address);
    REDIRECT(0x006205BE, address);
    REDIRECT(0x0062063C, address);
    REDIRECT(0x0062067D, address);
    REDIRECT(0x00631AE8, address);
    REDIRECT(0x00631EE9, address);
    REDIRECT(0x0063D876, address);
    }
	{
	FUNC_ADDRESS(address, &pause_menu_message_log::get_body);
    REDIRECT(0x00620294, address);
    REDIRECT(0x00631D0E, address);
    }
	
    {
	FUNC_ADDRESS(address, &pause_menu_message_log::get_body);
    REDIRECT(0x0061F969, address);
    REDIRECT(0x00631CB5, address);
    }
	
}
