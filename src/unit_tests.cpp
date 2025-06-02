#include "unit_tests.h"

#include "custom_math.h"
#include "func_wrapper.h"
#include "mstring.h"
#include "quaternion.h"
#include "simple_classes.h"
#include "trace.h"
#include "variable.h"
#include "vector3d.h"

#include <cassert>
#include <cstdio>

void quaternion_unit_test()
{
    quaternion a2 {0.867351, -0.396007, -0.089976, 0.287723};
    quaternion a3 {0.856533, -0.408080, -0.231215, 0.215320};
    float lambda = 0.0f;

    void (__cdecl *func)(quaternion *, const quaternion *, const quaternion *, Float) = CAST(func, 0x00588630);

    quaternion res;
    func(&res, &a2, &a3, lambda);

    quaternion res1 = slerp(a2, a3, lambda);

    assert(res == res1);

    //sp_log("%s", res1.to_string().c_str());

    assert(res1 == quaternion(0.867352, -0.396084, -0.090612, 0.287417));
}

void vector3d_unit_test()
{
    TRACE("vector3d_unit_test");

    {
        vector3d forward {0.985648, -0.0516526, 0.160719};
        vector3d up {0, 0, 0};
        vector3d a1 {0.981168, -0.0855519, 0.173175};
        vector3d a4 {0.158808, 0.867648, -0.471135};
        vector3d a10 {0.123463, 0.869854,-0.47761};

        reorient_vectors(a1,
                        a4,
                        forward,
                        a10,
                        forward, up, 0.5f);

        assert(approx_equals(forward, vector3d(0.983571, -0.0686128, 0.166977), LARGE_EPSILON));
        assert(approx_equals(up, vector3d(0.141156, 0.868903, -0.474428), LARGE_EPSILON));
    }

    {
        vector3d a2 {6.716872, -32.072525, 14.798354};
        vector3d a3 {0.606163, 0.554652, 0.570024};
        auto res = sub_444A60(a2, a3);

        //sp_log("%s", res.to_string().c_str());

        assert(approx_equals(res, vector3d (9.918718, -29.142769, 17.809311), LARGE_EPSILON));

        a2 = {16.458485, -15.374539, 26.640459};
        a3 = {0.563501, 0.673320, 0.478650};

        res = sub_444A60(a2, a3);

        assert(approx_equals(res, vector3d (9.880253, -23.234779, 21.052763), LARGE_EPSILON));
    }
}

void mString_unit_test()
{
    TRACE("mString_unit_test");

    for (int i = 0; i < 10; ++i)
    {
        assert(mString(1).to_int() == 1);

        assert(approx_equals(mString(1.0f).to_float(), 1.0f, EPSILON));

        assert(mString("   blah    ").remove_surrounding_whitespace() == mString("blah"));

        assert(mString("blah").remove_leading("bl") == mString("ah"));

        assert(mString("blah").remove_leading("alb") == mString("h"));

        assert(mString("blah").remove_trailing("a") == mString("blah"));

        assert(mString("blah").remove_trailing("ahl") == mString("b"));

        assert(mString("blah").remove_trailing("ahl") == mString("b"));

        assert(mString("qqq") == mString("qqq").data());

        assert(mString("qqq") == mString("qqq").c_str());

        assert(mString("qwerty").compare("qwerty") == 0);

        assert(mString("").empty());

        assert(mString("qwerty").is_equal(mString("QwERtY").to_lower().c_str()));

        assert(mString("QWERTY").is_equal(mString("qwErTy").to_upper().c_str()));

        assert(mString("012345").substr(0, 6) == mString("012345"));

        assert(mString("012345").substr(1, 2) == mString("12"));

        assert(mString("012345").truncate(2) == mString("01"));

        assert(mString("foobar").slice(2, 4) == "ob");

        assert(mString("foobar").find("oo") == 1);

        assert(mString("foobar").find("oo", 4) == mString::npos);

        assert(mString("foobar").find({4}, 'o') == mString::npos);

        assert(mString("foobar").find(mString::pos_t{0}, 'b') == 3);

        assert(mString("foobar").rfind('b') == 3);

        assert(mString("foobar").rfind('b', 2) == mString::npos);

        assert(mString("foobar").rfind("oo") == 1);

        assert(mString("foobarboobar").rfind("oo") == 7);

        assert(mString("a") + mString("b") == mString("ab"));

        assert("a" + mString("b") == mString("ab"));

        assert(mString("a") + "b" == mString("ab"));

        assert(mString("a") + "b" == "ab");

        assert(mString("a") + "b" != "abb");

        assert(mString("a") + "b" != mString("abb"));

        assert(mString("a") < mString("b"));

        assert(mString("b") > mString("a"));

        mString s1{};
        assert(s1 == "");

        static const char *test_strings[10] = {
            "",
            "1",
            "1234567",
            "12345678",
            "1234567890abcdef1234567890abcde",
            "1234567890abcdef1234567890abcdef",
            "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef123456"
            "7890abcdef1234567890abcdef1234567890abcde",
            "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef123456"
            "7890abcdef1234567890abcdef1234567890abcdef",
            "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef123456"
            "7890abcdef1234567890abcdef1234567890abcde",
            "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef123456"
            "7890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890ab"
            "cdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcde"};

        mString s2{test_strings[i]};
        assert(s2 == test_strings[i]);

        auto s3 = s2;
        assert(s3 == test_strings[i]);

        mString s4{0, "%s%s", test_strings[i], ""};
        assert(s4 == test_strings[i]);

        mString s41{0, "%s%s", "", test_strings[i]};
        assert(s41 == test_strings[i]);

        mString s5{0.1f};
        assert(approx_equals(atof(s5.c_str()), 0.1f, EPSILON));

        mString s6{123123};
        assert(s6 == "123123");

        mString s7{-123123};
        assert(s7 == "-123123");

        mString s8 {};
        s8.copy(s4);

        assert(s8 == test_strings[i]);

        s8.copy("", -1);
        s8 += s4;

        assert(s8 == test_strings[i]);

        if (std::strlen(test_strings[i]) < 512)
        {
            s8.copy(test_strings[i], -1);
            s8.append(test_strings[i], -1);

            char buf[2048];
            std::sprintf(buf, "%s%s", test_strings[i], test_strings[i]);
            assert(s8 == buf);
        }

        if (std::strlen(test_strings[i]) < 1023)
        {
            s8.copy(test_strings[i], -1);
            s8.append('a');

            char buf[2048];
            std::sprintf(buf, "%s%c", test_strings[i], 'a');
            assert(s8 == buf);

            if (s8.length() > 0) {
                assert(s8[s8.length() - 1] == 'a');
            }
        }

        s8.update_guts("a", -1);
        assert(s8 == "a");

        s8 = s2;
        assert(s8 == test_strings[i]);

        s8 = test_strings[i];
        assert(s8 == test_strings[i]);
    }
}

void unit_tests()
{
    TRACE("unit_tests");

    vector3d_unit_test();
    
    quaternion_unit_test();

    mString_unit_test();

    //binary_search_array_unit_test();
}

