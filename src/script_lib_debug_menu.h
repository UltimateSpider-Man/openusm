#pragma once

#include "script_library_class.h"

#include "vm_stack.h"

struct slf__create_debug_menu_entry__str__str__t : script_library_class::function {
    slf__create_debug_menu_entry__str__str__t(const char *a3);

    struct parms_t {
        vm_str_t str0;
        vm_str_t str1;
    };

    bool operator()(vm_stack &stack, [[maybe_unused]]script_library_class::function::entry_t entry) const;
};

void construct_debug_menu_lib();

extern void script_lib_debug_menu_patch();
