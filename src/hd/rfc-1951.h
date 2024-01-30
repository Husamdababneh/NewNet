/* ========================================================================
   $File: rfc-1951.h
   $Date: 2024-01-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


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
