#include "igozoomoutmap.h"

#include "common.h"
#include "func_wrapper.h"
#include "game.h"
#include "marky_camera.h"
#include "sound_instance_id.h"
#include "string_hash.h"
#include "variable.h"
#include "wds.h"

#include <utility.h>

VALIDATE_SIZE(IGOZoomOutMap, 0x82Cu);
VALIDATE_SIZE(IGOZoomOutMap::internal, 0x1Cu);
VALIDATE_SIZE(IGOZoomPOI, 0x14);
VALIDATE_SIZE(zoom_map_ui, 0x240u);
VALIDATE_OFFSET(IGOZoomOutMap, field_5CC, 0x5CC);
VALIDATE_OFFSET(IGOZoomOutMap, field_5C4, 0x5C4);

IGOZoomOutMap::IGOZoomOutMap() {
    THISCALL(0x006489A0, this);
}

void IGOZoomOutMap::UpdateInScene()
{
    if ( this->field_5C5 )
    {
        for ( int i = 0; i < this->field_5B4; ++i )
        {
            if ( this->field_5B8 == this->field_0[i].field_14 ) {
                this->field_0[i].field_0.UpdateInScene();
            }
        }
    }
}

void IGOZoomOutMap::DoneZoomingBack() {
    g_game_ptr->enable_marky_cam(false, false, -1000.0, 0.0);
    g_world_ptr->field_28.field_44->set_affixed_x_facing(false);
    g_game_ptr->unpause();
    g_game_ptr->field_15E = false;
}

bool IGOZoomOutMap::sub_55F320() {
    return this->field_5C4 || this->field_5C3;
}

void IGOZoomOutMap::sub_638AD0(int a2, int a3, int a4) {
    THISCALL(0x00638AD0, this, a2, a3, a4);
}

void IGOZoomOutMap::Update(Float a2) {
    THISCALL(0x0063A760, this, a2);
}

void IGOZoomOutMap::SetZoomLevel(int a2) {
    auto v2 = a2;
    if (a2 >= 1) {
        if (a2 > 4) {
            v2 = 4;
        }

    } else {
        v2 = 1;
    }

    if (this->field_5B0 != v2) {
        static string_hash sfx_id_hash{"FE_GENERIC_LRSCROLL"};

        [[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
    }

    this->field_5B0 = v2;
    auto *v4 = g_world_ptr->field_28.field_44;

    this->field_578 = v4->get_abs_position();

    this->field_578[1] = this->field_5B0 * 500.0f;
    this->field_5C3 = true;
}

void IGOZoomPOI::UpdateInScene()
{
    THISCALL(0x0062A160, this);
}


char zoom_map_ui::Draw()
{

	
    THISCALL(0x00632360, this);
}

void zoom_map_ui::Update(Float a2)
{

	
    THISCALL(0x00632020, this);
}


PanelFile zoom_map_ui::Init()
{

	
    THISCALL(0x00644330, this);
}

void zoom_map_ui::OnSquare()
{

	
    THISCALL(0x00621DA0, this);
}

int zoom_map_ui::OnX()
{

	
    THISCALL(0x006125E0, this);
}

void IGOZoomOutMap_patch() {
    {
        FUNC_ADDRESS(address, &IGOZoomOutMap::SetZoomLevel);
        SET_JUMP(0x00619550, address);
    }
}
