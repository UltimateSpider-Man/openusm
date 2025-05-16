#include "fe_health_widget.h"

#include "func_wrapper.h"
#include "panelfile.h"

// VALIDATE_SIZE(fe_health_widget, 0x58);

fe_health_widget::fe_health_widget() {
    this->field_30 = 0;
    this->field_34 = 6;
    auto v9 = 0;
    do {
        this->field_0[v9++] = nullptr;
    } while (v9 < this->field_34);

    this->field_54 = 0;
    this->field_55 = 0;
    this->field_38 = this->field_34;
    this->field_40 = nullptr;
    this->field_44 = nullptr;
    this->field_48 = nullptr;
    this->field_4C = 1.0;
    this->field_50 = 1.0;
}

void fe_health_widget::SetShown(bool a2) {
    THISCALL(0x0061A3F0, this, a2);
}

void fe_health_widget::UpdateMasking() {
    THISCALL(0x0061A5A0, this);
}

char fe_health_widget::clear_bars() {
    return static_cast<char>(THISCALL(0x0063B170, this));
}

void fe_health_widget::Init(int a2, const char *a3, bool a4) {
    this->field_0[a2] = PanelFile::UnmashPanelFile(a3, static_cast<panel_layer>(7));
    this->field_3C = a4;
    this->field_55 = false;
    this->clear_bars();
}

void fe_health_widget::DeInit(int a2) {
    this->field_0[a2] = nullptr;
    if (a2 == this->field_38) {
        this->field_54 = 0;
        this->field_38 = this->field_34;
    }
}
