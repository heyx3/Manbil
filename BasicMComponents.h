#pragma once

#include "MGameForwardDeclarations.h"

#include "Camera.h"
#include "TransformObject.h"
#include "ThreeDShapes.h"
#include "MGameMaterialManager.h"
#include <memory>
#include <assert.h>

class TransformComponent;

//TODO: Add a LightComponent.

class MComponent
{
public:

	//The different kinds of MComponents.
	enum ComponentTypes
	{
		COMPONENT_OTHER,

		COMPONENT_COLLISION,
		COMPONENT_CAMERA,
		COMPONENT_2DRENDERER,
		COMPONENT_3DRENDERER,

		//Transform components should never be manually created! Every MObject has exactly one of them -- created on MObject instantiation.
		COMPONENT_TRANSFORM,
	};
	virtual ComponentTypes VGetThisType(void) const { return ComponentTypes::COMPONENT_OTHER; }

	
	MComponent(MObject & _owner, unsigned int _id, TransformComponent * transform) : Transform(transform), owner(_owner), id(_id) { }
	MComponent(MComponent & cpy) : owner(cpy.owner), Transform(cpy.Transform) { assert(false); }
	void operator=(MComponent & cpy) { assert(false); }

	//Returns whether or not the message was successfully sent (i.e. whether or not it was a valid message).
	virtual bool ReceiveMessage(int message, void * pMessageData) { return false; }

	MObject & GetOwner(void) const { return owner; }
	unsigned int GetID(void) const { return id; }

	virtual void VUpdate(void) { }
	//Takes in a boolean indicating whether or not this is being called because its owner MObject is being destroyed.
	virtual void VOnKill(bool isMObjectDying) { }

protected:

	TransformComponent * const Transform;

private:

	MObject & owner;
	unsigned int id;
};

//Adds transformation data.
class TransformComponent final : public MComponent, public TransformObject
{
public:

	TransformComponent(MObject & owner, unsigned int id, Vector3f pos) : MComponent(owner, id, this), TransformObject(pos) { }

	virtual ComponentTypes VGetThisType(void) const override { return ComponentTypes::COMPONENT_TRANSFORM; }
};

//Adds collision data.
class CollisionComponent : public MComponent
{
public:

	CollisionComponent(MObject & owner, unsigned int id, TransformComponent * transform, std::shared_ptr<Shape> colShape) : MComponent(owner, id, transform), ColShape(colShape) { }

	bool Collides(const CollisionComponent & other) const { return ColShape->TouchingShape(*other.ColShape); }

	void ChangeColShape(std::shared_ptr<Shape> newShape) { ColShape.swap(newShape); }

private:

	std::shared_ptr<Shape> ColShape;
};

class CameraComponent;
struct MCameraInfo
{
	CameraComponent & Camera;
	Matrix4f & WorldMat, ViewMat, ProjMat,
			   VPMat, WVPMat;
	MCameraInfo(CameraComponent & camera, Matrix4f & worldMat, Matrix4f & viewMat, Matrix4f & projMat)
		: Camera(camera), WorldMat(worldMat), ViewMat(viewMat), ProjMat(projMat), VPMat(Matrix4f::Multiply(projMat, viewMat)), WVPMat() { }
};
//Adds camera functionality.
class CameraComponent : public MComponent, public Camera
{
public:

	//Different allowable messages.
	enum Messages
	{
		MESSAGE_ADD_RENDER_LAYERS = 0,
		MESSAGE_REMOVE_RENDER_LAYERS,
		MESSAGE_CAN_RENDER_ANY_RENDER_LAYERS,
		MESSAGE_CAN_RENDER_ALL_RENDER_LAYERS,
	};
	//Different message argument types.
	typedef unsigned int MessageArgs_AddOrRemoveRenderLayer;
	struct MessageArgs_CanComponentRenderLayers
	{
		unsigned int LayersToCheckFor;
		bool OutResult;
		MessageArgs_CanComponentRenderLayers(unsigned int layersToCheckFor) : LayersToCheckFor(layersToCheckFor), OutResult(false) { }
	};

	CameraComponent(MObject & owner, unsigned int id, TransformComponent * transform) : MComponent(owner, id, transform), Camera(transform->GetPosition()) { }

	virtual bool ReceiveMessage(int message, void* pMsgData) override;

	void AddRenderLayers(unsigned int layers) { layersToRender |= layers; }
	void RemoveRenderLayers(unsigned int layers) { layersToRender &= (~layers); }
	bool CanRenderAllLayers(unsigned int layers) const { return (layersToRender & layers) >= layers; }
	bool CanRenderAnyLayers(unsigned int layers) const { return (layersToRender & layers) > 0; }

	virtual ComponentTypes VGetThisType(void) const override { return ComponentTypes::COMPONENT_CAMERA; }

	virtual void VUpdate(void) override { SetPosition(Transform->GetPosition()); }
	
	//2D Screen-space (-1 to 1 on both axes) coordinates for the location of this camera's view on the screen.
	Box2D ScreenSpaceArea;
	//Gets the post-process material to be applied to this camera's viewport.
	unsigned int Get3DPostProcessMaterial(void) const { return ppmat3D; }
	unsigned int Get2DPostProcessEffect(void) const { return ppmat2D; }

private:

	unsigned int layersToRender;
	unsigned int ppmat3D, ppmat2D;
};

//Adds rendering capabilities.
class RenderableComponent : public MComponent
{
public:

	enum Messages
	{
		MESSAGE_RENDER_GAME = 0,
		MESSAGE_ADD_RENDER_LAYERS,
		MESSAGE_REMOVE_RENDER_LAYERS,
		MESSAGE_GET_IS_ON_ANY_RENDER_LAYERS,
	};
	//The data to be passed into the "render game" message.
	typedef MCameraInfo MessageArgs_RenderGame;
	typedef unsigned int MessageArgs_AddOrRemoveRenderLayer;
	struct MessageArgs_IsComponentOnAnyRenderLayers
	{
		unsigned int LayersToCheckFor;
		bool OutResult;
		MessageArgs_IsComponentOnAnyRenderLayers(unsigned int layersToCheckFor) : LayersToCheckFor(layersToCheckFor), OutResult(false) { }
	};

	RenderableComponent(MObject & owner, unsigned int compID, TransformComponent * transform) : renderLayers(1), MComponent(owner, compID, transform) { }

	virtual bool ReceiveMessage(int message, void * pMsgData) override;
	
	void AddRenderLayers(unsigned int layers) { renderLayers |= layers; }
	void RemoveRenderLayers(unsigned int layers) { renderLayers &= (~layers); }
	bool IsOnAnyRenderLayers(unsigned int layers) const { return (renderLayers & layers) > 0; }

	virtual ComponentTypes VGetThisType(void) const override { return ComponentTypes::COMPONENT_OTHER; }

	void Render(MCameraInfo & camInf) { if (!camInf.Camera.CanRenderAnyLayers(renderLayers)) return; VRender(camInf); }

protected:

	virtual void VRender(MCameraInfo & camInf) = 0;

private:

	unsigned int renderLayers;
};
class Renderable2DComponent : public RenderableComponent
{
public:

	Renderable2DComponent(MObject & owner, unsigned int compID, TransformComponent * transform) : RenderableComponent(owner, compID, transform) { }

	virtual ComponentTypes VGetThisType(void) const final override { return ComponentTypes::COMPONENT_2DRENDERER; }
};
class Renderable3DComponent : public RenderableComponent
{
public:
	
	Renderable3DComponent(MObject & owner, unsigned int compID, TransformComponent * transform) : RenderableComponent(owner, compID, transform) { }

	virtual ComponentTypes VGetThisType(void) const final override { return ComponentTypes::COMPONENT_3DRENDERER; }
};