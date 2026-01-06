#pragma once

#include "global_text_enum.h"

struct localized_string_table {
    struct internal {
        const char *field_0[479];
        const char *field_778[1];
    };

    internal *field_0;
    int field_4;
    int field_8;
    int scripttext_number;

    void sub_60BD30();

    //0x0060BDD0
    const char *lookup_scripttext_string(int num);

    //0x0060BDC0
    const char *lookup_localized_string(global_text_enum num);

    //0x0062EF10
    static void load_localizer();
};

extern void localized_string_table_patch();
