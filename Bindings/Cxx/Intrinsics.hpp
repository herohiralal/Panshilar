#ifndef __cplusplus
    #error "Please use the C bindings.";
#endif

#ifndef PNSLR_INTRINSICS_CXX_MAIN_H
#define PNSLR_INTRINSICS_CXX_MAIN_H

typedef bool                b8;
typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef signed char         i8;
typedef signed short int    i16;
typedef signed int          i32;
typedef signed long long    i64;
typedef float               f32;
typedef double              f64;
typedef char*               cstring;
typedef void*               rawptr;
template <typename T> struct ArraySlice { T* data; i64 count; };

struct utf8str : ArraySlice<u8>
{
    utf8str() = default;
    utf8str(const ArraySlice<u8>& other) : ArraySlice<u8>(other) { }

    template <u64 N>
    utf8str(const u8 (&arr)[N]) { data = (u8*) arr; count = (i64)(N - 1); }

    #ifdef PNSLR_CXX_MAIN_H
        b8 operator==(const utf8str& other) const;
        b8 operator!=(const utf8str& other) const;
    #endif
};


namespace Intrinsics
{

    // #######################################################################################
    // Collections
    // #######################################################################################

    /**
     * A raw type-unspecific array slice.
     */
    struct RawArraySlice
    {
       rawptr data;
       i64 count;
    };

    /**
     * UTF-8 string type, with length info (not necessarily null-terminated).
     */
    // typedef ArraySlice<u8> utf8str;

} // namespace end

#endif//PNSLR_INTRINSICS_CXX_MAIN_H

#ifdef PNSLR_CXX_IMPL

#if (_MSC_VER)
    #define PNSLR_STRUCT_OFFSET(type, member) ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
#elif (__clang__) || (__GNUC__)
    #define PNSLR_STRUCT_OFFSET(type, member) ((u64) offsetof(type, member))
#else
    #error "UNSUPPORTED COMPILER!";
#endif

b8      PNSLR_Bindings_Convert(b8      x) { return x; }
u8      PNSLR_Bindings_Convert(u8      x) { return x; }
u16     PNSLR_Bindings_Convert(u16     x) { return x; }
u32     PNSLR_Bindings_Convert(u32     x) { return x; }
u64     PNSLR_Bindings_Convert(u64     x) { return x; }
i8      PNSLR_Bindings_Convert(i8      x) { return x; }
i16     PNSLR_Bindings_Convert(i16     x) { return x; }
i32     PNSLR_Bindings_Convert(i32     x) { return x; }
i64     PNSLR_Bindings_Convert(i64     x) { return x; }
f32     PNSLR_Bindings_Convert(f32     x) { return x; }
f64     PNSLR_Bindings_Convert(f64     x) { return x; }
rawptr  PNSLR_Bindings_Convert(rawptr  x) { return x; }
cstring PNSLR_Bindings_Convert(cstring x) { return x; }
struct PNSLR_RawArraySlice
{
   rawptr data;
   i64 count;
};
static_assert(sizeof(PNSLR_RawArraySlice) == sizeof(Intrinsics::RawArraySlice), "size mismatch");
static_assert(alignof(PNSLR_RawArraySlice) == alignof(Intrinsics::RawArraySlice), "align mismatch");
PNSLR_RawArraySlice* PNSLR_Bindings_Convert(Intrinsics::RawArraySlice* x) { return reinterpret_cast<PNSLR_RawArraySlice*>(x); }
Intrinsics::RawArraySlice* PNSLR_Bindings_Convert(PNSLR_RawArraySlice* x) { return reinterpret_cast<Intrinsics::RawArraySlice*>(x); }
PNSLR_RawArraySlice& PNSLR_Bindings_Convert(Intrinsics::RawArraySlice& x) { return *PNSLR_Bindings_Convert(&x); }
Intrinsics::RawArraySlice& PNSLR_Bindings_Convert(PNSLR_RawArraySlice& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_RawArraySlice, data) == PNSLR_STRUCT_OFFSET(Intrinsics::RawArraySlice, data), "data offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_RawArraySlice, count) == PNSLR_STRUCT_OFFSET(Intrinsics::RawArraySlice, count), "count offset mismatch");

typedef struct { b8* data; i64 count; } PNSLR_ArraySlice_b8;
static_assert(sizeof(PNSLR_ArraySlice_b8) == sizeof(ArraySlice<b8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_b8) == alignof(ArraySlice<b8>), "align mismatch");
PNSLR_ArraySlice_b8* PNSLR_Bindings_Convert(ArraySlice<b8>* x) { return reinterpret_cast<PNSLR_ArraySlice_b8*>(x); }
ArraySlice<b8>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_b8* x) { return reinterpret_cast<ArraySlice<b8>*>(x); }
PNSLR_ArraySlice_b8& PNSLR_Bindings_Convert(ArraySlice<b8>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<b8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_b8& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_b8, count) == PNSLR_STRUCT_OFFSET(ArraySlice<b8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_b8, data) == PNSLR_STRUCT_OFFSET(ArraySlice<b8>, data), "data offset mismatch");

typedef struct { u8* data; i64 count; } PNSLR_ArraySlice_u8;
static_assert(sizeof(PNSLR_ArraySlice_u8) == sizeof(ArraySlice<u8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_u8) == alignof(ArraySlice<u8>), "align mismatch");
PNSLR_ArraySlice_u8* PNSLR_Bindings_Convert(ArraySlice<u8>* x) { return reinterpret_cast<PNSLR_ArraySlice_u8*>(x); }
ArraySlice<u8>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_u8* x) { return reinterpret_cast<ArraySlice<u8>*>(x); }
PNSLR_ArraySlice_u8& PNSLR_Bindings_Convert(ArraySlice<u8>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<u8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_u8& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u8, count) == PNSLR_STRUCT_OFFSET(ArraySlice<u8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u8, data) == PNSLR_STRUCT_OFFSET(ArraySlice<u8>, data), "data offset mismatch");

typedef struct { u16* data; i64 count; } PNSLR_ArraySlice_u16;
static_assert(sizeof(PNSLR_ArraySlice_u16) == sizeof(ArraySlice<u16>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_u16) == alignof(ArraySlice<u16>), "align mismatch");
PNSLR_ArraySlice_u16* PNSLR_Bindings_Convert(ArraySlice<u16>* x) { return reinterpret_cast<PNSLR_ArraySlice_u16*>(x); }
ArraySlice<u16>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_u16* x) { return reinterpret_cast<ArraySlice<u16>*>(x); }
PNSLR_ArraySlice_u16& PNSLR_Bindings_Convert(ArraySlice<u16>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<u16>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_u16& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u16, count) == PNSLR_STRUCT_OFFSET(ArraySlice<u16>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u16, data) == PNSLR_STRUCT_OFFSET(ArraySlice<u16>, data), "data offset mismatch");

typedef struct { u32* data; i64 count; } PNSLR_ArraySlice_u32;
static_assert(sizeof(PNSLR_ArraySlice_u32) == sizeof(ArraySlice<u32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_u32) == alignof(ArraySlice<u32>), "align mismatch");
PNSLR_ArraySlice_u32* PNSLR_Bindings_Convert(ArraySlice<u32>* x) { return reinterpret_cast<PNSLR_ArraySlice_u32*>(x); }
ArraySlice<u32>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_u32* x) { return reinterpret_cast<ArraySlice<u32>*>(x); }
PNSLR_ArraySlice_u32& PNSLR_Bindings_Convert(ArraySlice<u32>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<u32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_u32& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u32, count) == PNSLR_STRUCT_OFFSET(ArraySlice<u32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u32, data) == PNSLR_STRUCT_OFFSET(ArraySlice<u32>, data), "data offset mismatch");

typedef struct { u64* data; i64 count; } PNSLR_ArraySlice_u64;
static_assert(sizeof(PNSLR_ArraySlice_u64) == sizeof(ArraySlice<u64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_u64) == alignof(ArraySlice<u64>), "align mismatch");
PNSLR_ArraySlice_u64* PNSLR_Bindings_Convert(ArraySlice<u64>* x) { return reinterpret_cast<PNSLR_ArraySlice_u64*>(x); }
ArraySlice<u64>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_u64* x) { return reinterpret_cast<ArraySlice<u64>*>(x); }
PNSLR_ArraySlice_u64& PNSLR_Bindings_Convert(ArraySlice<u64>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<u64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_u64& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u64, count) == PNSLR_STRUCT_OFFSET(ArraySlice<u64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_u64, data) == PNSLR_STRUCT_OFFSET(ArraySlice<u64>, data), "data offset mismatch");

typedef struct { i8* data; i64 count; } PNSLR_ArraySlice_i8;
static_assert(sizeof(PNSLR_ArraySlice_i8) == sizeof(ArraySlice<i8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_i8) == alignof(ArraySlice<i8>), "align mismatch");
PNSLR_ArraySlice_i8* PNSLR_Bindings_Convert(ArraySlice<i8>* x) { return reinterpret_cast<PNSLR_ArraySlice_i8*>(x); }
ArraySlice<i8>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_i8* x) { return reinterpret_cast<ArraySlice<i8>*>(x); }
PNSLR_ArraySlice_i8& PNSLR_Bindings_Convert(ArraySlice<i8>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<i8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_i8& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i8, count) == PNSLR_STRUCT_OFFSET(ArraySlice<i8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i8, data) == PNSLR_STRUCT_OFFSET(ArraySlice<i8>, data), "data offset mismatch");

typedef struct { i16* data; i64 count; } PNSLR_ArraySlice_i16;
static_assert(sizeof(PNSLR_ArraySlice_i16) == sizeof(ArraySlice<i16>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_i16) == alignof(ArraySlice<i16>), "align mismatch");
PNSLR_ArraySlice_i16* PNSLR_Bindings_Convert(ArraySlice<i16>* x) { return reinterpret_cast<PNSLR_ArraySlice_i16*>(x); }
ArraySlice<i16>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_i16* x) { return reinterpret_cast<ArraySlice<i16>*>(x); }
PNSLR_ArraySlice_i16& PNSLR_Bindings_Convert(ArraySlice<i16>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<i16>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_i16& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i16, count) == PNSLR_STRUCT_OFFSET(ArraySlice<i16>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i16, data) == PNSLR_STRUCT_OFFSET(ArraySlice<i16>, data), "data offset mismatch");

typedef struct { i32* data; i64 count; } PNSLR_ArraySlice_i32;
static_assert(sizeof(PNSLR_ArraySlice_i32) == sizeof(ArraySlice<i32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_i32) == alignof(ArraySlice<i32>), "align mismatch");
PNSLR_ArraySlice_i32* PNSLR_Bindings_Convert(ArraySlice<i32>* x) { return reinterpret_cast<PNSLR_ArraySlice_i32*>(x); }
ArraySlice<i32>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_i32* x) { return reinterpret_cast<ArraySlice<i32>*>(x); }
PNSLR_ArraySlice_i32& PNSLR_Bindings_Convert(ArraySlice<i32>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<i32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_i32& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i32, count) == PNSLR_STRUCT_OFFSET(ArraySlice<i32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i32, data) == PNSLR_STRUCT_OFFSET(ArraySlice<i32>, data), "data offset mismatch");

typedef struct { i64* data; i64 count; } PNSLR_ArraySlice_i64;
static_assert(sizeof(PNSLR_ArraySlice_i64) == sizeof(ArraySlice<i64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_i64) == alignof(ArraySlice<i64>), "align mismatch");
PNSLR_ArraySlice_i64* PNSLR_Bindings_Convert(ArraySlice<i64>* x) { return reinterpret_cast<PNSLR_ArraySlice_i64*>(x); }
ArraySlice<i64>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_i64* x) { return reinterpret_cast<ArraySlice<i64>*>(x); }
PNSLR_ArraySlice_i64& PNSLR_Bindings_Convert(ArraySlice<i64>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<i64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_i64& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i64, count) == PNSLR_STRUCT_OFFSET(ArraySlice<i64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_i64, data) == PNSLR_STRUCT_OFFSET(ArraySlice<i64>, data), "data offset mismatch");

typedef struct { f32* data; i64 count; } PNSLR_ArraySlice_f32;
static_assert(sizeof(PNSLR_ArraySlice_f32) == sizeof(ArraySlice<f32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_f32) == alignof(ArraySlice<f32>), "align mismatch");
PNSLR_ArraySlice_f32* PNSLR_Bindings_Convert(ArraySlice<f32>* x) { return reinterpret_cast<PNSLR_ArraySlice_f32*>(x); }
ArraySlice<f32>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_f32* x) { return reinterpret_cast<ArraySlice<f32>*>(x); }
PNSLR_ArraySlice_f32& PNSLR_Bindings_Convert(ArraySlice<f32>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<f32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_f32& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_f32, count) == PNSLR_STRUCT_OFFSET(ArraySlice<f32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_f32, data) == PNSLR_STRUCT_OFFSET(ArraySlice<f32>, data), "data offset mismatch");

typedef struct { f64* data; i64 count; } PNSLR_ArraySlice_f64;
static_assert(sizeof(PNSLR_ArraySlice_f64) == sizeof(ArraySlice<f64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_f64) == alignof(ArraySlice<f64>), "align mismatch");
PNSLR_ArraySlice_f64* PNSLR_Bindings_Convert(ArraySlice<f64>* x) { return reinterpret_cast<PNSLR_ArraySlice_f64*>(x); }
ArraySlice<f64>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_f64* x) { return reinterpret_cast<ArraySlice<f64>*>(x); }
PNSLR_ArraySlice_f64& PNSLR_Bindings_Convert(ArraySlice<f64>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<f64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_f64& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_f64, count) == PNSLR_STRUCT_OFFSET(ArraySlice<f64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_f64, data) == PNSLR_STRUCT_OFFSET(ArraySlice<f64>, data), "data offset mismatch");

typedef struct { char* data; i64 count; } PNSLR_ArraySlice_char;
static_assert(sizeof(PNSLR_ArraySlice_char) == sizeof(ArraySlice<char>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_char) == alignof(ArraySlice<char>), "align mismatch");
PNSLR_ArraySlice_char* PNSLR_Bindings_Convert(ArraySlice<char>* x) { return reinterpret_cast<PNSLR_ArraySlice_char*>(x); }
ArraySlice<char>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_char* x) { return reinterpret_cast<ArraySlice<char>*>(x); }
PNSLR_ArraySlice_char& PNSLR_Bindings_Convert(ArraySlice<char>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<char>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_char& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_char, count) == PNSLR_STRUCT_OFFSET(ArraySlice<char>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_char, data) == PNSLR_STRUCT_OFFSET(ArraySlice<char>, data), "data offset mismatch");

typedef PNSLR_ArraySlice_u8 PNSLR_UTF8STR;

typedef struct { PNSLR_UTF8STR* data; i64 count; } PNSLR_ArraySlice_PNSLR_UTF8STR;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_UTF8STR) == sizeof(ArraySlice<utf8str>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_UTF8STR) == alignof(ArraySlice<utf8str>), "align mismatch");
PNSLR_ArraySlice_PNSLR_UTF8STR* PNSLR_Bindings_Convert(ArraySlice<utf8str>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_UTF8STR*>(x); }
ArraySlice<utf8str>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_UTF8STR* x) { return reinterpret_cast<ArraySlice<utf8str>*>(x); }
PNSLR_ArraySlice_PNSLR_UTF8STR& PNSLR_Bindings_Convert(ArraySlice<utf8str>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<utf8str>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_UTF8STR& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_UTF8STR, count) == PNSLR_STRUCT_OFFSET(ArraySlice<utf8str>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_UTF8STR, data) == PNSLR_STRUCT_OFFSET(ArraySlice<utf8str>, data), "data offset mismatch");

#ifndef PNSLR_CXX_SKIP_PRIMITIVE_SIZE_TESTS
#define PNSLR_CXX_SKIP_PRIMITIVE_SIZE_TESTS
    static_assert(sizeof(b8 ) == 1, "Size mismatch.");
    static_assert(sizeof(u8 ) == 1, "Size mismatch.");
    static_assert(sizeof(u16) == 2, "Size mismatch.");
    static_assert(sizeof(u32) == 4, "Size mismatch.");
    static_assert(sizeof(u64) == 8, "Size mismatch.");
    static_assert(sizeof(i8 ) == 1, "Size mismatch.");
    static_assert(sizeof(i16) == 2, "Size mismatch.");
    static_assert(sizeof(i32) == 4, "Size mismatch.");
    static_assert(sizeof(i64) == 8, "Size mismatch.");
    static_assert(sizeof(f32) == 4, "Size mismatch.");
    static_assert(sizeof(f64) == 8, "Size mismatch.");
#endif//PNSLR_CXX_SKIP_PRIMITIVE_SIZE_TESTS
#undef PNSLR_STRUCT_OFFSET

#endif//PNSLR_CXX_IMPL

