#pragma once

#include "../Basic Rendering/Mesh.h"
#include "../Basic Rendering/Material.h"
#include "../Basic Rendering/Vertices.h"


//A simple square mesh stretching from {-1, -1, 0} to {1, 1, 0}.
//Uses the VertexPosUV struct for the vertices.
class DrawingQuad
{
public:
 
    //Must be called before using the quad.
    //Assumes that the quad hasn't already been initialized.
    static void InitializeQuad(void);
    //Must be called after the quad is no longer needed.
    //Assumes that the quad has already been initialized.
    static void DestroyQuad(void);

    //Gets the quad. Returns 0 if it hasn't been initialized yet.
    static DrawingQuad* GetInstance(void) { return instance; }

    //Gets the vertex attributes that this quad uses.
    static RenderIOAttributes GetVertexInputData(void) { return VertexPosUV::GetVertexAttributes(); }
    

    Mesh& GetMesh(void) { return mesh; }
    const Mesh& GetMesh(void) const { return mesh; }

    Vector2f GetPos(void) const { return mesh.Transform.GetPosition().XY(); }
    float GetDepth(void) const { return mesh.Transform.GetPosition().z; }
    Vector2f GetSize(void) const { return mesh.Transform.GetScale().XY(); }
    float GetRotation(void) const { return mesh.Transform.GetRotationAngles().z; }

    void SetBounds(Vector2f min, Vector2f max);
    void SetPos(Vector2f pos);
    void IncrementPos(Vector2f increment);
    void SetSize(Vector2f size);
    void MakeSizePositive(void);
    void SetDepth(float depth);
    void SetRotation(float newRot);
    void Rotate(float amount);

    void Render(const RenderInfo& info, const UniformDictionary& params, Material& mat);


private:

    Mesh mesh;


    DrawingQuad(void) { }
    DrawingQuad(const DrawingQuad& cpy) = delete;
    DrawingQuad& operator=(const DrawingQuad& cpy) = delete;


    static DrawingQuad* instance;
};