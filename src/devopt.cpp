#include "devopt.h"

#include "os_developer_options.h"

game_option_t *get_option(int idx)
{
    constexpr auto NUM_OPTIONS = 150 + 76;
    assert(idx >= 0);
    assert(idx < NUM_OPTIONS);

    static game_option_t option{};
    if (idx < 150)
    {
        auto& name = flag_names[idx];
        BOOL* flag = (BOOL*) &os_developer_options::instance->m_flags[idx];

        option.m_name = name;
        option.m_type = game_option_t::FLAG_OPTION;
        option.m_value.p_bval = flag;

        return &option;
    }

    idx = idx - 150;
    auto& name = int_names[idx];
    int* i = &os_developer_options::instance->m_ints[idx];

    option.m_name = name;
    option.m_type = game_option_t::INT_OPTION;
    option.m_value.p_ival = i;

    return &option;
}
