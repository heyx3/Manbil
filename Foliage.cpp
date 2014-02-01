#include "Foliage.h"

#include <iostream>

#include <memory>
#include <assert.h>
#include "RenderDataHandler.h"
#include "Materials.h"
#include "Mesh.h"



//Creates a single foliage object with 8 vertices and 12 indices.
void CreateVertices(Vertex * vertexStart, unsigned int * indexStart, int indexOffset, Vector3f pos, float scale)
{
    //The foliage encodes x and y wave amount in the red and green values of the vertices.

	float halfScale = scale * 0.5f;

	vertexStart[0] = Vertex(Vector3f(pos.x - halfScale, pos.y, pos.z), Vector2f(0.0f, 0.0f), Vector4f(), Vector3f(0.0f, 1.0f, 0.0f));
	vertexStart[1] = Vertex(Vector3f(pos.x + halfScale, pos.y, pos.z), Vector2f(1.0f, 0.0f), Vector4f(), Vector3f(0.0f, 1.0f, 0.0f));
	vertexStart[2] = Vertex(Vector3f(pos.x - halfScale, pos.y, pos.z + scale), Vector2f(0.0f, 1.0f), Vector4f(1.0f, 0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	vertexStart[3] = Vertex(Vector3f(pos.x + halfScale, pos.y, pos.z + scale), Vector2f(1.0f, 1.0f), Vector4f(1.0f, 0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));


	indexStart[0] = indexOffset;
	indexStart[1] = indexOffset + 2;
	indexStart[2] = indexOffset + 1;
	indexStart[3] = indexOffset + 1;
	indexStart[4] = indexOffset + 3;
	indexStart[5] = indexOffset + 2;

	vertexStart[4] = Vertex(Vector3f(pos.x, pos.y - halfScale, pos.z), Vector2f(0.0f, 0.0f), Vector4f(), Vector3f(1.0f, 0.0f, 0.0f));
	vertexStart[5] = Vertex(Vector3f(pos.x, pos.y + halfScale, pos.z), Vector2f(1.0f, 0.0f), Vector4f(), Vector3f(1.0f, 0.0f, 0.0f));
	vertexStart[6] = Vertex(Vector3f(pos.x, pos.y - halfScale, pos.z + scale), Vector2f(0.0f, 1.0f), Vector4f(0.0f, 1.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f));
	vertexStart[7] = Vertex(Vector3f(pos.x, pos.y + halfScale, pos.z + scale), Vector2f(1.0f, 1.0f), Vector4f(0.0f, 1.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f));
	
	indexStart[6] = indexOffset + 4;
	indexStart[7] = indexOffset + 6;
	indexStart[8] = indexOffset + 5;
	indexStart[9] = indexOffset + 5;
	indexStart[10] = indexOffset + 7;
	indexStart[11] = indexOffset + 6;
}

RenderingPass Foliage::GetFoliageRenderer(void)
{
    return Materials::LitTexture;
    //TODO: Finish testing out the leaning thing.
    return RenderingPass(
        std::string("uniform float waveSpeed;\n\
					 uniform float waveScale;\n\
                     uniform float leanMaxDist;\n\
					 \n\
					 void main()\n\
					 {\n\
				 	    float waveOffsetInner = u_elapsed_seconds * waveSpeed;\n\
				 	    vec2 waveOffset = waveScale * sin(vec2(waveOffsetInner, waveOffsetInner) * in_col.xy);\n\
                        \n\
                        vec3 finalObjPos = in_pos + vec3(waveOffset.x, waveOffset.y, 0.0);\n\
                        vec4 pos4D = worldTo4DScreen(finalObjPos);\n\
				 	    out_tex = in_tex;\n\
                        out_normal = normalize((u_world * vec4(in_normal, 0.0)).xyz);\n\
                        \n\
                        //Lean away from the player.\n\
                        //float dist = pos4D.z;\n\
                        //float distLerp = dist / leanMaxDist;\n\
                        //float rotAmount = 0.5 * 3.14159;\n\
                        //vec4 rotationQ = getQuaternionRotation(vec3(in_col.xy, 0.0), distLerp * rotAmount);\n\
                        //pos4D = worldTo4DScreen(applyQuaternionRotation(finalObjPos, rotationQ));\n\
                        \n\
				 	    gl_Position = pos4D;\n\
                        \n\
                     }"),
        Materials::LitTexture.FragmentShader,
        RenderingState(true, true));
}


Foliage::Foliage(std::vector<Vector3f> vertexBasePoses, float foliageScale, Material * foliageMat)
    : Mat(foliageMat)
{
    //Create our own foliage material if needed.
    if (Mat == 0)
    {
        madeMaterial = true;
        Mat = new Material(GetFoliageRenderer());
        if (Mat->HasError())
        {
            errorMsg = "Error generating foliage material: ";
            errorMsg += Mat->GetErrorMessage();
            return;
        }
    }
    else
    {
        madeMaterial = false;
    }

    //Get uniforms.
    Mat->AddUniform("waveSpeed");
    Mat->AddUniform("waveScale");
    Mat->AddUniform("leanMaxDist");
    Materials::LitTexture_GetUniforms(*Mat);
    Materials::LitTexture_DirectionalLight light;
    light.Ambient = 0.2;
    light.Diffuse = 0.8;
    light.Specular = 0.0;
    light.Dir = Vector3f(1.0f, 1.0f, -1.0f).Normalized();
    light.Col = Vector3f(1.0f, 1.0f, 1.0f);
    Materials::LitTexture_SetUniforms(*Mat, light);

	//Create the vertices/indices array.
	nVertices = vertexBasePoses.size() * 8;
	nIndices = vertexBasePoses.size() * 12;
	Vertex * vertices = new Vertex[nVertices];
	unsigned int * indices = new unsigned int[nIndices];

    //Generate vertices and indices for each piece of foliage.
	int vertexOffset = 0, indexOffset = 0;
	for (unsigned int i = 0; i < vertexBasePoses.size(); ++i)
	{
		if (vertexOffset / 8 >= vertexBasePoses.size() ||
			indexOffset / 12 >= vertexBasePoses.size())
		{
			vertexOffset += 0;
		}
		CreateVertices(&vertices[vertexOffset], &indices[indexOffset],
					   vertexOffset, vertexBasePoses[i], foliageScale);

		vertexOffset += 8;
		indexOffset += 12;

        //std::cout << vertexBasePoses[i].x << ";" << vertexBasePoses[i].y << ";" << vertexBasePoses[i].z << "\n";
        //std::cout << vertexOffset << "\n";

        //if (i % 10 > 7)
        //{
        //    char dummy;
        //    std::cin >> dummy;
        //}
	}


	RenderDataHandler::CreateVertexBuffer(vbo, vertices, nVertices, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	RenderDataHandler::CreateIndexBuffer(ibo, indices, nIndices, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

	delete[] indices;
	delete[] vertices;

}
Foliage::~Foliage(void)
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
    if (madeMaterial) delete Mat;
}

bool Foliage::Render(const RenderInfo & info)
{
	VertexIndexData vid(nVertices, vbo, nIndices, ibo);
	Mesh m(PrimitiveTypes::Triangles, 1, &vid);

    PassSamplers samplers;
    m.TextureSamplers.insert(m.TextureSamplers.begin(), samplers);

	std::vector<const Mesh *> meshes;
	meshes.insert(meshes.begin(), &m);

	return Mat->Render(info, meshes);
}