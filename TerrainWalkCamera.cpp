#include "TerrainWalkCamera.h"

const Vector2i TerrainWalkCamera::mouseTarget = Vector2i(800, 600);


TerrainWalkCamera::TerrainWalkCamera(Vector3f p, Vector3f f, Vector3f u, Heightmap * hMap, float mSpd, float rSpd, sf::Window * w)
{
	SetPosition(p);
	SetRotation(f, u, false);

	heightmap = hMap;

	moveSpeed = mSpd;
	rotSpeed = rSpd;

	window = w;
}


bool TerrainWalkCamera::Update(float elapsedTime, std::shared_ptr<OculusDevice> orDevice)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		return true;
	}


	//Update keyboard movement.

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		IncrementPosition(GetForward() * moveSpeed * elapsedTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		IncrementPosition(-GetForward() * moveSpeed * elapsedTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		IncrementPosition(GetSideways() * moveSpeed * elapsedTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		IncrementPosition(-GetSideways() * moveSpeed * elapsedTime);
	}


	//Set Z position.
	Vector2f pos(GetPosition().x, GetPosition().y);
	pos = Vector2f(BasicMath::Clamp(pos.x, 0, heightmap->GetSize() - 2),
				   BasicMath::Clamp(pos.y, 0, heightmap->GetSize() - 2));
	
	float z = (*heightmap)[pos];
	SetPositionZ(z);


	//Update mouse rotation.

	if (window == 0)
	{
		return false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
	{
		window->setMouseCursorVisible(true);
		return false;
	}

	window->setMouseCursorVisible(false);

	Vector2i mousePos = Conv(sf::Mouse::getPosition());
	Vector2i delta = mousePos - mouseTarget;

	Vector2f rotAmount(delta.x * rotSpeed * elapsedTime, delta.y * rotSpeed * elapsedTime);

	sf::Mouse::setPosition(Conv(mouseTarget));

	AddPitch(rotAmount.y);
	AddYaw(rotAmount.x);


	//Update oculus rift rotation.

	if (orDevice.get() != 0)
	{
		YawPitchRoll rots;
		orDevice->GetYawPitchRoll(rots);
		SetRotation(Vector3f(0, 0, -1), Vector3f(1, 0, 0), true);
		AddYaw(rots.yaw);
		AddPitch(rots.pitch);
		AddRoll(rots.roll);
	}

	return false;
}