#include "script_controller.h"

#include "common.h"
#include "entity.h"
#include "fe_controller_disconnect.h"
#include "femanager.h"
#include "func_wrapper.h"
#include "game.h"
#include "input_device.h"
#include "input_mgr.h"
#include "log.h"
#include "pausemenusystem.h"
#include "trace.h"
#include "utility.h"

Var<script_controller *> script_pad{0x0096BB40};

VALIDATE_SIZE(script_controller, 0x48);

script_controller::script_controller() : signaller(false) {}

void script_controller::update()
{
    TRACE("script_controller::update");

    {
        auto v1 = input_mgr::instance->field_58;
        // sp_log("0x%X", v1);
    }

    if constexpr (0)
    {}
    else
    {
        THISCALL(0x0065F8A0, this);
    }
}

bool script_controller::is_button_pressed(int a1) const
{
    auto v3 = input_mgr::instance->field_58;
    if ( v3 == -1 ) {
        return false;
    }

    bool result = false;
    auto *device = input_mgr::instance->get_device_from_map(v3);
    if ( device != nullptr )
    {
        if ( 1.0f != device->get_axis_state(22, 0) )
        {
            auto v7 = device->get_axis_id(a1);
            if ( 1.0f == device->get_axis_delta(v7, 0) ) {
                return true;
            }
        }
    }

    return result;
}

float script_controller::get_axis_position(int a1) const
{
    auto v3 = input_mgr::instance->field_58;
    if ( v3 == -1 ) {
        return 0.0f;
    }
    
    auto *device = input_mgr::instance->get_device_from_map_internal(v3);
    if ( device != nullptr
        && device->get_id() != -1
        && 1.0f != device->get_axis_state(22, 0) )
    {
        auto v6 = device->get_axis_id(a1);
        return device->get_axis_state(v6, 0);
    }

    return 0.0f;
}

void script_controller_patch()
{
    {
        FUNC_ADDRESS(address, &script_controller::update);
        set_vfunc(0x0089BD50 + 0x168, address);
    }
}
