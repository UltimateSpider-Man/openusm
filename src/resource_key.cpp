#include "resource_key.h"

#include "common.h"
#include "func_wrapper.h"
#include "log.h"
#include "mash_info_struct.h"
#include "mstring.h"
#include "trace.h"
#include "utility.h"
#include "variable.h"

#include <cassert>
#include <cstring>
#include <string>

#ifdef TARGET_XBOX
const char *g_resource_key_type_ext[71] {"none", ".PCANIM", ".PCSKEL",
                                        ".ALS", ".ENT", ".ENTEXT", ".DDS", ".DDSMP", ".IFL", ".DESC",
                                        ".ENS", ".SPL", ".AB", ".QP", ".TRIG", ".PCSX", ".INST", ".FDF", ".PANEL", ".TXT", ".ICN", ".PCMESH", ".PCMORPH", ".PCMAT", ".COLL", ".PCPACK", ".PCSANIM", ".MSN", ".MARKER", ".HH", ".WAV", ".WBK", ".M2V", ".M2V", ".PFX", ".CSV", ".CLE", ".LIT", ".GRD", ".GLS", ".LOD", ".SIN", ".GV", ".SV", ".TOKENS", ".DSG", ".PATH", ".PTRL", ".LANG", ".SLF", ".VISEME", ".PCMESHDEF", ".PCMORPHDEF", ".PCMATDEF", ".MUT", ".FX_CACHE", ".ASG", ".BAI", ".CUT", ".INTERACT", ".CSV", ".CSV", "._ENTID_", "._ANIMID_", "._REGIONID_", "._AI_GENERIC_ID_", "._RADIOMSG", "._GOAL_", "._IFC_ATTRIBUTE_", "._SIGNAL_", "._PACKSTATE_"};
#endif

static auto & resource_key_type_dir = var<char *[1]>(0x0091EC28);

resource_key_type resource_key::resolve_extension(const char *target_string, bool a2)
{
    assert(target_string != nullptr);
    assert(target_string[0] != '\0');
    assert(strlen(target_string) < EXTENSION_LENGTH);

    char Dest[EXTENSION_LENGTH];
    strncpy(Dest, target_string, EXTENSION_LENGTH);
    Dest[EXTENSION_LENGTH - 1] = '\0';
    strupr(Dest);

    for (auto &ext : resource_key_type_ext)
    {
        for (size_t i{0}; i < 70u; ++i) {
            auto Src = ext[i];
            if (strcmp(Dest, Src) == 0) {
                return static_cast<resource_key_type>(i);
            }
        }
    }

    if (a2) {
        sp_log("Woah Momma!  We couldn't find extension %s in the list of resource_key extensions",
               Dest);
    }

    return RESOURCE_KEY_TYPE_NONE;
}

mString resource_key::get_platform_string(int a1) const
{
    auto *v3 = this->m_hash.to_string();
    mString a2a {v3};
    auto *v4 = [](const resource_key &self, int a2) -> const char *
    {
#ifdef TARGET_XBOX
        return g_resource_key_type_ext[self.m_type];
#else
        return resource_key_type_ext[a2][self.m_type];
#endif
    }(*this, a1);

    auto result = a2a + v4;

    return result;
}

const char *to_string(resource_key_type type)
{
    return resource_key_type_str[type];
}

void resource_key::unmash(mash_info_struct *a1, void *)
{
    TRACE("resource_key::unmash");
    a1->unmash_class_in_place(this->m_hash, this);
}

resource_key create_resource_key_from_path(const char *in_string, resource_key_type a3) {
    TRACE("create_resource_key_from_path");

    if (in_string != nullptr && in_string[0] != 0) {
        //sp_log("create_resource_key_from_path %s", a2);
    }

    mString v8;
    resource_key::calc_resource_string_and_type_from_path(in_string, &v8, &a3);

    string_hash v5{v8.c_str()};

    resource_key out_key{v5, a3};

    return out_key;
}

resource_key *create_resource_key_from_path_hook(resource_key *out_key, const char *in_string, resource_key_type type_override)
{
    *out_key = create_resource_key_from_path(in_string, type_override);
    return out_key;
}

void resource_key::calc_resource_string_and_type_from_path(const char *in_string,
                                                           mString *out_string,
                                                           resource_key_type *type_override)
{
    TRACE("resource_key::calc_resource_string_and_type_from_path", in_string);

 //   assert(in_string != nullptr);
    assert(out_string != nullptr);
    assert(type_override != nullptr);

    auto func = [](const char *a1, char a2, const int *a3) -> int {
        if (a1 == nullptr) {
            return -1;
        }

        int v5;
        if (a3 != nullptr) {
            v5 = *a3;
        } else {
            v5 = strlen(a1);
        }

        if (v5 <= 0) {
            return -1;
        }

        for (auto i = v5 - 1; i >= 0; --i) {
            if (a1[i] == a2) {
                return i;
            }
        }

        return -1;
    };

    int str_len = strlen(in_string);
    auto v7 = func(in_string, '\\', &str_len);
    auto v6 = func(in_string, '/', &str_len);
    if (v6 > v7) {
        v7 = v6;
    }

    auto *v3 = &in_string[v7 + 1];
    str_len = strlen(v3);
    auto idx = func(v3, '.', &str_len);

    if (idx <= 0) {
        idx = str_len;
    } else if (*type_override == RESOURCE_KEY_TYPE_NONE) {
        *type_override = resolve_extension(&v3[idx], true);
    }

    if (idx > 30) {
        sp_log("Resource string %s too long(%d), can be at most %d characters.", v3, idx, 30);
        assert(0);

        idx = 30;
    }

    char Dest[31]{};
    strncpy(Dest, v3, idx);
    Dest[idx] = '\0';
    strupr(Dest);
    *out_string = Dest;
}

void resource_key::operator=(const resource_key &a2) {
    if (&a2 != this) {
        this->m_hash = a2.m_hash;
        this->m_type = a2.m_type;
    }
}

bool resource_key::is_set() const
{
    resource_key temp_key{};
    temp_key.m_hash.initialize(0, nullptr, 0);

    return (this->m_hash != temp_key.m_hash);
}

void resource_key::set(const resource_key &a2) {
    *this = a2;
}

void resource_key::set(string_hash a2, resource_key_type a3) {
    this->m_hash = a2;
    this->m_type = a3;
}

void resource_key::destruct_mashed_class() {
    this->m_hash.destruct_mashed_class();
}

void resource_key_patch()
{
    {
        SET_JUMP(0x004217B0, create_resource_key_from_path_hook);
    }

    {
        SET_JUMP(0x004200D0, resource_key::calc_resource_string_and_type_from_path);
    }

    return;
    REDIRECT(0x004201B0, resource_key::resolve_extension);
}
