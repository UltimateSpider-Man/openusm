#include "player_controller_inode.h"

#include "common.h"

#include "actor.h"
#include "ai_player_controller.h"
#include "from_mash_in_place_constructor.h"
#include "func_wrapper.h"
#include "trace.h"

namespace ai {

VALIDATE_SIZE(player_controller_inode, 0xEC);

player_controller_inode::player_controller_inode(from_mash_in_place_constructor *a2) {
    THISCALL(0x004813F0, this, a2);
}

game_button player_controller_inode::_get_button(controller_inode::eControllerButton a3)
{
    TRACE("player_controller_inode::get_button");
    sp_log("%d", int(a3));

    if constexpr (0)
    {
        auto v3 = this->field_C->get_player_controller();
        switch (a3) {
        case 0:
        case 7:
            return v3->gb_jump;
        case 1:
        case 8:
            return v3->gb_grab;
        case 2:
        case 9:
            return v3->gb_attack;
        case 3:
        case 10:
            return v3->gb_attack_secondary;
        case 4:
        case 15:
            return v3->gb_range;
        case 5:
        case 11:
        case 12: {
            return v3->field_1B8;
        }
        case 13: {
            return v3->gb_swing_raw;
        }
        case 14:
            return {};
        case 16:
            return v3->field_254;
        case 17:
            return v3->field_288;
            break;
        default:
            assert(0 && "should never ask for a button we don't have");

            return {};
        }
    }
    else
    {
        game_button result;
        game_button * (__fastcall *func)(void *, void *, game_button *, ai::controller_inode::eControllerButton) = CAST(func, 0x00467E10);
        func(this, nullptr, &result, a3);

        return result;
    }
}

} // namespace ai


void * __fastcall player_controller_inode__get_button(ai::player_controller_inode *self,
        void *,
        game_button *out,
        ai::controller_inode::eControllerButton a3)
{
    TRACE("player_controller_inode::get_button");
    // sp_log("%d", int(a3));

    game_button * (__fastcall *func)(void *, void *, game_button *, ai::controller_inode::eControllerButton) = CAST(func, 0x00467E10);
    func(self, nullptr, out, a3);

    return out;
}


void player_controller_inode_patch()
{
    set_vfunc(0x0087D3C8, player_controller_inode__get_button);
}
