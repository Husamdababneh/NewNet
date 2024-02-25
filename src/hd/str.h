/* ========================================================================
   $File: hd_str.cpp
   $Date: 2023-03-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/



//~ Strings and String Views 
struct String {
	union {U64   length;  U64  size; };
	union {char* str_char; S8*  str; };
};

#define SV_SPECIFIER "%.*s"
#define SV_PRINT(x)  (int)x.length, x.str

typedef   String StringView; // do i need this ?
constexpr String    operator ""_s (const char* string, U64 length);
constexpr const S8* operator ""_s8(const char* string, U64 length);
internal inline    String cstr_to_string(char* string, U64 length);
internal inline    String cstr_to_string(char* string);
internal inline    B64    is_null_terminated(const String str);
internal inline    S8     compare_strings(const String left, const String right);
internal inline    B8     is_strings_equal(const String left, const String right);
internal inline    String sub_str(const String str, U64 offset);

