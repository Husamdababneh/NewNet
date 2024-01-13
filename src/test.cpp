/* ========================================================================
   $File: test.cpp
   $Date: 2023-10-13
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/




#pragma warning(disable : 4189)
#pragma warning (disable : 4235)

#define SUCCESS(x) x == 0


#define PNG_MISC_CHUNKS


int test()
{
    Initalize_LinearAllocate(&global_allocator, MB(100));
    
    LinearAllocator allocator;
    Initalize_LinearAllocate(&allocator, KB(4));
    
    // Get info about the file 
    //File file = open_file("../assets/Rock035_1K-PNG_Color.png"_s8);
    //File file = open_file("../assets/Rock035.png"_s8);
    File file = open_file("../assets/tests/8x8_uncompressed-no-interlace.png"_s8);
    //File file = open_file("../assets/tests/8x8_uncompressed-no-interlace"_s8);

    // This needs to be fixed
    if (file.handle == NULL)
    {
        print_string("File not found\n"_s);
        ExitProcess(1);
    }
    
    // Allocate Enough memory to house the file itself
    void* memory = VirtualAlloc(0, (SIZE_T)file.size, MEM_COMMIT, PAGE_READWRITE);

    // Read entire file to the allocated memory above
    StreamingBuffer entire_file = read_entire_file(file, memory);

    // Get info about the image itself
    PNG_FILE image_info = parse_png_file(entire_file, &allocator);

    // Compute how much memory does the image need, 
    // usually width*height*channels*(bit_depth/8)
    U32 requested_memory_size =  compute_requested_memory_size(&image_info);

    // Allocate enough memory for the image data
    void* image_data = VirtualAlloc(0, (SIZE_T)requested_memory_size, MEM_COMMIT, PAGE_READWRITE);    

    // Recounstruct the image data (load image)
    //load_png_image(&image_info, image_data);
    
    // Deallocate/free the file memory (give it back to the allocator)
    Free_LinearAllocate(&allocator);
    Free_LinearAllocate(&global_allocator);
    ExitProcess(1);
}
int png2()
{
    //File file = open_file("../assets/Rock035_1K-PNG_Color.png"_s8);
    File file = open_file("../assets/Rock035.png"_s8);

    if (file.handle == nullptr)
    {
        print_string("Couldn't open file\n"_s);
        return -1;
    }
    
    LinearAllocator allocator;
    Initalize_LinearAllocate(&allocator, KB(4));
    
    {
        void* memory = VirtualAlloc(0, (SIZE_T)file.size, MEM_COMMIT, PAGE_READWRITE);
        StreamingBuffer entire_file = read_entire_file(file, memory);
        StreamingBuffer cursor = entire_file;
        
        constexpr U32 png_magic_size = 8;
        U8 png_magic[png_magic_size] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        if (memcmp(cursor.content, png_magic, png_magic_size) != 0)
        {
            print_string("This is not a png file\n"_s);
            ExitProcess(1);
        }
    
        PNG_File_Header* header = ConsumeType(&cursor, PNG_File_Header);

        PNG_Important_Chunks importantChunks = {};
        StreamingBufferLL idatBuffers = {};
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

            char buffer[256] = {};
            buffer[0] = (char)chunkHeader->signature[3];
            buffer[1] = (char)chunkHeader->signature[2];
            buffer[2] = (char)chunkHeader->signature[1];
            buffer[3] = (char)chunkHeader->signature[0];
            buffer[4] = ':';

            S32 size = snprintf(buffer+4, 256, ": Length : %u\n", chunkHeader->length);
            
            print_string(cstr_to_string(buffer, 5+(U64)size));



#ifdef SWITCH_STMT
#else SWITCH_STMT
            if (chunkHeader->numeric == 'IHDR'){
                importantChunks.ihdr = ConsumeType(&cursor, PNG_IHDR_Chunk);
                bswap_ip(&importantChunks.ihdr->width);
                bswap_ip(&importantChunks.ihdr->height);
                print_ihdr(importantChunks.ihdr);
                
            } else if (chunkHeader->numeric == 'IDAT'){
                StreamingBuffer* node =  AllocateType(&allocator, StreamingBuffer);
                node->content = cursor.content;
                node->size = chunkHeader->length;
                node->next = NULL;
                    
                idatBuffers.last =
                    ((idatBuffers.last ? idatBuffers.last->next : idatBuffers.first) = node);
                
                ConsumeSize(&cursor, chunkHeader->length);
            }
            else if (chunkHeader->numeric == 'tIME'){
                PNG_TIME_Chunk* time = ConsumeType(&cursor, PNG_TIME_Chunk);
                bswap_ip(&time->year);
                print_time(time);
            }
            else if (chunkHeader->numeric == 'tEXt'){
                // This will eat the null 
                String keyword = ConsumeCString(&cursor);
                ConsumeSize(&cursor, 1);
                print_string("    "_s);
                print_string(keyword);
                print_string(": "_s);
                U64 textSize = chunkHeader->length - (keyword.length + 1);
                U8* text = (U8*)ConsumeSize(&cursor, textSize);
                String textStr = {textSize, (char*)text};
                print_string(textStr);
                print_string("\n"_s);
            } else if (chunkHeader->numeric == 'pHYs'){
                PNG_PHYS_Chunk* phys = ConsumeType(&cursor, PNG_PHYS_Chunk);
                bswap_ip(&phys->ppu_x);
                bswap_ip(&phys->ppu_y);
                print_phys(phys);
            } else if (chunkHeader->numeric == 'cHRM'){
                PNG_CHRM_Chunk* chrm = ConsumeType(&cursor, PNG_CHRM_Chunk);
                bswap_ip(&chrm->white_point_x);
                bswap_ip(&chrm->white_point_y);
                bswap_ip(&chrm->red_x);
                bswap_ip(&chrm->red_y);
                bswap_ip(&chrm->green_x);
                bswap_ip(&chrm->green_y);
                bswap_ip(&chrm->blue_x);
                bswap_ip(&chrm->blue_y);
                
                print_chrm(chrm);
            }
            else if (chunkHeader->numeric == 'IEND')
            {
                // assert size == 0
            }
            else{
                ConsumeSize(&cursor, chunkHeader->length);
                print_string("    Unsupported Chunk\n"_s);
            }
            ConsumeType(&cursor, PNG_Chunk_Footer);
        }
#endif
        
        if (idatBuffers.first == NULL || idatBuffers.first == NULL)
        {
            print_string("Something went wrong... Exiting\n"_s);
            ExitProcess(1);
        }


        // Allocate Image Memory
        SIZE_T pixelsSize = importantChunks.ihdr->width *
            importantChunks.ihdr->height * importantChunks.ihdr->bit_depth;
        
        void* pixels = VirtualAlloc(0, pixelsSize, MEM_COMMIT, PAGE_READWRITE);
        U64 pixels_index = 0;
        // Just a copy so we dont mess the original
        StreamingBuffer* current = idatBuffers.first;
        while(current)
        {

            U64 actualDataSize = current->size - (sizeof(PNG_IDAT_Chunk_Header) + sizeof(PNG_IDAT_Chunk_Footer));
            PNG_IDAT_Chunk_Header* idatHeader = ConsumeType(current, PNG_IDAT_Chunk_Header);

            void* content = ConsumeSize(current, actualDataSize);
            StreamingBuffer contentBuffer = {};
            contentBuffer.content = (U8*)content;
            contentBuffer.size    = actualDataSize;

            // Unpack zlib compression
            if (idatHeader->_cm.CM != 8 || idatHeader->_flags.FDICT != 0)
            {
                print_string("something is wrong with the commpression\n"_s);
                ExitProcess(1);
            }

            U32 BFINAL = ConsumeBits(&contentBuffer, 1);// this indecates that last block
            U32 BTYPE  = ConsumeBits(&contentBuffer, 2);

            if (BTYPE == 0) {
                FlushStreamingBufferBitBuffer(&contentBuffer);
                U16 LEN  = *ConsumeType(&contentBuffer, U16); // LEN  is the number of data bytes in the block.
                U16 NLEN = *ConsumeType(&contentBuffer, U16); // NLEN is the one's complement of LEN.
                void* data = ConsumeSize(&contentBuffer, LEN);
                memcpy((U8*)pixels + pixels_index, data, LEN);
                pixels_index += LEN;
            }
            else if (BTYPE == 1){
                // fixed Huffman codes
                print_string("Fixed Huffman codes\n"_s);                    
            }
            else if (BTYPE == 2){
                // dynamic Huffman codes
                print_string("dynamic Huffman codes\n"_s);
            }
            else if (BTYPE == 3){
                // Error
                print_string("BTYPE is 3\n"_s);
                break;
            }
                
                    

            FlushStreamingBufferBitBuffer(&contentBuffer);
            PNG_IDAT_Chunk_Footer* idatFooter = ConsumeType(current, PNG_IDAT_Chunk_Footer);
            current = current->next;
        } 

        VirtualFree(memory, 0, MEM_RELEASE);        
    }
    Free_LinearAllocate(&allocator);
    ExitProcess(0);
}
