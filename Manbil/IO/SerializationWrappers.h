#pragma once

#include "DataSerialization.h"

#include "../Math/LowerMath.hpp"
#include "../Rendering/Basic Rendering/GLVectors.h"
#include "../Rendering/Basic Rendering/RenderIOAttributes.h"
#include "../Rendering/Basic Rendering/UniformCollections.h"
#include "../Rendering/Textures/MTexture2D.h"
#include "../Rendering/Textures/MTexture3D.h"
#include "../Rendering/Textures/MTextureCubemap.h"


//Provides ISerializable wrappers for many simple data structures.
//Use the following convention to serialize these kinds of data structures:
/*
    Vector2f myVec;
    Matrix4f myMat;
    ...
    writer->WriteDataStructure(Vector2f_Writable(myVec), "MyVec");
    writer->WriteDataStructure(Matrix4f_Writable(myMat), "My Matrix");
    ...
    reader->ReadDataStructure(Vector2f_Readable(myVec));
    reader->ReadDataStructure(Matrix4f_Readable(myMat));
*/



//In the interest of efficiency, there's quite a bit of macro use in here.

#define MAKE_SERIALIZABLE(typeName) MAKE_SERIALIZABLE_FULL(typeName, typeName)

#define MAKE_SERIALIZABLE_FULL(typeName, className) \
    struct className ## _Writable : public IWritable \
    { \
    public: \
        const typeName& Value; \
        explicit className ## _Writable(const typeName& value) : Value(value) { } \
        virtual void WriteData(DataWriter* data) const override; \
    private: \
        className ## _Writable(const className ## _Writable& cpy) = delete; \
    }; \
    struct className ## _Readable : public IReadable \
    { \
    public: \
        typeName& Value; \
        explicit className ## _Readable(typeName& value) : Value(value) { } \
        virtual void ReadData(DataReader* data) override; \
    private: \
        className ## _Readable(const className ## _Readable& cpy) = delete; \
    };


MAKE_SERIALIZABLE(Vector2f)
MAKE_SERIALIZABLE(Vector3f)
MAKE_SERIALIZABLE(Vector4f)
MAKE_SERIALIZABLE(Vector2i)
MAKE_SERIALIZABLE(Vector3i)
MAKE_SERIALIZABLE(Vector4i)
MAKE_SERIALIZABLE(Vector2u)
MAKE_SERIALIZABLE(Vector3u)
MAKE_SERIALIZABLE(Vector4u)
MAKE_SERIALIZABLE(Vector2b)
MAKE_SERIALIZABLE(Vector3b)
MAKE_SERIALIZABLE(Vector4b)

MAKE_SERIALIZABLE(Matrix4f)
MAKE_SERIALIZABLE(Quaternion)
MAKE_SERIALIZABLE(Interval)

MAKE_SERIALIZABLE(VectorF)
MAKE_SERIALIZABLE(VectorI)

MAKE_SERIALIZABLE_FULL(RenderIOAttributes::Attribute, RenderIOAttributes_Attribute)

MAKE_SERIALIZABLE(RenderIOAttributes)


MAKE_SERIALIZABLE(UniformValueArrayF)
MAKE_SERIALIZABLE(UniformValueArrayI)

MAKE_SERIALIZABLE_FULL(SubroutineDefinition::Parameter, SubroutineDefinition_Parameter)
MAKE_SERIALIZABLE(SubroutineDefinition)
MAKE_SERIALIZABLE(UniformValueSubroutine)


//Uniforms DO NOT serialize their current value or the location of the uniform in the shader.
MAKE_SERIALIZABLE(Uniform)


MAKE_SERIALIZABLE(TextureSampleSettings2D);
MAKE_SERIALIZABLE(TextureSampleSettings3D);

MAKE_SERIALIZABLE(MTexture2D)
MAKE_SERIALIZABLE(MTexture3D)
MAKE_SERIALIZABLE(MTextureCubemap)
