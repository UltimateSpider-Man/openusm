#include "main_menu_start.h"

#include "common.h"
#include "func_wrapper.h"

VALIDATE_SIZE(main_menu_start, 0x130);

main_menu_start::main_menu_start(FEMenuSystem *a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 8, 0) {
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

