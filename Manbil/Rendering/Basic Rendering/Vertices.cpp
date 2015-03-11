#include "Vertices.h"


typedef RenderIOAttributes::Attribute RenderAttr;


RenderIOAttributes VertexPos::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"));
}
RenderIOAttributes VertexPosColor::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(3, false, "vIn_Color3"));
}
RenderIOAttributes VertexPosUV::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(2, false, "vIn_UV"));
}
RenderIOAttributes VertexPosNormal::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(3, true, "vIn_Normal"));
}
RenderIOAttributes VertexPosUVNormal::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(2, false, "vIn_UV"),
                              RenderAttr(3, true, "vIn_Normal"));
}
RenderIOAttributes VertexPosUVNormalColor::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(2, false, "vIn_UV"),
                              RenderAttr(3, true, "vIn_Normal"),
                              RenderAttr(3, false, "vIn_Color3"));
}