/* ========================================================================
   $File: image_internal.h
   $Date: 2023-10-08
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


#pragma pack(push, 1)
struct PNG_File_Header
{
    U8 highBit;
    U8 signature[3];
    U8 dosEOL[2];
    U8 dosEOF;
    U8 unixEOL;
};

struct PNG_Chunk_Header
{
    U32 length; // length of [data]
    union {
        U8  signature[4];
        U32 numeric;
    };
};

struct PNG_Chunk_Footer
{
    S32 crc;
};

struct PNG_IHDR_Chunk
{
    /*
     * Width: 4 bytes
     * Height: 4 bytes
     * Bit depth: 1 byte
     * Color type: 1 byte
     * Compression method: 1 byte
     * Filter method: 1 byte
     * Interlace method: 1 byte
    */

    U32 width;
    U32 height;
    U8 bit_depth;
    U8 color_type;
    U8 compression_method;
    U8 filter_method;
    U8 interlace_method;
};

struct PNG_PHYS_Chunk
{
    U32 ppu_x;
    U32 ppu_y;
    U8  unit;
};

struct PNG_CHRM_Chunk
{
    U32 white_point_x;
    U32 white_point_y;
    U32 red_x;
    U32 red_y;
    U32 green_x;
    U32 green_y;
    U32 blue_x;
    U32 blue_y;
};
 
struct ZLIB_Header 
{
    union {
        U8 compression_method;
        struct  {
            //U8 CM     = (U8)((compression_method & 0x0F) >> 0); // First 4 bits 0 - 3
            //U8 CINFO  = (U8)((compression_method & 0xF0) >> 4); // Last  4 bits 4 - 7
            U8 CM : 4;
            U8 CINFO : 4;
        }_cm;
    };
    union {
        U8 flags;
        struct {
            //U8 FCHECK = (U8)((flags              & 0x1F) >> 0); // First 5 bits 0 - 4
            //U8 FDICT  = (U8)((flags              & 0x20) >> 5); // Bit 5
            //U8 FLEVEL = (U8)((flags              & 0xC0) >> 6); // Last 2 bits  6 - 7 
            U8 FCHECK : 5;
            U8 FDICT  : 1;
            U8 FLEVEL : 2;
        }_flags;
    };
};

typedef ZLIB_Header PNG_IDAT_Chunk_Header;

struct PNG_IDAT_Chunk_Footer
{
    U32 check_value;
};

struct PNG_TIME_Chunk
{
    U16 year;
    U8  month;
    U8  day;
    U8  hour;
    U8  minute;
    U8  second;
};

struct PNG_Important_Chunks
{
    PNG_IHDR_Chunk* ihdr;
    //PNG_IDAT_Streaming_Chunk * stream;
    PNG_IHDR_Chunk* as;
};


#pragma pack(pop)


enum PNG_COLOR_TYPE
{
    PNG_COLOR_GRAY_SCALE       = 0,
    PNG_COLOR_RBG              = 2,
    PNG_COLOR_PLTE             = 3,
    PNG_COLOR_GRAY_SCALE_ALPHA = 4,
    PNG_COLOR_RGBA             = 6,
};













