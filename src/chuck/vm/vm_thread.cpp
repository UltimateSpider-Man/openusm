#include "vm_thread.h"

#include "chuck_str.h"
#include "script_object.h"

#include <cassert>

#include <common.h>
#include <trace.h>
#include <utility.h>
#include <vtbl.h>

#include <func_wrapper.h>
#include <script_manager.h>

#include "vm_executable.h"

VALIDATE_SIZE(vm_thread, 0x1E8);
VALIDATE_OFFSET(vm_thread, dstack, 0x20);
VALIDATE_OFFSET(vm_thread, entry, 0x1D8);

Var<char[64][256]> vm_thread::string_registers{0x00961940};

Var<void (*)(vm_thread *, string_hash)> dword_965F20 {0x00965F20};

Var<fixed_pool> vm_thread::pool {0x00922D58};

vm_thread::vm_thread(script_instance *a2, const vm_executable *a3) : dstack(this)
{
    TRACE("vm_thread::vm_thread");

    if constexpr (1)
    {
        this->ex = a3;
        this->inst = a2;
        this->field_14 = nullptr;
        this->field_18 = 0;
        this->flags = SUSPENDABLE;

        this->PC = this->ex->get_start();
        this->field_1DC = nullptr;
        this->field_1E0 = 0;
        this->field_1E4 = ++id_counter();
        this->PC_stack.reserve(4u);
        this->field_1B0 = nullptr;
        this->field_1B4 = 0.0;
    }
    else
    {
        THISCALL(0x005A5420, this, a2, a3);
    }

    sp_log("%d", this->dstack.size());
}

vm_thread::vm_thread(script_instance *a2, const vm_executable *a3, void *a4) : dstack(this)
{
    TRACE("vm_thread::vm_thread");

    if constexpr(1)
    {
        this->ex = a3;
        this->inst = a2;
        this->field_14 = nullptr;
        this->field_18 = 0;
        this->flags = SUSPENDABLE;

        this->PC = this->ex->get_start();
        this->field_1DC = a4;
        this->field_1E0 = 0;
        this->field_1E4 = ++id_counter();
        this->PC_stack.reserve(4u);
        this->field_1B0 = nullptr;
        this->field_1B4 = 0.0;
    }
    else
    {
        THISCALL(0x005A5500, this, a2, a3, a4);
    }

    sp_log("%d", this->dstack.size());
}

vm_thread::~vm_thread()
{
    if ( this->inst != nullptr ) {
        this->inst->run_callbacks(static_cast<script_instance_callback_reason_t>(1), this);
    }

    while ( this->field_1C8.size() != 0 )
    {
        auto size = this->field_1C8.size();
        struct {
            string_hash field_0;
            int field_4;
        } *data = CAST(data, this->field_1C8.m_first);
        auto *back = data + (size - 1);

        dword_965F20()(this, back->field_0);

        this->field_1C8.pop_back();
    }
}

void * vm_thread::operator new(size_t )
{
    return pool().allocate_new_block();
}

void vm_thread::operator delete(void *ptr)
{
    pool().remove(ptr);
}



const vm_executable * vm_thread::get_running_executable() const
{
    if ( this->PC != nullptr ) {
        return script_manager::find_function_by_address(this->PC);
    }

    assert(this->ex != nullptr);
    return this->ex;
}

void vm_thread::set_flag(flags_t a2, bool a3)
{
    this->flags = ( a3
                    ? (this->flags | a2)
                    : (this->flags & ~a2)
                    );
}

void vm_thread::set_suspended(bool a2)
{
    TRACE("vm_thread::set_suspended");
    if ( !a2 || this->is_flagged(SUSPENDABLE) )
    {
        this->set_flag(SUSPENDED, a2);
    }
}

void vm_thread::pop_PC()
{
	if constexpr (0)
    {
		if ( this->PC_stack.empty() )
		{
			this->PC = nullptr;
		}
		else
		{
			this->PC = this->PC_stack.back();
			this->PC_stack.pop_back();
		}
	}
    else
    {
		THISCALL(0x005A0C10, this);
	}
}

void vm_thread::create_event_callback(const vm_thread::argument_t &a2, bool a3)
{
    TRACE("vm_thread::create_event_callback");
    printf("%s\n", a2.sfr->get_fullname().to_string());

    THISCALL(0x0058F890, this, &a2, a3);
}

void vm_thread::create_static_event_callback(const vm_thread::argument_t &a2, bool a3)
{
    TRACE("vm_thread::create_static_event_callback");

    THISCALL(0x0058F900, this, &a2, a3);
}

void vm_thread::spawn_sub_thread(const vm_thread::argument_t &a2)
{
    THISCALL(0x005AB670, this, &a2);
}

void vm_thread::spawn_parallel_thread(const vm_thread::argument_t &a2)
{
    THISCALL(0x005AB710, this, &a2);
}

void vm_thread::slf_error(const mString &a2)
{
    const auto *cur_ex = script_manager::find_function_by_address(this->PC);
    if ( cur_ex != nullptr )
    {
        auto &v23 = cur_ex->get_fullname();
        auto *v2 = v23.to_string();
        mString v21 {v2};
        auto *owner = (const script_object *) cur_ex->get_owner();
        auto &v19 = owner->get_name();
        auto *v4 = v19.to_string();
        mString a3 {v4};
        auto &v16 = this->inst->get_name();
        auto *v5 = v16.to_string();
        auto v9 = mString {v5} + ": " + a3 + "::" + v21 + ":\n" + a2;
        auto *v6 = v9.c_str();
        script_manager::run_callbacks((script_manager_callback_reason)4, nullptr, v6);
    }
    else
    {
        auto &v24 = this->inst->get_name();
        auto *v7 = v24.to_string();
        auto a3a = mString {v7} + ": (UNKNOWN FUNCTION!):\n" + a2;
        auto *v8 = a3a.c_str();
        script_manager::run_callbacks((script_manager_callback_reason)4, nullptr, v8);
    }
}

bool vm_thread::run()
{
    //TRACE("vm_thread::run");

    assert(PC_stack.end() >= PC_stack.begin());

    {
        printf("Thread %s %s\n\tPC 0x%08X stack(0x%08X to 0x%08X) size %d\n",
                this->ex->get_fullname().to_string(),
                this->inst->get_name().to_string(),
                (uint32_t) this->PC,
                bit_cast<uint32_t>(this->PC_stack.begin()),
                bit_cast<uint32_t>(this->PC_stack.end()),
                this->PC_stack.size());
    }

    if constexpr (1)
    {
        auto dword_965F24 = (int)&this->PC;
        bool v109 = false;

        opcode_arg_t prev_argtype = OP_ARG_NULL;
        opcode_arg_t argtype = OP_ARG_NULL;

        argument_t arg {};

        argument_t prev_arg {};
        uint16_t v113 = 0; 
        float v114 = 0.0;
        bool running = true;
        bool kill_me = false;

        opcode_t op = OP_ADD;
        opcode_t prev_op = OP_ADD;

        while ( running )
        {
            auto *oldPC = this->PC;
            uint16_t opword = *this->PC++;
            printf("\nopword = 0x%04X\n", opword);

            if ( op != 60 && op != 61 ) {
                prev_op = op;
                prev_argtype = argtype;
                prev_arg = arg;
            }

            op = opcode_t(opword >> 8);
            if (op < 35) {
                printf("%d %s\n", op, opcode_t_str[op]);
            }

            argtype = opcode_arg_t(opword & OP_ARGTYPE_MASK);
            printf("%d %s\n", argtype, opcode_arg_t_str[argtype]);

            uint16_t dsize = 4;
            if ( (opword & OP_DSIZE_FLAG) != 0 ) {
                dsize = *this->PC++;
            }

            switch ( argtype )
            {
            case OP_ARG_NULL:
                break;
            case OP_ARG_NUM:
            case OP_ARG_NUMR:
            case OP_ARG_STR:
                arg.binary = (*this->PC++) << 16;
                arg.binary += *this->PC++;
                break;
            case OP_ARG_WORD:
            case OP_ARG_PCR:
            case OP_ARG_SPR:
            case OP_ARG_POPO:
                arg.word = *this->PC++;
                break;
            case OP_ARG_SDR:
            case OP_ARG_SFR:
            case OP_ARG_LFR:
            case OP_ARG_CLV:
            case OP_ARG_SIG:
            case OP_ARG_PSIG:
            case 17: {
                arg.binary = (*this->PC++) << 16;
                arg.binary += *this->PC++;
                break;
            }
            default:
                assert(0);
                break;
            }

            assert(PC_stack.end() >= PC_stack.begin());

            auto shr = [](int a, int b) {
                return a >> b;
            };

            auto shl = [](int a, int b) {
                return a << b;
            };

            auto unary_func = [this](auto func) -> void {
                this->dstack.top_num() = func(this->dstack.top_num());
            };

            auto binary_func = [this](opcode_arg_t argtype, const argument_t &arg, auto func) -> void {
                switch (argtype) {
                case OP_ARG_NULL: {
                    vm_num_t v = this->dstack.pop_num();
                    sp_log("%f, %f", this->dstack.top_num(), v);
                    this->dstack.top_num() = func(this->dstack.top_num(), v);
                    break;
                }
                case OP_ARG_NUM:
                    sp_log("%f, %f", this->dstack.top_num(), arg.val);
                    this->dstack.top_num() = func(this->dstack.top_num(), arg.val);
                    break;
                case OP_ARG_NUMR:
                    this->dstack.top_num() = func(arg.val, this->dstack.top_num());
                    break;
                default:
                    assert(0);
                    break;
                }
            };

            auto commutative_binary_func = [this](opcode_arg_t argtype, const argument_t &arg, auto func) -> void {
                switch (argtype) {
                case OP_ARG_NULL: {
                    vm_num_t v = this->dstack.pop_num();
                    this->dstack.top_num() = func(this->dstack.top_num(), v);
                    break;
                }
                case OP_ARG_NUM:
                    this->dstack.top_num() = func(this->dstack.top_num(), arg.val);
                    break;
                default:
                    assert(0);
                    break;
                }
            };

            auto compare_string = [this](opcode_arg_t argtype, const argument_t &arg, auto func) -> void {
                switch(argtype) {
                case OP_ARG_NULL: {
                    vm_str_t v = this->dstack.pop_str();
                    this->dstack.top_num() = func(strcmp(this->dstack.top_str(), v), 0);
                    break;
                }
                case OP_ARG_STR:
                    this->dstack.top_num() = func(strcmp(this->dstack.top_str(), arg.str), 0);
                    break;
                default:
                    assert(0);
                    break;
                }
            };

            //printf("op = %d, argtype = %s\n", int(op), opcode_arg_t_str[argtype]);
            switch ( op )
            {
            case OP_ADD: {
                assert(dsize == 4);

                if ( argtype == OP_ARG_NUM )
                {
                    if ( arg.binary == UNINITIALIZED_SCRIPT_PARM
                            || (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                    {
                        auto *ex = this->get_executable();
                        auto &v704 = ex->get_fullname();
                        auto *v118 = v704.to_string();
                        mString a3bs {v118};
                        auto v328 = "Uninitilized 'num' in thread " + a3bs + "\n\n" + "OP_ADD (a + b)";
                        this->slf_error(v328);
                    }
                }

                binary_func(argtype, arg, std::plus<vm_num_t>{});
                break;
            }
            case OP_AND: {
                assert(dsize == 4);

                commutative_binary_func(argtype, arg, std::bit_and<int>{});
                break;
            }
            case OP_BF: {
                assert(argtype == OP_ARG_PCR);

                if ( (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto &v704 = ex->get_fullname();
                    auto *v118 = v704.to_string();
                    mString a3bs {v118};
                    auto v328 = "Uninitilized 'num' in thread " + a3bs + "\n\n" + "OP_BF ( if(!a) <branch> )";
                    this->slf_error(v328);
                }

                auto val = this->dstack.pop_num();
                if ( 0.0f == val ) {
                    (uint32_t &)this->PC += arg.word;
                }

                sp_log("%f", val);

                break;
            }
            case OP_BRA: {
                assert(argtype == OP_ARG_PCR);

                sp_log("%d", arg.word);

                (uint32_t &)this->PC += arg.word;
                break;
            }
            case OP_BSL:
                assert(argtype == OP_ARG_LFR);

                running = this->call_script_library_function(arg, oldPC);
                break;
            case OP_BSR: {
                assert(argtype == OP_ARG_SFR);

                this->push_PC();
                this->PC = arg.sfr->get_start();
                break;
            }
            case OP_BST:
                assert(argtype == OP_ARG_SFR);

                this->spawn_sub_thread(arg);
                break;
            case OP_BTH:
                assert(argtype == OP_ARG_SFR);

                this->spawn_parallel_thread(arg);
                break;
            case OP_DEC:
                assert(dsize == 4);
                assert(argtype == OP_ARG_NULL);

                if ( (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto *v26 = ex->get_fullname().to_string();
                    mString a3j {v26};
                    auto v295 = "Uninitilized 'num' in thread " + a3j + "\n\n" + "OP_DEC (a-- or --a)";
                    this->slf_error(v295);
                }

                this->dstack.top_num() -= 1.0f;
                break;
            case OP_DIV:
                assert(dsize == 4);

                binary_func(argtype, arg, std::divides<vm_num_t>{});
                break;
            case OP_DUP: {
                switch ( argtype )
                {
                case OP_ARG_NULL:
                    this->dstack.push(this->dstack.get_SP() - dsize, dsize);
                    break;
                case OP_ARG_SPR:
                    memcpy(this->dstack.get_SP() + arg.word,
                            this->dstack.get_SP() - dsize,
                            dsize);
                    break;
                case OP_ARG_POPO: {
                    auto *si = static_cast<script_instance *>(this->dstack.pop_addr());
                    if ( (uint32_t) si == 0x0
                      || (uint32_t) si == 0x7B7B7B7B
                      || (uint32_t) si == 0x7D7D7D7D
                      || (uint32_t) si == 0x7F7F7F7F
                      || (uint32_t) si == 0x7BAD05CF ) {

                        this->slf_error(mString {"reference to bad or uninitialized script object instance value"});
                    }
                    memcpy(si->get_buffer() + arg.word,
                            this->dstack.get_SP() - dsize,
                            dsize);
                    break;
                }
                case OP_ARG_SDR:
                    memcpy(arg.sdr, this->dstack.get_SP() - dsize, dsize);
                    break;
                default:
                    assert(0);
                    break;
                }

                break;
            }
            case OP_EQ: {
                assert(dsize == 4);

                if ( argtype == OP_ARG_NUM )
                {
                    if ( arg.binary == UNINITIALIZED_SCRIPT_PARM
                            || (int &) this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                    {
                        auto *ex = this->get_executable();
                        auto &v704 = ex->get_fullname();
                        auto *v118 = v704.to_string();
                        mString a3bs {v118};
                        auto v328 = "Uninitilized 'num' in thread " + a3bs + "\n\n" + "OP_EQ (a == b)";
                        this->slf_error(v328);
                    }
                }

                commutative_binary_func(argtype, arg, std::equal_to<vm_num_t>{});
                break;
            }
            case OP_GE:
                assert(dsize == 4);

                if ( argtype != OP_ARG_NULL )
                {
                    if ( arg.binary == UNINITIALIZED_SCRIPT_PARM
                            || (int &) this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                    {
                        auto *ex = this->get_executable();
                        auto &v704 = ex->get_fullname();
                        auto *v118 = v704.to_string();
                        mString a3bs {v118};
                        auto v328 = "Uninitilized 'num' in thread " + a3bs + "\n\n" + "OP_GE (a >= b)";
                        this->slf_error(v328);
                    }
                }

                binary_func(argtype, arg, std::greater_equal<vm_num_t>{});
                break;
            case OP_GT:
                assert(dsize == 4);

                binary_func(argtype, arg, std::greater<vm_num_t>{});
                break;
            case OP_INC: {
                assert(dsize == 4);
                assert(argtype == OP_ARG_NULL);

                if ( (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto *v26 = ex->get_fullname().to_string();
                    mString a3j {v26};
                    auto v295 = "Uninitilized 'num' in thread " + a3j + "\n\n" + "OP_DEC (a++ or ++a)";
                    this->slf_error(v295);
                }

                auto &v = this->dstack.top_num();
                v += 1.f;

                break;
            }
            case OP_KIL: {

                switch (argtype) {
                case OP_ARG_NULL:
                    kill_me = true;
                    running = false;
                    break;
                case OP_ARG_SFR: {

                    vm_thread *t = nullptr;
                    if ( this->ex == arg.sfr )
                    {
                        kill_me = true;
                        running = false;
                        t = this;
                    }

                    this->inst->kill_thread(arg.sfr, t);
                    break;
                }
                default:
                    break;
                }

                if ( argtype != 0 ) {
                    if ( argtype == OP_ARG_SFR )
                    {
                    }
                } else {
                }

                break;
            }
            case OP_LE:
                assert(dsize == 4);

                {
                }

                binary_func(argtype, arg, std::less_equal<vm_num_t>{});
                break;
            case OP_LNT: {
                assert(dsize == 4);
                assert(argtype == OP_ARG_NULL);

                if ( (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto *v26 = ex->get_fullname().to_string();
                    mString a3j {v26};
                    auto v295 = "Uninitilized 'num' in thread " + a3j + "\n\n" + "OP_LNT (!a)";
                    this->slf_error(v295);
                }

                auto &v = this->dstack.top_num();
                v = (v != 0.0f ? 0.0f : 1.0f);

                break;
            }
            case OP_LT: {
                assert(dsize == 4);

                if ( argtype != OP_ARG_NULL )
                {
                    if ( arg.binary == UNINITIALIZED_SCRIPT_PARM
                            || (int &) this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                    {
                        auto *ex = this->get_executable();
                        auto &v704 = ex->get_fullname();
                        auto *v118 = v704.to_string();
                        mString a3bs {v118};
                        auto v328 = "Uninitilized 'num' in thread " + a3bs + "\n\n" + "OP_LT (a < b)";
                        this->slf_error(v328);
                    }
                }

                binary_func(argtype, arg, std::less<vm_num_t>{});
                break;
            }
            case OP_MOD: {
                assert(dsize == 4);


                binary_func(argtype, arg, std::modulus<int>{});
                break;
            }
            case OP_MUL: {
                assert(dsize == 4);

                commutative_binary_func(argtype, arg, std::multiplies<vm_num_t>{});
                break;
            }
            case OP_NE:
                assert(dsize == 4);

                commutative_binary_func(argtype, arg, std::not_equal_to<vm_num_t>{});
                break;
            case OP_NEG: {
                assert(dsize == 4);
                assert(argtype == OP_ARG_NULL);

                unary_func(std::negate<vm_num_t>{});
                break;
            }
            case OP_NOP:
                assert(argtype == OP_ARG_NULL);

                break;
            case OP_NOT: {
                assert(dsize == 4);
                assert(argtype == OP_ARG_NULL);

                if ( (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto *v26 = ex->get_fullname().to_string();
                    mString a3j {v26};
                    auto v295 = "Uninitilized 'num' in thread " + a3j + "\n\n" + "OP_NOT (~a)";
                    this->slf_error(v295);
                }

                unary_func(std::bit_not<int>{});
                break;
            }
            case OP_OR: {
                assert(dsize == 4);

                commutative_binary_func(argtype, arg, std::bit_or<int>{});
                break;
            }
            case OP_POP: {
                switch ( argtype )
                {
                case OP_ARG_NULL: {
                    this->dstack.pop(dsize);
                    break;
                }
                case OP_ARG_SPR: {
                    if ( !v109 )
                    {
                        auto *v58 = this->dstack.get_SP() - dsize;
                        auto *v59 = this->dstack.get_SP() + arg.word;

                        memcpy(v59, v58, dsize);
                        this->dstack.pop(dsize);
                    }
                    else
                    {
                        v109 = false;
                        memcpy(
                            this->dstack.get_SP() + v113 * int(v114) + arg.word,
                            this->dstack.get_SP() - v113,
                            v113);

                        this->dstack.pop(v113);
                    }

                    break;
                }
                case OP_ARG_POPO: {
                    script_instance *si = static_cast<script_instance *>(this->dstack.pop_addr());
                    if ( (uint32_t)si == 0
                      || (uint32_t)si == 0x7B7B7B7B
                      || (uint32_t)si == 0x7D7D7D7D
                      || (uint32_t)si == 0x7F7F7F7F
                      || (uint32_t)si == 0x7BAD05CF ) {

                        this->slf_error(mString {"reference to bad or uninitialized script object instance value"});
                    }

                    if ( v109 )
                    {
                        v109 = false;
                        memcpy(
                            si->get_buffer() + arg.word + v113 * int(v114),
                            this->dstack.get_SP() - v113,
                            v113);

                        this->dstack.pop(v113);
                    }
                    else
                    {
                        memcpy(si->get_buffer() + arg.word,
                                this->dstack.get_SP() - dsize,
                                dsize);

                        this->dstack.pop(dsize);
                    }

                    break;
                }
                case OP_ARG_SDR: {
                    if ( !v109 )
                    {
                        int offset;
                        for (offset = 4; offset < dsize; offset += 4) {
                            assert((*(int*)( dstack.get_SP() - offset )) != UNINITIALIZED_SCRIPT_PARM);
                        }

                        assert(offset == dsize && "dsize should be divisible by 4");

                        auto *v183 = this->dstack.get_SP();
                        memcpy(arg.sdr, v183 - dsize, dsize);
                        this->dstack.pop(dsize);
                        break;
                    }

                    v109 = false;
                    int offset;
                    for ( offset = 4; offset < v113; offset += 4 ) {
                        assert((*(int*)( dstack.get_SP() - offset )) != UNINITIALIZED_SCRIPT_PARM);
                    }

                    assert(offset == dsize && "dsize should be divisible by 4");

                    auto *v182 = this->dstack.get_SP();
                    memcpy(arg.sdr + v113 * int(v114),
                            v182 - v113,
                            v113);
                    this->dstack.pop(v113);
                    break;
                }
                case 17: {
                    if ( v109 )
                    {
                        v109 = false;
                        auto *v58 = this->dstack.get_SP() - v113;
                        auto v59 = arg.sdr + v113 * int(v114);
                        memcpy(v59, v58, v113);
                        this->dstack.pop(v113);
                    }
                    else
                    {
                        auto *v64 = this->dstack.get_SP();
                        auto *v58 = v64 - dsize;
                        memcpy(arg.sdr, v58, dsize);
                        this->dstack.pop(dsize);
                    }

                    break;
                }
                default:
                    assert(0);
                    break;
                }

                break;
            }
            case OP_PSH: {

                switch ( argtype )
                {
                case OP_ARG_NUM:
                    if (arg.binary == UNINITIALIZED_SCRIPT_PARM)
                    {
                        auto *ex = this->get_executable();
                        auto &v728 = ex->get_fullname();
                        auto *v186 = v728.to_string();
                        auto v352 = "Uninitilized 'num' in thread " + mString {v186} + "\n\n" + "OP_PSH (?)";
                        this->slf_error(v352);
                    }

                    sp_log("%f", arg.val);

                    this->dstack.push(arg.val);
                    break;
                case OP_ARG_STR:
                    this->dstack.push(arg.str);
                    break;
                case OP_ARG_SPR:
                    sp_log("%d", arg.word);
                    this->dstack.push(this->dstack.get_SP() + arg.word, dsize);
                    break;
                case OP_ARG_POPO: {

                    auto *si = static_cast<script_instance *>(this->dstack.pop_addr());

                    if ( (uint32_t)si == 0
                      || (uint32_t)si == 0x7B7B7B7B
                      || (uint32_t)si == 0x7D7D7D7D
                      || (uint32_t)si == 0x7F7F7F7F
                      || (uint32_t)si == UNINITIALIZED_SCRIPT_PARM ) 
                    {

                        this->slf_error(mString {"reference to bad or uninitialized script object instance value"});
                    }

                    this->dstack.push(si->get_buffer() + arg.word, dsize);
                    break;
                }
                case OP_ARG_SDR:
                case 17:
                    this->dstack.push(arg.sdr, dsize);
                    break;
                case OP_ARG_CLV:
                    this->dstack.push(static_cast<int>(arg.binary));
                    break;
                case OP_ARG_SIG: {
                    this->field_18 = 0;
                    this->dstack.push(static_cast<int>(arg.binary));
                    break;
                }
                case OP_ARG_PSIG: {
                    this->dstack.pop(4);
                    this->field_18 = *(int *)this->dstack.get_SP();
                    this->dstack.push(static_cast<int>(arg.binary));
                    break;
                }
                default:
                    assert(0);
                    break;
                }

                break;
            }
            case OP_RET: {
                assert(argtype == OP_ARG_NULL || argtype == OP_ARG_WORD);

                this->pop_PC();
                if ( this->PC == nullptr ) {
                    kill_me = true;
                    running = false;
                }

                break;
            }
            case OP_SHL: {
                assert(dsize == 4);

                binary_func(argtype, arg, shl);
                break;
            }
            case OP_SHR: {
                assert(dsize == 4);

                binary_func(argtype, arg, shr);
                break;
            }
            case OP_SPA: {
                assert(argtype == OP_ARG_WORD);

                this->dstack.move_SP(arg.word);
                while ( this->field_1C8.size() )
                {
                    auto size = this->field_1C8.size();
                    struct {
                        int field_0;
                        int field_4;
                    } *data = CAST(data, this->field_1C8.m_first);
                    auto *v80 = (char *)data[size - 1].field_4;
                    auto *v81 = &data[size - 1];
                    if ( v80 < this->dstack.get_SP() ) {
                        break;
                    }

                    dword_965F20()(this, v81->field_0);
                    if ( this->field_1C8.size() ) {
                        this->field_1C8.m_last -= 8;
                    }
                }

                break;
            }
            case OP_SUB: {
                assert(dsize == 4);

                binary_func(argtype, arg, std::minus<vm_num_t>{});
                break;
            }
            case OP_XOR:
                assert(dsize == 4);

                commutative_binary_func(argtype, arg, std::bit_xor<int>{});
                break;
            case OP_STR_EQ: {
                assert(dsize == 4);

                compare_string(argtype, arg, std::equal_to<int>{});
                break;
            }
            case OP_STR_NE: {
                assert(dsize == 4);

                compare_string(argtype, arg, std::not_equal_to<int>{});
                break;
            }
            case OP_ECB:
                assert(argtype == OP_ARG_SFR);
                this->create_event_callback(arg, false);
                break;
            case OP_ESB:
                assert(argtype == OP_ARG_SFR);
                this->create_static_event_callback(arg, false);
                break;
            case OP_ECO:
                assert(argtype == OP_ARG_SFR);
                this->create_event_callback(arg, true);
                break;
            case OP_SCO:
                assert(argtype == OP_ARG_SFR);
                this->create_static_event_callback(arg, true);
                break;
            case 47:
                this->raise_event(arg, argtype);
                break;
            case 48:
                this->raise_all_event(arg, argtype);
                break;
            case 49: {
                assert(argtype == OP_ARG_SFR);
                assert(prev_op == OP_PSH);
                assert(prev_argtype == OP_ARG_SPR);

                auto *si = static_cast<script_instance *>(this->dstack.pop_addr());
                vm_thread *v32 = nullptr;
                if ( this->inst == si && this->ex == arg.sfr )
                {
                    running = false;
                    kill_me = true;
                    v32 = this;
                }

                si->kill_thread(arg.sfr, v32);
                break;
            }
            case 50: {
                assert(argtype == OP_ARG_PCR);

                if ( (int &)this->dstack.top_num() == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto &v704 = ex->get_fullname();
                    auto *v118 = v704.to_string();
                    mString a3bs {v118};
                    auto v328 = "Uninitilized 'num' in thread " + a3bs + "\n\n" + "OP_BF ( if(!a) <branch> )";
                    this->slf_error(v328);
                }

                if ( this->dstack.top_num() == 0.0f ) {
                    (uint32_t &)this->PC += arg.word;
                }

                break;
            }
            case 51:
            case 52: {
                assert(argtype == OP_ARG_WORD);

                auto a1 = this->dstack.pop_num();
                static char byte_967F90[256]{};
                if ( op == 51 ) {
                    chuck_itoa(a1, byte_967F90, 0x100);
                } else {
                    chuck_ftoa(a1, 3, byte_967F90, 0x100);
                }

                auto *v105 = install_temp_string(byte_967F90);
                this->dstack.push(v105);
                break;
            }
            case 53: {
                assert(argtype == OP_ARG_WORD);

                auto *str = this->dstack.pop_str();
                auto *v106 = this->dstack.pop_str();

                static char byte_967E90[255]{};
                chuck_strcpy(byte_967E90, v106, 0x100u);
                chuck_strcat(byte_967E90, str, 0x100u);
                auto *v105 = install_temp_string(byte_967E90);
                this->dstack.push(v105);

                break;
            }
            case 54: {
                auto *v33 = this->inst;
                if ( argtype == OP_ARG_NULL ) {
                    kill_me = true;
                    running = false;
                    v33->massacre_threads(nullptr, this);
                } else {
                    assert(argtype == OP_ARG_SFR);
                    v33->massacre_threads(arg.sfr, this);
                }

                break;
            }
            case OP_MS2: {
                assert(argtype == OP_ARG_SFR);
                assert(prev_op == OP_PSH);
                assert(prev_argtype == OP_ARG_SPR);

                auto *si = static_cast<script_instance *>(this->dstack.pop_addr());

                if ( si == nullptr || int(si) == UNINITIALIZED_SCRIPT_PARM )
                {
                    auto *ex = this->get_executable();
                    auto *v26 = ex->get_fullname().to_string();
                    mString a3j {v26};
                    auto v295 = "Use of OP_MS2 by " + a3j + ": invalid local script object instance pointer";
                    this->slf_error(v295);
                }

                if ( this->inst == si && this->ex == arg.sfr )
                {
                    kill_me = true;
                    running = false;
                }

                si->massacre_threads(arg.sfr, this);
                break;
            }
            case 56: {
                this->dstack.pop(arg.word);
                vm_num_t v1258 = this->dstack.pop_num();
                auto v278 = arg.word;
                auto *v221 = this->dstack.get_SP();
                auto *src = v221 + prev_arg.word + arg.word + arg.word * int(v1258);
                auto *v222 = this->dstack.get_SP();
                memcpy(v222, src, v278);
                this->dstack.move_SP(arg.word);
                break;
            }
            case 57: {
                assert(argtype == OP_ARG_WORD);

                v109 = true;
                v113 = arg.word;
                v114 = this->dstack.pop_num();
                break;
            }
            case 58: {
                auto *si = static_cast<script_instance *>(this->dstack.pop_addr());
                if ( (uint32_t)si == 0
                  || (uint32_t)si == 0x7B7B7B7B
                  || (uint32_t)si == 0x7D7D7D7D
                  || (uint32_t)si == 0x7F7F7F7F
                  || (uint32_t)si == 0x7BAD05CF ) {
                    this->slf_error(mString {"reference to bad or uninitialized script object instance value"});
                }

                auto num_0 = this->dstack.pop_num();
                auto num_1 = this->dstack.pop_num();
                auto v89 = arg.word;

                auto *src = si->get_buffer() + int(num_0) + v89 * int(num_1);
                memcpy(
                    this->dstack.get_SP(),
                    src,
                    v89);
                this->dstack.move_SP(v89);
                break;

            }
            case 59: {
                this->dstack.pop(arg.word);
                auto v93 = this->dstack.pop_num();
                auto v85 = arg.word;
                auto *v88 = prev_arg.sdr + v85 * int(v93);
                auto *v87 = this->dstack.get_SP();
                memcpy(v87, v88, v85);
                this->dstack.move_SP(v85);

                break;
            }
            case 60: {
                assert(argtype == OP_ARG_WORD);
                break;
            }
            case 61:
            case 62:
            case 63:
            case 64:
            case 65:
                continue;
            default:
                assert(0 && "unknown opcode");
            }
        }

        dword_965F24 = 0;
        return kill_me;
    }
    else
    {
        bool (__fastcall *func)(void *) = CAST(func, 0x005ADD00);
        return func(this);
    }
}

void vm_thread::push_PC()
{
    TRACE("vm_thread::push_PC");

    if constexpr (0)
    {
        this->PC_stack.push_back(this->PC);
    }
    else
    {
        THISCALL(0x005A56F0, this);
    }

    {
        auto *ex = script_manager::find_function_by_address(this->PC);
        printf("%s\n", ex->name.to_string());
    }
}

void vm_thread::raise_event(const vm_thread::argument_t &a2, opcode_arg_t arg_type) {
    assert(arg_type == OP_ARG_SIG || arg_type == OP_ARG_PSIG);

    THISCALL(0x00599710, this, &a2, arg_type);
}

void vm_thread::raise_all_event(const vm_thread::argument_t &a2, opcode_arg_t arg_type) {
    assert(arg_type == OP_ARG_SIG);

    THISCALL(0x0058F960, this, &a2, arg_type);
}

char *vm_thread::install_temp_string(const char *a1) {
    static Var<int> index_18926{0x00967E0C};

    auto v2 = index_18926()++;
    if (index_18926() >= 64) {
        index_18926() = 0;
    }

    auto *v3 = vm_thread::string_registers()[v2];
    chuck_strcpy(v3, a1, 256u);
    return v3;
}

bool vm_thread::call_script_library_function(const vm_thread::argument_t &arg, const uint16_t *oldPC)
{
    //TRACE("vm_thread::call_script_library_function");

    if constexpr (1)
    {
        auto *oldSP = this->dstack.get_SP();

        printf("arg.lfr = 0x%08X\n", int(arg.lfr->m_vtbl));

#if SLC_NAME_FIELD
        printf("arg.lfr = %s\n", arg.lfr->get_name());
#endif
        if (arg.lfr->operator()(this->dstack, this->entry)) {
            this->entry = script_library_class::function::entry_t::FIRST_ENTRY;
            this->field_1B0 = nullptr;

            return true;
        }

        auto v5 = (oldPC == this->field_1B0);
        this->PC = oldPC;
        this->dstack.set_SP(oldSP);
        this->entry = script_library_class::function::entry_t::RECALL_ENTRY;
        if (v5) {
            this->field_1B4 += script_manager::get_time_inc();
            if (this->field_1B4 > 10.0f) {
                this->field_1B4 = 0.0;
            }

        } else {
            this->field_1B0 = oldPC;
            this->field_1B4 = 0.0;
        }

        return false;
    }
    else
    {

        bool (__fastcall *func)(void *, void *edx, const vm_thread::argument_t *arg, const uint16_t *oldPC) = CAST(func, 0x0058F7E0);
        return func(this, nullptr, &arg, oldPC);
    }
}

void vm_thread_patch()
{
    {
        FUNC_ADDRESS(address, &vm_thread::call_script_library_function);
        SET_JUMP(0x0058F7E0, address);
    }

    {
        FUNC_ADDRESS(address, &vm_thread::push_PC);
        REDIRECT(0x005ADF11, address);
    }

    {
        FUNC_ADDRESS(address, &vm_thread::run);
        REDIRECT(0x005AF142, address);
        REDIRECT(0x005AF5A6, address);
        REDIRECT(0x005AF68C, address);
        REDIRECT(0x005AF844, address);
    }

    {
        FUNC_ADDRESS(address, &vm_thread::create_event_callback);
        REDIRECT(0x005AEDCD, address);
    }

    {
        FUNC_ADDRESS(address, &vm_thread::create_static_event_callback);
        REDIRECT(0x005AEDE0, address);
    }
}
