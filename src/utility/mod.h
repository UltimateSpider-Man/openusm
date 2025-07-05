#pragma once

#include <cstdint>

#include <map>
#include <filesystem>

struct Mod {
    std::filesystem::path Path;
    int Type;
    std::vector<uint8_t> Data;
};
extern std::map<uint32_t, Mod> Mods;

[[maybe_unused]] static bool hasMod(uint32_t hash) {
    return Mods.find(hash) != Mods.end();
}

[[maybe_unused]] static Mod* getMod(uint32_t hash) {
    auto it = Mods.find(hash);
    if (it != Mods.end())
        return &it->second;
    return nullptr;
}
[[maybe_unused]] static uint8_t* getModDataByHash(uint32_t hash) {
    if (hasMod(hash))
        if (auto mod = getMod(hash))
            return &mod->Data.data()[0];
    return nullptr;
}

[[maybe_unused]] static std::string transformToLower(const std::string& name)
{
    std::string res = name;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

// this is O(n) (don't use this unless necessary!)
[[maybe_unused]] static Mod* getModByFilemame(const std::string& name) {
    std::string search = transformToLower(name);
    for (auto& [hash, mod] : Mods) {
        std::string filename = transformToLower(mod.Path.filename().string());
        if (filename == search)
            return &mod;
    }
    return nullptr;
}

