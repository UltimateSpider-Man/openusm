#include "script_library_class.h"

#include "chuck_str.h"
#include "common.h"
#include "func_wrapper.h"
#include "memory.h"
#include "mstring.h"
#include "script_manager.h"
#include "slab_allocator.h"
#include "trace.h"
#include "utility.h"
#include "vtbl.h"
#include "variables.h"
#include "vm_stack.h"
#include "vm_thread.h"

#include <cassert>


#if SLC_NAME_FIELD
VALIDATE_SIZE(script_library_class::function, 0x8);
#else
VALIDATE_SIZE(script_library_class::function, 0x4);
#endif

#if SLC_FUNC_LIST_FIELD
VALIDATE_SIZE(script_library_class, 0x2C);
#else
VALIDATE_SIZE(script_library_class, 0x20);
#endif

Var<script_library_class *> slc_global{0x00965EC4};

Var<slc_num_t *> slc_num{0x00965ECC};

Var<slc_str_t *> slc_str{0x00965ED0};

script_library_class::script_library_class(
        const char *a2,
        int a3,
        const char *a4,
        bool a5)
{
    if constexpr (0)
    {}
    else
    {
        THISCALL(0x005AA860, this, a2, a3, a4, a5);
    }
}

void script_library_class::store_name(const char *a2)
{
    if ( this->name != nullptr ) {
        operator delete[](this->name);
    }

    auto len = strlen(a2);
    this->name = new char[len + 1];
    chuck_strcpy(this->name, a2, len + 1);
}

void verify_parms_integrity(
        script_library_class::function *func,
        vm_stack *the_stack,
        unsigned int *parms,
        int parms_size)
{
    for ( auto i = 0; i < parms_size; ++i )
    {
        if ( parms[i] == UNINITIALIZED_SCRIPT_PARM ) {
            auto v6 = mString {"uninitialized parameters in call to "} + func->get_name();
            the_stack->get_thread()->slf_error(v6);
            assert(0 && "uninitialized parameters in call to script library function");
        }
    }
}

bool script_library_class::function::operator()(vm_stack &a2,
                                                script_library_class::function::entry_t a3) const {
    bool __stdcall (
        *func)(vm_stack & a2,
               script_library_class::function::entry_t a3) = CAST(func, get_vfunc((int)m_vtbl, 0x4));

    return func(a2, a3);
}

const char *script_library_class::get_name() const {
    assert(name != nullptr);

    return this->name;
}

script_library_class::function *script_library_class::get_func(int index)
{
    assert(index >= 0);
    assert(index < total_funcs);
    
    return this->funcs[index];
}

void script_library_class::add_function(script_library_class::function *f)
{
    if ( g_is_the_packer() || script_manager::using_chuck_old_fashioned() ) {
#if SLC_FUNC_LIST_FIELD 
        if ( ! this->func_list.empty() ) {
            if ( this->func_list.back() != nullptr ) {
                assert(strcmp( func_list.back()->get_name(), f->get_name() ) < 0);
            }
        }

        this->func_list.push_back(f);
#endif
    } else {
        assert(total_funcs > 0);

        assert(are_funcs_from_mash());

        assert(funcs != nullptr);

        bool found = false;
        for ( auto i = this->next_func_slot; i < this->total_funcs; ++i ) {
            if ( this->funcs[i] != nullptr ) {
                found = true;
                this->next_func_slot = i;
                break;
            }
        }

        assert(found && "invalid mash!!!!");

        assert(next_func_slot < total_funcs);

        this->funcs[this->next_func_slot++] = f;
    }
}

void script_library_class::add_functions_complete()
{
    if ( g_is_the_packer() || script_manager::using_chuck_old_fashioned() ) {
        assert(this->funcs == nullptr);
    }
}

uint32_t script_library_class::find_instance(const mString &a1)
{
    uint32_t (__fastcall *func)(void *, void *, const mString *) = CAST(func, get_vfunc(this->m_vtbl, 0x4));

    return func(this, nullptr, &a1);
}

script_library_class::function::function(const char *name)
{
    if constexpr (1)
    {
        this->m_vtbl = CAST(this->m_vtbl, 0x0088EAFC);

#if SLC_NAME_FIELD
        this->m_name = name;
#endif

        assert(slc_global() != nullptr);

        slc_global()->add_function(this);

    } else {
        THISCALL(0x0058EE30, this, name);
    }
}

script_library_class::function::function(script_library_class *slc, const char *name) {
    TRACE("script_library_class::function::function", name);

    if constexpr (1) {

#if SLC_NAME_FIELD
        this->m_name = name;
#endif

        slc->add_function(this);
    } else {
        THISCALL(0x0058EDE0, this, slc, name);
    }
}

void script_library_class_patch() {
}
