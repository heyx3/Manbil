#include "TextureSampleNode.h"

#include "../DataNodeIncludes.h"


DataNodePtr TextureSampleNode::CreateComplexTexture(std::string samplerName, DataLine scale, DataLine pan, DataLine offset, DataLine uvs)
{
    DataLine scaled = (!scale.IsConstant(Vector2f(1.0f, 1.0f)) ?
                          DataLine(DataNodePtr(new MultiplyNode(uvs, scale)), 0) :
                          uvs);
    DataLine offsetted = (!offset.IsConstant(Vector2f(0.0f, 0.0f)) ?
                             DataLine(DataNodePtr(new AddNode(scaled, offset)), 0) :
                             scaled);
    DataLine panned = (!pan.IsConstant(Vector2f(0.0f, 0.0f)) ?
                          DataLine(DataNodePtr(new AddNode(offsetted,
                                                           DataLine(DataNodePtr(new MultiplyNode(pan,
                                                                                                 DataLine(DataNodePtr(new TimeNode()), 0))), 0))), 0) :
                          offsetted);

    return DataNodePtr(new TextureSampleNode(samplerName, panned));
}

void TextureSampleNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + GetSamplerUniformName() + ", " + GetUVInput().GetValue() + ");\n";
}