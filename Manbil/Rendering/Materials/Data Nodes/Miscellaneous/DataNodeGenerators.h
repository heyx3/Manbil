#pragma once


#include "../DataNode.h"
#include "../Parameters/CameraDataNode.h"
#include "../Parameters/TimeNode.h"


//Simplifies the creation of common data node combinations.
class DataNodeGenerators
{
public:

    //Outputs the dot of the surface normal and the normal from the surface to the camera.
    //A value of 1 indicates the camera is pointing directly at the surface.
    static DataLine FresnelEffect(DataLine surfacePos, DataLine surfaceNormal,
                                  DataLine cameraPos = CameraDataNode::GetCamPos());

    //Calculates a UV output using an offset, panned, and/or scaled input UV (done in that order).
    //Any effects with default values (values that don't change anything) are optimized out.
    //Note: if you do this UV stuff in the vertex shader,
    //    then the texture reads in the fragment shader can be constant ("independent", i.e. no math involved),
    //    which improves performance.
    static DataLine CreateComplexUV(const DataLine & uvs,
                                    DataLine scale = DataLine(VectorF(Vector2f(1.0f, 1.0f))),
                                    DataLine offset = DataLine(VectorF(Vector2f(0.0f, 0.0f))),
                                    DataLine pan = DataLine(VectorF(Vector2f(0.0f, 0.0f))),
                                    DataLine time = TimeNode::GetTime());

    //Takes the object-space position from the given vertex input and converts it into homogenous screen-space coordinates.
    //The vertex type must have a static function with the header "VertexAttributes GetAttributeData(void)".
    template<typename VertexType>
    static DataLine ObjectPosToScreenPos(int objPosVertexInputIndex)
    {
        return DataLine(DataNodePtr(new ObjectPosToScreenPosCalcNode(DataLine(DataNodePtr(new VertexInputNode(VertexType::GetAttributeData())), objPosVertexInputIndex))),
                        ObjectPosToScreenPosCalcNode::GetHomogenousPosOutputIndex());
    }
};