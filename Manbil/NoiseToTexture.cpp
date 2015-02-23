#include "NoiseToTexture.h"


void NoiseToTexture::GetImage(Array2D<Vector4b>& outImage) const
{
	//The pixel array.
    outImage.Reset(NoiseToUse->GetWidth(), NoiseToUse->GetHeight());
	
	//Some temp variables.
	unsigned int x, y, pixX, pixY;
	Vector4f col;
	Vector4b colB;
	float tempF, readNoise;
	
	//Go through every pixel and set its value.
	for (x = 0; x < NoiseToUse->GetWidth(); ++x)
	{
		for (y = 0; y < NoiseToUse->GetHeight(); ++y)
		{
			tempF = (*NoiseToUse)[Vector2u(x, y)];
            readNoise = Mathf::Clamp(tempF, 0.0f, 1.0f);

			//Get the coordinates of the pixel in the out array.
			pixX = x * 4;
			pixY = y;

			//Set the pixel values.
			col = GradientToUse->GetColor(readNoise) * 255.0f;
			col = Vector4f(Mathf::Clamp(col.x, 0.0f, 255.0f), Mathf::Clamp(col.y, 0.0f, 255.0f),
						   Mathf::Clamp(col.z, 0.0f, 255.0f), Mathf::Clamp(col.w, 0.0f, 255.0f));
			colB = Vector4b((unsigned char)Mathf::RoundToInt(col.x),
							(unsigned char)Mathf::RoundToInt(col.y),
							(unsigned char)Mathf::RoundToInt(col.z),
							(unsigned char)Mathf::RoundToInt(col.w));
            outImage[Vector2u(pixX, pixY)] = colB;
		}
	}
}
void NoiseToTexture::GetImage(Array2D<Vector4f>& outImage) const
{
	//The pixel array.
    outImage.Reset(NoiseToUse->GetWidth(), NoiseToUse->GetHeight());
	
	//Some temp variables.
	unsigned int x, y, pixX, pixY;
	float tempF, readNoise;
	
	//Go through every pixel and set its value.
	for (x = 0; x < NoiseToUse->GetWidth(); ++x)
	{
		for (y = 0; y < NoiseToUse->GetHeight(); ++y)
		{
			tempF = (*NoiseToUse)[Vector2u(x, y)];
            readNoise = Mathf::Clamp(tempF, 0.0f, 1.0f);

			//Get the coordinates of the pixel in the out array.
			pixX = x * 4;
			pixY = y;

			//Set the pixel values.
            outImage[Vector2u(pixX, pixY)] = GradientToUse->GetColor(readNoise);
		}
	}
}