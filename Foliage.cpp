#include "Foliage.h"

#include "RenderDataHandler.h"
#include <memory>
#include <assert.h>


/*
	Used for rendering the foliage.
	Has two uniforms:
		1) "waveSpeed" (float) -- the speed of the foliage waving.
		2) "waveScale" (flaot) -- the amount of waving.
	Encodes x and y wave scale in the red and green values of the vertices.
*/
std::unique_ptr<Material> foliageMat = std::unique_ptr<Material>();
bool materialInitialized = false;

std::string InitializeMaterial(void)
{
	if (materialInitialized) return "";
	materialInitialized = true;

	foliageMat = std::unique_ptr<Material>(new Material(std::string("uniform float waveSpeed;\n\
																	 uniform float waveScale;\n\
																	 \n\
																	 void main()\n\
																	 {\n\
																		float waveOffsetInner = u_elapsed_seconds * waveSpeed;\n\
																		vec2 waveOffset = waveScale * sin(vec2(waveOffsetInner, waveOffsetInner) * in_col.xy);\n\
																		gl_Position = worldTo4DScreen(in_pos + vec3(waveOffset.x, waveOffset.y, 0.0));\n\
																		out_tex = in_tex;\n\
																	 }"),
														std::string("void main()\n\
																	 {\n\
																		out_finalCol = vec4(texture2D(u_sampler0, out_tex).xyz, 1.0);\n\
																	 }")));
	if (foliageMat->HasError()) return std::string("Error creating material: ") + foliageMat->GetErrorMessage();


	if (!foliageMat->AddUniform("waveSpeed") || !foliageMat->AddUniform("waveScale"))
	{
		return "Attempt to find 'waveSpeed' or 'waveScale' uniforms failed!";
	}

	float vals = 1.0f;
	if (!foliageMat->SetUniformF("waveSpeed", &vals, 1) ||
		!foliageMat->SetUniformF("waveScale", &vals, 1))
	{
		return "Error attempting to set 'waveSpeed' or 'waveScale' uniforms.";
	}

	return "";
}


//Creates a single foliage object with 8 vertices and 12 indices.
void CreateVertices(Vertex * vertexStart, int * indexStart, int indexOffset, Vector3f pos, float scale)
{
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
	
	indexStart[6] = indexOffset + 5;
	indexStart[7] = indexOffset + 7;
	indexStart[8] = indexOffset + 6;
	indexStart[9] = indexOffset + 6;
	indexStart[10] = indexOffset + 8;
	indexStart[11] = indexOffset + 7;
}


bool Foliage::foliageInitialized = false;

const Material& Foliage::GetFoliageMaterial(void) { return *foliageMat; }


void Foliage::StartFoliageRendering(bool enableVertexAttributes)
{
	if (enableVertexAttributes)
	{
		Vertex::EnableVertexAttributes();
	}

	foliageInitialized = true;
}
void Foliage::EndFoliageRendering(bool disableVertexAttributes)
{
	if (disableVertexAttributes)
	{
		Vertex::DisableVertexAttributes();
	}

	foliageInitialized = false;
}

void Foliage::SetFoliageTexture(BufferObjHandle texObj)
{
	foliageMat->TextureSamplers[0] = texObj;
}
void Foliage::SetFoliageTextureUnit(int unit)
{
	foliageMat->SetUniformI("u_sampler0", &unit, 1);
}


Foliage::Foliage(std::vector<Vector3f> vertexBasePoses, float foliageScale)
{
	errorMsg = InitializeMaterial();
	if (HasRenderError()) return;

	
	nVertices = vertexBasePoses.size() * 8;
	nIndices = vertexBasePoses.size() * 12;
	Vertex * vertices = new Vertex[nVertices];
	int * indices = new int[nIndices];

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
	}


	delete[] indices;
	delete[] vertices;


	RenderDataHandler::CreateVertexBuffer(vbo, vertices, nVertices, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	RenderDataHandler::CreateIndexBuffer(ibo, indices, nIndices, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
}
Foliage::~Foliage(void)
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

bool Foliage::Render(const RenderInfo & info)
{
	VertexIndexData vid(nVertices, vbo, nIndices, ibo);
	Mesh m(PrimitiveTypes::Triangles, 1, &vid);

	std::vector<const Mesh *> meshes;
	meshes.insert(meshes.begin(), &m);

	return foliageMat->Render(info, meshes);
}