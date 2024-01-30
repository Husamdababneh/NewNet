/* ========================================================================
   $File: rfc-1951.cpp
   $Date: 2024-01-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: DEFLATE Compressed Data Format Specification version 1.3 Implementation
   ========================================================================*/

// NOTE:
/*
 * This implementation is highly influenced by Casey Muratori's implementation
 * 
 * What I do here for now, at least, for the Dynamic Huffman is the following
 * 1- Read the bits as stated in the spec
 * 2- Instead of createing a 'tree' to represent the codes, I generate a table with the codes 
 */


#include "base.h"
#include "rfc-1951.h"


// Forward declarations here , move these to .h file
internal HuffmanTable AllocateHuffman(U32 bitsCount);
internal void BuildHuffman(HuffmanTable* huffman, U32* code_lengths, U32 LENGTHS_COUNT);
internal B8 inflate(StreamingBuffer sb, Buffer*);
internal U32 DecodeHuffman(const HuffmanTable*, StreamingBuffer*);
internal B8 HuffmanDecompress(StreamingBuffer*, HuffmanTable* hlitHuffman, HuffmanTable* hdistHuffman, Buffer*);

internal B8 ComputeDynamicHuffman(StreamingBuffer sb, HuffmanTable* hlitHuffman, HuffmanTable* hdistHuffman);

// TODO: move this to memory or io
constexpr
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



constexpr U8 HuffmanCompressedBlockLengthsMap[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
};

constexpr U8 HuffmanCompressedBlockDistancesMap[] =
{
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
};

constexpr U16 FIXED_HUFFMAN_SYBMOLS_COUNT = 288;
constexpr U32 FixedHuffmanCodeLengths[FIXED_HUFFMAN_SYBMOLS_COUNT] =
{
    // 144 * 8 bits per symbol
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8, 
	
    // 112 * 9 bits per symbol
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,9,9,9,9,
	
    // 24 * 7 bits per symbol
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	
    // 8 * 8 bits per symbol
    8,8,8,8,8,8,8,8
};
constexpr U32 FIXED_HUFFMAN_DISTANCE_CODES_LENGTHS                        = 32;
constexpr U32 FixedHuffmanDistanceCodes[FIXED_HUFFMAN_DISTANCE_CODES_LENGTHS] =
{
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static_assert(countof(FixedHuffmanCodeLengths) == FIXED_HUFFMAN_SYBMOLS_COUNT);
static_assert(countof(FixedHuffmanDistanceCodes) == FIXED_HUFFMAN_DISTANCE_CODES_LENGTHS);

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
U32 DecodeHuffman(const HuffmanTable* huffman, StreamingBuffer* sb)
{
    U32 index = PeekBits(sb, huffman->maxBitLen);
    Assert(index < huffman->entriesCount);
	
    HuffmanTableEntry entry = huffman->entries[index];
    U32 result = entry.symbol;
	
    Assert(entry.bitsUsed > 0);
    DiscardBits(sb, entry.bitsUsed);
	
    return result;
};


internal
void BuildHuffman(HuffmanTable* huffman, U32* code_lengths, U32 LENGTHS_COUNT)
{
    constexpr U64 HUFFMAN_MAX_BIT_COUNT = 19;
    U64 codeLengthsHistogram[HUFFMAN_MAX_BIT_COUNT] = {};
	
    // INFLATE spec for generating codes
	
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

[[nodiscard]]
internal
B8 inflate(StreamingBuffer sb, Buffer* result)
{
    
    ZLIB_Header* zLibHeader = ConsumeType(&sb, ZLIB_Header);
	
    if (zLibHeader->_cm.CM != 8 || zLibHeader->_flags.FDICT != 0)
    {
        print_string("\tError[inflate]: Cannot find ZLIB header\n"_s);
        return false;
    }
	
    HuffmanTable hlitHuffman;
    HuffmanTable hdistHuffman;
    U32 BFINAL;
    do {
        BFINAL = ConsumeBits(&sb, 1);// this indecates that this is last block
        U32 BTYPE  = ConsumeBits(&sb, 2);
		
        if(0b00 == BTYPE)
        {
            print_verbos("    Zlib Store\n"_s);
            FlushStreamingBufferBitBuffer(&sb);
            U16 LEN    = *ConsumeType(&sb, U16); // LEN  is the number of data bytes in the block.
            U16 NLEN   = (U16)~(*ConsumeType(&sb, U16)); // NLEN is the one's complement of LEN.
            Assert(LEN == NLEN);
            U8* data =  (U8*)ConsumeSize(&sb, LEN);
			
            String s = {(U64)LEN, (char*)data};
            print_string(s);
            print_string("\n"_s);
        }
        else if(0b11 == BTYPE)
        {
            print_verbos("Error\n"_s);
            return false;
        }
        else
        {
            
            if(0b01 == BTYPE)
            {
                print_verbos("    Fixed Huffman\n"_s);
				
                // TODO: Evaluate this at compile-time, this will never change 
                hlitHuffman = AllocateHuffman(9);
                hdistHuffman = AllocateHuffman(5);
				
                BuildHuffman(&hlitHuffman, (U32*)FixedHuffmanCodeLengths, FIXED_HUFFMAN_SYBMOLS_COUNT);
                BuildHuffman(&hdistHuffman, (U32*)FixedHuffmanDistanceCodes, 32);
				
            }
            else //  (0b10 == BTYPE)
            {
                print_verbos("    Dynamic Huffman\n"_s);
				
                hlitHuffman = AllocateHuffman(15);
                hdistHuffman = AllocateHuffman(15);
                
                ComputeDynamicHuffman(sb, &hlitHuffman, &hdistHuffman);
            }
            HuffmanDecompress(&sb, &hlitHuffman, &hdistHuffman, result);
        }
		
        if (BFINAL == 1) break;
		
    }while(!BFINAL);
    
    return 1;
}

#define __OUT__

internal
B8 HuffmanDecompress(StreamingBuffer* sb, HuffmanTable* hlitHuffman, HuffmanTable* hdistHuffman, Buffer* buff)
{
    U32 buffIt = 0;
    while(sb->size)
    {
        U32 code_or_len = DecodeHuffman(hlitHuffman, sb);
        if (code_or_len <= 255)
        {
            U8 code = (U8)code_or_len;
            buff->content[buffIt++] = code;
        }
        else if (code_or_len >= 257)
        {
            U32 index = code_or_len - 257;
            Assert(index < countof(HuffmanCompressedBlockLengthsMap));
            U8 extraBits = HuffmanCompressedBlockLengthsMap[index];
			
            U32 len = 3 + index;
            if (extraBits > 0){//husamd break and see tempVal
                auto tempVal = ConsumeBits(sb, extraBits);
                len += (U32)reverseBits(tempVal, extraBits);
            }
			
            U32 distance = DecodeHuffman(hdistHuffman, sb);
            Assert(distance < countof(HuffmanCompressedBlockDistancesMap));
			
            extraBits = HuffmanCompressedBlockDistancesMap[distance];
            distance += 1;
            if (extraBits > 0){
                auto tempVal = ConsumeBits(sb, extraBits);
                distance += (U32)reverseBits(tempVal, extraBits);
            }
			
            Assert(distance < buffIt);
            auto it = buffIt - distance;
            while(len--)
            {
                buff->content[buffIt++] = buff->content[it++];
            }
        }
        else
        {
            // End of block
            break;
        }                 
    }
    return true;
}

internal
B8 ComputeDynamicHuffman(StreamingBuffer sb,
                         HuffmanTable* hlitHuffman,
                         HuffmanTable* hdistHuffman)
{
    HuffmanTable huffman = AllocateHuffman(7);
	
    constexpr U8  LENGTHS_COUNT                  = 19;
    constexpr U32 length_dezigzag[LENGTHS_COUNT] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
	
    U32 HLIT  = ConsumeBits(&sb, 5);
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
	
    // @Cleanup: 512?
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
	
    BuildHuffman(hlitHuffman, new_code_lengths, HLIT);
    BuildHuffman(hdistHuffman, new_code_lengths + HLIT, HDIST);
	
    return 0;
};
