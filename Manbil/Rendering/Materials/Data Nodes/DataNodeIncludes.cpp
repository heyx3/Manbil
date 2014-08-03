#include "DataNodeIncludes.h"


//PRIORITY: Take the following #defines, put them in DataNode.h, and use them in every DataNode child class instead of using this initialization system.
#define MAKE_NODE_READABLE_H(nodeClass) private: nodeClass _IGNORE_MEEEE_;
#define MAKE_NODE_READABLE_CPP(nodeClass, pretendConstructionArguments) nodeClass nodeClass::_IGNORE_MEEEE_ = nodeClass(pretendConstructionArguments);


void PrepareBasicNodesToBeRead(void)
{

    VectorF one(1.0f),
            two(1.0f, 1.0f),
            three(1.0f, 1.0f, 1.0f),
            four(1.0f, 1.0f, 1.0f);
    std::string a = "a";

    AddNode(one, one, a);
    ClampNode(one, one, a);
    CrossNode(three, three, a);
    DistanceNode(three, three, a);
    DivideNode(one, one, a);
    DotNode(three, three, a);
    GetLerpComponentNode(one, one, one, a);
    InterpolateNode(one, one, one, InterpolateNode::IT_Linear, a);
    LogNode(one, one, a);
    MaxNode(one, one, a);
    MinNode(one, one, a);
    ModuloNode(one, one, a);
    MultiplyNode(one, one, a);
    PowNode(one, one, a);
    ReflectNode(three, three, a);
    RefractNode(three, three, one, a);
    RemapNode(one, one, one, one, one, a);
    RotateAroundAxisNode(three, three, one, a);
    SubtractNode(one, one, a);

    CombineVectorNode(one, one, a);
    CustomExpressionNode("1.0f", 1, a);
    LightingNode(three, three, three, a, one, one, one, one, three);
    SignNode(one, a);
    CeilNode(one, a);
    FloorNode(one, a);
    AbsNode(one, a);
    SineNode(one, a);
    CosineNode(one, a);
    InverseSineNode(one, a);
    InverseCosineNode(one, a);
    NormalizeNode(three, a);
    FractNode(one, a);
    OneMinusNode(one, a);
    NegativeNode(one, a);
    SwizzleNode(two, SwizzleNode::C_X, a);
    VectorComponentsNode(two, a);
    WhiteNoiseNode(one, a);

    LinearizeDepthSampleNode(one, a);
    ParamNode(1, "haha", a);
    ShaderInNode(1, a, 0);
    SpaceConverterNode(three, SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN, SpaceConverterNode::DT_POSITION, a);
    //TODO: Once SubroutineNode is figured out, add code here.
    TextureSample2DNode(two, "hahaha", a);
    TextureSample3DNode(three, "hahahaha", a);
    TextureSampleCubemapNode(three, "hahahahahaha", a);
}