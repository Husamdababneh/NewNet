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

              print_text(keyword, "Yet To be decoded"_s);
          }break;
          case 'iCCP':
          case 'zTXt':
          {
              String keyword = ConsumeCString(&cursor);
              U8 compression_method = *(U8*)ConsumeSize(&cursor, 1);
              U64 textLen = chunkHeader->length - (keyword.length + 1);

              StreamingBuffer sb;
              sb.content = (U8*)ConsumeSize(&cursor, textLen);//ConsumeString(&cursor, textLen);
              sb.size    = textLen;
              print_text(keyword, "Decode me"_s);
          }break;
          case 'IEND':
          {
              //print_string("IEND: Reached End of file\n"_s);
          }break;
          default:
          {
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
    (header);
    
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


/*
#include <stdio.h>
#include <stdlib.h>

// Define a structure for a node in the adaptive Huffman tree
typedef struct Node {
    int value;        // Symbol value (or special value for internal nodes)
    struct Node* parent;
    struct Node* left;
    struct Node* right;
} Node;

// Function to create a new node
Node* createNode(int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;
    newNode->parent = newNode->left = newNode->right = NULL;
    return newNode;
}

// Function to rebuild the adaptive Huffman tree from encoded data
Node* rebuildTree(unsigned char* encodedData, size_t dataSize) {
    Node* root = createNode(0);  // Initialize with a root node
    
    // Loop through the encoded data and rebuild the tree
    Node* currentNode = root;
    size_t i = 0;
    
    while (i < dataSize) {
        // Read a bit from the encoded data
        unsigned char bit = encodedData[i] & 0x80;  // Get the most significant bit
        int value = encodedData[i] & 0x7F;          // Get the remaining 7 bits as a value
        
        // Traverse the tree based on the bit (0 for left, 1 for right)
        Node* nextNode = (bit == 0) ? currentNode->left : currentNode->right;
        
        if (nextNode == NULL) {
            // If the node doesn't exist, create a new node and attach it
            nextNode = createNode(value);
            nextNode->parent = currentNode;
            if (bit == 0) {
                currentNode->left = nextNode;
            } else {
                currentNode->right = nextNode;
            }
            currentNode = root;  // Reset to the root for next iteration
        } else if (nextNode->value == 0) {
            // Internal node, continue traversal
            currentNode = nextNode;
        } else {
            // Leaf node, emit the symbol and restart from the root
            printf("Symbol: %d\n", nextNode->value);
            currentNode = root;
        }
        
        if (bit == 0) {
            // Shift to the next bit if we used the current one
            encodedData[i] = (encodedData[i] << 1);
            if (encodedData[i] == 0) {
                i++;  // Move to the next byte
            }
        }
    }
    
    return root;
}

int main() {
    // Example encoded data (you should replace this with your actual encoded data)
    unsigned char encodedData[] = {0b10000001, 0b10101011};
    size_t dataSize = sizeof(encodedData) / sizeof(encodedData[0]);
    
    // Rebuild the adaptive Huffman tree
    Node* root = rebuildTree(encodedData, dataSize);
    
    // You can use the 'root' to navigate the Huffman tree and decode data
    
    // Don't forget to free the memory allocated for the tree when done
    // (This example doesn't include memory cleanup for simplicity)
    
    return 0;
}


*/


