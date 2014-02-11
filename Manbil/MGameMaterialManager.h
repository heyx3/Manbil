/*#pragma once

#include "Material.h"
#include <unordered_map>
#include <memory>

//Manages materials.
class MGameMaterialManager
{
public:
	
	//TODO: Create a buch of basic materials/PPE's in the constructor, and expose their ID's.

	typedef std::shared_ptr<Material> MaterialPtr;
	
	template<typename MaterialType>
	//Converts the given material into a bare pointer of a more specific material type for manipulation.
	//This bare pointer isn't reference-counted, so don't hold onto it for very long!
	static MaterialType* GetTempPtr(MaterialPtr ptr) { return (MaterialType*)&*ptr; }

	MGameMaterialManager(void) : nextID(0) { }

	unsigned int RegisterMaterial(MaterialPtr material) { materials[nextID] = material; nextID += 1; }
	void RemoveMaterial(unsigned int material) { materials.erase(material); }

	MaterialPtr GetMaterial(unsigned int matID) { return materials[matID]; }

private:

	typedef std::unordered_map<unsigned int, MaterialPtr> MaterialMap;
	MaterialMap materials;
	unsigned int nextID;
};

*/