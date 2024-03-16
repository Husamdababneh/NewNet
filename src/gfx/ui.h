/* ========================================================================
   $File: ui.h
   $Date: 2024-03-16
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once



struct BackEndRect
{
    U32 id; // offset in GPU buffer
};

struct GLGuiData
{
    Arena* rects;
    U64 rect_count;
};

struct GLGui
{
    GLGuiData data;

    GLBufferId vbo, ebo;
    GLVertexArray vao;
    GLShaderProgram shaderProgram;
};
