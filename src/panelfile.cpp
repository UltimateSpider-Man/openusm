#include "panelfile.h"

#include "common.h"
#include "femanager.h"
#include "fixedstring.h"
#include "func_wrapper.h"
#include "ngl.h"
#include "mash_info_struct.h"
#include "unlockables_menu.h"
#include "pausemenusystem.h"
#include "resource_manager.h"
#include "resource_key.h"
#include "resource_directory.h"
#include "trace.h"
#include "utility.h"
#include "log.h"

VALIDATE_SIZE(PanelFile, 0x3c);
VALIDATE_OFFSET(PanelFile, field_28, 0x28);

void PanelFile::Draw() {
    THISCALL(0x00616A60, this);
}

void PanelFile::unmash(mash_info_struct *a1, void *)
{
    TRACE("PanelFile::unmash");
    a1->unmash_class_in_place(this->pquads, this);
    a1->unmash_class_in_place(this->ptext, this);
    a1->unmash_class_in_place(this->field_28, this);
}

constexpr auto UnmashPanelFile_hook = 1;
PanelFile *PanelFile::UnmashPanelFile(const char *a1, panel_layer a2)
{
    TRACE("PanelFile::UnmashPanelFile", a1);

    if constexpr (UnmashPanelFile_hook)
    {
        assert(g_curmeshfile == nullptr);

        tlFixedString v11 {a1};
        g_curmeshfile = nglLoadMeshFile(v11);

        resource_key resource_id {string_hash {a1}, RESOURCE_KEY_TYPE_PANEL};

        int mash_data_size;
        auto *the_panel_image = resource_manager::get_resource(resource_id, &mash_data_size, nullptr);
        assert(the_panel_image != nullptr);
        sp_log("0x%08X", the_panel_image);

#ifndef TARGET_XBOX
        mash_info_struct v10 {the_panel_image, mash_data_size};
#else
        mash_info_struct v10 {mash::UNMASH_MODE, the_panel_image, mash_data_size, true};
#endif

        PanelFile *v6 = nullptr;
        v10.unmash_class(v6, nullptr
#ifdef TARGET_XBOX
            , mash::NORMAL_BUFFER
#endif 
                );
        v6->PostUnmashFixup(a2);
        mash_info_struct::construct_class(v6);

        g_curmeshfile = nullptr;

        return v6;
    }
    else
    {
        return (PanelFile *) CDECL_CALL(0x00643000, a1, a2);
    }

}

PanelAnimFile *PanelFile::GetAnimationPointer(int a1)
{
    return this->field_28.at(a1);
}

FEText *PanelFile::GetTextPointer(const char *a2) {
    TRACE("PanelFile::GetTextPointer", a2);
    for (uint16_t i = 0; i < this->ptext.size(); ++i) {
        auto *v5 = this->ptext.m_data[i];

        struct Vtbl {
            int field_0[44];
            mString *(__fastcall *GetName)(void *, int edx, const mString *);
        };

        Vtbl *vtbl = CAST(vtbl, v5->m_vtbl);

        mString v8;
        auto v6 = (strcmp(vtbl->GetName(v5, 0, &v8)->c_str(), a2) == 0);
        if (v6)
        {
            return this->ptext.m_data[i];
        }
    }

    sp_log("Object %s not found in PANEL file\n", a2);
    assert(0);

    return nullptr;
}

PanelQuad *PanelFile::GetPQ(const char *a2) {
    TRACE("PanelFile::GetPQ", a2);
    auto v3 = this->pquads.m_data;

    sp_log("size = %d", this->pquads.size());
    for (uint16_t i = 0; i < this->pquads.size(); ++i)
    {
        auto &pquad = v3[i];
        //sp_log("%d %s", i, pquad->field_3C.c_str());

        if (strcmp(pquad->field_3C.c_str(), a2) == 0)
        {
            return pquad;
        }
    }

    sp_log("Object %s not found in PANEL file\n", a2);

    return g_femanager.GetDefaultPQ();
}

void PanelFile::Update(Float a2)
{
    if constexpr (1)
    {
        for (uint16_t i = 0; i < this->pquads.m_size; ++i)
        {
            auto *v4 = this->pquads.m_data[i];
            v4->Update(a2);
        }

        for (uint16_t j = 0; j < this->field_28.m_size; ++j) {
            this->field_28.m_data[j]->Update(a2);
        }
    } else {
        THISCALL(0x0062E4D0, this, a2);
    }
}

void PanelFile::PostUnmashFixup(panel_layer a3)
{
    if constexpr (1)
    {
        for ( auto i = 0; i < this->pquads.m_size; ++i )
        {
            auto *v4 = this->pquads.m_data[i];
            auto v10 = v4->GetZvalue();
            v4->SetZvalue(v10, a3);
        }

        for ( auto j = 0; j < this->ptext.m_size; ++j )
        {
            auto *v7 = this->ptext.m_data[j];
            auto v11 = v7->GetZvalue();
            v7->SetZvalue(v11, a3);
        }

        for ( auto k = 0; k < this->field_28.size(); ++k )
        {
            auto *v1 = this->field_28.m_data[k];
            v1->PostUnmashFixup(this);
        }
    }
    else
    {
        THISCALL(0x00628960, this, a3);
    }
}

void PanelFile_patch()
{
    if constexpr (UnmashPanelFile_hook)
    {
        SET_JUMP(0x000643000, PanelFile::UnmashPanelFile);
    }

    {
        FUNC_ADDRESS(address, &PanelFile::GetPQ);
        SET_JUMP(0x00639E40, address);
    }

    {
        FUNC_ADDRESS(address, &PanelFile::GetTextPointer);
        SET_JUMP(0x00616B40, address);
    }

    if constexpr(1)
    {
        FUNC_ADDRESS(address, &PanelFile::PostUnmashFixup);
        REDIRECT(0x006430AE, address);
    }

    if constexpr (1)
    {
        FUNC_ADDRESS(address, &mVector<PanelQuad>::custom_unmash);
        REDIRECT(0x0064D153, address);
    }

    {
        FUNC_ADDRESS(address, &mVector<FEText>::custom_unmash);
        REDIRECT(0x0064D168, address);
    }

    if constexpr (1)
    {
        FUNC_ADDRESS(address, &mVector<PanelAnimFile>::custom_unmash);
        REDIRECT(0x0064D17D, address);
    }

    return;
    {
        FUNC_ADDRESS(address, &mash_info_struct::unmash_class<PanelFile>);
        REDIRECT(0x0064309E, address);
    }

}
