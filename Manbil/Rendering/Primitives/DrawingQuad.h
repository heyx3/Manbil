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
    static DrawingQuad* GetInstance(void) { assert(instance != 0); return instance; }

    //Gets the vertex attributes that this quad uses.
    static RenderIOAttributes GetVertexInputData(void) { return VertexPosUV::GetVertexAttributes(); }
    

    Mesh& GetMesh(void) { return mesh; }
    const Mesh& GetMesh(void) const { return mesh; }


    void Render(const Transform& transform, const RenderInfo& info,
				const Material& material, const UniformDictionary& params);


private:

    Mesh mesh;


    DrawingQuad(void) : mesh(false, PrimitiveTypes::PT_TRIANGLE_LIST) { }
    DrawingQuad(const DrawingQuad& cpy) = delete;
    DrawingQuad& operator=(const DrawingQuad& cpy) = delete;


    static DrawingQuad* instance;
};