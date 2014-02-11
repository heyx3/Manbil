/*#pragma once

#include "SFMLOpenGLWorld.h"
#include <unordered_map>
#include <memory>

#include "MGameForwardDeclarations.h"

#include "BasicMComponents.h"
#include "BasicMComponentFactories.h"
#include "RenderInfo.h"

typedef std::shared_ptr<MObject> MObjPtr;
typedef std::vector<MCompPtr> MCompList;


//A collection of components. Uses a bitwise mask for splitting rendering into layers.
//Do not create an object directly! Instead, use MWorld::CreateObject().
class MObject
{
public:

	MObject(MWorld & _owner);
	MObject(MObject & cpy) : owner(cpy.owner) { assert(false); }
	void operator=(MObject & cpy) { assert(false); }

	void Update(void);
	void Render3D(MCameraInfo & cam);
	void Render2D(MCameraInfo & cam);

	void BirthComponent(MComponentFactory * factory);
	//Sets the given component to be killed next Update() call.
	void KillComponent(unsigned int id) { assert(components[id]->VGetThisType() != CompTypes::COMPONENT_TRANSFORM); toDelete.insert(toDelete.end(), id); }

	void GetComponents(MComponent::ComponentTypes type, MCompList & outComponents);

	void OnDeath(void);

	TransformComponent * GetTransform(void) const { return transform; }

private:

	typedef MComponent::ComponentTypes CompTypes;
	typedef std::unordered_map<unsigned int, MCompPtr> ComponentMap;
	ComponentMap components;
	unsigned int nextID;

	TransformComponent * transform;
	std::vector<Renderable2DComponent *> renderables2D;
	std::vector<Renderable3DComponent *> renderables3D;

	std::vector<unsigned int> toDelete;

	MWorld & owner;
};

//A world full of MObjects.
class MWorld : public SFMLOpenGLWorld
{
public:

	MWorld(Vector2i windowSize) : SFMLOpenGLWorld(windowSize.x, windowSize.y) { }

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	MObjPtr CreateObject(void) { MObjPtr obj(new MObject(*this)); objects.insert(objects.end(), obj); return obj; }
	void KillObject(MObject * toKill) { objectsToDestroy.insert(objectsToDestroy.end(), toKill); }

	std::vector<MObjPtr>::const_iterator GetFirstObject(void) const { return objects.begin(); }

	float GetElapsedSecondsThisFrame(void) const { return elapsed; }

	void GetAllCameras(std::vector<MCompPtr> & outCameras) const;

	void RegisterCamera(MCompPtr cameraComponent) { cameras.insert(cameras.end(), cameraComponent); }
	void UnregisterCamera(MCompPtr cameraComponent) { cameras.erase(std::find(cameras.begin(), cameras.end(), cameraComponent)); }

private:

	std::vector<MObjPtr> objects;
	std::vector<MObject*> objectsToDestroy;

	std::vector<MCompPtr> cameras;

	float elapsed;
};*/