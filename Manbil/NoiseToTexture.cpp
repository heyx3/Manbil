#include "NoiseToTexture.h"

bool NoiseToTexture::GetImage(sf::Image & img) const
{
	//The pixel array.
	Array2D<sf::Uint8> outP(NoiseToUse->GetWidth() * 4, NoiseToUse->GetHeight(), 255);
	
	//Some temp variables.
	unsigned int x, y, pixX, pixY;
	sf::Uint8 noiseVal;
	Vector4f col;
	Vector4b colB;
	float tempF, readNoise;
	
	//Go through every pixel and set its value.
	for (x = 0; x < NoiseToUse->GetWidth(); ++x)
	{
		for (y = 0; y < NoiseToUse->GetHeight(); ++y)
		{
			tempF = (*NoiseToUse)[Vector2i(x, y)];
            readNoise = BasicMath::Clamp(tempF, 0.0f, 1.0f);

			//Convert to a byte value.
			noiseVal = (sf::Uint8)BasicMath::RoundToInt(readNoise * 255.0f);

			//Get the coordinates of the pixel in the out aray.
			pixX = x * 4;
			pixY = y;

			//Set the pixel values.
			col = GradientToUse->GetColor(readNoise) * 255.0f;
			col = Vector4f(BasicMath::Clamp(col.x, 0.0f, 255.0f), BasicMath::Clamp(col.y, 0.0f, 255.0f),
						   BasicMath::Clamp(col.z, 0.0f, 255.0f), BasicMath::Clamp(col.w, 0.0f, 255.0f));
			colB = Vector4b((unsigned char)BasicMath::RoundToInt(col.x),
							(unsigned char)BasicMath::RoundToInt(col.y),
							(unsigned char)BasicMath::RoundToInt(col.z),
							(unsigned char)BasicMath::RoundToInt(col.w));

			outP[Vector2i(pixX, pixY)] = colB.x;
			outP[Vector2i(pixX + 1, pixY)] = colB.y;
			outP[Vector2i(pixX + 2, pixY)] = colB.z;
			outP[Vector2i(pixX + 3, pixY)] = colB.w;
		}
	}


	//Put the pixels into the Image.
	img.create(NoiseToUse->GetWidth(), NoiseToUse->GetHeight(), outP.GetArray());

	return true;
}