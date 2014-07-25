#include "DataNodeSerialization.h"

#include "../DataNodeIncludes.h"
#include <sstream>



const std::string DataNodeNames::Name_Add = "add",
                  DataNodeNames::Name_Subtract = "subtract",
                  DataNodeNames::Name_Divide = "divide",
                  DataNodeNames::Name_Multiply = "multiply",
                  DataNodeNames::Name_Combine = "combine",
                  DataNodeNames::Name_Log = "log",
                  DataNodeNames::Name_Sign = "sign",
                  DataNodeNames::Name_Ceil = "ceil",
                  DataNodeNames::Name_Floor = "floor",
                  DataNodeNames::Name_Abs = "abs",
                  DataNodeNames::Name_Sin = "sine",
                  DataNodeNames::Name_Cos = "cosine",
                  DataNodeNames::Name_ASin = "inverseSine",
                  DataNodeNames::Name_ACos = "inverseCosine",
                  DataNodeNames::Name_Normalize = "normalize",
                  DataNodeNames::Name_Fract = "fract",
                  DataNodeNames::Name_OneMinus = "oneMinus",
                  DataNodeNames::Name_Negative = "negative",
                  DataNodeNames::Name_ObjectPosToScreen = "objectPosToScreenPos",
                  DataNodeNames::Name_ObjectNormalToScreen = "objectNormalToScreenNormal",
                  DataNodeNames::Name_ObjectPosToWorld = "objectPosToWorldPos",
                  DataNodeNames::Name_ObjectNormalToWorld = "objectNormalToWorldNormal",
                  DataNodeNames::Name_WorldPosToScreen = "worldPosToScreenPos",
                  DataNodeNames::Name_WorldNormalToScreen = "worldNormalToScreenNormal",
                  DataNodeNames::Name_Clamp = "clamp",
                  DataNodeNames::Name_Cross = "cross",
                  DataNodeNames::Name_Dot = "dot",
                  DataNodeNames::Name_Distance = "distance",
                  DataNodeNames::Name_GetLerpComponent = "getLerpComponent",
                  DataNodeNames::Name_Lerp = "lerp",
                  DataNodeNames::Name_SmoothLerp = "smoothLerp",
                  DataNodeNames::Name_SuperSmoothLerp = "superSmoothLerp",
                  DataNodeNames::Name_Max = "max",
                  DataNodeNames::Name_Min = "min",
                  DataNodeNames::Name_Modulo = "modulo",
                  DataNodeNames::Name_Pow = "pow",
                  DataNodeNames::Name_Reflect = "reflect",
                  DataNodeNames::Name_Refract = "refract",
                  DataNodeNames::Name_RotateAroundAxis = "rotateAroundAxis",
                  DataNodeNames::Name_Remap = "remap",
                  DataNodeNames::Name_SplitComponents = "splitComponents",
                  DataNodeNames::Name_SurfaceLightCalc = "calcSurfaceBrightness",
                  DataNodeNames::Name_WhiteNoise = "whiteNoise",
                  DataNodeNames::Name_CustomExpression = "customExpression",
                  DataNodeNames::Name_Parameter = "parameter",
                  DataNodeNames::Name_Texture2D = "texture2D",
                  DataNodeNames::Name_Texture3D = "texture3D",
                  DataNodeNames::Name_TextureCubemap = "textureCubemap",
                  DataNodeNames::Name_Swizzle = "swizzle";




//Represents an input to another DataNode (i.e. a DataLine).
//Consists of the input's name and the output index.
class DataNodeInput : public ISerializable
{
public:

    std::string Name;
    unsigned int OutputIndex;

    VectorF ConstantValue;


    bool IsConstantValue(void) const { return ConstantValue.GetSize() > 0; }

    DataNodeInput(std::string name, unsigned int outputIndex) : Name(name), OutputIndex(outputIndex) { }
    DataNodeInput(VectorF constantValue) : ConstantValue(constantValue) { }
    DataNodeInput(void) { }

    virtual bool ReadData(DataReader * data, std::string & outError) override
    {
        MaybeValue<bool> tryIsConstant = data->ReadBool(outError);
        if (!tryIsConstant.HasValue())
        {
            outError = "Error reading whether this input is constant: " + outError;
            return false;
        }

        if (tryIsConstant.GetValue())
        {
            //Get the value of the vector.
            MaybeValue<std::string> tryVector = data->ReadString(outError);
            if (!tryVector.HasValue())
            {
                outError = "Error reading constant input value: " + outError;
                return false;
            }

            //Make sure it's formatted correctly.
            std::string vecS = tryVector.GetValue();
            if (vecS[0] != '{' || vecS[vecS.size() - 1] != '}' ||
                vecS[1] != ' ' || vecS[vecS.size() - 2] != ' ')
            {
                outError = "Invalid vector input string '" + tryVector.GetValue() + "'; no '{ ' or ' }' at the ends of the string!";
                return false;
            }
            vecS.erase(vecS.begin());
            vecS.erase(vecS.end());
            
            //Parse out each float.
            unsigned int numbElements = 0;
            ConstantValue = VectorF((unsigned int)4);
            std::string floatStr;
            for (unsigned int i = 0; i < vecS.size(); ++i)
            {
                if (vecS[i] != ' ') floatStr += vecS[i];

                if (vecS[i] == ' ' || i == vecS.size() - 1)
                {
                    if (numbElements >= 4)
                    {
                        outError = "Too many floats in constant node input '{ " + vecS + " }'";
                        return false;
                    }

                    std::istringstream ss(floatStr);
                    if (!(ss >> ConstantValue.GetValue()[numbElements]))
                    {
                        outError = "Couldn't convert '" + floatStr + "' to a float value from the constant node input '" + vecS + "'";
                        return false;
                    }
                    floatStr.clear();

                    numbElements += 1;
                }
            }
            if (numbElements == 0)
            {
                outError = "No data inside the constant node input '{ " + vecS + " }'";
                return false;
            }
        }
        else
        {
            MaybeValue<std::string> tryName = data->ReadString(outError);
            if (!tryName.HasValue())
            {
                outError = "Error reading non-constant input's name: " + outError;
                return false;
            }
            Name = tryName.GetValue();

            MaybeValue<unsigned int> tryOutIndex = data->ReadUInt(outError);
            if (!tryOutIndex.HasValue())
            {
                outError = "Error reading non-constant input '" + Name + "'s output index: " + outError;
                return false;
            }
            OutputIndex = tryOutIndex.GetValue();
        }
        
        return true;
    }
    virtual bool WriteData(DataWriter * data, std::string & outError) const override
    {
        if (!data->WriteBool(IsConstantValue(), "isConstant", outError))
        {
            outError = "Error writing 'isConstant' value '" + std::to_string(IsConstantValue()) + "': " + outError;
            return false;
        }

        if (IsConstantValue())
        {
            std::string outVal = "{ ";
            for (unsigned int i = 0; i < ConstantValue.GetSize(); ++i)
                outVal += std::to_string(ConstantValue.GetValue()[i]) + " ";
            outVal += "}";
            if (!data->WriteString(outVal, "constantValue", outError))
            {
                outError = "Unable to write constant value '" + outVal + "': " + outError;
                return false;
            }
        }
        else
        {
            if (!data->WriteString(Name, "name", outError))
            {
                outError = "Unable to write this input's name, '" + Name + "'";
                return false;
            }
            if (!data->WriteUInt(OutputIndex, "outputIndex", outError))
            {
                outError = "Unable to write output index value " + std::to_string(OutputIndex) + ": " + outError;
                return false;
            }
        }

        return true;
    }
};


//A declaration of a data node -- its name, type, inputs, and a pointer to the actual node.
class DataNodeDeclaration : public ISerializable
{
public:

    static std::unordered_map<std::string, DataNodePtr> NodeMap;


    DataNodePtr Node;
    std::string Name;
    std::string TypeName;
    std::vector<DataLine> Inputs;


    virtual bool ReadData(DataReader * data, std::string & outError) override
    {
        //Read the name and node type.
        MaybeValue<std::string> tryName = data->ReadString(outError);
        if (!tryName.HasValue())
        {
            outError = "Error reading this node's name: " + outError;
            return false;
        }
        Name = tryName.GetValue();
        MaybeValue<std::string> tryType = data->ReadString(outError);
        if (!tryType.HasValue())
        {
            outError = "Error reading the type of this node, '" + Name + "': " + outError;
            return false;
        }
        TypeName = tryType.GetValue();

        //Read the inputs.
        MaybeValue<unsigned int> tryNumbInputs = data->ReadUInt(outError);
        if (!tryNumbInputs.HasValue())
        {
            outError = "Error reading the number of inputs for this '" + TypeName + "' node, '" + Name + "': " + outError;
            return false;
        }
        std::vector<DataNodeInput> inputs;
        inputs.resize(tryNumbInputs.GetValue());
        for (unsigned int i = 0; i < tryNumbInputs.GetValue(); ++i)
        {
            if (!data->ReadDataStructure(inputs[i], outError))
            {
                outError = "Error reading input index " + std::to_string(i) + " for node '" + Name + "' of type '" + TypeName + "': " + outError;
                return false;
            }
        }

        //Convert the read-in inputs into DataLine instances.
        Inputs.resize(inputs.size());
        for (unsigned int i = 0; i < inputs.size(); ++i)
        {
            if (inputs[i].IsConstantValue())
            {
                Inputs[i] = DataLine(inputs[i].ConstantValue);
            }
            else
            {
                auto searched = NodeMap.find(inputs[i].Name);
                if (searched == NodeMap.end())
                {
                    outError = "Couldn't find input '" + inputs[i].Name + "' (index " + std::to_string(i) + ") in the static 'NodeMap' collection";
                    return false;
                }
                Inputs[i] = DataLine(searched->second, inputs[i].OutputIndex);
            }
        }


        #pragma region Parse this node type to a DataNodePtr


        //Gets whether the given value equals the read-in node type.
        #define VALUE_IS(str) (TypeName.compare(DataNodeNames::str) == 0)

        //Sets the error message and returns false if the read-in "Inputs" collection isn't the given size.
        #define ASSERT_INPUT_SIZE_EQUAL(numbInputs) \
            if (Inputs.size() != numbInputs) \
            { \
                outError = "Expected " + std::to_string(numbInputs) + " inputs for '" + tryType.GetValue() + \
                                "' node named '" + Name + "', but there were " + std::to_string(Inputs.size()); \
                return false; \
            }
        //Sets the error message and returns false if the read-in "Inputs" collection isn't larger than the given size.
        #define ASSERT_INPUT_SIZE_GREATER_THAN(inputSize) \
            if (Inputs.size() <= inputSize) \
            { \
                outError = "Expected at least " + std::to_string(inputSize + 1) + " inputs for '" + tryType.GetValue() + \
                                "' node named '" + Name + "', but there were only " + std::to_string(Inputs.size()); \
                return false; \
            }
        //Sets the error message and returns false if the read-in "Inputs" collection isn't between "numbRequired" and "numbOptional" in size.
        #define ASSERT_INPUT_SIZE_OPTIONAL(numbRequired, numbOptional) \
            if (Inputs.size() < numbRequired) \
            { \
                outError = "Expected at least " + std::to_string(numbRequired) + " inputs for '" + tryType.GetValue() + \
                                "' node named '" + Name + "', but there were " + std::to_string(Inputs.size()); \
                return false; \
            } \
            else if (Inputs.size() > (numbRequired + numbOptional)) \
            { \
                outError = "Expected no more than " + std::to_string(numbRequired + numbOptional) + " inputs for '" + tryType.GetValue() + \
                                "' node named '" + Name + "', but there were " + std::to_string(Inputs.size()); \
                return false; \
            }

        //Parses a simple DataNode (i.e. the only requirement is that it has a specific number of inputs).
        #define PARSE_SIMPLE_OP_NODE(nameStr, inputSize, constructor) \
            else if (VALUE_IS(nameStr)) \
            { \
                ASSERT_INPUT_SIZE_EQUAL(inputSize); \
                Node = DataNodePtr(new constructor); \
            }
        //Parses the simplest-possible kind of DataNode (i.e. a simple one-input-one-output GLSL function).
        #define PARSE_SIMPLEST_FUNCTION_NODE(nodeStr, nodeType) PARSE_SIMPLE_OP_NODE(nodeStr, 1, nodeType(Inputs[0]))

        try
        {
            if (VALUE_IS(Name_Add))
            {
                ASSERT_INPUT_SIZE_GREATER_THAN(0);
                Node = DataNodePtr(new AddNode(Inputs));
            }
            else if (VALUE_IS(Name_Subtract))
            {
                ASSERT_INPUT_SIZE_GREATER_THAN(1);
                Node = DataNodePtr(new SubtractNode(Inputs[0], std::vector<DataLine>(Inputs.begin() + 1, Inputs.end())));
            }
            else if (VALUE_IS(Name_Divide))
            {
                ASSERT_INPUT_SIZE_GREATER_THAN(1);
                Node = DataNodePtr(new DivideNode(Inputs[0], std::vector<DataLine>(Inputs.begin() + 1, Inputs.end())));
            }
            else if (VALUE_IS(Name_Multiply))
            {
                ASSERT_INPUT_SIZE_GREATER_THAN(1);
                Node = DataNodePtr(new MultiplyNode(Inputs));
            }
            else if (VALUE_IS(Name_Combine))
            {
                ASSERT_INPUT_SIZE_GREATER_THAN(0);
                Node = DataNodePtr(new CombineVectorNode(Inputs));
            }
            else if (VALUE_IS(Name_Log))
            {
                ASSERT_INPUT_SIZE_OPTIONAL(1, 1);
                if (Inputs.size() == 1)
                    Node = DataNodePtr(new LogNode(Inputs[0]));
                else Node = DataNodePtr(new LogNode(Inputs[0], Inputs[1]));
            }
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Sign, SignNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Ceil, CeilNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Floor, FloorNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Abs, AbsNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Sin, SineNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Cos, CosineNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_ASin, InverseSineNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_ACos, InverseCosineNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Normalize, NormalizeNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Fract, FractNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_OneMinus, OneMinusNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_Negative, NegativeNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_ObjectPosToScreen, ObjectPosToScreenPosCalcNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_ObjectNormalToScreen, ObjectNormalToScreenNormalCalcNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_ObjectPosToWorld, ObjectPosToWorldPosCalcNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_ObjectNormalToWorld, ObjectNormalToWorldNormalCalcNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_WorldPosToScreen, WorldPosToScreenPosCalcNode)
            PARSE_SIMPLEST_FUNCTION_NODE(Name_WorldNormalToScreen, WorldNormalToScreenNormalCalcNode)
            PARSE_SIMPLE_OP_NODE(Name_Clamp, 3, ClampNode(Inputs[0], Inputs[1], Inputs[2]))
            PARSE_SIMPLE_OP_NODE(Name_Cross, 2, CrossNode(Inputs[0], Inputs[1]))
            PARSE_SIMPLE_OP_NODE(Name_Dot, 2, DotNode(Inputs[0], Inputs[1]))
            PARSE_SIMPLE_OP_NODE(Name_Distance, 2, DistanceNode(Inputs[0], Inputs[1]))
            PARSE_SIMPLE_OP_NODE(Name_GetLerpComponent, 3, GetLerpComponentNode(Inputs[0], Inputs[1], Inputs[2]))
            PARSE_SIMPLE_OP_NODE(Name_Lerp, 3, InterpolateNode(Inputs[0], Inputs[1], Inputs[2], InterpolateNode::IT_Linear))
            PARSE_SIMPLE_OP_NODE(Name_SmoothLerp, 3, InterpolateNode(Inputs[0], Inputs[1], Inputs[2], InterpolateNode::IT_Smooth))
            PARSE_SIMPLE_OP_NODE(Name_SuperSmoothLerp, 3, InterpolateNode(Inputs[0], Inputs[1], Inputs[2], InterpolateNode::IT_VerySmooth))
            PARSE_SIMPLE_OP_NODE(Name_Max, 2, MaxMinNode(Inputs[0], Inputs[1], true))
            PARSE_SIMPLE_OP_NODE(Name_Min, 2, MaxMinNode(Inputs[0], Inputs[1], false))
            PARSE_SIMPLE_OP_NODE(Name_Modulo, 2, ModuloNode(Inputs[0], Inputs[1]))
            PARSE_SIMPLE_OP_NODE(Name_Pow, 2, PowNode(Inputs[0], Inputs[1]))
            PARSE_SIMPLE_OP_NODE(Name_Reflect, 2, ReflectNode(Inputs[0], Inputs[1]))
            PARSE_SIMPLE_OP_NODE(Name_Refract, 3, RefractNode(Inputs[0], Inputs[1], Inputs[2]))
            PARSE_SIMPLE_OP_NODE(Name_RotateAroundAxis, 3, RotateAroundAxisNode(Inputs[0], Inputs[1], Inputs[2]))
            PARSE_SIMPLE_OP_NODE(Name_Remap, 5, RemapNode(Inputs[0], Inputs[1], Inputs[2], Inputs[3], Inputs[4]))
            PARSE_SIMPLE_OP_NODE(Name_SplitComponents, 1, VectorComponentsNode(Inputs[0]))
            PARSE_SIMPLE_OP_NODE(Name_SurfaceLightCalc, 8, LightingNode(Inputs[0], Inputs[1], Inputs[2], Inputs[3],
                                                                     Inputs[4], Inputs[5], Inputs[6], Inputs[7]))
            else if (VALUE_IS(Name_WhiteNoise))
            {
                ASSERT_INPUT_SIZE_OPTIONAL(1, 1);
                if (Inputs.size() == 1)
                    Node = DataNodePtr(new WhiteNoiseNode(Inputs[0]));
                else Node = DataNodePtr(new WhiteNoiseNode(Inputs[0], Inputs[1]));
            }
            else if (VALUE_IS(Name_CustomExpression))
            {
                MaybeValue<std::string> tryExpr = data->ReadString(outError);
                if (!tryExpr.HasValue())
                {
                    outError = "Error reading custom expression for '" + Name + "': " + outError;
                    return false;
                }

                MaybeValue<unsigned int> tryOutSize = data->ReadUInt(outError);
                if (!tryOutSize.HasValue())
                {
                    outError = "Error reading custom expression's output size for '" + Name + "': " + outError;
                    return false;
                }

                Node = DataNodePtr(new CustomExpressionNode(tryExpr.GetValue(), tryOutSize.GetValue(), Inputs));
            }
            else if (VALUE_IS(Name_Parameter))
            {
                MaybeValue<std::string> tryParamName = data->ReadString(outError);
                if (!tryParamName.HasValue())
                {
                    outError = "Error reading name of parameter for '" + Name + "': " + outError;
                    return false;
                }
                MaybeValue<unsigned int> tryParamSize = data->ReadUInt(outError);
                if (!tryParamSize.HasValue())
                {
                    outError = "Error reading output size of parameter for '" + Name + "': " + outError;
                    return false;
                }

                Node = DataNodePtr(new ParamNode(tryParamSize.GetValue(), tryParamName.GetValue()));
            }
            else if (VALUE_IS(Name_Texture2D))
            {
                ASSERT_INPUT_SIZE_EQUAL(1);
                MaybeValue<std::string> tryTexName = data->ReadString(outError);
                if (!tryTexName.HasValue())
                {
                    outError = "Error reading name of '" + Name + "'s 2D texture: " + outError;
                    return false;
                }

                Node = DataNodePtr(new TextureSample2DNode(Inputs[0], tryTexName.GetValue()));
            }
            else if (VALUE_IS(Name_Texture3D))
            {
                ASSERT_INPUT_SIZE_EQUAL(1);
                MaybeValue<std::string> tryTexName = data->ReadString(outError);
                if (!tryTexName.HasValue())
                {
                    outError = "Error reading name of '" + Name + "'s 3D texture: " + outError;
                    return false;
                }

                Node = DataNodePtr(new TextureSample3DNode(Inputs[0], tryTexName.GetValue()));
            }
            else if (VALUE_IS(Name_TextureCubemap))
            {
                ASSERT_INPUT_SIZE_EQUAL(1);
                MaybeValue<std::string> tryTexName = data->ReadString(outError);
                if (!tryTexName.HasValue())
                {
                    outError = "Error reading name of '" + Name + "'s cubemap texture: " + outError;
                    return false;
                }

                Node = DataNodePtr(new TextureSampleCubemapNode(Inputs[0], tryTexName.GetValue()));
            }
            else if (VALUE_IS(Name_Swizzle))
            {
                ASSERT_INPUT_SIZE_EQUAL(1);

                MaybeValue<unsigned int> numbSwizzles = data->ReadUInt(outError);
                if (!numbSwizzles.HasValue())
                {
                    outError = "Error reading the size of the swizzle operator for '" + Name + "': " + outError;
                    return false;
                }

                if (numbSwizzles.GetValue() == 0)
                {
                    outError = "Need at least one swizzle value for '" + Name + "'!";
                    return false;
                }
                if (numbSwizzles.GetValue() > 4)
                {
                    outError = "Too many swizzle values for '" + Name + "' -- can have up to four, but there are " + std::to_string(numbSwizzles.GetValue());
                    return false;
                }

                std::vector<SwizzleNode::Components> components;
                for (unsigned int i = 0; i < numbSwizzles.GetValue(); ++i)
                {
                    MaybeValue<std::string> trySwizzleValue = data->ReadString(outError);
                    if (!trySwizzleValue.HasValue())
                    {
                        outError = "Couldn't read swizzle value " + std::to_string(i + 1) + " for '" + Name + "': " + outError;
                        return false;
                    }
                    std::string swizVal = trySwizzleValue.GetValue();
                    if (swizVal.compare("x") * swizVal.compare("r") == 0)
                        components.insert(components.end(), SwizzleNode::C_X);
                    else if (swizVal.compare("y") * swizVal.compare("g"))
                        components.insert(components.end(), SwizzleNode::C_Y);
                    else if (swizVal.compare("z") * swizVal.compare("b"))
                        components.insert(components.end(), SwizzleNode::C_Z);
                    else if (swizVal.compare("w") * swizVal.compare("a"))
                        components.insert(components.end(), SwizzleNode::C_W);
                    else
                    {
                        outError = "Unknown swizzle value '" + trySwizzleValue.GetValue() + "' for '" + Name + "'";
                        return false;
                    }
                }
                switch (numbSwizzles.GetValue())
                {
                    case 1:
                        Node = DataNodePtr(new SwizzleNode(Inputs[0], components[0]));
                        break;
                    case 2:
                        Node = DataNodePtr(new SwizzleNode(Inputs[0], components[0], components[1]));
                        break;
                    case 3:
                        Node = DataNodePtr(new SwizzleNode(Inputs[0], components[0], components[1], components[2]));
                        break;
                    case 4:
                        Node = DataNodePtr(new SwizzleNode(Inputs[0], components[0], components[1], components[2], components[3]));
                        break;

                    default:
                        outError = "Unexpected number of swizzle values for '" + Name + "': " + std::to_string(numbSwizzles.GetValue());
                        assert(false);
                        return false;
                }
            }
            else
            {
                outError = "Unknown node type '" + tryType.GetValue() + "'";
                return false;
            }
        }
        catch (int exc)
        {
            outError = "Error creating node '" + Name + "' of type '" + tryType.GetValue() + "': " + Node->GetError();
            return false;
        }


        #pragma endregion


        if (NodeMap.find(Name) != NodeMap.end())
        {
            outError = "A DataNode with the name '" + Name + "' already exists!";
            return false;
        }
        NodeMap[Name] = Node;

        return true;
    }
    virtual bool WriteData(DataWriter * data, std::string & outError) const override
    {
        if (!data->WriteString(Name, "name", outError))
        {
            outError = "Error writing this node's name, '" + Name + "': " + outError;
            return false;
        }

        if (!data->WriteString(TypeName, "type", outError))
        {
            outError = "Error writing this node's ('" + Name +
                            "') 'type' property, which has a value of '" + TypeName + "': " + outError;
        }


        //Convert the inputs to the corresponding data structure.
        std::vector<DataNodeInput> inputs;
        for (unsigned int i = 0; i < Inputs.size(); ++i)
        {
            if (Inputs[i].IsConstant())
            {
                inputs.insert(inputs.end(), DataNodeInput(Inputs[i].GetConstantValue()));
            }
            else
            {
                //Search for the name of the input.
                const DataNode * dnp = Inputs[i].GetDataNodeValue().get();
                bool foundIt = false;
                for (auto element = NodeMap.begin(); element != NodeMap.end(); ++element)
                {
                    if (dnp == element->second.get())
                    {
                        foundIt = true;
                        inputs.insert(inputs.end(), DataNodeInput(element->first, Inputs[i].GetDataNodeLineIndex()));
                        break;
                    }
                }
                if (!foundIt)
                {
                    outError = "Couldn't find the name of the data node in the static 'NodeMap' structure";
                    return false;
                }
            }
        }


        #pragma region Output data based on the type of node this is


        if (!data->WriteUInt(inputs.size(), "numbInputs", outError))
        {
            outError = "Error writing the number of inputs for '" + TypeName + "' node named '" + Name + "': " + outError;
            return false;
        }

        //Checks whether this node's type name is equal to the given string in "DataNodeNames".
        #define TYPE_IS(str) (TypeName.compare(DataNodeNames:: ## str) == 0)

        #define WRITE_INPUT_STRUCTURE(index, varName) \
            if (!data->WriteDataStructure(inputs[index], varName, outError)) \
            { \
                outError = "Error writing input value '" + std::string(varName) + "' for " + TypeName + " node '" + Name + "': " + outError; \
                return false; \
            }

        //Writes out inputs for a node with 1 input.
        #define WRITE_SIMPLE_1NODE(nameVar, varStr) \
            else if (TYPE_IS(nameVar)) \
            { \
                WRITE_INPUT_STRUCTURE(0, varStr) \
            }
        //Writes out inputs for a node with 2 inputs.
        #define WRITE_SIMPLE_2NODE(nameVar, var1Str, var2Str) \
            else if (TYPE_IS(nameVar)) \
            { \
                WRITE_INPUT_STRUCTURE(0, var1Str) \
                WRITE_INPUT_STRUCTURE(1, var2Str) \
            }
        //Writes out inputs for a node with 3 inputs.
        #define WRITE_SIMPLE_3NODE(nameVar, var1Str, var2Str, var3Str) \
            else if (TYPE_IS(nameVar)) \
            { \
                WRITE_INPUT_STRUCTURE(0, var1Str) \
                WRITE_INPUT_STRUCTURE(1, var2Str) \
                WRITE_INPUT_STRUCTURE(3, var3Str) \
            }
                    

        if (TYPE_IS(Name_Add))
        {
            for (unsigned int i = 0; i < inputs.size(); ++i)
            {
                WRITE_INPUT_STRUCTURE(i, std::string("input") + std::to_string(i))
            }
        }
        else if (TYPE_IS(Name_Subtract))
        {
            WRITE_INPUT_STRUCTURE(0, "baseValue")
            for (unsigned int i = 1; i < inputs.size(); ++i)
            {
                WRITE_INPUT_STRUCTURE(i, std::string("subValue") + std::to_string(i))
            }
        }
        else if (TYPE_IS(Name_Multiply))
        {
            for (unsigned int i = 0; i < inputs.size(); ++i)
            {
                WRITE_INPUT_STRUCTURE(i, std::string("multiplyVal") + std::to_string(i))
            }
        }
        else if (TYPE_IS(Name_Divide))
        {
            WRITE_INPUT_STRUCTURE(0, "baseValue")
            for (unsigned int i = 1; i < inputs.size(); ++i)
            {
                WRITE_INPUT_STRUCTURE(i, std::string("divideBy") + std::to_string(i))
            }
        }
        else if (TYPE_IS(Name_Combine))
        {
            for (unsigned int i = 0; i < inputs.size(); ++i)
            {
                WRITE_INPUT_STRUCTURE(i, std::string("input") + std::to_string(i))
            }
        }
        WRITE_SIMPLE_1NODE(Name_Sign, "input")
        WRITE_SIMPLE_1NODE(Name_Ceil, "input")
        WRITE_SIMPLE_1NODE(Name_Floor, "input")
        WRITE_SIMPLE_1NODE(Name_Abs, "input")
        WRITE_SIMPLE_1NODE(Name_Sin, "input")
        WRITE_SIMPLE_1NODE(Name_Cos, "input")
        WRITE_SIMPLE_1NODE(Name_ASin, "input")
        WRITE_SIMPLE_1NODE(Name_ACos, "input")
        WRITE_SIMPLE_1NODE(Name_Normalize, "vectorIn")
        WRITE_SIMPLE_1NODE(Name_Fract, "input")
        WRITE_SIMPLE_1NODE(Name_OneMinus, "input")
        WRITE_SIMPLE_1NODE(Name_Negative, "input")
        WRITE_SIMPLE_1NODE(Name_SplitComponents, "toSplit")
        WRITE_SIMPLE_1NODE(Name_WhiteNoise, "seed")
        WRITE_SIMPLE_1NODE(Name_ObjectPosToScreen, "inputObjectPos")
        WRITE_SIMPLE_1NODE(Name_ObjectNormalToScreen, "inputObjectNormal")
        WRITE_SIMPLE_1NODE(Name_ObjectPosToWorld, "inputObjectPos")
        WRITE_SIMPLE_1NODE(Name_ObjectNormalToWorld, "inputObjectNormal")
        WRITE_SIMPLE_1NODE(Name_WorldPosToScreen, "inputWorldPos")
        WRITE_SIMPLE_1NODE(Name_WorldNormalToScreen, "inputWorldNormal")
        WRITE_SIMPLE_2NODE(Name_Max, "first", "second")
        WRITE_SIMPLE_2NODE(Name_Min, "first", "second")
        WRITE_SIMPLE_2NODE(Name_Modulo, "numerator", "denominator")
        WRITE_SIMPLE_2NODE(Name_Pow, "base", "exponent")
        WRITE_SIMPLE_2NODE(Name_Reflect, "toReflect", "reflectionNormal")
        WRITE_SIMPLE_2NODE(Name_Log, "inputValue", "base")
        WRITE_SIMPLE_2NODE(Name_Cross, "first", "second")
        WRITE_SIMPLE_2NODE(Name_Dot, "first", "second")
        WRITE_SIMPLE_2NODE(Name_Distance, "first", "second")
        WRITE_SIMPLE_3NODE(Name_Clamp, "min", "max", "inputVal")
        WRITE_SIMPLE_3NODE(Name_GetLerpComponent, "min", "max", "value")
        WRITE_SIMPLE_3NODE(Name_Lerp, "min", "max", "lerpVal")
        WRITE_SIMPLE_3NODE(Name_SmoothLerp, "min", "max", "lerpVal")
        WRITE_SIMPLE_3NODE(Name_SuperSmoothLerp, "min", "max", "lerpVal")
        WRITE_SIMPLE_3NODE(Name_Refract, "toRefract", "refractionNormal", "indexOfRefraction")
        WRITE_SIMPLE_3NODE(Name_RotateAroundAxis, "toRotate", "rotationAxis", "rotationAmount")
        else if (TYPE_IS(Name_Remap))
        {
            WRITE_INPUT_STRUCTURE(0, "valueToMap")
            WRITE_INPUT_STRUCTURE(1, "srcMin")
            WRITE_INPUT_STRUCTURE(2, "srcMax")
            WRITE_INPUT_STRUCTURE(3, "destMin")
            WRITE_INPUT_STRUCTURE(4, "destMax")
        }
        else if (TYPE_IS(Name_SurfaceLightCalc))
        {
            WRITE_INPUT_STRUCTURE(0, "surfaceWorldPos")
            WRITE_INPUT_STRUCTURE(1, "surfaceWorldNormal")
            WRITE_INPUT_STRUCTURE(2, "lightDir")
            WRITE_INPUT_STRUCTURE(3, "ambient")
            WRITE_INPUT_STRUCTURE(4, "diffuse")
            WRITE_INPUT_STRUCTURE(5, "specular")
            WRITE_INPUT_STRUCTURE(5, "specIntensity")
            WRITE_INPUT_STRUCTURE(5, "camWorldPos")
        }
        else if (TYPE_IS(Name_CustomExpression))
        {
            for (unsigned int i = 0; i < inputs.size(); ++i)
            {
                WRITE_INPUT_STRUCTURE(i, std::string("exprInput") + std::to_string(i))
            }

            CustomExpressionNode * nde = (CustomExpressionNode*)Node.get();
            if (!data->WriteString(nde->GetExpression(), "expression", outError))
            {
                outError = "Error writing node '" + Name + "'s custom expression '" + nde->GetExpression() + "': " + outError;
                return false;
            }
            if (!data->WriteUInt(Node->GetOutputs()[0], "outputSize", outError))
            {
                outError = "Error writing node's output size (" + std::to_string(Node->GetOutputs()[0]) + "): " + outError;
                return false;
            }
        }
        else if (TYPE_IS(Name_Parameter))
        {
            ParamNode* nde = (ParamNode*)Node.get();
            if (!data->WriteString(nde->GetParamName(), "paramName", outError))
            {
                outError = "Error writing param node '" + Name + "'s parameter name '" + nde->GetParamName() + "': " + outError;
                return false;
            }
            if (!data->WriteUInt(nde->GetParamSize(), "paramSize", outError))
            {
                outError = "Error writing param node '" + Name + "'s parameter size '" + std::to_string(nde->GetParamSize()) + "': " + outError;
                return false;
            }
        }
        else if (TYPE_IS(Name_Texture2D))
        {
            TextureSample2DNode* nde = (TextureSample2DNode*)Node.get();
            if (!data->WriteString(nde->GetSamplerUniformName(), "texName", outError))
            {
                outError = "Error writing tex2D node '" + Name + "'s texture param name '" + nde->GetSamplerUniformName() + "': " + outError;
                return false;
            }
        }
        else if (TYPE_IS(Name_Texture3D))
        {
            TextureSample3DNode* nde = (TextureSample3DNode*)Node.get();
            if (!data->WriteString(nde->GetSamplerUniformName(), "texName", outError))
            {
                outError = "Error writing tex3D node '" + Name + "'s texture param name '" + nde->GetSamplerUniformName() + "': " + outError;
                return false;
            }
        }
        else if (TYPE_IS(Name_TextureCubemap))
        {
            TextureSampleCubemapNode* nde = (TextureSampleCubemapNode*)Node.get();
            if (!data->WriteString(nde->GetSamplerUniformName(), "texName", outError))
            {
                outError = "Error writing texCube node '" + Name + "'s texture param name '" + nde->GetSamplerUniformName() + "': " + outError;
                return false;
            }
        }
        else if (TYPE_IS(Name_Swizzle))
        {
            WRITE_INPUT_STRUCTURE(0, "toBeSwizzled")

            SwizzleNode * nde = (SwizzleNode*)Node.get();
            if (!data->WriteUInt(nde->GetNumbComponents(), "numbComponents", outError))
            {
                outError = "Error writing number of swizzle components (" + std::to_string(nde->GetNumbComponents()) + ") for swizzle node '" + Name + "': " + outError;
                return false;
            }

            std::string swizzleStr;
            for (unsigned int i = 0; i < nde->GetNumbComponents(); ++i)
            {
                switch (nde->GetComponent(i))
                {
                    case SwizzleNode::C_X:
                        swizzleStr = "x";
                        break;
                    case SwizzleNode::C_Y:
                        swizzleStr = "y";
                        break;
                    case SwizzleNode::C_Z:
                        swizzleStr = "z";
                        break;
                    case SwizzleNode::C_W:
                        swizzleStr = "w";
                        break;

                    default:
                        outError = "Unknown swizzle component. Index: " + std::to_string(i) + "; value: '" + std::to_string(nde->GetComponent(i)) + "'";
                        return false;
                }
                if (!data->WriteString(swizzleStr, "swizzleComponent" + std::to_string(i + 1), outError))
                {
                    outError = "Error writing swizzle component #" + std::to_string(i + 1) + ", '" + swizzleStr + "': " + outError;
                    return false;
                }
            }
        }
        else
        {
            outError = "This node, '" + Name + "', is an unknown type '" + TypeName + "'";
            return false;
        }


        #pragma endregion
    }
};

static std::unordered_map<std::string, DataNodePtr> NodeMap;



bool DataNodeSerialization::ReadData(DataReader * data, std::string & outError)
{

}
bool DataNodeSerialization::WriteData(DataWriter * data, std::string & outError) const
{

}