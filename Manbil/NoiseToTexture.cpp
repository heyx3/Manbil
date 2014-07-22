#include "NoiseToTexture.h"

void NoiseToTexture::GetImage(Array2D<Vector4b> & outImage) const
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
            readNoise = BasicMath::Clamp(tempF, 0.0f, 1.0f);

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
            outImage[Vector2u(pixX, pixY)] = colB;
		}
	}
}