#include "script_object.h"

#include "chunk_file.h"

#include "func_wrapper.h"
#include "memory.h"
#include "parse_generic_mash.h"
#include "script_executable.h"
#include "script_executable_entry.h"
#include "script_manager.h"
#include "vm_executable.h"
#include "vm_thread.h"
#include "common.h"
#include "trace.h"
#include "utility.h"

#include <cassert>

VALIDATE_SIZE(script_object, 0x34);
VALIDATE_SIZE(script_object::function, 0x10);

VALIDATE_SIZE(script_instance, 0x44);

VALIDATE_SIZE(vm_symbol, 0x4C);

script_object::script_object()
{
    this->instances = nullptr;
    this->flags = 0;
    this->constructor_common();
}

void script_object::constructor_common() {
    TRACE("script_object::constructor_common");

    assert(instances == nullptr);

    if constexpr (1) {
        auto *mem = mem_alloc(sizeof(*this->instances));
        this->instances = new (mem) simple_list<script_instance> {};
    } else {
        THISCALL(0x005A0750, this);
    }

    assert(instances != nullptr);
}

script_object::~script_object()
{
    if ( (this->flags & 2) != 0 ) {
        this->destructor_common();
    } else {
        this->destroy();
    }
}

void * script_object::operator new(size_t size)
{
    return mem_alloc(size);
}

void script_object::operator delete(void *ptr, size_t size)
{
    mem_dealloc(ptr, size);
}

void script_object::release_mem()
{
    TRACE("script_object::release_mem");

    this->destructor_common();
}

void script_object::destructor_common()
{
    TRACE("script_object::destructor_common");

    if ( this->instances != nullptr )
    {
        while ( !this->instances->empty() )
        {
            auto v5 = this->instances->begin();
            this->instances->common_erase(v5._Ptr);

            auto *v3 = v5._Ptr;
            if ( v3 != nullptr ) {
                delete v3;
            }
        }

        mem_dealloc(this->instances, sizeof(*this->instances));
        this->instances = nullptr;
    }

    this->global_instance = nullptr;
}

void script_object::destroy()
{
    if ( debug_info != nullptr )
    {
        this->debug_info->~debug_info_t();
        ::operator delete(debug_info);
        this->debug_info = nullptr;
    }

    if ( this->funcs != nullptr )
    {
        for ( auto i = 0; i < this->total_funcs; ++i )
        {
            auto &v5 = this->funcs[i];
            if ( v5 != nullptr ) {
                v5->~vm_executable();
                ::operator delete(v5);
            }
        }

        operator delete[](this->funcs);
        this->funcs = nullptr;
        this->total_funcs = 0;
    }

    this->destructor_common();
}

void script_object::create_destructor_instances()
{
	if constexpr (1)
    {
		this->global_instance = nullptr;
		if ( this->instances != nullptr )
		{
			for ( auto &v2 : (*this->instances) )
			{
				v2.massacre_threads(nullptr, nullptr);
				if ( v2.field_28 != nullptr )
				{
					if ( !v2.field_28->is_from_mash() )
					{
						v2.field_28->~vm_executable();
						::operator delete(v2.field_28);
					}

					v2.field_28 = nullptr;
				}
			}
		}

		if ( this->field_28 != -1
				&& this->field_28 < this->total_funcs
				&& this->instances != nullptr )
		{
			for ( auto &v3 : (*this->instances) ) {
				this->add_thread(&v3, this->field_28);
			}
		}
	}
    else
    {
		THISCALL(0x005AF320, this);
	}
}

void script_object::quick_un_mash()
{
    this->constructor_common();
    if ( this->is_global_object() ) {
        this->create_auto_instance(0.0);
    }
}

simple_list<vm_thread>::iterator script_instance::delete_thread(
        simple_list<vm_thread>::iterator a3)
{
    TRACE("script_instance::delete_thread");

    if constexpr (0)
    {
        auto *condemned = &(*a3);
        assert(condemned != nullptr);

        for ( auto &it : this->threads )
        {
            auto *v9 = &it;
            if ( v9 != condemned && v9->field_14 == condemned ) {
                v9->field_14 = nullptr;
            }
        }

        auto v8 = this->threads.erase(condemned);

        if ( condemned != nullptr ) {
            delete condemned;
            condemned = nullptr;
        }

        return v8;

    }
    else
    {
        using iterator_t = simple_list<vm_thread>::iterator;

        iterator_t it {};

        void (__fastcall *func)(void *, void *edx, iterator_t *, iterator_t) = CAST(func, 0x005AAE60);
        func(this, nullptr, &it, a3);
        return it;
    }
}

void script_instance::dump_threads_to_file(FILE *a2)
{
    TRACE("script_instance::dump_threads_to_file");

    for ( auto &v7 : this->threads )
    {
        if ( !v7.is_suspended() )
        {
            auto *exec = v7.get_executable();
            auto &name = exec->get_name();
            auto *v4 = name.to_string();
            auto *v3 = this->name.to_string();
            fprintf(a2, "%s %s\n", v3, v4);
        }
    }
}

void script_instance::run(bool a2)
{
    TRACE("script_instance::run");

    this->flags |= 2u;
    this->build_parameters();
    auto it = this->threads.begin();
    auto end = this->threads.end();
    while (it != end)
    {
        auto *t = it._Ptr;
        assert(t != nullptr);

        if ( (a2 || !t->is_suspended()) && t->run() ) {
            it = this->delete_thread(it);
        } else {
            ++it;
        }
    }
}

void script_instance::run_callbacks(
        script_instance_callback_reason_t a2,
        vm_thread *a3)
{
    TRACE("script_instance::run_callbacks");

    for ( auto &v1 : this->field_38 ) {
        this->m_callback(a2, this, a3, v1);
    }
}

void script_instance::build_parameters()
{
    TRACE("script_instance::build_parameters");

    if constexpr (1)
    {
        if ( this->field_28 != nullptr )
        {
            assert(parent != nullptr);

            static const string_hash inst_name {"__parms_builder"};
            auto *v6 = parent->add_instance(inst_name, nullptr, nullptr);
            vm_thread t {v6, this->field_28};
            t.run();
            this->parent->remove_instance(v6);
            t.inst = nullptr;
            t.ex = nullptr;
            auto *v12 = t.get_data_stack().buffer;
            assert(threads.size() == 1);

            auto *v11 = this->threads._first_element;
            auto constructor_parmsize = this->parent->get_constructor_parmsize();
            auto v4 = v12;
            auto &stack = v11->get_data_stack();
            stack.push(v4, constructor_parmsize);
            if ( !this->field_28->is_from_mash() )
            {
                auto *v10 = this->field_28;
                auto *v9 = v10;
                if ( v10 != nullptr ) {
                    v9->~vm_executable();
                    delete(v9);
                }
            }

            this->field_28 = nullptr;
        }
    }
    else
    {
        THISCALL(0x005AF500, this);
    }
}

int script_object::get_constructor_parmsize() {
    auto *func = this->get_func(0);
    auto v2 = func->get_parms_stacksize();
    if ( !func->is_static() ) {
        v2 -= 4;
    }

    return v2;
}

void script_object::run(bool a2)
{
    TRACE("script_object::run");

    for (auto &v1 : (*this->instances) ) {
        v1.run(a2);
    }
}

bool script_object::has_threads() const
{
    if (this->instances != nullptr)
	{
        for (auto &v1 : (*this->instances)) {
            if (v1.has_threads()) {
                return true;
            }
        }
    }

    return false;
}

void script_object::dump_threads_to_file(FILE *a2)
{
    TRACE("script_object::dump_threads_to_file");

    if ( this->instances != nullptr ) {
        for ( auto &v2 : (*this->instances) ) {
            v2.dump_threads_to_file(a2);
        }
    }
}

script_instance * script_object::add_instance(string_hash a2, char *a3, vm_thread **a4)
{
    TRACE("script_object::add_instance");

    assert(!this->is_global_object()
                && "please don't create global object instances with this method");

    if constexpr (1)
    {
        auto *inst = new script_instance {a2, this->data_blocksize, 0u};
        assert(inst != nullptr);

        this->add(inst);
        auto *con = this->get_func(0);
        assert(con->get_name() == name);

        auto *v9 = inst->add_thread(con);
        auto &stack = v9->get_data_stack();
        stack.push((char *) &inst, 4);

        auto v10 = con->get_parms_stacksize();
        if ( !con->is_static() ) {
            v10 -= 4;
        }

        if ( a3 != nullptr )
        {
            auto &stack = v9->get_data_stack();
            stack.push(a3, v10);
        }

        if ( a4 != nullptr ) {
            *a4 = v9;
        }

        return inst;
    } else {
        return (script_instance *) THISCALL(0x005AB120, this, a2, a3, a4);
    }
}

script_instance * script_object::add_instance(string_hash a2, vm_executable *parms_builder)
{
    TRACE("script_object::add_instance", a2.to_string());

    assert(!is_global_object() && "please don't create global object instances with this method");

    //assert(parms_builder->is_from_mash() && "this function should only be used for mashed parms_builders");

    if constexpr (0)
    {
        auto *inst = new script_instance {a2, this->data_blocksize, 0};
        assert(inst != nullptr);

        this->add(inst);

        auto *func = this->get_func(0);
        assert(func->get_name() == this->name);

        auto *t = inst->add_thread(func);
        auto &stack = t->get_data_stack();
        stack.push((const char *) &inst, 4);

        inst->field_28 = parms_builder;
        assert(parent != nullptr);

        if ( inst->field_28 != nullptr ) {
            inst->field_28->link(this->parent);
        }

        return inst;
    }
    else
    {
        script_instance * (__fastcall *func)(void *, void *edx, string_hash a2, vm_executable *parms_builder) = CAST(func, 0x005AB200);
        return func(this, nullptr, a2, parms_builder);
    }
}

void script_object::remove_instance(script_instance *a2)
{
    TRACE("script_object::remove_instance");

	assert(this->instances != nullptr);

	if constexpr (1)
	{
		for ( auto &v7 : (*this->instances) )
		{
			if ( (&v7) == a2 )
			{
				auto *v2 = &v7;
				if ( v2 == this->global_instance ) {
					this->global_instance = nullptr;
				}

				this->instances->common_erase({v2});

				delete v2;
				return;
			}
		}

		assert(0);
	}
    else
    {
		THISCALL(0x005ADC60, this, a2);
	}
}

script_instance * script_object::add_game_init_instance(string_hash a2, int a3)
{
    TRACE("script_object::add_game_init_instance");

    auto *inst = new script_instance {a2, this->data_blocksize, a3 | 4u};
    assert(inst != nullptr);

    this->add(inst);
    return inst;
}

void script_object::add(script_instance *a2)
{
    TRACE("script_object::add");

    if constexpr (1)
    {
        assert(instances != nullptr);

        a2->set_parent(this);
        this->instances->emplace_back(a2);
    }
    else
    {
        THISCALL(0x0059ECC0, this, a2);
    }
}

void script_object::link(const script_executable *a2)
{
    TRACE("script_object::link");

    for ( auto i = 0; i < this->total_funcs; ++i )
    {
        auto &x = this->funcs[i];
        x->link(a2);
    }
}

void script_object::un_mash(generic_mash_header *header, void *a3, void *a4, generic_mash_data_ptrs *a5)
{
    TRACE("script_object::un_mash");

    if constexpr (1)
    {
        this->parent = static_cast<script_executable *>(a3);
        assert(((int)header) % 4 == 0);

        this->static_data.un_mash(header, &this->static_data, a5);

        rebase(a5->field_0, 4u);

        this->funcs = a5->get<vm_executable *>(this->total_funcs);
        for ( auto i = 0; i < this->total_funcs; ++i )
        {
            rebase(a5->field_0, 4u);

            this->funcs[i] = a5->get<vm_executable>();

            assert(((int)header) % 4 == 0);
            this->funcs[i]->un_mash(header, this, this->funcs[i], a5);
        }

        this->constructor_common();
        if ( this->is_global_object() ) {
            this->create_auto_instance(Float{0.0});
        }

    }
    else
    {
        THISCALL(0x005AB350, this, header, a3, a4, a5);
    }

    sp_log("flags = 0x%08X", this->flags);
    //assert(this->debug_info == nullptr);
}

void script_object::create_auto_instance(Float a2)
{
    TRACE("script_object::create_auto_instance");

    if constexpr (1)
    {
        assert(this->instances != nullptr);

        auto &con = *this->get_func(0);
        assert(con.get_name() == name);

        if ( con.get_parms_stacksize() == 4 )
        {
            static string_hash auto_inst_name {int(to_hash("__auto"))};

            auto *inst = new script_instance {auto_inst_name, this->data_blocksize, 0};

            assert(inst != nullptr);
            inst->set_parent(this);

            if ( this->is_global_object() )
            {
                assert(global_instance == nullptr);

                this->instances->emplace_back(inst);
                this->global_instance = inst;
            }
            else
            {
                this->instances->push_back(inst);
            }

            auto *new_thread = inst->add_thread(&con);
            if ( this->is_global_object() ) {
                auto &stack = new_thread->get_data_stack();
                stack.push(a2);
            } else {
                auto &stack = new_thread->get_data_stack();
                stack.push((const char *)&inst, 4);
            }
        }
    }
    else
    {
        THISCALL(0x005AAEF0, this, a2);
    }
}

vm_executable * script_object::get_func(int i)
{
    assert(funcs != nullptr);

    assert(i >= 0);

    assert(i < total_funcs);

    return this->funcs[i];
}

int script_object::get_size_instances() const
{
    return ( this->instances == nullptr
            ? 0
            : this->instances->size()
            );
}

int script_object::find_func(string_hash a2) const
{
    TRACE("script_object::find_func", a2.to_string());

    if constexpr (1) {
        const auto v14 = a2.source_hash_code % 20;

        auto idx = v14;
        auto v12 = 0x7FFFFFFF;
        auto lru_index = -1;
        while ( function_cache()[idx].field_8 != -1 ) {
            if ( function_cache()[idx].field_0 == this 
                && function_cache()[idx].field_4 == a2 )
            {
                static int dword_1597B60 {0};
                ++dword_1597B60;
                function_cache()[idx].field_C = usage_counter()++;
                auto v4 = function_cache()[idx].field_8;
                return v4;
            }

            if ( v12 > function_cache()[idx].field_C ) {
                v12 = function_cache()[idx].field_C;
                lru_index = idx;
            }

            if ( (int)++idx >= 20 ) {
                idx = 0;
            }

            if ( idx == v14 ) {
                goto LABEL_12;
            }
        }

        lru_index = idx;
        LABEL_12:

        static int dword_1597B64 {0};
        ++dword_1597B64;
        for ( auto i = 0; i < this->total_funcs; ++i ) {
            auto &v9 = this->funcs[i];
            auto v3 = v9->get_fullname();
            if ( v3 == a2 ) {
                assert(lru_index != -1);
                function_cache()[lru_index].field_0 = this;
                function_cache()[lru_index].field_8 = i;
                function_cache()[lru_index].field_4 = a2;
                function_cache()[lru_index].field_C = usage_counter()++;
                return i;
            }
        }

        return -1;

    } else {
        return THISCALL(0x0058EF80, this, a2);
    }
}

int script_object::find_func_short(string_hash a2) const
{
    for ( int i = 0; i < this->total_funcs; ++i )
    {
        auto v3 = this->funcs[i]->get_name();
        if ( v3 == a2 ) {
            return i;
        }
    }

    return -1;
}

int script_object::find_function_by_address(const uint16_t *a2) const
{
    //TRACE("script_object::find_function_by_address");

    for ( auto i = 0; i < this->total_funcs; ++i )
    {
        auto &v4 = this->funcs[i];
        if ( v4 != nullptr )
        {
            if ( a2 >= v4->get_start() )
            {
                auto *v2 = v4->get_start();
                if ( a2 < v2 + v4->get_size() ) {
                    return i;
                }
            }
        }
    }

    return -1;
}

void vm_symbol::read(chunk_file *file) {
    this->field_0 = file->read<mString>();
    this->field_C = file->read<mString>();

    this->field_30 = file->read<int>();
    this->field_34 = file->read<int>();

    this->field_38 = file->read<bool>();

    this->field_18 = file->read<mString>();
    this->field_24 = file->read<mString>();
}

void script_object::read(chunk_file *file, script_object *so)
{
    TRACE("script_object::load");

    auto *mem = mem_alloc(sizeof(debug_info_t));
    so->debug_info = new (mem) debug_info_t{}; 
    assert(so->debug_info != nullptr);

    assert(so->parent != nullptr);

    chunk_flavor cf = file->read<chunk_flavor>();

    sp_log("so->flags = 0x%08X", so->flags);
    if ( cf == CHUNK_EXTERNAL ) {
        so->flags |= SCRIPT_OBJECT_FLAG_EXTERNAL;
        cf = file->read<chunk_flavor>();
    }

    if ( cf == CHUNK_GLOBAL ) {
        so->flags |= SCRIPT_OBJECT_FLAG_GLOBAL;
    }

    if ( cf == CHUNK_STANDARD ) {
        cf = file->read<chunk_flavor>();
        if ( cf == CHUNK_PARENT )
        {
            auto v39 = file->read<uint32_t>();
            auto *system_string = so->parent->get_system_string(v39);
            so->debug_info->field_0 = string_hash {system_string};
            cf = file->read<chunk_flavor>();
        }
    } else {
        cf = file->read<chunk_flavor>();
    }

    if ( cf == CHUNK_NSTATIC ) {
        auto i = file->read<int>();
        while ( i ) {
            vm_symbol v38{};
            v38.read(file);

            so->debug_info->field_4.push_back(v38);
            --i;
        }

        cf = file->read<chunk_flavor>();
    }

    sp_log("%s", cf.field_0);
    assert(cf == CHUNK_STATIC_BLOCKSIZE);

    so->static_data = file->read<int>();
    cf = file->read<chunk_flavor>();

    while ( cf == CHUNK_STAT_INIT ) {
        auto v36 = file->read<int>();
        auto v35 = file->read<int>();
        auto *buffer = so->static_data.get_buffer();
        auto *v34 = (float *) &buffer[v36];
        if ( v35 != 0 )
        {
            if ( v35 == 1 )
            {
                auto v31 = file->read<float>();
                *v34 = v31;
            }
            else if ( v35 == 2 )
            {
                auto v30 = file->read<int>();
                auto *str = so->parent->get_system_string(v30);
                auto *pso = so->parent->find_object(string_hash {str}, nullptr);
                assert(pso);

                v30 = file->read<int>();
                [[maybe_unused]] auto *v27 = pso->parent->get_system_string(v30);
                auto v26 = 0;
                chunk_flavor v25 {"UNREG"};
                v25 = file->read<chunk_flavor>();
                if ( v25 == CHUNK_PARMS )
                {
                    //v26 = pso->sub_6870D3(string_hash {v27}, file, nullptr);
                }
                else if ( v25 == CHUNK_NULL )
                {
                    //v26 = (string_hash *)pso->add_instance(string_hash {v27}, this, nullptr);
                }
                else if ( v25 == CHUNK_GAME_INIT )
                {
                    //v26 = (string_hash *)pso->sub_68757E(string_hash {v27}, nullptr);
                }
                else
                {
                    assert(0 && "bad sx file");
                }

                *bit_cast<DWORD *>(v34) = v26;
            }
        }
        else
        {
            auto v33 = file->read<int>();
            auto *permanent_string = so->parent->get_permanent_string(v33);
            *(DWORD *)v34 = (int)permanent_string;
        }

        cf = file->read<chunk_flavor>();
    }

    if ( cf == CHUNK_NDATA ) {
        auto i = file->read<int>();
        while ( i != 0 ) {
            vm_symbol v23{};
            v23.read(file);
            so->debug_info->field_10.push_back(v23);
            --i;
        }

        cf = file->read<chunk_flavor>();
    }

    assert(cf == CHUNK_DATA_BLOCKSIZE);

    so->data_blocksize = file->read<int>();
    cf = file->read<chunk_flavor>();
    if ( cf == chunk_flavor {"desidx"} ) {
        so->field_28 = file->read<int>();
        cf = file->read<chunk_flavor>();
    } else {
        so->field_28 = -1;
    }

    assert(cf == CHUNK_FUNCS);
    so->total_funcs = file->read<int>();
    sp_log("so->total_funcs = %d", so->total_funcs);
    if ( so->total_funcs > 0 ) {
        so->funcs = (vm_executable **)operator new(4 * so->total_funcs);
        assert(so->funcs != nullptr);

        for ( auto i = 0; i < so->total_funcs; ++i )
        {
            auto *x = new vm_executable {so};
            assert(x != nullptr);

            vm_executable::read(file, x);
            so->funcs[i] = x;
        }
    }

    if ( so->is_global_object() ) {
        so->create_auto_instance(0.0);
    }
}

script_instance::script_instance(
        string_hash a2,
        int size,
        unsigned int a4) : name(a2),
                            data(size),
                            field_28(nullptr),
                            parent(nullptr),
                            flags(a4)
{
    TRACE("script_instance::script_instance", a2.to_string());
    sp_log("%d", size);
}

script_instance::~script_instance()
{
    TRACE("script_instance::~script_instance");

    this->run_callbacks(static_cast<script_instance_callback_reason_t>(0), nullptr);

    while ( !this->threads.empty() )
    {
        auto *t = &(*this->threads.begin());
        this->threads.common_erase(t);

        t->~vm_thread();
        vm_thread::pool().remove(t);
    }
}

void * script_instance::operator new(size_t size) {
    return mem_alloc(size);
}

void script_instance::operator delete(void *ptr, size_t size) {
    mem_dealloc(ptr, size);
}

bool script_instance::run_single_thread(vm_thread *a2, bool a3)
{
    TRACE("script_instance::run_single_thread");

    if constexpr (0)
    {
        this->flags |= 2u;
        bool v4 = false;
        auto *inst = a2->get_instance();
        auto *so = inst->get_parent();
        auto *parent = so->get_parent();
        auto *entry = script_manager::find_entry(parent);
        assert(entry != nullptr);

        script_manager::run_callbacks(static_cast<script_manager_callback_reason>(10), parent, entry->field_8);
        if ( (a3 || !a2->is_suspended()) && a2->run() )
        {
            auto end = this->threads.end();
            for (auto it = this->threads.begin(); it != end; ++it )
            {
                if ( &(*it) == a2 ) {
                    this->delete_thread(it);
                }
            }

            v4 = true;
        }

        script_manager::run_callbacks(static_cast<script_manager_callback_reason>(11), parent, entry->field_8);
        return v4;
    }
    else
    {
        return THISCALL(0x005AF100, this, a2, a3);
    }
}

void script_instance::register_callback(
    void (*cb)(script_instance_callback_reason_t, script_instance *, vm_thread *, void *),
    void *user_data)
{
    assert(cb != nullptr);

    if constexpr (0)
    {
        this->m_callback = cb;

        decltype(this->field_38)::ret_t ret;

        void (__fastcall *func)(void *, void *edx, decltype(ret) *, void **) = CAST(func, 0x005B50E0);

        func(&this->field_38, nullptr,
           &ret,
           &user_data);

        assert(ret.second && "tried to insert user_data more than once!!!");
    }
    else
    {
        THISCALL(0x005A33F0, this, cb, user_data);
    }
}

vm_thread *script_instance::add_thread(const vm_executable *ex, const char *parms)
{
    TRACE("script_instance::add_thread");

    auto *nt = this->add_thread(ex);
    assert(nt != nullptr);

    if ( parms != nullptr ) {
        auto v5 = ex->get_parms_stacksize();
        nt->get_data_stack().push(parms, v5);
    }

    nt->PC = ex->buffer;
    return nt;
}

void script_instance::add_thread(void *a2, const vm_executable *a3, const char *a4)
{
    if constexpr (0)
    {
        auto *nt = new vm_thread {this, a3, a2};

        assert(nt != nullptr);

        this->threads.emplace_back(nt);

        if ( (this->flags & 1) != 0 ) {
            nt->set_suspended(true);
        }

        if ( a4 != nullptr )
        {
            auto parms_stacksize = a3->get_parms_stacksize();
            auto &data_stack = nt->get_data_stack();

            data_stack.push(a4, parms_stacksize);
        }

        nt->PC = a3->get_start();
    }
    else
    {
        THISCALL(0x005AAD50, this, a2, a3, a4);
    }
}

void script_instance::recursive_massacre_threads(vm_thread *root)
{
    assert(root != nullptr);

    auto it = this->threads.begin();
    auto end = this->threads.end();
    while (it != end) 
    {
        auto *t = &(*it);
        assert(t != nullptr);

        if ( t->field_14 == root )
        {
            assert(t != root);

            this->recursive_massacre_threads(t);
            it = this->delete_thread(it);
        }
        else
        {
            ++it;
        }
    }
}

void script_instance::massacre_threads(const vm_executable *a2, const vm_thread *a3)
{
    TRACE("script_instance::massacre_threads");

    if constexpr (1)
    {
        if ( a2 != nullptr )
        {
            auto it = this->threads.begin();
            auto end = this->threads.end();
            while ( it != end )
            {
                auto *t = &(*it);
                assert(t != nullptr);

                bool v9 = false;
                if ( t != a3 )
                {
                    auto v8 = t->get_executable()->get_name();
                    if ( a2->get_name() == v8 ) {
                        v9 = true;
                    }
                }

                if ( v9 )
                {
                    this->recursive_massacre_threads(t);
                    it = this->delete_thread(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        else
        {
            auto it = this->threads.begin();
            auto end = this->threads.end();
            while (it != end) 
            {
                auto *t = &(*it);
                assert(t != nullptr);

                if ( t == a3 ) {
                    ++it;
                } else {
                    it = this->delete_thread(it);
                }
            }
        }
    } else {
        THISCALL(0x005ADB80, this, a2, a3);
    }
}

void script_instance::kill_thread(const vm_executable *a2, const vm_thread *a3)
{
    TRACE("script_instance::kill_thread");

    if constexpr (1)
    {
        auto it = this->threads.begin();
        auto end = this->threads.end();
        while ( it != end )
        {
            auto *t = &(*it);
            assert(t != nullptr);

            bool v7 = false;
            if ( t->get_instance() == this && t != a3 )
            {
                auto v6 = a2->get_name();
                if ( t->get_executable()->get_name() == v6 ) {
                    v7 = true;
                }
            }

            if ( v7 ) {
                it = this->delete_thread({t});
            } else {
                ++it;
            }
        }
    }
    else
    {
        THISCALL(0x005AD8D0, this, a2, a3);
    }
}

vm_thread *script_instance::add_thread(const vm_executable *a2)
{
    TRACE("script_instance::add_thread");

    if constexpr (1)
    {
        auto *nt = new vm_thread {this, a2};
        assert(nt != nullptr);

        this->threads.emplace_back(nt);

        if ( (this->flags & 1) != 0 ) {
            nt->set_suspended(true);
        }

        return nt;
    }
    else
    {
        vm_thread * (__fastcall *func)(void *, void *edx, const vm_executable *a2) = CAST(func, 0x005AAC20);
        return func(this, nullptr, a2);
    }
}

vm_thread *script_object::add_thread(script_instance *a2, int fidx)
{
	assert(fidx < this->total_funcs);

	auto *t = a2->add_thread(this->funcs[fidx]);
	auto &stack = t->get_data_stack();
	stack.push((const char *)&a2, 4);
	return t;
}

bool script_instance::has_threads() const
{
    TRACE("script_instance::has_threads");

    return (this->threads.size() != 0);
}

void script_instance_patch()
{
    {
        FUNC_ADDRESS(address, &script_instance::run);
        SET_JUMP(0x005AF660, address);
    }

    {
        FUNC_ADDRESS(address, &script_object::create_auto_instance);
        SET_JUMP(0x005AAEF0, address);
    }

    {
        FUNC_ADDRESS(address, &script_object::find_func);
        SET_JUMP(0x0058EF80, address);
    }
}
