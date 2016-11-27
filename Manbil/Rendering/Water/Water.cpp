#include "Water.h"

#include "../Basic Rendering/OpenGLIncludes.h"
#include "../../Math/Higher Math/Terrain.h"
#include "../Basic Rendering/Material.h"


RenderIOAttributes WaterVertex::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                              RenderIOAttributes::Attribute(2, false, "vIn_TexCoord"));
}


void CreateWaterMesh(unsigned int size, Mesh& outM)
{
    //Just create a flat terrain and let it do the math.
    Terrain terr(Vector2u(size, size));
	std::vector<WaterVertex> verts;
	std::vector<unsigned int> indices;
	{
		Array2D<float> heightmap(size, size, 0.0f);
		terr.SetHeightmap(heightmap);
		terr.GenerateTrianglesFull<WaterVertex>(
			verts, indices,
			[](WaterVertex& v) { return &v.Pos; },
			[](WaterVertex& v) { return &v.TexCoord; });
	}

    //Convert the terrain vertices into water vertices.
	float invSize = 1.0f / (float)size;
    for (unsigned int i = 0; i < verts.size(); ++i)
    {
        verts[i].Pos = Vector3f(verts[i].Pos.x * invSize,
								verts[i].Pos.y * invSize,
								verts[i].Pos.z);
    }

    //Upload the mesh data into vertex/index buffers.
	outM.SetVertexData(verts, Mesh::BUF_STATIC, WaterVertex::GetVertexAttributes());
	outM.SetIndexData(indices, Mesh::BUF_STATIC);
}

Water::Water(unsigned int size, unsigned int maxCircularRipples, unsigned int maxDirectionalRipples)
    : maxRipples_Circular(maxCircularRipples), maxRipples_Directional(maxDirectionalRipples),
      waterMat(nullptr), MyMesh(false, PrimitiveTypes::PT_TRIANGLE_LIST)
{
    //Create mesh.
    CreateWaterMesh(size, MyMesh);

    //Set up circular ripples.
	ripplesUniformCircular_dp_tsc_h_p.resize(maxRipples_Circular);
	ripplesUniformCircular_sXY_sp.resize(maxRipples_Circular);
	for (unsigned int rippleI = 0; rippleI < maxRipples_Circular; ++rippleI)
	{
		ripplesUniformCircular_dp_tsc_h_p[rippleI].x = 0.001f;
		ripplesUniformCircular_dp_tsc_h_p[rippleI].w = 999.0f;
		ripplesUniformCircular_sXY_sp[rippleI].z = 0.001f;
	}
	Params[WaterNode::UniformName_DP_TSC_H_P] =
		Uniform(WaterNode::UniformName_DP_TSC_H_P, UT_VALUE_F_ARRAY);
	Params[WaterNode::UniformName_sXY_SP] =
		Uniform(WaterNode::UniformName_sXY_SP, UT_VALUE_F_ARRAY);
	Params[WaterNode::UniformName_DP_TSC_H_P].FloatArray() =
		UniformValueArrayF(&ripplesUniformCircular_dp_tsc_h_p[0].x, maxRipples_Circular, 4);
	Params[WaterNode::UniformName_sXY_SP].FloatArray() =
		UniformValueArrayF(&ripplesUniformCircular_sXY_sp[0].x, maxRipples_Circular, 3);
	
	//Set up directional ripples.
	ripplesUniformDirectional_d_a_p.resize(maxRipples_Directional);
	ripplesUniformDirectional_tsc.resize(maxRipples_Directional);
	for (unsigned int rippleI = 0; rippleI < maxRipples_Directional; ++rippleI)
	{
		ripplesUniformDirectional_d_a_p[rippleI] = Vector4f(0.001f, 0.0f, 0.0f, 9999.0f);
		ripplesUniformDirectional_tsc[rippleI] = 0.0f;
	}
	Params[WaterNode::UniformName_D_A_P] = Uniform(WaterNode::UniformName_D_A_P, UT_VALUE_F_ARRAY);
	Params[WaterNode::UniformName_TSC] = Uniform(WaterNode::UniformName_TSC, UT_VALUE_F_ARRAY);
	Params[WaterNode::UniformName_D_A_P].FloatArray() =
		UniformValueArrayF(&ripplesUniformDirectional_d_a_p[0][0], maxRipples_Directional, 4);
	Params[WaterNode::UniformName_TSC].FloatArray() =
		UniformValueArrayF(&ripplesUniformDirectional_tsc[0], maxRipples_Directional, 1);
}

Water::RippleID Water::AddRipple(CircularRipple newRipple, RippleID removeIfFull)
{
	assert(maxRipples_Circular > 0);

	//Find the first unused ID.
	RippleID id = 0;
	while (ripples_Circular.find(id) != ripples_Circular.end())
		id += 1;
	//If we have too many ripples already, either quit or remove one of them.
	if (id >= maxRipples_Circular)
	{
		if (removeIfFull == RIPPLEID_INVALID)
			return RIPPLEID_INVALID;
		else
			id = removeIfFull;
	}

	//Make sure uniform values are valid.
	if (newRipple.DropoffPoint <= 0.0f)
		newRipple.DropoffPoint = 9999.0f;
	if (newRipple.Period <= 0.0f)
		newRipple.Period = 0.00001f;
	if (newRipple.Speed <= 0.0f)
		newRipple.Speed = 1.0f;

	ripples_Circular[id] = newRipple;

	return id;
}
Water::RippleID Water::AddRipple(DirectionalRipple newRipple, RippleID removeIfFull)
{
	assert(maxRipples_Directional > 0);

	//Find the first unused ID.
	RippleID id = 0;
	while (ripples_Directional.find(id) != ripples_Directional.end())
		id += 1;
	//If we have too many ripples already, either quit or remove one of them.
	if (id >= maxRipples_Directional)
	{
		if (removeIfFull == RIPPLEID_INVALID)
			return RIPPLEID_INVALID;
		else
			id = removeIfFull;
	}

	//Make sure uniform values are valid.
	if (newRipple.Flow == Vector2f())
		newRipple.Flow = Vector2f(0.001f, 0.0f);
	if (newRipple.Period <= 0.0f)
		newRipple.Period = 0.001f;

	ripples_Directional[id] = newRipple;

	return id;
}

void Water::ChangeRipple(RippleID toChange, const DirectionalRipple& changedRipple)
{
	assert(ripples_Directional.find(toChange) != ripples_Directional.end());
	ripples_Directional[toChange] = changedRipple;
}
void Water::ChangeRipple(RippleID toChange, const CircularRipple& changedRipple)
{
	assert(ripples_Circular.find(toChange) != ripples_Circular.end());
	ripples_Circular[toChange] = changedRipple;
}

void Water::RemoveRipple_Circular(RippleID toRemove)
{
	assert(ripples_Circular.find(toRemove) != ripples_Circular.end());
	ripples_Circular.erase(toRemove);
}
void Water::RemoveRipple_Directional(RippleID toRemove)
{
	assert(ripples_Directional.find(toRemove) != ripples_Directional.end());
	ripples_Directional.erase(toRemove);
}

bool Water::RippleExists_Circular(RippleID ripple) const
{
	return ripples_Circular.find(ripple) != ripples_Circular.end();
}
bool Water::RippleExists_Directional(RippleID ripple) const
{
	return ripples_Directional.find(ripple) != ripples_Directional.end();
}

const Water::DirectionalRipple* Water::GetRipple_Directional(RippleID id) const
{
	auto found = ripples_Directional.find(id);
	return (found == ripples_Directional.end() ?
				nullptr :
				&found->second);
}
const Water::CircularRipple* Water::GetRipple_Circular(RippleID id) const
{
	auto found = ripples_Circular.find(id);
	return (found == ripples_Circular.end() ?
				nullptr :
				&found->second);
}

void Water::SetMaterial(Material* mat)
{
    waterMat = mat;

	//Update the uniform handles.
    if (maxRipples_Circular > 0)
    {
		Params[WaterNode::UniformName_DP_TSC_H_P].Loc =
			mat->GetUniformLoc(WaterNode::UniformName_DP_TSC_H_P);
		Params[WaterNode::UniformName_sXY_SP].Loc =
			mat->GetUniformLoc(WaterNode::UniformName_sXY_SP);
    }
    if (maxRipples_Directional > 0)
    {
		Params[WaterNode::UniformName_D_A_P].Loc =
			mat->GetUniformLoc(WaterNode::UniformName_D_A_P);
		Params[WaterNode::UniformName_TSC].Loc =
			mat->GetUniformLoc(WaterNode::UniformName_TSC);
    }
}

void Water::Update(float elapsed)
{
	//Update the "time since created" values for each ripple.

    //Keep in mind that negative time values indicate the ripple source was stopped
    //    and is fading out.

	for (auto& rippleNameAndValue : ripples_Circular)
		if (rippleNameAndValue.second.TimeSinceCreated >= 0.0f)
			rippleNameAndValue.second.TimeSinceCreated += elapsed;
		else
			rippleNameAndValue.second.TimeSinceCreated -= elapsed;

	for (auto& rippleNameAndValue : ripples_Directional)
		if (rippleNameAndValue.second.TimeSinceCreated > 0.0f)
			rippleNameAndValue.second.TimeSinceCreated += elapsed;
		else
			rippleNameAndValue.second.TimeSinceCreated -= elapsed;
}
void Water::Render(const Transform& tr, const RenderInfo& info)
{
	assert(waterMat != nullptr);

	UpdateMeshUniforms();
	waterMat->Render(MyMesh, tr, info, Params);
}

void Water::UpdateMeshUniforms(void)
{
	//Put all circular ripples into the uniform arrays.
	unsigned int i = 0;
	for (auto& rippleNameAndValue : ripples_Circular)
	{
		ripplesUniformCircular_dp_tsc_h_p[i] = Vector4f(rippleNameAndValue.second.DropoffPoint,
														rippleNameAndValue.second.TimeSinceCreated,
														rippleNameAndValue.second.Amplitude,
														rippleNameAndValue.second.Period);
		ripplesUniformCircular_sXY_sp[i] = Vector3f(rippleNameAndValue.second.Source.x,
													rippleNameAndValue.second.Source.y,
													rippleNameAndValue.second.Speed);
		i += 1;
	}
	//For every other entry in the uniform arrays, add an invisible ripple.
	for (; i < maxRipples_Circular; ++i)
	{
		ripplesUniformCircular_dp_tsc_h_p[i] = Vector4f(0.00001f, 99999.0f, 0.0f, 99999.0f);
		ripplesUniformCircular_sXY_sp[i] = Vector3f(0.0f, 0.0f, 0.0000001f);
	}
	//Finally, update the uniform values.
	Params[WaterNode::UniformName_DP_TSC_H_P].FloatArray().SetData(&ripplesUniformCircular_dp_tsc_h_p[0][0],
																   maxRipples_Circular, 4);
	Params[WaterNode::UniformName_sXY_SP].FloatArray().SetData(&ripplesUniformCircular_sXY_sp[0][0],
															   maxRipples_Circular, 3);

	//Do the same thing for directional ripples.
	i = 0;
	for (auto& rippleNameAndValue : ripples_Directional)
	{
		ripplesUniformDirectional_d_a_p[i] = Vector4f(rippleNameAndValue.second.Flow.x,
													  rippleNameAndValue.second.Flow.y,
													  rippleNameAndValue.second.Amplitude,
													  rippleNameAndValue.second.Period);
		ripplesUniformDirectional_tsc[i] = ripples_Directional[i].TimeSinceCreated;

		i += 1;
	}
	for (; i < maxRipples_Directional; ++i)
	{
		ripplesUniformDirectional_d_a_p[i] = Vector4f(0.00001f, 0.0f, 0.0f, 999999.0f);
		ripplesUniformDirectional_tsc[i] = 999999.0f;
	}
    Params[WaterNode::UniformName_D_A_P].FloatArray().SetData(&ripplesUniformDirectional_d_a_p[0][0],
															  maxRipples_Directional, 4);
    Params[WaterNode::UniformName_TSC].FloatArray().SetData(&ripplesUniformDirectional_tsc[0],
															maxRipples_Directional, 1);
}