/* ========================================================================
   $File: image.h
   $Date: 2023-09-20
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


#include "image_internal.h"


struct PNG_Image_Info {
    U32 width;
    U32 height;
    U8 bit_depth;
    U8 color_type;
    U8 compression_method;
    U8 filter_method;
    U8 interlace_method;
    Padding8 paddd[3];
};

struct PNG_FILE
{
    StreamingBufferLL idats;
    U32 width;
    U32 height;
    U8 bit_depth;
    U8 color_type;
    U8 compression_method;
    U8 filter_method;
    U8 interlace_method;
    U8 padding1[3];
};

/*
struct Image {
    void* pixels;
    U32 width, height;
    U8 bit_depth;
};
*/


PNG_FILE parse_png_file(StreamingBuffer entire_file, LinearAllocator* allocator);
PNG_Image_Info get_png_image_info(StreamingBuffer entire_file);

void load_png_image(PNG_FILE* image_info, void* image_data);
