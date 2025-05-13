#include "render_text.h"

#include "fetext.h"
#include "mstring.h"
#include "os_developer_options.h"

#include "vector2di.h"

void render_text(const mString &a1, const vector2di &a2, color32 a3, Float a4, Float a5) {
    //if ()
    if (os_developer_options::instance->get_flag(mString{ "SHOW_DEBUG_TEXT" }))
    {
        FEText fe_text{static_cast<font_index>(0),
                       static_cast<global_text_enum>(0),
                       (float) a2.x,
                       (float) a2.y,
                       (int) a4,
                       static_cast<panel_layer>(0),
                       a5,
                       16,
                       0,
                       a3};

        fe_text.field_1C = a1;

        fe_text.Draw();
    }
}
