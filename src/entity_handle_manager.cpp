#include "entity_handle_manager.h"

#include "common.h"
#include "entity_base.h"
#include "entity_base_vhandle.h"
#include "func_wrapper.h"
#include "log.h"
#include "mstring.h"
#include "osassert.h"
#include "trace.h"
#include "utility.h"
#include "variables.h"
#include "wds.h"

#include <cassert>
#include <map.hpp>

using map = _std::map<int, string_hash>;

VALIDATE_SIZE(map, 0xC);
VALIDATE_SIZE(map::_Mybase, 0xC);

using Tree_nod = decltype(*map::iterator::_Ptr);

VALIDATE_SIZE(Tree_nod, 0x18);

VALIDATE_OFFSET(map::_Mybase, _Myhead, 0x4);
VALIDATE_OFFSET(map::_Mybase, _Mysize, 0x8);

VALIDATE_SIZE(*map::_Mybase::_Myhead, 0x18);

Var<entity_slot *> entity_handle_manager::ent_slots{0x0095A0D0};

Var<entity_slot *> entity_handle_manager::ENTS{0x0095A6F4};

Var<int> entity_handle_manager::curr_idx{0x0095BBD8};

Var<bool> entity_handle_manager::check_world_lists{0x0091FE64};

Var<simple_queue<int, 32>> entity_handle_manager::free_slot_indices{0x0091FFC0};

Var<stdext::hash_map<string_hash, entity_base *>> entity_handle_manager::the_map{0x0095B79C};

VALIDATE_SIZE(entity_handle_manager::the_map(), 40);

VALIDATE_SIZE(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mybase, 40);
VALIDATE_SIZE(std::decay_t<decltype(entity_handle_manager::the_map())>::_Myvec, 16);
VALIDATE_SIZE(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mylist, 12);

VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mybase, _List, 0x4);
VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mybase, _Vec, 0x10);
VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mybase, _Mask, 0x20);
VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mybase, _Maxidx, 0x24);

VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Myvec, m_first, 0x4);
VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Myvec, m_last, 0x8);
VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Myvec, m_end, 0xC);

VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mylist, m_head, 0x4);
VALIDATE_OFFSET(std::decay_t<decltype(entity_handle_manager::the_map())>::_Mylist, m_size, 0x8);

Var<void *> dword_95B7A4{0x0095B7A4};

Var<const char *[30]> entity_flavor_names { 0x0091FD38 };

entity_base *entity_handle_manager::find_entity(const string_hash &arg0,
                                                entity_flavor_t a2,
                                                bool a3) {
    TRACE("entity_handle_manager::find_entity", arg0.to_string());

    entity_base *result = nullptr;
    if constexpr (1)
    {
        using map = std::decay_t<decltype(the_map())>;
        using iterator = map::iterator;

        iterator v13;
        THISCALL(0x00506790, &the_map(), &v13, &arg0);
        if ( !a3 )
        {
            if ( v13 == the_map().end() )
            {
                auto *v3 = arg0.to_string();
                mString v4 {v3};
                mString v5 {"Unable to find entity "};
                auto out = v5 + v4;
                sp_log("%s", out.c_str());
            }

        }

        if ( v13 == the_map().end() )
        {
            return nullptr;
        }

        if ( a2 != IGNORE_FLAVOR && v13._Ptr->_Myval.second->get_flavor() != a2 )
        {
            auto v21 = mString {"Entity "}
                        + mString {arg0.to_string()}
                        + " is not a "
                        + entity_flavor_names()[a2];
            sp_log("%s", v21.c_str());
            assert(0);
        }

        return v13._Ptr->_Myval.second;
    }
    else {
        result = (entity_base *) CDECL_CALL(0x004DC300, &arg0, a2, a3);
    }

    assert(result != nullptr);
    return result;
}



/*

extern "C" {
#include "utility/lua.h"
#include "utility/lauxlib.h"
#include "utility/lualib.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

static lua_State* g_L;

entity_base* __cdecl find_entity(const string_hash* a1, entity_flavor_t a2, bool a3)
{
    return (entity_base*)CDECL_CALL(0x004DC300, a1, a2, a3);
}


int lua_find_entity(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    string_hash hash = { to_hash(name) };
    entity_base* entity = find_entity(&hash, (entity_flavor_t)(SWITCH | BEAM), 0);
    if (!entity)
        return luaL_error(L, "Entity '%s' not found", name);

    lua_pushlightuserdata(L, entity);
    return 1;
}
*/
void entity_handle_manager::deregister_entity(entity_base *a1) {
    TRACE("deregister_entity %s", a1->field_10.to_string());

    if constexpr (0) {
        if (a1->field_10 != ANONYMOUS) {
#if 0
            sub_506790(&entity_handle_manager::the_map(), &a1, (unsigned int *) &a1->field_10);
            if (a1 != bit_cast<entity_base *>(dword_95B7A4())) {
                sub_56CAA0(&entity_handle_manager::the_map(), (int) &a1, a1);
            }
#else
            entity_handle_manager::the_map().erase(a1->field_10);
#endif
        }
    } else {
        CDECL_CALL(0x004DC510, a1);
    }
}

void entity_handle_manager::register_entity(entity_base *a1)
{
    TRACE("entity_handle_manager::register_entity", a1->field_10.to_string());

    if constexpr (1)
    {
        if (a1->field_10.source_hash_code != ANONYMOUS.source_hash_code) {
            const _std::pair<string_hash, entity_base *> value{a1->field_10, a1};

            using map = std::decay_t<decltype(the_map())>;
            using iterator = map::iterator;
            using pair = _std::pair<iterator, bool>;

            VALIDATE_SIZE(value, 8);
            VALIDATE_SIZE(pair, 8);

            pair result;
            if constexpr (1) {
                THISCALL(0x00509440, &the_map(), &result, &value);
            } else {
                result = the_map().insert(value);
            }

            if (!g_is_the_packer() && !result.second) {
                const char *v1 = a1->field_10.to_string();

                mString v2{v1};

                mString v4 = mString {"Same entity name appears twice: "} + v2;

                sp_log("%s", v4.c_str());
            }
        }
    } else {
        CDECL_CALL(0x004EF970, a1);
    }
}

int entity_slot::occupy(entity_base *a2) {
    assert(!in_use());

    this->my_id += 0x4000;
    this->my_ptr = a2;
    return this->my_id;
}



void entity_slot::vacate(const entity_base_vhandle &ent_handle)
{
    int which_slot = ent_handle.field_0 & 0x3FFF;
    assert(which_slot < MAX_ENTITIES);

    auto ENTITY_HANDLE_ID = [](const auto &ent_handle)
    {
        return ent_handle.field_0;
    };

    assert(this->my_id == ENTITY_HANDLE_ID( ent_handle ));

    assert(this->my_ptr != nullptr);

    assert(this->my_ptr->get_my_vhandle().get_goodies() == ent_handle.get_goodies());
    this->my_ptr = nullptr;
}

void entity_handle_manager::create_inst()
{
    TRACE("entity_handle_manager::create_inst");

    if constexpr (1) {
        ent_slots() = new entity_slot[10240];
        ENTS() = ent_slots();
        clear_ent_slots();
        free_slot_indices().field_4 = 0;
        free_slot_indices().field_6 = -1;
        free_slot_indices().m_count = 0;
        check_world_lists() = true;
    } else {
        CDECL_CALL(0x004D0F20);
    }
}

void entity_handle_manager::clear_ent_slots() {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        auto *v1 = &ent_slots()[i];
        v1->my_id = i;
        v1->my_ptr = nullptr;
    }

    for (int i = 0; i < 32; ++i) {
        auto v3 = ++free_slot_indices().field_6;
        if (free_slot_indices().field_6 >= free_slot_indices().size) {
            v3 = 0;
            free_slot_indices().field_6= 0;
        }

        free_slot_indices().field_0[v3] = i;
        ++free_slot_indices().m_count;
    }
}

void entity_handle_manager::remove_entity(const entity_base_vhandle &ent_handle)
{
    if ( g_world_ptr != nullptr && check_world_lists() )
    {
        assert("Entity handle is being released while entity still exists in lists" && !g_world_ptr->ent_mgr.is_entity_valid( (entity *) ent_handle.field_0 ));

        assert("Item handle is being released while item still exists in lists" && !g_world_ptr->ent_mgr.is_item_valid( (item *) ent_handle.field_0 ));
    }

    int which_slot = ent_handle.field_0 & 0x3FFF;
    assert(which_slot < MAX_ENTITIES);

    assert(ent_slots()[which_slot].in_use());

    ent_slots()[which_slot].vacate(ent_handle);
    if ( free_slot_indices().get_unused_capacity() > 0 )
    {
        free_slot_indices().push_back(which_slot);
    }
}

entity_base *find_marker(const string_hash &a1)
{
    TRACE("find_marker", a1.to_string());
    return entity_handle_manager::find_entity(a1, MARKER, false);
}

mic *find_mic(const string_hash &a1) {
    return (mic *) entity_handle_manager::find_entity(a1, MIC, false);
}

void entity_handle_manager::delete_inst() {
    if constexpr (0) {
        operator delete[](entity_handle_manager::ent_slots());
        entity_handle_manager::ent_slots() = nullptr;
    } else {
        CDECL_CALL(0x004C2A80);
    }
}

void sub_4CCEA0() {
    if constexpr (0) {
        while (entity_handle_manager::free_slot_indices().get_unused_capacity() != 0)
        {
            assert(entity_handle_manager::curr_idx() < MAX_ENTITIES);

            if ( !entity_handle_manager::ent_slots()[entity_handle_manager::curr_idx()].in_use() )
            {
                entity_handle_manager::free_slot_indices().push_back(entity_handle_manager::curr_idx());
            }

            if (++entity_handle_manager::curr_idx() >= MAX_ENTITIES) {
                entity_handle_manager::curr_idx() = 0;
            }

        }

    } else {
        CDECL_CALL(0x004CCEA0);
    }
}

int entity_handle_manager::get_free_slot()
{
    if ( free_slot_indices().count() == 0 )
        sub_4CCEA0();

    if (free_slot_indices().count() == 0)
    {
        error("We have exceeded our maximum number of entities %d... Impressive!", 10240);
    }

    return free_slot_indices().pop_front();
}

int entity_handle_manager::add_entity(entity_base *ent_to_add)
{
    TRACE("entity_handle_manager::add_entity");

    if constexpr (1)
    {
        static constexpr entity_base_vhandle INVALID_VHANDLE {0};

        assert(ent_to_add != nullptr);
        assert(ent_to_add->get_my_vhandle() == INVALID_VHANDLE && "re-adding an entity!");

        auto slot_to_use = entity_handle_manager::get_free_slot();

        assert(slot_to_use >= 0);
        assert(slot_to_use < MAX_ENTITIES);
        assert(!ent_slots()[slot_to_use].in_use());
        assert(ent_slots()[slot_to_use].my_ptr == nullptr);

        auto &v2 = ent_slots()[slot_to_use];

        return v2.occupy(ent_to_add);
    } else {
        return CDECL_CALL(0x004CCF60, ent_to_add);
    }
}

void entity_handle_manager_patch()
{
    SET_JUMP(0x004CCF60, entity_handle_manager::add_entity);

    REDIRECT(0x005AD2E4, entity_handle_manager::create_inst);

    {
        REDIRECT(0x004F33B0, entity_handle_manager::register_entity);
        REDIRECT(0x004F34F4, entity_handle_manager::register_entity);
        REDIRECT(0x004FCC19, entity_handle_manager::register_entity);
        REDIRECT(0x004FCF7B, entity_handle_manager::register_entity);
        REDIRECT(0x00509DF9, entity_handle_manager::register_entity);
    }

    REDIRECT(0x0055B470, find_marker);
    return;

    REDIRECT(0x004F365B, entity_handle_manager::deregister_entity);

    {
        REDIRECT(0x004DCE59, entity_handle_manager::find_entity);
        REDIRECT(0x0055D23E, entity_handle_manager::find_entity);
    }

}
