#include "DataNodeGenerators.h"

#include "../DataNodeIncludes.h"


DataLine FresnelEffect(DataLine surfacePos, DataLine surfaceNormal, DataLine cameraPos)
    {
        return DataLine(DataNodePtr(new DotNode(surfaceNormal,
                                                DataLine(DataNodePtr(new NormalizeNode(DataLine(DataNodePtr(new SubtractNode(cameraPos,
                                                                                                                             surfacePos)), 0))), 0))), 0);
    }


DataLine DataNodeGenerators::CreateComplexUV(const DataLine & uvs, DataLine scale, DataLine offset, DataLine pan, DataLine time)
{
    DataLine offsetted = (!offset.IsConstant(Vector2f(0.0f, 0.0f)) ?
                          DataLine(DataNodePtr(new AddNode(uvs, offset)), 0) :
                          uvs);
    DataLine panned = (!pan.IsConstant(Vector2f(0.0f, 0.0f)) ?
                       DataLine(DataNodePtr(new AddNode(offsetted,
                                                        DataLine(DataNodePtr(new MultiplyNode(pan,
                                                                                              DataLine(DataNodePtr(new TimeNode()), 0))), 0))), 0) :
                       offsetted);
    DataLine scaled = (!scale.IsConstant(Vector2f(1.0f, 1.0f)) ?
                       DataLine(DataNodePtr(new MultiplyNode(panned, scale)), 0) :
                       panned);

    return scaled;
}