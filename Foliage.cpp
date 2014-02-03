#include "Foliage.h"

#include <iostream>

#include <memory>
#include <assert.h>
#include "RenderDataHandler.h"
#include "Materials.h"
#include "Mesh.h"



//Creates a single foliage object with 8 vertices and 12 indices.
void CreateVertices(Vertex * vertexStart, unsigned int * indexStart, int indexOffset, Vector3f pos, Vector2f scale)
{
    //The foliage encodes x and y wave amount in the red and green values of the vertices.
    //The blue value of the vertex is a pseudo-random value to make each foliage piece wave a little differently.
    //The alpha will be -1 for a foliage quad along the x axis, and +1 for a foliage quad along the y axis.
    //The normal will be used to give the base position.

	float halfScale = scale.x * 0.5f;
    FastRand fr(indexOffset + 12345);
    float rand1 = fr.GetZeroToOne();
    fr.GetRandInt();
    fr.GetRandInt();
    fr.GetRandInt();
    float rand2 = fr.GetZeroToOne();

	vertexStart[0] = Vertex(Vector3f(pos.x - halfScale, pos.y, pos.z),           Vector2f(0.0f, 0.0f), Vector4f(),                        pos);
	vertexStart[1] = Vertex(Vector3f(pos.x + halfScale, pos.y, pos.z),           Vector2f(1.0f, 0.0f), Vector4f(),                        pos);
    vertexStart[2] = Vertex(Vector3f(pos.x - halfScale, pos.y, pos.z + scale.y), Vector2f(0.0f, 1.0f), Vector4f(1.0f, 0.0f, rand1, 1.0f), pos);
    vertexStart[3] = Vertex(Vector3f(pos.x + halfScale, pos.y, pos.z + scale.y), Vector2f(1.0f, 1.0f), Vector4f(1.0f, 0.0f, rand1, 1.0f), pos);


	indexStart[0] = indexOffset;
	indexStart[1] = indexOffset + 2;
	indexStart[2] = indexOffset + 1;
	indexStart[3] = indexOffset + 1;
	indexStart[4] = indexOffset + 3;
	indexStart[5] = indexOffset + 2;

	vertexStart[4] = Vertex(Vector3f(pos.x, pos.y - halfScale, pos.z),           Vector2f(0.0f, 0.0f), Vector4f(),                        pos);
	vertexStart[5] = Vertex(Vector3f(pos.x, pos.y + halfScale, pos.z),           Vector2f(1.0f, 0.0f), Vector4f(),                        pos);
    vertexStart[6] = Vertex(Vector3f(pos.x, pos.y - halfScale, pos.z + scale.y), Vector2f(0.0f, 1.0f), Vector4f(0.0f, 1.0f, rand2, -1.0f), pos);
    vertexStart[7] = Vertex(Vector3f(pos.x, pos.y + halfScale, pos.z + scale.y), Vector2f(1.0f, 1.0f), Vector4f(0.0f, 1.0f, rand2, -1.0f), pos);
	
	indexStart[6] = indexOffset + 4;
	indexStart[7] = indexOffset + 6;
	indexStart[8] = indexOffset + 5;
	indexStart[9] = indexOffset + 5;
	indexStart[10] = indexOffset + 7;
	indexStart[11] = indexOffset + 6;
}

RenderingPass Foliage::GetFoliageRenderer(void)
{
    //TODO: Test out the leaning thing.
    return RenderingPass(
        std::string("uniform float waveSpeed;\n\
					 uniform float waveScale;\n\
                     uniform float leanMaxDist;\n\
					 \n\
					 void main()\n\
					 {\n\
				 	    float waveOffsetInner = u_elapsed_seconds * waveSpeed;\n\
                        waveOffsetInner += (in_col.z * 6117.34);\n\
				 	    vec2 waveOffset = waveScale * sin(in_col.z + (vec2(waveOffsetInner) * in_col.xy));\n\
                        \n\
                        vec3 finalObjPos = in_pos + vec3(waveOffset, 0.0);\n\
                        //Lean away from the player.\n\
                        float dist = worldTo4DScreen(finalObjPos).z;\n\
                        float distLerp = clamp(1.0 - dist / leanMaxDist, 0.0, 1.0);\n\
                        float rotAmount = -0.5 * 3.14159;\n\
                        vec4 rotationQ = getQuaternionRotation(u_cam_sideways * (in_col.x + in_col.y), distLerp * rotAmount);\n\
                        finalObjPos = in_normal + applyQuaternionRotation(finalObjPos - in_normal, rotationQ);\n\
                        \n\
                        vec4 pos4D = worldTo4DScreen(finalObjPos);\n\
				 	    out_tex = in_tex;\n\
                        \n\
				 	    gl_Position = pos4D;\n\
                        \n\
                     }"),
        Materials::UnlitTexture.FragmentShader,
        RenderingState(true, true));
}


Foliage::Foliage(std::vector<Vector3f> vertexBasePoses, Vector2f foliageScale, Material * foliageMat)
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
    Mat->AddUniform("brightness");
    //Materials::LitTexture_GetUniforms(*Mat);
    //Materials::LitTexture_DirectionalLight light;

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