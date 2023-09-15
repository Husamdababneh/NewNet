/* ========================================================================
   $File: main.h
   $Date: 2023-05-06
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once



struct GfxBackend
{
    String name;
};

struct OpenGLBackend
{
    GfxBackend head = {"OpenGl"_s};
    HGLRC context;

    // For now these methods shall be here
};

struct Counter
{
};



inline internal void print_string(const String str);
