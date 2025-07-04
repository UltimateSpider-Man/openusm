#pragma once

#include "mcontainer.h"

#include "mash.h"
#include "nlPlatformEnum.h"
#include "slab_allocator.h"
#include "variable.h"

#include <windef.h>

#include <string>

extern Var<int> mString_count;

//0x00936BD0
extern Var<const char *[4]> packfile_dir;

//0x00936BF0
extern Var<const char *[4]> packfile_ext;

inline constexpr size_t MAX_MSTRING_LENGTH = 65535u;

struct mash_info_struct;

struct mString : mContainer {
    struct fmtd {
        int value;
    };

    char *guts;

public:
    slab_allocator::slab_t *field_C;

    //0x00420F00
    mString();
	


    //0x00420F60
    mString(mString::fmtd fmt, const char *Format, ...);

    //0x00421100
    mString(const char *a2);

    //0x00421090
    explicit mString(float a2);

    //0x00420FD0
    explicit mString(int a2);

    //0x00421220
    mString(const mString &a2);

    //0x00420F60
    mString(int a2, const char *Format, ...);

    //0x0041FE30
    mString & operator=(const char *a2);

    //0x0041FE10
    mString & operator=(const mString &a2);

    void copy(const char *a1, int a2);

    void copy(const mString &a2);
	
		    // Constructor


    //0x004015C0
    ~mString();

    inline void set_size(size_t size) {
        this->m_size = size;
    }
	
   inline void append2(const char* a2) {
		 std::string str;
        str += a2;
    }

    // Method to get the string value
   inline const char* to_string() const {
		 std::string str;	  
        return str.c_str();
    }

  inline  void from_char(const char* a2) {
		 std::string str;
        str = a2;
    }


    inline bool empty() const {
        return m_size == 0;
    }

    inline int size() const {
        return m_size;
    }

    inline int length() const {
        return m_size;
    }

    void initialize(mash::allocation_scope );

    //0x0041F9D0
    void update_guts(const char *from_string, int n);

    int compare(const char *str) const;

    bool is_equal(const char *a2) const;

    //0x0041FDE0
    mString truncate(int a2);

    void append(char a3);

    //0x0041FAB0
    void append(const char *from_string, int from_string_length = -1);

    struct pos_t {
        int value;

        operator int() {
            return value;
        }
    };

    //0x0041FD00
    int find(pos_t a2, char a3) const;

    //0x0041FC70
    int find(const char *str, int a3 = 0) const;

    //0x0043D7D0
    [[nodiscard]] mString slice(int start, int end);

    //0x0041FC00
    mString & operator+=(const char *a2);

    //0x0041FBE0
    mString & operator+=(const mString &a2);

    //0x00421260
    [[nodiscard]] mString substr(int a3, int Count) const;

    const char *c_str() const;

    char *data();

    //0x004209C0
    void finalize(mash::allocation_scope);

    //0x0041FD90
    mString & to_lower();

    mString & to_upper();

    char at(int i) const;

    char operator[](int i) const;

    void unmash(mash_info_struct *a1, void *a2);

    //0x004209F0
    void custom_unmash(mash_info_struct *a1, void *a2);

    //0x0041F970
    void destroy_guts();

    //0x00503980
    bool operator==(const char *a2) const;

    int rfind(const char *str) const;

    //0x0041FD30
    int rfind(char a2, int a3 = -1) const;

    //0x0055DAD0
    [[nodiscard]] static mString from_int(int a2);

    [[nodiscard]] int to_int() const;

    [[nodiscard]] double to_float() const;

    //0x00421B60
    mString & remove_leading(const char *a1);

    //0x00421C50
    mString & remove_trailing(const char *a2);

    mString & remove_surrounding_whitespace();

    //0x0055DAB0
    [[nodiscard]] static mString from_float(float a2);

    //0x00421410
    friend mString operator+(const char *a2, const mString &a3);

    //0x004214E0
    friend mString operator+(const mString &arg4, const char *Source);

    //0x00421330
    friend mString operator+(const mString &arg4, const mString &a3);

    friend bool operator==(const mString &a1, const mString &a2);

    friend bool operator!=(const mString &a1, const mString &a2);

    friend bool operator<(const mString &a1, const mString &a2);
    friend bool operator>(const mString &a1, const mString &a2);

    //0x005BAA80
    [[nodiscard]] static mString get_standalone_filename(const mString &str, _nlPlatformEnum a3);

    static int npos;

    static inline Var<char *> null {0x0091E7C0};
};

//0x0064DF30
extern void *dialog_box_formatting(mString *out_string, mString a2, int a3, int a4);

void sub_79F7D0(int *a1, int a2);

int sub_79F690(int *a1, int a2);

signed int sub_79F760(int *a1);

//signed int sub_79E180(int a1, char *a2, int a3, int *a4);

int sub_79E490(int a1, char *a2);
