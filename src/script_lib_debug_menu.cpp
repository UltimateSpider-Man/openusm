#include "script_lib_debug_menu.h"

#include "debug_menu.h"
#include "mstring.h"
#include "script_executable.h"
#include "script_manager.h"
#include "script_object.h"
#include "trace.h"
#include "utility.h"
#include "vm_executable.h"
#include "vm_thread.h"

static debug_menu *script_menu = nullptr;

static debug_menu *progression_menu = nullptr;

static int vm_debug_menu_entry_garbage_collection_id = -1;

void init_script_debug_menu()
{
    if ( script_menu == nullptr )
    {
        script_menu = new debug_menu {"Script", (DWORD)debug_menu::sort_mode_t::undefined};

        progression_menu = new debug_menu {"Progression", (DWORD)debug_menu::sort_mode_t::undefined};

        debug_menu::root_menu->add_entry(script_menu);
        debug_menu::root_menu->add_entry(progression_menu);
    }
}

void vm_debug_menu_entry_garbage_collection_callback(script_executable *,
                                                    _std::list<uint32_t> &a2,
                                                    _std::list<mString> &)
{
    for ( auto &v2 : a2 )
    {
        assert(script_menu != nullptr);

        auto *entry = bit_cast<debug_menu_entry *>(v2);
        
        // script_menu->remove_entry(entry);
        remove_debug_menu_entry(entry);
    }
}

void construct_debug_menu_lib()
{
    if ( vm_debug_menu_entry_garbage_collection_id == -1 ) {
        vm_debug_menu_entry_garbage_collection_id = script_manager::register_allocated_stuff_callback(vm_debug_menu_entry_garbage_collection_callback);
    }
}

slf__create_debug_menu_entry__str__str__t::slf__create_debug_menu_entry__str__str__t(const char *a3) : function(a3)
{
    m_vtbl = CAST(m_vtbl, 0x0089C70C);
    FUNC_ADDRESS(address, &slf__create_debug_menu_entry__str__str__t::operator());
    m_vtbl->__cl = CAST(m_vtbl->__cl, address);
}

bool slf__create_debug_menu_entry__str__str__t::operator()(vm_stack &stack, [[maybe_unused]]script_library_class::function::entry_t entry) const
{
    TRACE("slf__create_debug_menu_entry__str__str__t::operator()");

    if constexpr (0)
    {
        SLF_PARMS;

        init_script_debug_menu();
        assert(script_menu != nullptr);

        mString v14 {parms->str0};
        auto *result = new debug_menu_entry {v14};

        mString v15 {parms->str1};
        auto *nt = stack.get_thread();
        auto *v4 = nt->get_instance();
        result->set_script_handler(v4, v15);
        mString v16 {};
        uint32_t v11 = int(result);
        auto v10 = vm_debug_menu_entry_garbage_collection_id;
        auto *v6 = nt->get_executable();
        auto *so = v6->get_owner();
        auto *v8 = so->get_parent();

        v8->add_allocated_stuff(v10, v11, v16);
        script_menu->add_entry(result);

        SLF_RETURN;
        SLF_DONE;
    }
    else
    {
        bool (__fastcall *func)(const void *, void *edx, vm_stack *, entry_t) = CAST(func, 0x00678210);
        return func(this, nullptr, &stack, entry);
    }
}

void script_lib_debug_menu_patch()
{
    REDIRECT(0x0089C710, construct_debug_menu_lib);
}
