#pragma once

#include "DataSerialization.h"

#include "../Math/LowerMath.hpp"
#include "../Rendering/Basic Rendering/GLVectors.h"
#include "../Rendering/Basic Rendering/RenderIOAttributes.h"
#include "../Rendering/Textures/MTexture2D.h"
#include "../Rendering/Textures/MTexture3D.h"
#include "../Rendering/Textures/MTextureCubemap.h"


//Provides ISerializable wrappers for many simple classes.


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

#define MAKE_SERIALIZABLE(typeName) MAKE_SERIALIZABLE_FULL(typeName, typeName)


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

MAKE_SERIALIZABLE(TextureSampleSettings2D);
MAKE_SERIALIZABLE(TextureSampleSettings3D);

MAKE_SERIALIZABLE(MTexture2D)
MAKE_SERIALIZABLE(MTexture3D)
MAKE_SERIALIZABLE(MTextureCubemap)