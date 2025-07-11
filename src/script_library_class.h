#pragma once

#include <variable.h>

#include <list.hpp>

#include <cstdint>

struct mString;
struct vm_stack;

#define SLC_NAME_FIELD 1
#define SLC_FUNC_LIST_FIELD 0 

struct script_library_class {
    struct function {
        enum entry_t {
            FIRST_ENTRY,
            RECALL_ENTRY,
        };

        struct {
            void (__fastcall *finalize)(function *, void *edx, bool);
            bool (__fastcall *__cl)(const function *, void *edx, vm_stack &stack, entry_t entry);
        } *m_vtbl;

#if SLC_NAME_FIELD
        const char *m_name;
#endif

        //0x0058EE30
        function(const char *name);

        //0x0058EDE0
        function(script_library_class *a2, const char *a3);

        //0x006794B0
        /* virtual */ bool operator()(vm_stack &a2,
                                      script_library_class::function::entry_t a3) const;

        const char *get_name() const { 
#if SLC_NAME_FIELD
            return this->m_name;
#else
            return "";
#endif
        }
    };

    std::intptr_t m_vtbl;
    char *name;
    int field_8;
    const char *field_C;

#if SLC_FUNC_LIST_FIELD
    _std::list<script_library_class::function *> func_list;
#endif

    script_library_class::function **funcs = nullptr;
    int total_funcs;
    int next_func_slot;
    uint32_t field_1C;

    script_library_class() = default;

    //0x005AA860
    script_library_class(
        const char *a2,
        int a3,
        const char *a4,
        bool a5);

    bool operator<(const script_library_class &slc) const {
        return strcmp(this->get_name(), slc.get_name()) < 0;
    }

    void store_name(const char *a2);

    int get_size() const {
        return this->field_8;
    }

    const char *get_name() const;

    function *get_func(int index);

    bool are_funcs_from_mash() const
    {
        return (this->field_1C & 1) != 0;
    }

    void add_function(script_library_class::function *a2);


    void add_functions_complete();

    //virtual
    uint32_t find_instance(const mString &a1);
	


};

extern void verify_parms_integrity(
        script_library_class::function *func,
        vm_stack *the_stack,
        unsigned int *parms,
        int parms_size);

struct slc_str_t : script_library_class {};

struct slc_num_t : script_library_class {};

extern Var<script_library_class *> slc_global;

extern Var<slc_num_t *> slc_num;

extern Var<slc_str_t *> slc_str;

extern void script_library_class_patch();

#ifdef DEBUG
#define SLF_PARMS \
  assert( (sizeof(parms_t) & 3) == 0 ); \
  stack.pop(sizeof(parms_t)); \
  parms_t* parms = (parms_t*)stack.get_SP(); \
  verify_parms_integrity(this, &stack, (unsigned int *) parms, sizeof(parms_t)/4)
#else
#define SLF_PARMS \
  stack.pop(sizeof(parms_t)); \
  parms_t* parms = (parms_t*)stack.get_SP()
#endif

#define SLF_RETURN \
  stack.push((char*)&result,sizeof(result)) \

// Use this to be re-called next frame.
#define SLF_RECALL \
  return false

// Use this to signal termination.
#define SLF_DONE \
  return true
