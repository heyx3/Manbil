#include "CompositeVector2Inputs.h"

Vector2f FourButtonVector2Input::GetRawInput(float elapsed, float total)
{
	Vector2f raw;

	PositiveX->Update(elapsed, total);
	PositiveY->Update(elapsed, total);
	NegativeX->Update(elapsed, total);
	NegativeY->Update(elapsed, total);

	if (PositiveX->GetValue()) raw.x += 1;
	if (NegativeX->GetValue()) raw.x -= 1;
	if (PositiveY->GetValue()) raw.y += 1;
	if (NegativeY->GetValue()) raw.y -= 1;

	return raw;
}