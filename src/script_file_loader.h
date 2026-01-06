#pragma once

#include "mstring.h"
#include "os_file.h"

#include <vector>
#include <cstdint>

enum class script_file_type {
    PCSX,   // .pcsx - compiled script executable
    PCSST,  // .pcsst - script string table
    PCPST   // .pcpst - permanent string table
};

struct script_file_data {
    mString path;
    mString filename;
    script_file_type type;
    uint8_t *data;
    uint32_t size;

    script_file_data()
        : data(nullptr)
        , size(0)
        , type(script_file_type::PCSX)
    {}

    ~script_file_data() {
        release();
    }

    void release() {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
            size = 0;
        }
    }
};

struct script_file_loader {
    std::vector<script_file_data *> loaded_files;
    mString scripts_directory;

    script_file_loader();

    explicit script_file_loader(const mString &directory);

    ~script_file_loader();

    //@ Scans directory and loads all script files (.pcsx, .pcsst, .pcpst)
    //@ Returns number of files loaded
    int scan_and_load_all();

    //@ Loads a single script file by path
    //@ Returns nullptr on failure
    script_file_data *load_file(const mString &filepath);

    //@ Gets all loaded files of a specific type
    std::vector<script_file_data *> get_files_by_type(script_file_type type) const;

    //@ Finds a loaded file by filename (without extension)
    script_file_data *find_by_name(const mString &name) const;

    //@ Releases all loaded file data
    void release_all();

    //@ Returns the file type based on extension
    static script_file_type get_type_from_extension(const mString &filepath);

    //@ Checks if a file has a valid script extension
    static bool is_valid_script_extension(const mString &filepath);

private:
    //@ Internal: enumerates files in directory matching pattern
    std::vector<mString> enumerate_files(const mString &pattern) const;
};

extern script_file_loader *g_script_file_loader;

extern void script_file_loader_patch();
