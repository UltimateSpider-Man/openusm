#pragma once

#include "fetext.h"

#include "color32.h"
#include "float.hpp"
#include "global_text_enum.h"

struct MultiLineString;
struct mash_info_struct;

struct FEMultiLineText : FEText {
    color32 field_68;
    float field_6C;
    float field_70;
    float field_74;
    float field_78;
    int field_7C;
    int field_80;
    int line_avail_num;
    MultiLineString *lines;
    int field_8C;
    int field_90;
    int field_94;
    int field_98;
    bool field_9C;
    bool field_9D;
    bool field_9E;

    FEMultiLineText();

    FEMultiLineText(font_index a2,
                    Float a4,
                    Float a5,
                    int a6,
                    panel_layer a7,
                    Float a8,
                    int a9,
                    int a10,
                    color32 a11);

    bool CheckIfNotTooLong(int a2);

    //0x006182D0
    void AdjustForJustification();

    void sub_60A4A0(mString &a1);

    //0x0062EAD0
    int MakeBox(char *a2, int a3, int a4, Float a5, Float a6, bool a7);

    //virtual
    void _unmash(mash_info_struct *, void *);

    //virtual
    int _get_mash_sizeof();

    /* virtual */ void Draw();

    //0x00617D20
    //virtual
    void Draw(int a2, int a3);

    //0x0043DAB0
    //virtual
    void GetPos(Float &a2, Float &a3);
	
	void ReadFileBoxFormat(FEMultiLineText* self, const char* path, int width, bool replace_newlines);



    //0x0043DA90
    //virtual
    void SetButtonColor(color32 a2);

    //0x0043DA80
    //virtual
    void SetButtonScale(Float a2);

    //0x00618070
    //virtual
    void SetTextBox(global_text_enum a2, int a3, Float a4);

    //0x00633AB0
    //virtual
    void SetTextBoxNoLocalize(string a2, int a7, Float a8);

    //0x006180B0
    //virtual
    void SetTextAlloc(global_text_enum a2);

    //0x00618030
    //virtual
    void SetText(global_text_enum a2);

    //0x0062E720
    //virtual
    void _SetTextNoLocalize(string);

    //0x0062E8D0
    //virtual
    int SetTextAllocNoLocalize(const char *a2, int a3);

    //0x00618140
    //virtual
    void SetTextBoxAlloc(global_text_enum a1, int a3, Float a4);

    //0x00633C00
    //virtual
    void SetTextBoxAllocNoLocalize(mString a2, int a6, Float a7);

    /* virtual */ void SetNumLines(int a2);

    //0x0060A3E0
    [[nodiscard]] static mString ReplaceEndlines(mString a2);
};

void FEMultiLineText_patch();
