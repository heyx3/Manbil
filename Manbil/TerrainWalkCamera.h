#pragma once

/*

#include "Math/Higher Math/Camera.h"
#include "Math/Higher Math/Terrain.h"
#include "SFML/Window.hpp"
#include "OculusDevice.h"

//Represents a camera that stays on a Terrain.
class TerrainWalkCamera : public Camera
{
private:
	
	typedef Terrain Heightmap;

	static const Vector2i mouseTarget;

public:

    Vector3f TerrainScale;
    float HeightOffset;

	TerrainWalkCamera(Vector3f pos = Vector3f(), Vector3f forward = Vector3f(1, 0, 0), Vector3f up = Vector3f(0, 0, 1),
					  Heightmap * _heightmap = 0,
					  float _moveSpeed = 20.0f, float _rotSpeed = 0.05f,
					  sf::Window * wind = 0);
	~TerrainWalkCamera(void) { }

	void SetWindow(sf::Window * wind) { window = wind; }
	sf::Window * GetWindow(void) const { return window; }

	void SetHeightmap(Heightmap * newHeightmap) { heightmap = newHeightmap; }
	Heightmap * GetHeightmap(void) const { return heightmap; }
	
	float GetMoveSpeed(void) const { return moveSpeed; }
	float GetRotSpeed(void) const { return rotSpeed; }

	void SetMoveSpeed(float newVal) { moveSpeed = newVal; }
	void SetRotSpeed(float newVal) { rotSpeed = newVal; }

	//Returns whether or not the "escape" key was pressed.
	bool Update(float elapsedTime, std::shared_ptr<OculusDevice> orDevice);

private:

	float moveSpeed, rotSpeed;
	Heightmap * heightmap;
	sf::Window * window;

	sf::Vector2i Conv(Vector2i i) const { return sf::Vector2i(i.x, i.y); }
	Vector2i Conv(sf::Vector2i i) const { return Vector2i(i.x, i.y); }
};

*/