/* ========================================================================
   $File: image.cpp
   $Date: 2023-09-20
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/


#include "image.h"


internal inline void _print_chunk_header(PNG_Chunk_Header* chunk_header);
internal inline void _print_ihdr(PNG_IHDR_Chunk* ihdr);
internal inline void _print_text(String keyword, String text);
internal inline void _print_time(PNG_TIME_Chunk* time);
internal inline void _print_phys(PNG_PHYS_Chunk* phys);
internal inline void _print_chrm(PNG_CHRM_Chunk* chrm);

#define _PNG_VERBOS
#ifdef  _PNG_VERBOS
#define print_chunk_header(x) _print_chunk_header(x);
#define print_ihdr(x)         _print_ihdr(x)
#define print_time(x)         _print_time(x)
#define print_phys(x)         _print_phys(x)
#define print_chrm(x)         _print_chrm(x)
#define print_text(x, y)      _print_text(x, y);
#else
#define print_chunk_header(x) 
#define print_ihdr(x)         
#define print_time(x)         
#define print_phys(x)         
#define print_chrm(x)
#define print_text(x, y)
#endif


internal inline void
parse_ihdr_data(PNG_IHDR_Chunk* ihdr, PNG_FILE* png_file);

// I dont want this API to allocate or mange any memory
// This API shall open the Image file and then report back with a struct
// telling the user any and all information that it can regarding this Image
// I will start with PNG for now
// See image.h for PNG_FILE struct


// @CleanUp(husamd):
#pragma warning (disable : 4189)



Buffer deflate(StreamingBuffer sb);


/*
 * This Procedure shall not allocate memory, hence we require buffer containing the data for now
 * 
 * 
 */
PNG_FILE parse_png_file(StreamingBuffer entire_file, LinearAllocator* allocator)
{
    (allocator);
    PNG_FILE result = {};
    StreamingBuffer cursor = entire_file;
    
    PNG_File_Header* header = ConsumeType(&cursor, PNG_File_Header);
    
    constexpr U32 png_magic_size = 8;
    U8 png_magic[png_magic_size] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    if (memcmp(header, png_magic, png_magic_size) != 0)
    {
        print_string("This is not a png file\n"_s);
        ExitProcess(1);
    }
    StreamingBufferLL& idatBuffers = result.idats;
    while(cursor.size > 0)
    {
        PNG_Chunk_Header* chunkHeader = ConsumeType(&cursor, PNG_Chunk_Header);
        if (!chunkHeader)
        {
            print_string("Something went wrong\n"_s);
            break;
        }
        bswap_ip(&chunkHeader->length);
        bswap_ip(&chunkHeader->numeric);
        
        print_chunk_header(chunkHeader);

        switch(chunkHeader->numeric)
        {
          case 'IHDR':
          {
              PNG_IHDR_Chunk* ihdr = ConsumeType(&cursor, PNG_IHDR_Chunk);
              parse_ihdr_data(ihdr, &result);
          }break;
          case 'IDAT':
          {
              StreamingBuffer* node =  AllocateType(allocator, StreamingBuffer);
              node->content = cursor.content;
              node->size = chunkHeader->length;
              node->next = NULL;
                
              idatBuffers.last =
                  ((idatBuffers.last ? idatBuffers.last->next : idatBuffers.first) = node);
              
              ConsumeSize(&cursor, chunkHeader->length);
          }break;
          case 'tIME':
          {
              PNG_TIME_Chunk* time = ConsumeType(&cursor, PNG_TIME_Chunk);
              bswap_ip(&time->year);
              print_time(time);
          }break;
          case 'pHYs':
          {
              PNG_PHYS_Chunk* phys = ConsumeType(&cursor, PNG_PHYS_Chunk);
              bswap_ip(&phys->ppu_x);
              bswap_ip(&phys->ppu_y);
              print_phys(phys);
          }break;
          case 'cHRM':
          {
              PNG_CHRM_Chunk* chrm = ConsumeType(&cursor, PNG_CHRM_Chunk);
              (chrm);
              print_chrm(chrm);
          }break;
          case 'tEXt':
          {
              String keyword = ConsumeCString(&cursor);
              U64 textLen = chunkHeader->length - (keyword.length);
              String text = ConsumeString(&cursor, textLen);
              print_text(keyword, text);
          }break;
          case 'iTXt':
          {
              String keyword = ConsumeCString(&cursor);
              U8 compression_flag = *(U8*)ConsumeSize(&cursor, 1);
              U8 compression_method = *(U8*)ConsumeSize(&cursor, 1);
              String language_tag = ConsumeCString(&cursor);
              String translated_keyword = ConsumeCString(&cursor);
              
              U64 textLen = chunkHeader->length -
                  (keyword.length + language_tag.length + translated_keyword.length + 2);
              
              StreamingBuffer sb;
              sb.content = (U8*)ConsumeSize(&cursor, textLen);//ConsumeString(&cursor, textLen);
              sb.size    = textLen;

              //Buffer decoded = deflate(sb);
              //String str = {decoded.size, (char*)decoded.content};
              print_text(keyword, "Some data"_s);
          }break;
          case 'iCCP':
          case 'zTXt':
          {
              String keyword = ConsumeCString(&cursor);
              U8 compression_method = *(U8*)ConsumeSize(&cursor, 1);
              U64 textLen = chunkHeader->length - (keyword.length + 1);
              print_text(keyword, "Decode me"_s);

              StreamingBuffer sb = {};
              sb.content = (U8*)ConsumeSize(&cursor, textLen);//ConsumeString(&cursor, textLen);
              sb.size    = textLen;
              Buffer data = deflate(sb);
          }break;
          case 'IEND':
          {
              //print_string("IEND: Reached End of file\n"_s);
          }break;
          default:
          {
              print_string("    Chunk type not supported yet\n"_s);
              ConsumeSize(&cursor, chunkHeader->length);
          }break;
        };

        ConsumeType(&cursor, PNG_Chunk_Footer);
    }

   
    return result;
}

void load_png_image(PNG_FILE* image_info, void* image_data)
{
    Assert(image_data);

    StreamingBuffer* cursor = image_info->idats.first;


    U64 actualDataSize = cursor->size - (sizeof(PNG_IDAT_Chunk_Header) + sizeof(PNG_IDAT_Chunk_Footer));
    (actualDataSize);
    
    // This is only needed for one time, hmmmm
    PNG_IDAT_Chunk_Header* header = ConsumeType(cursor, PNG_IDAT_Chunk_Header);
    
    if (header->_cm.CM != 8 || header->_flags.FDICT != 0)
    {
        print_string("something is wrong with the commpression\n"_s);
        ExitProcess(1);
    }
    
    U32 BFINAL = ConsumeBits(cursor, 1);// this indecates that this is last block
    U32 BTYPE  = ConsumeBits(cursor, 2);

    U64 pixels_index = 0;
    if(0b00 == BTYPE)
    {
        print_verbos("No Compression\n"_s);
        FlushStreamingBufferBitBuffer(cursor);
        U16 LEN    = *ConsumeType(cursor, U16); // LEN  is the number of data bytes in the block.
        U16 NLEN   = *ConsumeType(cursor, U16); // NLEN is the one's complement of LEN.
        void* data =  ConsumeSize(cursor, LEN);
        memcpy((U8*)image_data + pixels_index,
               data,
               LEN);
    }
    else if(0b01 == BTYPE)
    {
        print_verbos("Fixed Huffman\n"_s);
    }
    else if(0b10 == BTYPE)
    {
        
        U32 HLIT  = ConsumeBits(cursor, 5);
        U32 HDIST = ConsumeBits(cursor, 5);
        U32 HCLEN = ConsumeBits(cursor, 4);

        
        print_verbos("Dynamic Huffman\n"_s);
        
    }
    else if(0b11 == BTYPE)
    {
        print_verbos("Error\n"_s);
        ExitProcess(1);
    }
    
    if(0 == BFINAL)
    {
        // Iterate over the next chunks
        print_string("Do work!!\n"_s);
    }
    
    
    // @CleanUp: This is here just to make sure that the Assert is working fine; 
    //ConsumeSize(cursor, );
        
    //ConsumeType(cursor, PNG_IDAT_Chunk_Footer);
    //Assert(cursor->size == 0);



    /*-
      do
          read block header from input stream.
          if stored with no compression
              skip any remaining bits in current partially processed byte
              read LEN and NLEN (see next section)
              copy LEN bytes of data to output
          otherwise
              if compressed with dynamic Huffman codes
                  read representation of code trees (see subsection below)
              loop (until end of block code recognized)
                  decode literal/length value from input stream
                  if value < 256
                      copy value (literal byte) to output stream
                  otherwise
                      if value = end of block (256)
                          break from loop
                      otherwise (value = 257..285)
                          decode distance from input stream

                          move backwards distance bytes in the output
                          stream, and copy length bytes from this
                          position to the output stream.
               end loop
       while not last block
     */

    
    
}



internal inline void
parse_ihdr_data(PNG_IHDR_Chunk* ihdr, PNG_FILE* result)
{
    result->width              = ihdr->width;
    result->height             = ihdr->height;
    result->bit_depth          = ihdr->bit_depth;
    result->color_type         = ihdr->color_type;
    result->compression_method = ihdr->compression_method;
    result->filter_method      = ihdr->filter_method;
    result->interlace_method   = ihdr->interlace_method;   
            
    bswap_ip(&result->width);
    bswap_ip(&result->height);
    print_ihdr(ihdr);
}


internal inline void
_print_ihdr(PNG_IHDR_Chunk* ihdr)
{
    constexpr auto formatString = R"str(    Width            : %u
    Height           : %u
    bit_depth        : %u
    color_type       : %u
    Compression Mode : %u
    Filter Mode      : %u
    Interlace Mode   : %u
)str";
    char buffer[256] = {};
    S32 size = snprintf((char*)buffer, 256, formatString,
                        bswap(ihdr->width),
                        bswap(ihdr->height),
                        ihdr->bit_depth,
                        ihdr->color_type,
                        ihdr->compression_method,
                        ihdr->filter_method,
                        ihdr->interlace_method);
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
}


internal inline void
_print_time(PNG_TIME_Chunk* time)
{
    constexpr auto formatString = R"str(    year   : %u
    month  : %u
    day    : %u
    hour   : %u
    minute : %u
    second : %u
)str";
    char buffer[256] = {};
    S32 size = snprintf((char*)buffer, 256, formatString,
                        time->year,
                        time->month,
                        time->day,
                        time->hour,
                        time->minute,
                        time->second);
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
}


internal inline void
_print_phys(PNG_PHYS_Chunk* phys)
{
    constexpr auto formatString = R"str(    Pixels per unit, X axis: %u
    Pixels per unit, Y axis: %u
    Unit : %u
)str";
    char buffer[256] = {};
    S32 size = snprintf((char*)buffer, 256, formatString,
                        phys->ppu_x,
                        phys->ppu_y,
                        phys->unit);
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
}


internal inline void
_print_chrm(PNG_CHRM_Chunk* chrm)
{
    constexpr auto formatString = R"str(    white_point_x   : %u
    white_point_y   : %u
    red_x           : %u
    red_y           : %u
    green_x         : %u
    green_y         : %u
    blue_x          : %u
    blue_y          : %u
)str";
    char buffer[256] = {};
    S32 size = snprintf((char*)buffer, 256, formatString,
                        bswap(chrm->white_point_x),
                        bswap(chrm->white_point_y),
                        bswap(chrm->red_x),
                        bswap(chrm->red_y),
                        bswap(chrm->green_x),
                        bswap(chrm->green_y),
                        bswap(chrm->blue_x),
                        bswap(chrm->blue_y));
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
}


internal inline void
_print_chunk_header(PNG_Chunk_Header* chunk_header)
{
    auto length =  chunk_header->length;
    auto numeric = chunk_header->numeric;
    auto signature  = (char*)(&numeric);
    
    char buffer[256] = {};
    buffer[0] = (char)signature[3];
    buffer[1] = (char)signature[2];
    buffer[2] = (char)signature[1];
    buffer[3] = (char)signature[0];
    buffer[4] = ':';

    S32 size = snprintf(buffer+4, 256, ": Length : %u\n", length);
    print_string(cstr_to_string(buffer, 5+(U64)size));

}

internal inline void
_print_text(String keyword, String text)
{
    print_string("    "_s);
    print_string(keyword);
    print_string(": "_s);
    print_string(text);
    print_string("\n"_s);
    
}


extern LinearAllocator global_allocator;

struct HuffmanTableEntry
{
    U16 symbol;
    U16 bitsUsed;    
};

struct HuffmanTable
{
    HuffmanTableEntry* entries;
    U32 entriesCount;
    U32 maxBitLen;
};


#define ArrayCount(x) (sizeof(x)/sizeof(x[0]))

constexpr U8 HuffmanCompressedBlockLengthsMap[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
};

constexpr U8 HuffmanCompressedBlockDistancesMap[] =
{
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
};


internal
HuffmanTable AllocateHuffman(U32 bitsCount)
{
    Assert((bitsCount <= 16) &&  "Implement Me ");
    HuffmanTable table;
    table.maxBitLen = bitsCount;
    table.entriesCount = ((U64)1 << bitsCount);
    table.entries = (HuffmanTableEntry*)AllocateSize(&global_allocator,
                                                     table.entriesCount * sizeof(HuffmanTableEntry));
    return table;
}

internal
U64 reverseBits(U64 value, U32 bitCount)
{
    Assert(bitCount <= 32);
    U64 result = 0;
    for(U32 bitIndex = 0;
        bitIndex <= bitCount/2;
        ++bitIndex)
    {
        U32 inv = (bitCount - (bitIndex + 1));
        result |= ((value >> bitIndex) & 0x1 ) << inv;
        result |= ((value >> inv) & 0x1) << bitIndex; 
    }
    
    return result;
}

internal
void BuildHuffman(HuffmanTable* huffman, U32* code_lengths, U32 LENGTHS_COUNT)
{
    constexpr U64 HUFFMAN_MAX_BIT_COUNT = 19;
    U64 codeLengthsHistogram[HUFFMAN_MAX_BIT_COUNT] = {};

    // DEFLATE spec for generating codes

    // 1) Count the number of codes for each code length. Let bl count[N] be the number of codes of length N, N>= 1.
    // Build the Histogram 
    for (U64 i = 0; i < LENGTHS_COUNT; ++i) ++codeLengthsHistogram[code_lengths[i]];
    codeLengthsHistogram[0] = 0;

    
    // Find the numerical value of the smallest code for each code length:
    U64 next_code[HUFFMAN_MAX_BIT_COUNT];
    next_code[0] = 0;
    
    for (U64 bitIndex = 1; bitIndex < HUFFMAN_MAX_BIT_COUNT; bitIndex++) 
    {
        next_code[bitIndex] = (next_code[bitIndex - 1] +
                               codeLengthsHistogram[bitIndex - 1]) << 1;
    }
    

    // 
    
    for (U64 SymbolIndex = 0;
         SymbolIndex < LENGTHS_COUNT;
         SymbolIndex++)
    {
        U32 len = code_lengths[SymbolIndex];
        if (len){
            U64 code = next_code[len]++;
            U64 arbitraryBits = huffman->maxBitLen - len;
            U64 entryCount = ((U64)1 << arbitraryBits);
            
            for(U64 entryIndex = 0;
                entryIndex < entryCount;
                ++entryIndex)
            {
                U64 baseIndex = (code << arbitraryBits) | entryIndex;
                // @Cleanup: This is slow -husamd
                U64 index = reverseBits(baseIndex, huffman->maxBitLen);
                
                HuffmanTableEntry* entry = huffman->entries + index;
                entry->symbol = (U16)SymbolIndex; 
                entry->bitsUsed = (U16)len; 
            }
        }
    }
    
}

internal
U32 DecodeHuffman(HuffmanTable* huffman, StreamingBuffer* sb)
{
    U32 index = PeekBits(sb, huffman->maxBitLen);
    Assert(index < huffman->entriesCount);

    HuffmanTableEntry entry = huffman->entries[index];
    U32 result = entry.symbol;

    Assert(entry.bitsUsed > 0);
    DiscardBits(sb, entry.bitsUsed);

    return result;
};


Buffer deflate(StreamingBuffer sb)
{
    Buffer result;
    result.content = nullptr;
    result.size    = 0;

    ZLIB_Header* zLibHeader = ConsumeType(&sb, ZLIB_Header);

    if (zLibHeader->_cm.CM != 8 || zLibHeader->_flags.FDICT != 0)
    {
        print_string("\tError[deflate]: something is wrong with the commpression\n"_s);
        return {0, 0};
        //ExitProcess(1);
    }


    U32 BFINAL = ConsumeBits(&sb, 1);// this indecates that this is last block
    U32 BTYPE  = ConsumeBits(&sb, 2);


    if(0b00 == BTYPE)
    {
        print_verbos("    No Compression\n"_s);
        FlushStreamingBufferBitBuffer(&sb);
        U16 LEN    = *ConsumeType(&sb, U16); // LEN  is the number of data bytes in the block.
        U16 NLEN   = *ConsumeType(&sb, U16); // NLEN is the one's complement of LEN.
        void* data =  ConsumeSize(&sb, LEN);
        //memcpy((U8*)image_data + pixels_index,
        //      data,
        //      LEN);
    }
    else if(0b01 == BTYPE)
    {
        print_verbos("    Fixed Huffman\n"_s);
    }
    else if(0b10 == BTYPE)
    {
        print_verbos("    Dynamic Huffman\n"_s);

        HuffmanTable huffman = AllocateHuffman(7);
        HuffmanTable hlitHuffman = AllocateHuffman(15);
        HuffmanTable hdistHuffman = AllocateHuffman(15);
        
        constexpr U32 length_dezigzag[] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
        constexpr U8 LENGTHS_COUNT = sizeof(length_dezigzag)/sizeof(length_dezigzag[0]);
        static_assert(LENGTHS_COUNT == 19);
        
        U32 HLIT  = ConsumeBits(&sb, 5) ;
        HLIT += 257;
        
        U32 HDIST = ConsumeBits(&sb, 5);
        HDIST += 1;
        
        U32 HCLEN = ConsumeBits(&sb, 4);
        HCLEN += 4;
        
        /*
          (HCLEN + 4) x 3 bits: code lengths for the code length
          alphabet given just above, in the order: 16, 17, 18,
          0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
          These code lengths are interpreted as 3-bit integers
          (0-7); as above, a code length of 0 means the
          corresponding symbol (literal/length or distance code
          length) is not used.
        */
        
        U32 code_lengths[LENGTHS_COUNT] = {};
        for(U64 i = 0; i < HCLEN; i++)
        {
            U8 value = (U8)ConsumeBits(&sb, 3); // 3 bits cannot exceed the value 7 
            code_lengths[length_dezigzag[i]] = value;
        }
        
        // Build huffman table for code_lengths
        BuildHuffman(&huffman, code_lengths, LENGTHS_COUNT);

        // Read the code lengths for the literal/length alphabet into a streaming buffer 
        //StreamingBuffer = 
        
        /*
          HLIT + 257 code lengths for the literal/length alphabet,
          encoded using the code length Huffman code
        */

        U32 counter = 0;

        U32 new_code_lengths[512] = {}; // zero it for now
        const auto limit = HLIT + HDIST;
        while(counter < limit)
        {
            U32 code_length = DecodeHuffman(&huffman, &sb);
            
            U32 repeatValue = 0;
            U32 repeatCount = 1;

            if (code_length <= 15)
            {
                repeatValue = code_length;
            }
            else if (code_length == 16)
            {
                repeatCount = ConsumeBits(&sb, 2) + 3;
                repeatValue = new_code_lengths[counter - 1];
            }
            else if (code_length == 17)
            {
                repeatCount = ConsumeBits(&sb, 3) + 3;
            }
            else if (code_length == 18)
            {
                repeatCount = ConsumeBits(&sb, 7) + 11;
            }

            while (repeatCount--)
                new_code_lengths[counter++] = repeatValue;
           
        }
        Assert(counter == limit);
        
        BuildHuffman(&hlitHuffman, new_code_lengths, HLIT);
        BuildHuffman(&hdistHuffman, new_code_lengths + HLIT, HDIST);
        
        print_verbos("    End Dynamic Huffman\n"_s);


        char buff[1024 *512] = {};
        U32 buffIt = 0;
        while(sb.size)
        {
            U32 code_or_len = DecodeHuffman(&hlitHuffman, &sb);
            if (code_or_len < 256)
            {
                U8 code = (U8)code_or_len;
                buff[buffIt++] = (char)code;
            }
            else if (code_or_len > 256)
            {
                U32 index = code_or_len - 257;
                Assert(index < ArrayCount(HuffmanCompressedBlockLengthsMap));
                U8 extraBits = HuffmanCompressedBlockLengthsMap[index];
                
                U32 len = 3 + index;
                if (extraBits > 0){//husamd break and see tempVal
                    auto tempVal = ConsumeBits(&sb, extraBits);
                    len += (U32)reverseBits(tempVal, extraBits);
                }

                U32 distance = DecodeHuffman(&hdistHuffman, &sb);
                Assert(distance < ArrayCount(HuffmanCompressedBlockDistancesMap));

                extraBits = HuffmanCompressedBlockDistancesMap[distance];
                distance += 1;
                if (extraBits > 0){
                    auto tempVal = ConsumeBits(&sb, extraBits);
                    distance += (U32)reverseBits(tempVal, extraBits);
                }

                Assert(distance < buffIt);
                auto it = buffIt - distance;
                while(len--)
                {
                    buff[buffIt++] = buff[it++];
                }
            }
            else
            {
                break;
            }                 
        }

        String str = {buffIt, buff};
        print_string(str);
        print_string("\n"_s);
    }
    else if(0b11 == BTYPE)
    {
        print_verbos("Error\n"_s);
        ExitProcess(1);
    }

    
    
    return result;
}
