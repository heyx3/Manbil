#include "Vertices.h"


typedef RenderIOAttributes::Attribute RenderAttr;


RenderIOAttributes VertexPos::GetAttributeData(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"));
}
RenderIOAttributes VertexPosColor::GetAttributeData(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(3, false, "vIn_Color3"));
}
RenderIOAttributes VertexPosUV::GetAttributeData(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(2, false, "vIn_UV"));
}
RenderIOAttributes VertexPosNormal::GetAttributeData(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(3, true, "vIn_Normal"));
}
RenderIOAttributes VertexPosUVNormal::GetAttributeData(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(2, false, "vIn_UV"),
                              RenderAttr(3, true, "vIn_Normal"));
}
RenderIOAttributes VertexPosUVNormalColor::GetAttributeData(void)
{
    return RenderIOAttributes(RenderAttr(3, false, "vIn_Pos"),
                              RenderAttr(2, false, "vIn_UV"),
                              RenderAttr(3, true, "vIn_Normal"),
                              RenderAttr(3, false, "vIn_Color3"));
}