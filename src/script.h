#pragma once

#include "string_hash.h"
#include "variable.h"

#include "func_wrapper.h"

struct script_instance;
struct script_object;
struct vm_thread;

namespace script {

//0x0064E4F0
extern int find_function(string_hash a1, const script_object *a2, bool a3);

inline Var<script_object *> gso {0x0096BB4C};

inline Var<script_instance*> gsoi {0x0096BB50};

inline script_instance * get_gsoi()
{
    return script::gsoi();
}

inline script_object * get_gso()
{
    return script::gso();
}

inline bool exec_thread(bool a1) {
    return (bool)CDECL_CALL(0x0064E740, a1);
}

// 0x0064E770
inline bool exec_thread_no_wait() {
    return (bool)CDECL_CALL(0x0064E770);
}



inline vm_thread * new_thread(int a1, script_instance *a2){
    return (vm_thread *)CDECL_CALL(0x0064E520, a1, a2);
}


inline vm_thread* sub_5028B0(string_hash function_hash, script_instance* instance)
{
     return (vm_thread*)CDECL_CALL(0x005028B0, function_hash, instance);
}

} // namespace script

extern void script_patch();
