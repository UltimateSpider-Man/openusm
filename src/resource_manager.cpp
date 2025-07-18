#include "resource_manager.h"

#include "binary_search_array_cmp.h"
#include "common.h"
#include "filespec.h"
#include "func_wrapper.h"
#include "game.h"
#include "limited_timer.h"
#include "log.h"
#include "trace.h"
#include "memory.h"
#include "nal_system.h"
#include "nfl_system.h"
#include "ngl.h"
#include "nlPlatformEnum.h"
#include "os_file.h"
#include "os_developer_options.h"
#include "debug_menu.h"
#include "resource_amalgapak_header.h"
#include "resource_directory.h"
#include "return_address.h"
#include "utility.h"
#include "variables.h"
#include "worldly_pack_slot.h"
#include "osassert.h"

#include <cassert>
#include <numeric>

namespace resource_manager {

VALIDATE_SIZE(resource_memory_map, 0x90);

VALIDATE_SIZE((*partitions), 16u);

_std::vector<resource_partition *> *& partitions = var<_std::vector<resource_partition *> *>(0x0095C7F0);

_std::vector<resource_pack_slot *> & resource_context_stack = var<_std::vector<resource_pack_slot *>>(0x0096015C);

mString & amalgapak_name = var<mString>(0x0095CAD4);

#if !STANDALONE_SYSTEM 

int & amalgapak_base_offset = var<int>(0x00921CB4);

nflFileID & amalgapak_id = var<nflFileID>(0x00921CB8);

int & resource_buffer_used = var<int>(0x0095C180);

int & memory_maps_count = var<int>(0x0095C7F4);

int & resource_buffer_size = var<int>(0x0095C1C8);

int & in_use_memory_map = var<int>(0x00921CB0);

uint8_t *& resource_buffer = var<uint8_t *>(0x0095C738);

bool & using_amalga = var<bool>(0x0095C800);

int & amalgapak_signature = var<int>(0x0095C804);

resource_memory_map *& memory_maps = var<resource_memory_map *>(0x0095C2F0);

int & amalgapak_pack_location_count = var<int>(0x0095C7FC);

resource_pack_location *& amalgapak_pack_location_table = var<resource_pack_location *>(0x0095C7F8);

int & amalgapak_prerequisite_count = var<int>(0x0095C174);

resource_key *& amalgapak_prerequisite_table = var<resource_key *>(0x0095C300);

#else

#define make_var(type, name) \
    static type g_##name {}; \
    type& name {g_##name}

make_var(int, amalgapak_base_offset);

make_var(nflFileID, amalgapak_id);

make_var(int, resource_buffer_used);

make_var(int, memory_maps_count);

make_var(int, resource_buffer_size);

make_var(int, in_use_memory_map);

make_var(uint8_t *, resource_buffer);

make_var(bool, using_amalga);

make_var(int, amalgapak_signature);

make_var(resource_memory_map *, memory_maps);

make_var(int, amalgapak_pack_location_count);

make_var(resource_pack_location *, amalgapak_pack_location_table);

make_var(int, amalgapak_prerequisite_count);

make_var(resource_key *, amalgapak_prerequisite_table);

//make_var(mString, amalgapak_name);

#undef make_var
#endif

//0x005BA9A0
[[nodiscard]] mString get_amalgapak_filename(_nlPlatformEnum arg4)
{
    const char *a2[] = {".PAK", "_XB.PAK", "_GC.PAK", "_PC.PAK"};

#ifdef TARGET_XBOX
    mString v1{a2[1]};
#else
    mString v1{a2[arg4]};
#endif
    
    mString v2{"packs\\amalga"};

    mString res = v2 + v1;

    return res;
}

int get_pack_location_count()
{
    assert(amalgapak_pack_location_table != nullptr);
    return amalgapak_pack_location_count;
}

resource_key *get_prerequisiste(int prereq_idx)
{
    assert(amalgapak_prerequisite_table != nullptr);
    assert(prereq_idx < amalgapak_prerequisite_count);

    return &amalgapak_prerequisite_table[prereq_idx];
}

void load_amalgapak()
{
    TRACE("resource_manager::load_amalgapak");

    if constexpr (1)
    {
        os_file file;

        {
            amalgapak_name = get_amalgapak_filename(g_platform);
            sp_log("Loading amalgapak...");

            mString a1 {amalgapak_name.c_str()};

            file.open(a1, os_file::FILE_READ);
        }

        if (!file.is_open()) {
            auto *v1 = amalgapak_name.c_str();
            error("Could not open amalgapak file %s!", v1);
        }

        resource_amalgapak_header pack_file_header{};
        file.read(&pack_file_header, sizeof(resource_amalgapak_header));

        {
            mString a1 {amalgapak_name.c_str()};

            pack_file_header.verify(a1);
        }

        if constexpr (1)
        {
            pack_file_header.field_18 = 0;
        }

        amalgapak_base_offset = pack_file_header.field_18;
        using_amalga = (pack_file_header.field_18 != 0);
        amalgapak_signature = pack_file_header.field_14;
        amalgapak_pack_location_count = pack_file_header.location_table_size /
            sizeof(resource_pack_location);

        amalgapak_pack_location_table = static_cast<resource_pack_location *>(
            arch_memalign(16u, pack_file_header.location_table_size));
        assert(amalgapak_pack_location_table != nullptr);

        file.set_fp(pack_file_header.field_1C, os_file::FP_BEGIN);
        auto how_many_did_we_get = file.read(amalgapak_pack_location_table,
                                             pack_file_header.location_table_size);
        assert(how_many_did_we_get == pack_file_header.location_table_size);

        amalgapak_prerequisite_count = static_cast<uint32_t>(
                                             pack_file_header.prerequisite_table_size) >>
            3;

        amalgapak_prerequisite_table = static_cast<resource_key *>(
            arch_memalign(8u, pack_file_header.prerequisite_table_size));
        assert(amalgapak_prerequisite_table != nullptr);

        file.set_fp(pack_file_header.field_2C, os_file::FP_BEGIN);
        how_many_did_we_get = file.read(amalgapak_prerequisite_table,
                                        pack_file_header.prerequisite_table_size);
        assert(how_many_did_we_get == pack_file_header.prerequisite_table_size);

        resource_buffer_size = pack_file_header.field_34;
        assert(pack_file_header.memory_map_table_size % sizeof(resource_memory_map) == 0);

        memory_maps_count = pack_file_header.memory_map_table_size / sizeof(resource_memory_map);

        memory_maps = new resource_memory_map[memory_maps_count];
        file.set_fp(pack_file_header.field_24, os_file::FP_BEGIN);
        how_many_did_we_get = file.read(memory_maps, pack_file_header.memory_map_table_size);
        assert(how_many_did_we_get == pack_file_header.memory_map_table_size);

        file.close();

        if (using_amalgapak())
        {
            amalgapak_id = nflOpenFile({1}, amalgapak_name.c_str());

            if (amalgapak_id == NFL_FILE_ID_INVALID)
            {
                amalgapak_id = nflOpenFile({2}, amalgapak_name.c_str());

                if (amalgapak_id == NFL_FILE_ID_INVALID)
                {
                    mString v12 {amalgapak_name.c_str()};
                    mString v13 {"data\\"};

                    mString a1 = v13 + v12;

                    amalgapak_id = nflOpenFile({2}, a1.c_str());
                }
            }

            sp_log("Using amalgapak found on the HOST");
        } else {
            sp_log("Using amalgapak found on the CD");
        }

    } else {
        CDECL_CALL(0x00537650);
    }
}


void add_resource_pack_modified_callback(void (*callback)(_std::vector<resource_key> &))
{
    assert(callback != nullptr);

    //push_back
    auto *v18 = resource_pack_modified_callbacks.m_last;
    auto *a2 = callback;
    if ( resource_pack_modified_callbacks.size() < resource_pack_modified_callbacks.capacity()
         )
    {
        *resource_pack_modified_callbacks.m_last = a2;
        resource_pack_modified_callbacks.m_last = v18 + 1;
    }
    else
    {
        void (__fastcall *_Insert_n)(void *, void *, void *, int, decltype(&callback)) = CAST(_Insert_n, 0x0056A260);
        _Insert_n(&resource_pack_modified_callbacks,
                nullptr,
                resource_pack_modified_callbacks.m_last,
                1,
                &a2);
    }
}

bool using_amalgapak()
{
    return using_amalga;
}

bool is_idle()
{
    if constexpr (1)
    {
        assert(partitions != nullptr);

        for ( auto &partition : (*partitions) )
        {
            assert(partition != nullptr);
            if ( !partition->get_streamer()->is_idle() )
            {
                return false;
            }
        }

        return true;
    }
    else
    {
        return (bool) CDECL_CALL(0x00537AC0);
    }
}

bool can_reload_amalgapak()
{
    if constexpr (1)
    {
        if ( using_amalgapak() )
        {
            return false;
        }

        if ( !is_idle() )
        {
            return false;
        }

        bool result = false;
        os_file v11{};
        auto *v1 = amalgapak_name.c_str();
        mString v4 {v1};
        v11.open(v4, os_file::FILE_READ);
        if ( v11.is_open() )
        {
            resource_amalgapak_header data{};
            v11.read(&data, sizeof(data));
            auto *v2 = amalgapak_name.c_str();
            auto a2 = mString{v2};
            data.verify(a2);
            if ( data.field_18 != 0 )
            {
                result = false;
            }
            else if ( data.field_14 == amalgapak_signature )
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
        else
        {
            result = false;
        }

        return result;
    }
    else
    {
        return (bool) CDECL_CALL(0x0053DE90);
    }
}

void reload_amalgapak()
{
    TRACE("resource_manager::reload_amalgapak");

    if constexpr (1)
    {
        assert(!using_amalgapak());

        assert(amalgapak_pack_location_table != nullptr);

        assert(amalgapak_prerequisite_table != nullptr);

        assert(memory_maps != nullptr);

        mem_freealign(amalgapak_prerequisite_table);
        mem_freealign(amalgapak_pack_location_table);

        delete[](memory_maps);
        amalgapak_prerequisite_table = nullptr;
        amalgapak_pack_location_table = nullptr;
        memory_maps = nullptr;

        load_amalgapak();

        _std::vector<resource_key> v3;
        for ( auto i = 0; i < amalgapak_pack_location_count; ++i )
        {
            if ( amalgapak_pack_location_table[i].field_2C != 0 )
            {
                v3.push_back(amalgapak_pack_location_table[i].loc.field_0);
            }
        }

        for ( auto &cb : resource_pack_modified_callbacks )
        {
            (*cb)(v3);
        }
    }
    else
    {
        CDECL_CALL(0x0054C2E0);
    }
}


resource_pack_slot *get_best_context(resource_pack_slot *slot)
{
    TRACE("resource_manager::get_best_context");

    if constexpr (1)
    {
        assert(slot != nullptr);
        assert(slot->is_data_ready());
        assert(partitions != nullptr);

        resource_partition *the_partition = nullptr;

        const auto &vec = (*partitions);
        sp_log("%d", vec.size());
        for (const auto &my_partition : vec)
        {
            assert(my_partition != nullptr);

            auto &pack_slots = my_partition->get_pack_slots();
            for (uint32_t i = 0; i < pack_slots.size(); ++i)
            {
                if (pack_slots[i] == slot) {
                    the_partition = my_partition;
                    sp_log("%d", i);
                    break;
                }
            }
        }

        assert(the_partition != nullptr && "what partition uses this slot!?");

        if (the_partition->field_0 != 2) {
            return slot;
        }

        assert(!the_partition->get_pack_slots().empty());

        auto *result = the_partition->get_pack_slots().front();
        //sp_log("0x%08X", result->pack_directory.field_4.m_vtbl);

        return result;
    }
    else
    {
        return (resource_pack_slot *) CDECL_CALL(0x005375A0, slot);
    }
}

resource_pack_slot *get_and_push_resource_context(resource_partition_enum a1)
{
    auto *v1 = get_best_context(a1);
    return push_resource_context(v1);
}

bool get_pack_location(int a1, resource_pack_location *a2)
{
    assert(amalgapak_pack_location_table != nullptr);
    assert(amalgapak_base_offset != -1);

    if ( a1 < 0 || a1 >= amalgapak_pack_location_count )
    {
        return false;
    }

    if ( a2 != nullptr )
    {
        *a2 = amalgapak_pack_location_table[a1];
        a2->loc.m_offset += amalgapak_base_offset;
    }

    return true;
}

resource_pack_slot *get_best_context(resource_partition_enum a1)
{
    if constexpr (1)
    {
        assert(partitions != nullptr);

        resource_partition *the_partition = partitions->at(a1);
        assert(the_partition != nullptr);

        const auto &pack_slots = the_partition->get_pack_slots();
        if (pack_slots.empty()) {
            the_partition = partitions->front();
        }

        resource_pack_slot *best_slot = the_partition->get_pack_slots().front();
        assert(best_slot != nullptr);

        return best_slot;
    } else {
        return (resource_pack_slot *) CDECL_CALL(0x00537610, a1);
    }
}

void frame_advance(Float a2)
{
    auto v8 =
        os_developer_options::instance->get_int(mString {"AMALGA_REFRESH_INTERVAL"});

    static float amalga_refresh_timer {0};
    amalga_refresh_timer += a2;
    if ( v8 > 0 && amalga_refresh_timer > v8 )
    {
        if ( can_reload_amalgapak() )
        {
            reload_amalgapak();
        }

        amalga_refresh_timer = 0.0;
    }

    if constexpr (0)
    {
        static auto & dword_960CB0 = var<int>(0x00960CB0);

        if (dword_960CB0 == 0)
        {
            limited_timer timer{0.02};

            if (g_game_ptr != nullptr && g_game_ptr->field_165)
            {
                limited_timer v4{0.5};

                timer = v4;
            }

            timer.reset();

            assert(partitions != nullptr);

            for (auto *partition : (*partitions)) {

                assert(partition != nullptr);

                partition->frame_advance(a2, &timer);
            }
        }
    }
    else
    {
        CDECL_CALL(0x00558D20, a2);
    }

#if defined(ENABLE_DEBUG_MENU) && DEBUG_MENU_REIMPL == 0
    debug_menu::frame_advance(a2);
#endif
}

bool get_pack_file_stats(const resource_key &a1, resource_pack_location *a2, mString *a3, int *a4)
{
    TRACE("resource_manager::get_pack_file_stats", a1.get_platform_string(g_platform).c_str());

    if constexpr (1)
    {
        assert(amalgapak_pack_location_table != nullptr);

        if (a3 != nullptr) {
            *a3 = amalgapak_name.c_str();
        }

        assert(amalgapak_base_offset != -1);

        {
            auto is_sorted = std::is_sorted(amalgapak_pack_location_table,
                    amalgapak_pack_location_table + amalgapak_pack_location_count,
                    [](auto &a1, auto &a2) {
                        return a1.loc.field_0 <= a2.loc.field_0;
                    });
        //    assert(is_sorted);
        }

        auto i = 0;
        if (!binary_search_array_cmp<const resource_key, const resource_pack_location>(
                &a1,
                amalgapak_pack_location_table,
                0,
                amalgapak_pack_location_count,
                &i,
                compare_resource_key_resource_pack_location))
        {
            return false;
        }


        if (a2 != nullptr) {
            *a2 = amalgapak_pack_location_table[i];
            a2->loc.m_offset += amalgapak_base_offset;
        }

        if (a4 != nullptr) {
            *a4 = i;
        }

        return true;
    } else {
        auto result = (bool) CDECL_CALL(0x0052A820, &a1, a2, a3, a4);
        sp_log("%s", result ? "true" : "false");
        return result;
    }
}

resource_pack_slot *push_resource_context(resource_pack_slot *pack_slot)
{
    TRACE("resource_manager::push_resource_context");

    sp_log("%s", pack_slot->get_name_key().get_platform_string(3).c_str());

    if constexpr (1)
    {
        assert(pack_slot != nullptr);

        resource_pack_slot *v2 = get_resource_context();

        //push_back
        if (resource_context_stack.size() < resource_context_stack.capacity())
        {
            *resource_context_stack.m_last = pack_slot;
            ++resource_context_stack.m_last;

        }
        else
        {
            if constexpr (1)
            {
                void (__fastcall *func)(void *, void *edx, void *, int, resource_pack_slot **) = CAST(func, 0x0056A260);
                func(&resource_context_stack, nullptr,
                     resource_context_stack.m_last,
                     1,
                     &pack_slot);
            }
            else
            {
                resource_context_stack.insert(resource_context_stack.end(), pack_slot);
            }
        }

        set_active_resource_context(pack_slot);

        return v2;
    } else {
        return (resource_pack_slot *) CDECL_CALL(0x00542740, pack_slot);
    }
}

resource_directory *get_resource_directory(const resource_key &a1)
{
    if constexpr (1)
    {
        assert(partitions != nullptr);

        for (size_t i = 0; i < partitions->size(); ++i) {
            auto &partition = partitions->at(i);
            assert(partition != nullptr);

            auto *streamer = partition->get_streamer();
            assert(streamer != nullptr);

            auto *pack_slots = streamer->get_pack_slots();
            assert(pack_slots != nullptr);

            for (auto &pack_slot : (*pack_slots)) {
                assert(pack_slot != nullptr);

                if (pack_slot->is_data_ready())
                {
                    if (pack_slot->get_name_key() == a1) {
                        return &pack_slot->get_resource_directory();
                    }
                }
            }
        }

        return nullptr;
    } else {
        return (resource_directory *) CDECL_CALL(0x00537A10, &a1);
    }
}

void set_active_resource_context(resource_pack_slot *a1)
{
    TRACE("resource_manager::set_active_resource_context");

    if constexpr (0)
    {
        if (a1 != nullptr && a1->is_data_ready())
        {
            auto &pack_dir = a1->get_resource_pack_directory();
            nglSetTextureDirectory(&pack_dir.field_4);
            nglSetMeshFileDirectory(&pack_dir.field_C);
            nglSetMeshDirectory(&pack_dir.field_14);
            nglSetMorphDirectory(&pack_dir.field_1C);
            nglSetMaterialFileDirectory(&pack_dir.field_34);
            nglSetMaterialDirectory(&pack_dir.field_2C);
            nalSetSkeletonDirectory(&pack_dir.field_54);
            nalSetAnimFileDirectory(&pack_dir.field_3C);
            nalSetAnimDirectory(&pack_dir.field_44);
            nalSetSceneAnimDirectory(&pack_dir.field_4C);
        }
        else
        {
            nglSetTextureDirectory(tlresource_directory<nglTexture, tlFixedString>::system_dir);
            nglSetMeshFileDirectory(tlresource_directory<nglMeshFile, tlFixedString>::system_dir);
            nglSetMeshDirectory(tlresource_directory<nglMesh, tlHashString>::system_dir);
            nglSetMorphDirectory(tlresource_directory<nglMorphSet, tlHashString>::system_dir);
            nglSetMaterialFileDirectory(
                tlresource_directory<nglMaterialFile, tlFixedString>::system_dir);
            nglSetMaterialDirectory(
                tlresource_directory<nglMaterialBase, tlHashString>::system_dir);
            nalSetAnimFileDirectory(tlresource_directory<nalAnimFile, tlFixedString>::system_dir);
            nalSetSkeletonDirectory(
                tlresource_directory<nalBaseSkeleton, tlFixedString>::system_dir);
            nalSetAnimDirectory(
                tlresource_directory<nalAnimClass<nalAnyPose>, tlFixedString>::system_dir);
            nalSetSceneAnimDirectory(
                tlresource_directory<nalSceneAnim, tlFixedString>::system_dir);
        }

    } else {
        CDECL_CALL(0x0051EC80, a1);
    }
}

resource_pack_slot *pop_resource_context()
{
    TRACE("resource_manager::pop_resource_context");

    if constexpr (1)
    {
        auto *old_context = get_resource_context();
        assert(old_context != nullptr);

#if 0 
        if (!resource_context_stack.empty())
        {
#ifndef TEST_CASE
            --resource_context_stack.m_last;
#else
            resource_context_stack.resize(resource_context_stack.size() - 1);
#endif
        }
    
#else
        sp_log("%d", resource_context_stack.size());
        resource_context_stack.pop_back();
        sp_log("%d", resource_context_stack.size());
#endif

        auto *v0 = get_resource_context();
        set_active_resource_context(v0);

        return old_context;
    } else {
        return (resource_pack_slot *) CDECL_CALL(0x00537530);
    }
}

void delete_inst() {
    TRACE("resource_manager::delete_inst");
    if constexpr (1)
    {
        if (amalgapak_pack_location_table != nullptr)
        {
            assert(amalgapak_pack_location_count > 0);

            mem_freealign(amalgapak_pack_location_table);
            amalgapak_pack_location_table = nullptr;
            nflCloseFile(amalgapak_id);
        }

        if (resource_buffer != nullptr) {
            mem_freealign(resource_buffer);
        }

        resource_buffer = nullptr;

        if (partitions != nullptr)
        {
            for (auto &part : (*partitions)) {
                if (part != nullptr) {
                    delete part;
                }
            }

            if (partitions != nullptr) {
                operator delete(partitions);
            }
        }

        partitions = nullptr;
        if (memory_maps_count > 0) {
            assert(memory_maps != nullptr);

            operator delete[](memory_maps);
        }
    }
    else
    {
        CDECL_CALL(0x00547AD0);
    }
}

void create_inst()
{
    TRACE("resource_manager::create_inst");

    if constexpr (1)
    {
        using vector_t = std::remove_pointer_t<std::decay_t<decltype(partitions)>>;
        partitions = new vector_t {};

        partitions->reserve(8u);

        in_use_memory_map = -1;
        amalgapak_base_offset = -1;
        amalgapak_id = NFL_FILE_ID_INVALID;
        memory_maps_count = 0;
        amalgapak_pack_location_count = 0;
        amalgapak_pack_location_table = nullptr;

        if (!g_is_the_packer())
        {
            load_amalgapak();
        }

        resource_buffer = static_cast<uint8_t *>(arch_memalign(4096u, resource_buffer_size));
        resource_buffer_used = 0;
        configure_packs_by_memory_map(0);

    }
    else
    {
        CDECL_CALL(0x0055BA30);
    }
}

void configure_packs_by_memory_map(int idx)
{
    TRACE("resource_manager::configure_packs_by_memory_map");

    assert(partitions != nullptr);

    {
        sp_log("--- begin ---");
        sp_log("in_use_memory_map = %d", in_use_memory_map);
        const auto partitions_size = partitions->size();
        sp_log("partitions_size = %u", partitions_size);

        sp_log("resource_buffer_used = %d", resource_buffer_used);
    }

    if constexpr (1)
    {
        const auto v14 = in_use_memory_map;
        int pop_start_idx = 0;

        const auto partitions_size = partitions->size();
        for (auto i = 0u; i < partitions_size; ++i) {
            auto func = [](const auto *self, const auto *a2) -> bool {
                return (self->field_0 == a2->field_0 && self->field_4 == a2->field_4
                        && self->field_8 == a2->field_8
                        && self->field_C == a2->field_C);
            };

            if (memory_maps[idx].field_10[i].field_4 == 1 &&
                func(&memory_maps[v14].field_10[i], &memory_maps[idx].field_10[i])) {
                ++pop_start_idx;
            }
        }

        for (int i = partitions_size - 1; i >= pop_start_idx; --i) {
            resource_buffer_used -= partitions->at(i)->partition_buffer_size;
            auto *part = partitions->back();
            assert(part != nullptr && part->get_streamer() != nullptr);

            auto *streamer = part->get_streamer();
            if (streamer->is_active()) {
                streamer->flush(nullptr);
                streamer->unload_all();
                streamer->flush(nullptr);
            }

            if (part != nullptr) {
                THISCALL(0x0053DFD0, part);
                operator delete(part);
                part = nullptr;
            }

            if (!partitions->empty()) {
#ifndef TEST_CASE
                --partitions->m_last;
#else
                partitions->resize(partitions->size() - 1);
#endif
            }
        }

        assert(static_cast<int>(partitions->size()) == pop_start_idx);

        for (uint32_t i = pop_start_idx; i < RESOURCE_PARTITION_END; ++i)
        {
            auto *new_partition = new resource_partition {static_cast<resource_partition_enum>(i)};

            auto &memory_map = memory_maps[idx];
            auto &tmp = memory_map.field_10[i];

            new_partition->field_0 = tmp.field_4;
            new_partition->partition_buffer_size = tmp.field_C *
                tmp.field_8;

            assert((new_partition->partition_buffer_size + resource_buffer_used <=
                    resource_buffer_size) &&
                   "Verify we have room for this partition");
        
            new_partition->partition_buffer_used = 0;
            new_partition->field_A8 = &resource_buffer[resource_buffer_used];
            resource_buffer_used += new_partition->partition_buffer_size;
            if (new_partition->field_0 >= 0 && new_partition->field_0 <= 1)
            {
                for (int j = 0; j < tmp.field_C; ++j) {
                    new_partition->push_pack_slot(tmp.field_8, nullptr);
                }
            }

            if constexpr (1)
            {
                if (partitions->size() < partitions->capacity())
                {
                    auto *v30 = partitions->m_last;
                    *v30 = new_partition;
                    partitions->m_last = v30 + 1;
                }
                else
                {
                    void (__fastcall *_Insert_n)(void *, void *edx, void *, int, resource_partition **) = CAST(_Insert_n, 0x0056A260);
                    _Insert_n(partitions, nullptr, partitions->m_last, 1, &new_partition);
                }
            }
            else
            {
                partitions->push_back(new_partition);
            }
        }

        assert(partitions->size() == RESOURCE_PARTITION_END &&
               "If this fails there's something wrong with the partition preserving code.");

        {
            auto begin = std::begin(memory_maps[idx].field_10);
            auto end = begin + RESOURCE_PARTITION_END;
            auto v7 = std::accumulate(begin, end, 0, [](auto prev_result, auto &v) {
                return v.field_C * v.field_8 + prev_result;
            });

            sp_log("Resource manager now using a memory map of size %d MB (%d KB)",
               v7 / 1024 / 1024,
               v7 / 1024);
        }

        in_use_memory_map = idx;
        set_active_resource_context(nullptr);
    }
    else
    {
        CDECL_CALL(0x00558930, idx);
    }

    {
        printf("\n");
        sp_log("--- end ---");

        sp_log("in_use_memory_map %d", in_use_memory_map);

        const auto partitions_size = partitions->size();
        sp_log("partitions_size = %u", partitions_size);

        sp_log("resource_buffer_used %d", resource_buffer_used);
    }
}

void set_active_district(bool a1)
{
    auto *district_partition = get_partition_pointer(RESOURCE_PARTITION_DISTRICT);
    assert(district_partition != nullptr);

    auto *district_streamer = district_partition->get_streamer();
    assert(district_streamer != nullptr);

    district_streamer->set_active(a1);
}

resource_partition *get_partition_pointer(resource_partition_enum which_type)
{
    assert(partitions != nullptr);
    assert(which_type >= 0 && which_type < static_cast<int>(partitions->size()));

    return partitions->at(which_type);
}

nflFileID open_pack(const char *name) {
    TRACE("resource_manager::open_pack", name);
    const char *ext = packfile_ext()[g_platform];

    //sp_log("open pack %s%s", name, ext);
    if constexpr (1)
    {
        mString v9{ext};
        mString v8{name};

        mString v11{"data\\"};

        const char *dir = packfile_dir()[g_platform];

        mString a1 = v11 + dir;

        filespec fileSpec {a1, v8, v9};

        mString v12 = fileSpec.fullname();

        auto handle = nflOpenFile(1, v12.c_str());

        if (handle == NFL_FILE_ID_INVALID) {
            mString v13 = fileSpec.fullname();

            handle = nflOpenFile(2, v13.c_str());

            if (handle == NFL_FILE_ID_INVALID) {
                sp_log("Could not open packfile %s", name);
            }
        }

        return handle;
    } else {
        return CDECL_CALL(0x0050DD70, name);
    }
}

resource_pack_slot *get_resource_context()
{
    resource_pack_slot *result = nullptr;

    if (!resource_context_stack.empty()) {
        result = resource_context_stack.back();
    }

    return result;
}

bool get_resource_if_exists(const resource_key &resource_id,
                            [[maybe_unused]] void *a2,
                            uint8_t **a3,
                            worldly_pack_slot *slot_ptr,
                            int *mash_data_size)
{
    TRACE("resource_manager::get_resource_if_exists");

    assert(slot_ptr != nullptr);

    auto v6 = slot_ptr->get_resource(resource_id, mash_data_size, nullptr);
    if (v6 == nullptr) {
        return false;
    }

    *a3 = v6;
    return true;
}

uint8_t *get_resource(const resource_key &resource_id, int *mash_data_size, resource_pack_slot **a3)
{
    TRACE("resource_manager::get_resource", resource_id.get_platform_string(g_platform).c_str());
    
    if constexpr (1)
    {
        assert(!g_is_the_packer() && "Don't call this function while packing!");
        assert(resource_id.is_set());
        assert(get_resource_context() != nullptr && "Can't get a resource without a context!");
        assert(get_resource_context()->is_data_ready() && "Invalid resource context");

        auto *result = get_resource_context()->get_resource(resource_id, mash_data_size, a3);

        //sp_log("resource_manager::get_resource:");

        return result;
    }
    else
    {
        uint8_t * (* func)(const resource_key *, int *, resource_pack_slot **) = CAST(func, 0x00531B30);
        return func(&resource_id, mash_data_size, a3);
    }
}

} // namespace resource_manager

void resource_manager_patch()
{
    SET_JUMP(0x00542740, resource_manager::push_resource_context);

    SET_JUMP(0x00537530, resource_manager::pop_resource_context);

    //REDIRECT(0x00594836, resource_manager::get_resource);

    {
        resource_pack_slot * (* func)(resource_pack_slot *) = &resource_manager::get_best_context;
        REDIRECT(0x00542A04, func);
    }

    //REDIRECT(0x0055A6E1, resource_manager::get_resource_if_exists);

    REDIRECT(0x005D70A6, resource_manager::frame_advance);

    SET_JUMP(0x0052A820, resource_manager::get_pack_file_stats);

    SET_JUMP(0x00537650, resource_manager::load_amalgapak);

    SET_JUMP(0x0055BA30, resource_manager::create_inst);

    SET_JUMP(0x00547AD0, resource_manager::delete_inst);

    SET_JUMP(0x0054C2E0, resource_manager::reload_amalgapak);

    SET_JUMP(0x0053DE90, resource_manager::can_reload_amalgapak);

    SET_JUMP(0x0051ED70, resource_manager::get_pack_location);

    {
        REDIRECT(0x0055A371, resource_manager::configure_packs_by_memory_map);
    }
}
