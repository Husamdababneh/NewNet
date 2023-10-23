/* ========================================================================
   $File: hd_str.cpp
   $Date: 2023-03-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/

#include "base.h"



#ifdef HD_STRING
constexpr //static inline
String operator ""_s(const char* string, U64 length)
{
	// NOTE(Husam): The +1 is to account for the null terminating,
	//               character which c/c++ adds to all the string literals
	return String{length + 1, (char*)string}; 
}

constexpr //static inline
const S8* operator ""_s8(const char* string, U64 length)
{
    (length);
	return (S8*)string; 
}


static inline
String cstr_to_string(char* string, U64 length)
{
	String str = {length, string}; 
	return str;
};

static inline
String cstr_to_string(char* string)
{
	U64 len = 0;
	char* str = string;
	while(*(str++)) len++;
	return cstr_to_string(string, len); 
}

static inline
B64 is_null_terminated(const String str)
{
	if (str.length <= 1) return true;
	return 0 == str.str[str.length - 1];
};


static inline
S8 compare_strings(const String left, const String right)
{
	/** 
-     1 -> right is smaller
-     0 -> equal each other
-    -1 -> left is smaller
-    -2 -> same size but not the same string 
	**/

	auto llength = left.length;
	auto rlength = right.length;

	// If the length does not match we do not need to check anything else
	if (llength != rlength) return left.length > right.length ? -1 : 1;
	
	// If they are looking at the same string and have the same size they are equal
	if (left.str == right.str && left.length == right.length) return 0;

	// They have the same length
	for(U64 it = 0; it < rlength; it++){
		if (left.str[it] != right.str[it]) return -2;
	}

#if defined(DEBUG_HD)	
	printf("2- Compareing [%.*s, %lld] and [%.*s, %lld]\n",
		   SV_PRINT(left) , llength,
		   SV_PRINT(right), rlength
		);
#endif
	return 0;
}

static inline
B8 is_strings_equal(String left, String right)
{
	return compare_strings(left, right) == 0;
}

static inline
String sub_str(const String str, U64 offset)
{
	return String{str.length - offset, (char*)str.str + offset};
}

#endif // HD_STRING
