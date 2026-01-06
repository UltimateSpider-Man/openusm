#include "script_file_loader.h"

#include "common.h"
#include "func_wrapper.h"
#include "trace.h"
#include "utility.h"

#include <windows.h>
#include <cstring>

script_file_loader *g_script_file_loader = nullptr;

script_file_loader::script_file_loader()
    : scripts_directory("scripts\\")
{}

script_file_loader::script_file_loader(const mString &directory)
    : scripts_directory(directory)
{
    if (scripts_directory.size() > 0) {
        char last = scripts_directory.c_str()[scripts_directory.size() - 1];
        if (last != '\\' && last != '/') {
            scripts_directory = scripts_directory + mString("\\");
        }
    }
}

script_file_loader::~script_file_loader() {
    release_all();
}

int script_file_loader::scan_and_load_all() {
    int total_loaded = 0;

    const char *extensions[] = { "*.pcsx", "*.pcsst", "*.pcpst" };

    for (int i = 0; i < 3; ++i) {
        mString pattern = scripts_directory + mString(extensions[i]);
        std::vector<mString> files = enumerate_files(pattern);

        for (const auto &file : files) {
            mString full_path = scripts_directory + file;
            script_file_data *loaded = load_file(full_path);
            if (loaded != nullptr) {
                ++total_loaded;
                printf("Loaded script file: %s (%u bytes)\n", 
                    file.c_str(), loaded->size);
            }
        }
    }

    printf("Total script files loaded: %d\n", total_loaded);
    return total_loaded;
}

script_file_data *script_file_loader::load_file(const mString &filepath) {
    if (!is_valid_script_extension(filepath)) {
        printf("Invalid script extension: %s\n", filepath.c_str());
        return nullptr;
    }

    os_file file(filepath, os_file::FILE_READ);
    if (!file.is_open()) {
        printf("Failed to open script file: %s\n", filepath.c_str());
        return nullptr;
    }

    int file_size = file.get_size();
    if (file_size <= 0) {
        printf("Script file is empty or invalid: %s\n", filepath.c_str());
        file.close();
        return nullptr;
    }

    uint8_t *buffer = new uint8_t[file_size];
    int bytes_read = file.read(buffer, file_size);
    file.close();

    if (bytes_read != file_size) {
        printf("Failed to read complete file: %s (read %d of %d bytes)\n",
            filepath.c_str(), bytes_read, file_size);
        delete[] buffer;
        return nullptr;
    }

    script_file_data *data = new script_file_data();
    data->path = filepath;
    data->data = buffer;
    data->size = static_cast<uint32_t>(file_size);
    data->type = get_type_from_extension(filepath);

    // Extract filename from path
    const char *path_str = filepath.c_str();
    const char *last_slash = strrchr(path_str, '\\');
    if (last_slash == nullptr) {
        last_slash = strrchr(path_str, '/');
    }
    if (last_slash != nullptr) {
        data->filename = mString(last_slash + 1);
    } else {
        data->filename = filepath;
    }

    loaded_files.push_back(data);
    return data;
}

std::vector<script_file_data *> script_file_loader::get_files_by_type(script_file_type type) const {
    std::vector<script_file_data *> result;
    for (auto *file : loaded_files) {
        if (file->type == type) {
            result.push_back(file);
        }
    }
    return result;
}

script_file_data *script_file_loader::find_by_name(const mString &name) const {
    for (auto *file : loaded_files) {
        // Compare without extension
        const char *filename = file->filename.c_str();
        const char *dot = strrchr(filename, '.');
        
        size_t name_len = (dot != nullptr) 
            ? static_cast<size_t>(dot - filename) 
            : strlen(filename);

        if (strncmp(filename, name.c_str(), name_len) == 0 
            && strlen(name.c_str()) == name_len) {
            return file;
        }
    }
    return nullptr;
}

void script_file_loader::release_all() {
    for (auto *file : loaded_files) {
        if (file != nullptr) {
            delete file;
        }
    }
    loaded_files.clear();
}

script_file_type script_file_loader::get_type_from_extension(const mString &filepath) {
    const char *path = filepath.c_str();
    const char *ext = strrchr(path, '.');
    
    if (ext != nullptr) {
        if (_stricmp(ext, ".pcsx") == 0) {
            return script_file_type::PCSX;
        } else if (_stricmp(ext, ".pcsst") == 0) {
            return script_file_type::PCSST;
        } else if (_stricmp(ext, ".pcpst") == 0) {
            return script_file_type::PCPST;
        }
    }
    
    return script_file_type::PCSX;
}

bool script_file_loader::is_valid_script_extension(const mString &filepath) {
    const char *path = filepath.c_str();
    const char *ext = strrchr(path, '.');
    
    if (ext == nullptr) {
        return false;
    }

    return (_stricmp(ext, ".pcsx") == 0) 
        || (_stricmp(ext, ".pcsst") == 0) 
        || (_stricmp(ext, ".pcpst") == 0);
}

std::vector<mString> script_file_loader::enumerate_files(const mString &pattern) const {
    std::vector<mString> result;

    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA(pattern.c_str(), &find_data);

    if (find_handle == INVALID_HANDLE_VALUE) {
        return result;
    }

    do {
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            result.push_back(mString(find_data.cFileName));
        }
    } while (FindNextFileA(find_handle, &find_data) != 0);

    FindClose(find_handle);
    return result;
}

void script_file_loader_patch() {
    g_script_file_loader = new script_file_loader();
}
