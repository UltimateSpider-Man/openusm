
#pragma once
#define MOD_DIR "mods"
#include <map>

#if !defined(_DEBUG)
#define printf //
#endif

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <map>
std::vector<std::string> script_static_funcs;
std::map<std::string, std::vector<std::string>> script_class_funcs;




struct resource_key2
{
    uint32_t m_hash;
    uint32_t m_type;
};

struct string_hash2 {
    uint32_t source_hash_code;
};

struct resource_location2
{
    resource_key2 field_0;
    unsigned __int8* field_8;
    int m_size;
};

struct mashable_vector__resource_location2
{
    resource_location2* m_data;
    unsigned __int16 m_size;
    char field_6;
    char empty;
};

struct fixedstring_42
{
    char field_0[32];
};

struct mashable_vector__fixedstring_42
{
    fixedstring_42* m_data;
    unsigned __int16 m_size;
    char field_6;
    char field_7;
};

struct resource_pack_group2
{
    int field_0;
    int field_4;
    mashable_vector__fixedstring_42 field_8;
    int* field_10;
    int* field_14;
    int field_18;
    int field_1C;
};
struct mashable_vector__resource_pack_group2
{
    resource_pack_group2* m_data;
    unsigned __int16 m_size;
    bool field_6;
    char field_7;
};
struct resource_allocation_pool2
{
    int field_0;
    int field_4;
    int field_8;
};

struct mashable_vector__resource_allocation_pool2
{
    resource_allocation_pool2* m_data;
    unsigned __int16 m_size;
    bool m_shared;
    char field_7;
};

struct tlresource_location2
{
    string_hash name;
    unsigned int m_type;
    void* mesh_file;
};
struct mashable_vector__tlresource_location2
{
    tlresource_location2* m_data;
    unsigned __int16 m_size;
    char field_6;
    char field_7;
};

struct resource_pack_token2
{
    void* field_0;
    int field_4;
};


struct resource_directory2;

struct resource_pack_directory2
{
    resource_directory2* field_0;
    void* field_4;
    void* field_8;
    void* field_C;
    void* field_10;
    void* field_14;
    void* field_18;
    void* field_1C;
    void* field_20;
    int field_24;
    void* field_28;
    int field_2C;
    void* field_30;
    int field_34;
    void* field_38;
    int field_3C;
    void* field_40;
    int field_44;
    void* field_48;
    int field_4C;
    void* field_50;
    int field_54;
    void* field_58;
};

struct mashable_vector__ptr_resource_directory2
{
    resource_directory2** m_data;
    unsigned __int16 m_size;
    char field_6;
    char field_7;
};

struct resource_pack_slot2
{
    void* m_vtbl;
    resource_key2 field_4;
    int m_slot_state;
    int slot_size;
    int pack_size;
    unsigned __int8* header_mem_addr;
    resource_pack_directory2 pack_directory;
    resource_pack_token2 field_78;
};


struct resource_directory2
{
    mashable_vector__ptr_resource_directory2 parents;
    mashable_vector__resource_location2 resource_locations;
    mashable_vector__tlresource_location2 texture_locations;
    mashable_vector__tlresource_location2 mesh_file_locations;
    mashable_vector__tlresource_location2 mesh_locations;
    mashable_vector__tlresource_location2 morph_file_locations;
    mashable_vector__tlresource_location2 morph_locations;
    mashable_vector__tlresource_location2 material_file_locations;
    mashable_vector__tlresource_location2 material_locations;
    mashable_vector__tlresource_location2 anim_file_locations;
    mashable_vector__tlresource_location2 anim_locations;
    mashable_vector__tlresource_location2 scene_anim_locations;
    mashable_vector__tlresource_location2 skeleton_locations;
    mashable_vector__resource_pack_group2 field_68;
    mashable_vector__resource_allocation_pool2 field_70;
    resource_pack_slot2* pack_slot;
    int base;
    unsigned __int8* field_80;
    unsigned int field_84;
    int field_88;
    int type_start_idxs[70];
    int type_end_idxs[70];
};


struct vm_thread2
{
    char pad[0x20 + 0x184];
    char* _SP;
};



struct vm_stack2 {
    int field_0[96];
    char* buffer;
    char* SP;
    vm_thread2* my_thread;
};

static lua_State* g_L;

entity_base* __cdecl find_entity(const string_hash* a1, EntityFlags a2, bool a3)
{
    return (entity_base*)CDECL_CALL(0x004DC300, a1, a2, a3);
}


int lua_find_entity(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    string_hash hash = { to_hash(name) };
    entity_base* entity = find_entity(&hash, (EntityFlags)(SWITCH | BEAM), 0);
    if (!entity)
        return luaL_error(L, "Entity '%s' not found", name);

    lua_pushlightuserdata(L, entity);
    return 1;
}


static void enumerate_scripts()
{
    fs::path scriptsDir = fs::current_path() / "scripts";
    if (!fs::is_directory(scriptsDir))
        return;

    for (const auto& entry : fs::directory_iterator(scriptsDir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path& path = entry.path();
        std::string ext = transformToLower(path.extension().string());
        if (ext != ".lua")
            continue;

        printf("[Lua] Loading %s...\n", path.string().c_str());
        if (luaL_dofile(g_L, path.string().c_str()) != LUA_OK) {
            const char* err = lua_tostring(g_L, -1);
            printf("[Lua] Error: %s\n", err);
            lua_pop(g_L, 1);
        }
    }
}

typedef void* (__thiscall* static_func_t)(char* self, const char*);
static_func_t script_func_orig;

typedef void* (__thiscall* register_t)(char* self, script_library_class*, const char*);
register_t script_func_reg_orig;

typedef bool(__cdecl* exec_t)(bool a1);
exec_t exec_orig;

typedef void(__cdecl* script_manager_run_t)(float unusedTime, bool run_once);
script_manager_run_t script_manager_run_orig;



char* script_gsoi = (char*)(0x0096BB50);

BOOL __cdecl script_push_entity(void* a)
{
    return CDECL_CALL(0x0064E5C0, a);
}
BOOL __cdecl script_push_str(const char* a)
{
    return CDECL_CALL(0x0064E610, a);
}
BOOL __cdecl script_push_num(int a)
{
    return CDECL_CALL(0x0064E6A0, a);
}
BOOL __cdecl script_push_vector3d(vector3d* a)
{
    return static_cast<BOOL>(CDECL_CALL(0x0064E6D0, a));
}


int __cdecl script_find_function(string_hash a1, const char* a2, bool a3)
{
    return CDECL_CALL(0x0064E4F0, a1, a2, a3);
}
char* __cdecl script_new_thread(int a1, char* si)
{
    return (char*)CDECL_CALL(0x0064E520, a1, si);
}

bool push_vector3(lua_State* L, int index)
{
    if (!lua_istable(L, index))
        return false;

    vector3d vec{};
    lua_getfield(L, index, "x"); vec.x = lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, index, "y"); vec.y = lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, index, "z"); vec.z = lua_tonumber(L, -1); lua_pop(L, 1);

    return script_push_vector3d(&vec);
}


int lua_script_dispatcher(lua_State* L)
{
    if (!lua_isstring(L, lua_upvalueindex(1)))
        return luaL_error(L, "Missing function name");

    const char* fn = lua_tostring(L, lua_upvalueindex(1));
    string_hash hash = {to_hash(fn)};
    
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i) {

        if (lua_isinteger(L, i) || lua_isnumber(L, i)) {
            if (lua_isinteger(L, i)) {
                int val = lua_tointeger(L, i);
                if (!script_push_num(val))
                    return luaL_error(L, "Failed to push int %d", i);
            }
            else if (lua_isnumber(L, i)) {
                float val = static_cast<float>(lua_tonumber(L, i));
                if (!script_push_num(*reinterpret_cast<int*>(&val)))
                    return luaL_error(L, "Failed to push float %f", val);
            }
        }
        else if (lua_isstring(L, i)) {
            const char* str = lua_tostring(L, i);
            if (!script_push_str(str))
                return luaL_error(L, "Failed to push string arg %d", i);
        }
        else if (lua_istable(L, i)) {
            if (!push_vector3(L, i))
                return luaL_error(L, "Failed to push vector3 arg %d", i);
        }
        else {
            return luaL_error(L, "Unsupported arg type at index %d", i);
        }
    }

    /*
    vm_thread* thr = (vm_thread*)(0x0096BB54);
    char* sp_before = thr->_SP;
    
    char* sp_after = thr->_SP;

    const int nrets = (int)((uintptr_t)(sp_after - sp_before)) / 4;
    for (int i = 0; i < nrets; ++i)
        lua_pushinteger(L, *reinterpret_cast<int32_t*>(sp_before + (i * 4)));

    printf("executed engine func\n");
    return nrets;
    */
    return 0;
}

void register_func(const char* fn) {
    if (!fn) return;

    std::string name(fn);
    auto p = name.find('(');
    if (p != std::string::npos)
        name.resize(p);

    script_static_funcs.emplace_back(fn);
    printf("registering static func %s\n", fn);

    lua_getglobal(g_L, "engine");
    lua_pushstring(g_L, fn);
    lua_pushcclosure(g_L, lua_script_dispatcher, 1);
    lua_setfield(g_L, -2, name.c_str());              // engine[fn] = closure

    lua_pop(g_L, 1);
}
void register_class_func(script_library_class* cl, const char* fn) {
    if (!cl || !fn) return;

    std::string name(fn);
    auto p = name.find('(');
    if (p != std::string::npos)
        name.resize(p);

    script_class_funcs[cl->name].emplace_back(fn);
    printf("registering class func %s:%s\n", cl->name, fn);

    lua_getglobal(g_L, "engine");
    lua_getfield(g_L, -1, cl->name);
    if (!lua_istable(g_L, -1)) {
        lua_pop(g_L, 1);
        lua_newtable(g_L);
        lua_setfield(g_L, -2, cl->name);
        lua_getfield(g_L, -1, cl->name);
    }

    lua_pushstring(g_L, fn); 
    lua_pushcclosure(g_L, lua_script_dispatcher, 1);
    lua_setfield(g_L, -2, name.c_str());              // engine[class][fn] = closure

    lua_pop(g_L, 2);
}

void* __fastcall hk_script_func(char* self, void* edx, const char* a3)
{
    if (a3)
        register_func(a3);

    return script_func_orig(self, a3);
}
void* __fastcall hk_script_func_reg(char* self, void* edx, script_library_class* a2, const char* a3)
{
    if (a3) 
        register_class_func(a2, a3);

    return script_func_reg_orig(self, a2, a3);
}

void trigger_callback(const char* callback) {
    lua_getglobal(g_L, "engine");
    lua_getfield(g_L, -1, callback);
    if (lua_isfunction(g_L, -1)) {
        if (lua_pcall(g_L, 0, 0, 0) != LUA_OK) {
            const char* err = lua_tostring(g_L, -1);
            printf("Lua error in %s: %s\n", callback, err);
            lua_pop(g_L, 1);
        }
    }
    lua_pop(g_L, 1);

}

void init_scripting()
{
    if (g_L)
        return;

    g_L = luaL_newstate();
    luaL_openlibs(g_L);

    lua_newtable(g_L);
    lua_setglobal(g_L, "engine");
}

void hk_script_ctor()
{
    CDECL_CALL(0x0058F9C0);

    lua_getglobal(g_L, "engine");
    lua_pushnil(g_L);
    while (lua_next(g_L, -2)) {
        printf("engine.%s (type: %s)\n",
            lua_tostring(g_L, -2),
            luaL_typename(g_L, -1));
        lua_pop(g_L, 1);
    }

    enumerate_scripts();
}

void destroy_scripting()
{
    trigger_callback("on_shutdown");
    lua_close(g_L);
}

bool __cdecl hk_exec(bool a1)
{
    bool ret = exec_orig(a1);
    trigger_callback("on_script_exec");
    return ret;
}

void __cdecl hk_script_manager_run(float unusedTime, bool run_once)
{
    trigger_callback("tick");
    script_manager_run_orig(unusedTime, run_once);
}


#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <cstdint>
#include "MinHook.h"
#include <fstream>
#include <ostream>
#include <filesystem>

#ifndef PCH_H
#define PCH_H


#define PLATFORM_PS2    0
#define PLATFORM_XBOX   1
#define PLATFORM_GC     2
#define PLATFORM_PC     3

#ifndef PLATFORM
#define PLATFORM        PLATFORM_PC
#else
#if defined(PLATFORM_PS2)
#define PLATFORM        PLATFORM_PS2
#elif defined(PLATFORM_XBOX)
#define PLATFORM        PLATFORM_XBOX
#elif defined(PLATFORM_GC)
#define PLATFORM        PLATFORM_GC
#elif defined(PLATFORM_PC)
#define PLATFORM        PLATFORM_PC
#endif

#endif

#endif //PCH_H



std::map <tlresource_type, int> tlres_to_res_key = {
    { TLRESOURCE_TYPE_NONE, -1 },
    { TLRESOURCE_TYPE_TEXTURE,  6 },
    { TLRESOURCE_TYPE_MESH_FILE, 21 },
    { TLRESOURCE_TYPE_MESH, 21 },

    { TLRESOURCE_TYPE_MORPH_FILE, 22 },
    { TLRESOURCE_TYPE_MORPH, 5 },

    { TLRESOURCE_TYPE_MATERIAL_FILE, 23 },
    { TLRESOURCE_TYPE_MATERIAL, 7 },

    { TLRESOURCE_TYPE_ANIM_FILE, 1 },
    { TLRESOURCE_TYPE_ANIM, 1 },

    { TLRESOURCE_TYPE_SCENE_ANIM, 26 },

    { TLRESOURCE_TYPE_SKELETON, 2 },
    { TLRESOURCE_TYPE_Z, 12 }, // ?
    { TLRESOURCE_TYPE_TEXTURE, 6 },
    { TLRESOURCE_TYPE_TEXTURE, 6 },
    { TLRESOURCE_TYPE_TEXTURE, 6 },
};

#pragma once

#include <unordered_map>
#include <filesystem>

#include "common.h"

static uint32_t current_pack2 = -1;

constexpr uint32_t rootDir2 = 0xFFFFFFFF;

struct Mod2 {
    std::filesystem::path Path;
    int Type;
    std::vector<uint8_t> Data;
};

static std::unordered_map<uint64_t, std::vector<Mod2>> Mods2;

static inline uint64_t make_key2(uint32_t nameHash, uint32_t dirHash = rootDir2) {
    return (uint64_t(dirHash) << 32) | nameHash;
}

[[maybe_unused]] static std::string transformToLower2(const std::string& name)
{
    std::string res = name;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

[[maybe_unused]] static std::string transformToUpper2(const std::string& name)
{
    std::string res = name;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::toupper(c); });
    return res;
}

static std::vector<uint8_t> read_file2(const std::filesystem::path& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        return std::vector<uint8_t>();

    file.seekg(0, std::ios::end);
    std::streamsize sz = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(sz);
    buffer.resize(sz); buffer.reserve(sz);
    file.read(reinterpret_cast<char*>(buffer.data()), sz);
    return buffer;
}




static inline constexpr char* lookup_string(uint32_t hash) {
    return ((char* (__cdecl*)(uint32_t))0x531990)(hash);
}

Mod2* findModOfExt2(uint64_t hash, const char* expected_ext) {
    auto it = Mods2.find(hash);
    if (it == Mods2.end()) return nullptr;
    for (auto& mod : it->second) {
        if (transformToUpper2(mod.Path.extension().string()) == transformToUpper2(std::string(expected_ext)))
            return &mod;
    }
    return nullptr;
}

Mod2* findModOfTlResType2(uint64_t hash, tlresource_type type) {
    auto it = Mods2.find(hash);
    if (it == Mods2.end()) return nullptr;
    for (auto& mod : it->second) {
        if (mod.Type == type)
            return &mod;
    }
    return nullptr;
}

Mod2* getModOfResType2(const resource_key2* resource_id, uint32_t dirHash)
{
    const uint32_t nameHash = resource_id->m_hash;
    const char* expected_ext = resource_key_type_ext[PLATFORM][resource_id->m_type];
    if (!expected_ext) return nullptr;

    Mod2 * ret =  findModOfExt2(make_key2(nameHash, dirHash), expected_ext);
    // fallback to curr pack
    if (!ret && current_pack2 != 0xFFFFFFFFu)
        ret = findModOfExt2(make_key2(nameHash, current_pack2), expected_ext);
    // fallback to root dir
    if (!ret)
        ret = findModOfExt2(make_key2(nameHash), expected_ext);
    return ret;
}

static void enumerate_mods2() {
    namespace fs = std::filesystem;

    const fs::path modsDir = fs::current_path() / MOD_DIR;
    if (!fs::is_directory(modsDir))
        return;

    auto process_dir = [&](const fs::path& dirPath, uint32_t dirHash) {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (!entry.is_regular_file())
                continue;

            const fs::path& path = entry.path();
            std::vector<uint8_t> fileData = read_file(path);

            int resType = 0;
            std::string ext = transformToLower(path.extension().string());
            if (ext == ".dds" || ext == ".tga")
                resType = 1; // @todo
            else if (ext == ".pcmesh")  // @todo: other exts
                resType = TLRESOURCE_TYPE_MESH_FILE;

            const uint32_t nameHash = to_hash(path.stem().string().c_str());
            const uint64_t pathHash = make_key2(nameHash, dirHash);

            Mods2[pathHash].push_back(Mod2{
                path,
                resType,
                std::move(fileData)
            });

       //     printf(__FUNCTION__ ": dir=%s (0x%08X)\n  name=%s (0x%08X)\n",
          //      dirPath.filename().string().c_str(), dirHash,
             //   path.stem().string().c_str(), nameHash);
        }
    };


    process_dir(modsDir, rootDir2);

    for (const auto& dir : fs::directory_iterator(modsDir)) {
        if (!dir.is_directory())
            continue;

        const std::string dirName = dir.path().filename().string();
        const uint32_t dirHash = to_hash(dirName.c_str());
        process_dir(dir.path(), dirHash);
    }
}


inline fs::path redirect_to_mods(std::string_view inPath)
{
    fs::path input(inPath);
    fs::path base = fs::current_path(); 
    fs::path mods = base / MOD_DIR;

    if (!input.is_absolute()) {
        return (fs::path(MOD_DIR) / input).lexically_normal();
    }

    fs::path input_can, base_can;
    try {
        input_can = fs::weakly_canonical(input);
        base_can = fs::weakly_canonical(base);
    }
    catch (...) {
        return (mods / input.filename()).lexically_normal();
    }

    fs::path rel = fs::relative(input_can, base_can);

    bool is_under_base = true;
    for (const auto& part : rel) {
        if (part == "..") {
            is_under_base = false;
            break;
        }
    }

    if (is_under_base && !rel.empty())
        return (mods / rel).lexically_normal();
    return (mods / input_can.filename()).lexically_normal();
}



struct generic_mash_header {
    int32_t total_size;
    int32_t flags;
    int32_t class_id;
    int32_t pad;
};




typedef tlresource_location2* (__thiscall * get_tlresource_loc_t)(resource_directory2* self, unsigned __int16 idx, tlresource_type a3);
get_tlresource_loc_t get_tlresource_loc_orig;

tlresource_location2* __fastcall hk_get_tlresource_loc(resource_directory2* self, void* edx, unsigned __int16 idx, tlresource_type a3)
{
    tlresource_location2* ret = get_tlresource_loc_orig(self, idx, a3);

    auto iter = tlres_to_res_key.find(a3);
    if (iter != tlres_to_res_key.end()) {
        const char* ext = resource_key_type_ext[PLATFORM][iter->second];
        uint32_t packhash = self->pack_slot->field_4.m_hash;
        uint32_t hash = make_key2(ret->name.source_hash_code, packhash);
        char* s = lookup_string(ret->name.source_hash_code);
    //    printf(__FUNCTION__ ": searching for %s\\%s%s\n", lookup_string(packhash), s ? s : std::format("0x{:08X}", ret->name.source_hash_code).c_str(), ext);

        if (auto mod = findModOfTlResType2(hash, a3)) {
            ret->mesh_file = reinterpret_cast<char*>(mod->Data.data());
        }
    }
    return ret;
}

struct tlFixedString2 {
    uint32_t hash;
    char name[28];
};



typedef char* (__thiscall * get_tlresource_t)(resource_directory2* self, tlFixedString2* out_loc_, tlresource_type tlres_type);
get_tlresource_t get_tlresource_orig;


char* __fastcall hk_get_tlresource(resource_directory2* self, void* edx, tlFixedString2* out_loc_, tlresource_type tlres_type)
{
    char* ret = get_tlresource_orig(self, out_loc_, tlres_type);

    if (ret) {

        uint32_t dirHash = rootDir2;
        if (self->pack_slot) {
            dirHash = self->pack_slot->field_4.m_hash;
        }
        const char* ext = nullptr;
        auto iter = tlres_to_res_key.find(tlres_type);
        if (iter != tlres_to_res_key.end()) {
            ext = resource_key_type_ext[PLATFORM][iter->second];

            auto hash = out_loc_->hash;
            char* s = lookup_string(hash);
           // printf(__FUNCTION__ ": searching for %s\\%s%s\n", lookup_string(dirHash), s ? s : std::format("0x{:08X}", hash).c_str(), ext);

            if (auto mod = findModOfTlResType2(make_key2(out_loc_->hash, self->pack_slot->field_4.m_hash), tlres_type)) {
                ret = reinterpret_cast<char*>(mod->Data.data());
            }
        }
    }


    return ret;
}

typedef void(__cdecl* set_active_resource_context_t)(resource_pack_slot2* pack_slot);
set_active_resource_context_t set_active_resource_context_orig;

void hk_set_active_resource_context(resource_pack_slot2* pack_slot)
{
    set_active_resource_context_orig(pack_slot);
    if (pack_slot)
        current_pack2 = pack_slot->field_4.m_hash;
}

typedef unsigned __int8* (__cdecl* get_resource_t)(const resource_key2*, int*, resource_pack_slot2**);
get_resource_t get_resource_orig;

unsigned __int8* __cdecl hk_get_resource(const resource_key2* resource_id, int* size, resource_pack_slot2** slot) {
    const uint32_t req_hash = resource_id->m_hash;
    const char* req_ext = resource_key_type_ext[PLATFORM][resource_id->m_type];

    uint8_t* ret = get_resource_orig(resource_id, size, slot);
    if (!ret || resource_id->m_type == 3)
        return ret;


    auto packhash = current_pack2;
   // printf(__FUNCTION__ ": searching for %s\\%s%s\n", lookup_string(packhash), lookup_string(req_hash), req_ext);
    auto mod = getModOfResType2(resource_id, packhash);
    if (mod) {
    //    printf(__FUNCTION__ ": found %s\n", mod->Path.string().c_str());
        if (mod) {
            if (size) *size = (int)mod->Data.size();
            ret = reinterpret_cast<uint8_t*>(mod->Data.data());
        }
        printf("ret = 0x%08X\n", ret);
    }
    return ret;
}


typedef int(__cdecl* nflopenfile_t)(int, const char*);
nflopenfile_t nflopenfile_orig;

int hk_nflopenfile(int type, const char* str)
{
    auto modPath = redirect_to_mods(str);
   // printf(__FUNCTION__": searching for %s\n", modPath.string().c_str());
    if (fs::exists(modPath)) {
        //printf(__FUNCTION__": found %s\n", modPath.string().c_str());
        return nflopenfile_orig(type, modPath.string().c_str());
    }
    return nflopenfile_orig(type, str);
}


typedef unsigned __int8* (__thiscall* get_resource_dir_t)(resource_directory2*, const resource_key2*, int*, resource_pack_slot2**);
get_resource_dir_t get_resource_dir_orig;

unsigned __int8* __fastcall hk_get_resource_dir(resource_directory2* self, void* edx, const resource_key2* resource_id, int* size, resource_pack_slot2** slot)
{
    uint32_t req_hash = resource_id->m_hash;
    uint32_t type = resource_id->m_type;
    const char* req_ext = resource_key_type_ext[PLATFORM][type];

    uint8_t* ret = get_resource_dir_orig(self, resource_id, size, slot);
    if (!ret)
        return ret;

    uint32_t dirHash = current_pack2;
    if (self->pack_slot)
        dirHash = self->pack_slot->field_4.m_hash;

    //printf(__FUNCTION__ ": searching for %s\\%s%s\n", lookup_string(dirHash), lookup_string(req_hash), req_ext);
    if (Mod2* mod = getModOfResType2(resource_id, dirHash)) {
       // printf(__FUNCTION__ ": found %s\n", mod->Path.string().c_str());
        if (mod) {
            if (size) *size = (int)mod->Data.size();
            ret = reinterpret_cast<uint8_t*>(mod->Data.data());
        }
        printf("ret = 0x%08X\n", ret);
        return ret;
    }
    return ret;
}


#pragma once

#include <windows.h>
#include <cstdint>
#include <cstring>

// Simple x86 inline hook implementation
class InlineHook {
public:
    void* target;
    void* detour;
    void* trampoline;
    uint8_t original[16];
    size_t stolenBytes;
    bool enabled;

    InlineHook() : target(nullptr), detour(nullptr), trampoline(nullptr), stolenBytes(0), enabled(false) {}

    ~InlineHook() {

        if (trampoline) {
            VirtualFree(trampoline, 0, MEM_RELEASE);
        }
    }
};

// Minimal x86 length disassembler for common instructions
static size_t GetInstructionLength(uint8_t* addr) {
    uint8_t* p = addr;
    
    // Handle prefixes
    while (*p == 0x66 || *p == 0x67 || *p == 0xF2 || *p == 0xF3 ||
           (*p >= 0x26 && *p <= 0x3E && (*p & 7) == 6)) {
        p++;
    }

    uint8_t op = *p++;
    
    // Common single-byte instructions
    if ((op >= 0x50 && op <= 0x5F) ||  // PUSH/POP reg
        (op >= 0x90 && op <= 0x97) ||  // NOP/XCHG
        op == 0xC3 || op == 0xCB ||    // RET
        op == 0xCC ||                   // INT3
        op == 0xC9 ||                   // LEAVE
        op == 0x9C || op == 0x9D)      // PUSHF/POPF
        return p - addr;

    // MOV reg, imm32
    if (op >= 0xB8 && op <= 0xBF)
        return (p - addr) + 4;

    // PUSH imm8
    if (op == 0x6A)
        return (p - addr) + 1;

    // PUSH imm32
    if (op == 0x68)
        return (p - addr) + 4;

    // Short JMP/Jcc
    if (op == 0xEB || (op >= 0x70 && op <= 0x7F))
        return (p - addr) + 1;

    // Near JMP/CALL
    if (op == 0xE8 || op == 0xE9)
        return (p - addr) + 4;

    // ModR/M instructions
    auto parseModRM = [&]() -> size_t {
        uint8_t modrm = *p++;
        uint8_t mod = modrm >> 6;
        uint8_t rm = modrm & 7;

        if (mod == 3) return p - addr;  // Register direct
        
        // SIB byte
        if (rm == 4 && mod != 3) p++;
        
        if (mod == 0 && rm == 5) return (p - addr) + 4;  // disp32
        if (mod == 1) return (p - addr) + 1;              // disp8
        if (mod == 2) return (p - addr) + 4;              // disp32
        
        return p - addr;
    };

    // Two-byte opcode
    if (op == 0x0F) {
        uint8_t op2 = *p++;
        // Near Jcc
        if (op2 >= 0x80 && op2 <= 0x8F)
            return (p - addr) + 4;
        // MOVZX/MOVSX, etc.
        if ((op2 >= 0xB6 && op2 <= 0xBF) || (op2 >= 0x40 && op2 <= 0x4F))
            return parseModRM();
    }

    // ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r/m, r
    if ((op & 0xC4) == 0x00 || (op & 0xC4) == 0x04)
        return parseModRM();

    // MOV r/m, r or MOV r, r/m
    if ((op >= 0x88 && op <= 0x8B) || op == 0x8D)
        return parseModRM();

    // TEST/XCHG r/m, r
    if (op == 0x84 || op == 0x85 || op == 0x86 || op == 0x87)
        return parseModRM();

    // Group 1: ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r/m, imm
    if (op == 0x80 || op == 0x82)
        return parseModRM() + 1;
    if (op == 0x81)
        return parseModRM() + 4;
    if (op == 0x83)
        return parseModRM() + 1;

    // SUB esp, imm8
    if (op == 0x83) {
        uint8_t modrm = *p;
        if ((modrm & 0xF8) == 0xE8)
            return (p - addr) + 2;
    }

    // Default fallback - assume 1 byte
    return 1;
}

static size_t CalculateStolenBytes(void* target, size_t minBytes) {
    size_t total = 0;
    uint8_t* p = (uint8_t*)target;
    while (total < minBytes) {
        size_t len = GetInstructionLength(p);
        if (len == 0) len = 1;  // Safety fallback
        total += len;
        p += len;
    }
    return total;
}

static bool CreateHook(InlineHook* hook, void* target, void* detour, void** original) {
    if (!hook || !target || !detour) return false;

    hook->target = target;
    hook->detour = detour;
    hook->stolenBytes = CalculateStolenBytes(target, 5);
    
    if (hook->stolenBytes > sizeof(hook->original))
        return false;

    // Allocate trampoline near the target for short jumps
    hook->trampoline = VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!hook->trampoline) return false;

    // Copy original bytes
    memcpy(hook->original, target, hook->stolenBytes);
    
    // Build trampoline: stolen bytes + JMP back
    uint8_t* tramp = (uint8_t*)hook->trampoline;
    memcpy(tramp, hook->original, hook->stolenBytes);
    tramp += hook->stolenBytes;
    
    // JMP rel32 back to original + stolenBytes
    *tramp++ = 0xE9;
    int32_t relBack = (int32_t)((uint8_t*)target + hook->stolenBytes - (tramp + 4));
    memcpy(tramp, &relBack, 4);

    if (original)
        *original = hook->trampoline;

    return true;
}

static bool EnableHook(InlineHook* hook) {
    if (!hook || !hook->target || hook->enabled) return false;

    DWORD oldProtect;
    if (!VirtualProtect(hook->target, hook->stolenBytes, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    // Write JMP to detour
    uint8_t* p = (uint8_t*)hook->target;
    *p++ = 0xE9;
    int32_t rel = (int32_t)((uint8_t*)hook->detour - (p + 4));
    memcpy(p, &rel, 4);
    
    // NOP remaining bytes
    for (size_t i = 5; i < hook->stolenBytes; i++)
        ((uint8_t*)hook->target)[i] = 0x90;

    VirtualProtect(hook->target, hook->stolenBytes, oldProtect, &oldProtect);
    hook->enabled = true;
    return true;
}

static bool DisableHook(InlineHook* hook) {
    if (!hook || !hook->target || !hook->enabled) return false;

    DWORD oldProtect;
    if (!VirtualProtect(hook->target, hook->stolenBytes, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    memcpy(hook->target, hook->original, hook->stolenBytes);
    
    VirtualProtect(hook->target, hook->stolenBytes, oldProtect, &oldProtect);
    hook->enabled = false;
    return true;
}

// ============ Usage wrapper ============

#define MAX_HOOKS 32
static InlineHook g_hooks[MAX_HOOKS];
static int g_hookCount = 0;

bool Hook_Create(void* target, void* detour, void** original) {
    if (g_hookCount >= MAX_HOOKS) return false;
    InlineHook* h = &g_hooks[g_hookCount];
    if (!CreateHook(h, target, detour, original)) return false;
    g_hookCount++;
    return true;
}

bool Hook_EnableAll() {
    for (int i = 0; i < g_hookCount; i++) {
        if (!EnableHook(&g_hooks[i])) return false;
    }
    return true;
}

bool Hook_DisableAll() {
    for (int i = 0; i < g_hookCount; i++) {
        DisableHook(&g_hooks[i]);
    }
    return true;
}

void Hook_Cleanup() {
    Hook_DisableAll();
    for (int i = 0; i < g_hookCount; i++) {
        if (g_hooks[i].trampoline) {
            VirtualFree(g_hooks[i].trampoline, 0, MEM_RELEASE);
            g_hooks[i].trampoline = nullptr;
        }
    }
    g_hookCount = 0;
}

void destroy_hooks()
{
#if PLATFORM == PLATFORM_PC
    Hook_Cleanup();
    
#   if WIP_SCRIPTING
        destroy_scripting();
#   endif
#else
#   error "Unsupported platform"
#endif
}

void init_hooks()
{
#if WIP_SCRIPTING
    init_scripting();
    REDIRECT(0x005AD77D, hk_script_ctor);
#endif

#if PLATFORM == PLATFORM_PC
#   if defined(_DEBUG)
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#   endif

    bool ok = true;
    
	ok &= Hook_Create((void*)0x0052AA70, (void*)hk_get_resource_dir, (void**)&get_resource_dir_orig);
	
	ok &= Hook_Create((void*)0x00531B30, (void*)hk_get_resource, (void**)&get_resource_orig);
	
	//ok &= Hook_Create((void*)0x0052AAE0, (void*)hk_get_tlresource, (void**)&get_tlresource_orig);
	
	
		//ok &= Hook_Create((void*)0x0079E490, (void*)hk_nflopenfile, (void**)&nflopenfile_orig);
   
   		    ok &= Hook_Create((void*)0x00563090, (void*)hk_get_tlresource_loc, (void**)&get_tlresource_loc_orig);
      //ok &= Hook_Create((void*)0x0051EC80, (void*)hk_set_active_resource_context, (void**)&set_active_resource_context_orig);
#   if WIP_SCRIPTING
        ok &= Hook_Create((void*)0x0058EDE0, (void*)hk_script_func_reg, (void**)&script_func_reg_orig);
        ok &= Hook_Create((void*)0x0058EE30, (void*)hk_script_func, (void**)&script_func_orig);
        ok &= Hook_Create((void*)0x0064E740, (void*)hk_exec, (void**)&exec_orig);
        ok &= Hook_Create((void*)0x005AF9F0, (void*)hk_script_manager_run, (void**)&script_manager_run_orig);

	

   
   

#   endif

    if (ok) {
        ok = Hook_EnableAll();
    }

    if (ok) {
        enumerate_mods2();
    } else {
        destroy_hooks();
    }
#else
#   error "Unsupported platform"
#endif
}


