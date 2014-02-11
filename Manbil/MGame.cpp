/*#include "MGame.h"

#include "ScreenClearer.h"
#include "RenderDataHandler.h"


MObject::MObject(MWorld & _owner) : nextID(0), owner(_owner)
{
	MCompPtr comp = TransformComponentFactory(Vector3f()).BirthComponent(*this, transform, nextID);
	components[nextID] = comp;
	transform = (TransformComponent*)&*comp;
	nextID += 1;
}
void MObject::BirthComponent(MComponentFactory * factory)
{
	MCompPtr comp = factory->BirthComponent(*this, transform, nextID);
	components[nextID] = comp;
	nextID += 1;

	//Track renderable components.
	if (comp->VGetThisType() == CompTypes::COMPONENT_2DRENDERER)
	{
		renderables2D.insert(renderables2D.end(), (Renderable2DComponent*)comp.get());
	}
	if (comp->VGetThisType() == CompTypes::COMPONENT_3DRENDERER)
	{
		renderables3D.insert(renderables3D.end(), (Renderable3DComponent*)comp.get());
	}
	//Track cameras.
	else if (comp->VGetThisType() == CompTypes::COMPONENT_CAMERA)
	{
		owner.RegisterCamera(comp);
	}
}

void MObject::OnDeath(void)
{
	for (ComponentMap::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		it->second->VOnKill(true);
	}
}

void MObject::Update(void)
{
	//Delete all dead components.
	ComponentMap::const_iterator it;
	MCompPtr mcpt;
	for (int i = 0; i < toDelete.size(); ++i)
	{
		it = components.find(toDelete[i]);
		if (it != components.end())
		{
			mcpt = it->second;
			CompTypes cpt = mcpt->VGetThisType();

			//Make sure the transform component isn't being removed!
			if (cpt != CompTypes::COMPONENT_TRANSFORM)
			{
				mcpt->VOnKill(false);

				//Check if the component being removed was important to the engine (i.e. rendering/camera).
				if (cpt == CompTypes::COMPONENT_2DRENDERER)
				{
					renderables2D.erase(std::find(renderables2D.begin(), renderables2D.end(), (Renderable2DComponent*)&*mcpt));
				}
				else if (cpt == CompTypes::COMPONENT_3DRENDERER)
				{
					renderables3D.erase(std::find(renderables3D.begin(), renderables3D.end(), (Renderable3DComponent*)&*mcpt));
				}
				else if (cpt == CompTypes::COMPONENT_CAMERA)
				{
					owner.UnregisterCamera(mcpt);
				}

				components.erase(it);
			}
		}
	}
	toDelete.clear();

	//Update components.
	for (it = components.begin(); it != components.end(); ++it)
	{
		it->second->VUpdate();
	}
}

void MObject::Render3D(MCameraInfo & cam)
{
	//Set world matrix to the matrix for this MObject.
	Matrix4f mat;
	transform->GetWorldTransform(mat);
	cam.WorldMat.SetValues(&mat);

	//Recalculate WVP.
	mat = Matrix4f::Multiply(cam.VPMat, cam.WorldMat);
	cam.WVPMat.SetValues(&mat);

	//Render.
	for (int i = 0; i < renderables3D.size(); ++i)
	{
		renderables3D[i]->Render(cam);
	}
}
void MObject::Render2D(MCameraInfo & cam)
{
	//Set world matrix to the matrix for this MObject.
	Matrix4f mat;
	transform->GetWorldTransform(mat);
	cam.WorldMat.SetValues(&mat);

	//Recalculate MVP.
	mat = Matrix4f::Multiply(cam.VPMat, cam.WorldMat);
	cam.WVPMat.SetValues(&mat);

	//Render.
	for (int i = 0; i < renderables2D.size(); ++i)
	{
		renderables2D[i]->Render(cam);
	}
}

void MWorld::GetAllCameras(std::vector<MCompPtr> & outCams) const
{
	for (int i = 0; i < cameras.size(); ++i)
	{
		outCams.insert(outCams.end(), cameras[i]);
	}
}


std::vector<MObjPtr>::const_iterator LookFor(std::vector<MObjPtr> elements, MObject* toLookFor)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		if (elements[i].get() == toLookFor)
		{
			return elements.begin() + i;
		}
	}

	return elements.end();
}

void MWorld::UpdateWorld(float elapsedSeconds)
{
	//Delete objects.
	std::vector<MObjPtr>::const_iterator it;
	for (int i = 0; i < objectsToDestroy.size(); ++i)
	{
		it = LookFor(objects, objectsToDestroy[i]);
		if (it != objects.end())
		{
			(*it)->OnDeath();
			objects.erase(it);
		}
	}
	objectsToDestroy.clear();

	//Update objects.
	for (int i = 0; i < objects.size(); ++i)
	{
		objects[i]->Update();
	}
}
void MWorld::RenderWorld(float elapsedSeconds)
{
	ScreenClearer().ClearScreen();

	int i, j;

	//TODO: Render into texture for each camera, then draw it to the viewport with the camera's post-process effect.
	CameraComponent * cc;
	Matrix4f world, view, proj;
	world.SetAsIdentity();

	for (i = 0; i < cameras.size(); ++i)
	{
		cc = (CameraComponent*)&*cameras[i];
		cc->GetViewTransform(view);
		proj.SetAsPerspProj(cc->Info);
		MCameraInfo mci(*cc, world, view, proj);

		for (j = 0; j < objects.size(); ++j)
		{
			objects[j]->Render3D(mci);
		}
	}

	ScreenClearer clear2(false);
	//TODO: Now switch to 2D drawing for 2D-renderable components. Optionally use the camera's post-process effect here too.
	world.SetAsIdentity();
	for (i = 0; i < cameras.size(); ++i)
	{
		cc = (CameraComponent*)&*cameras[i];
		cc->GetViewTransform(view);
		//proj.SetAsOrthoProj(cc->Info);
		MCameraInfo mci(*cc, world, view, proj);

		for (j = 0; j < objects.size(); ++j)
		{
			objects[j]->Render2D(mci);
		}
	}
}*/