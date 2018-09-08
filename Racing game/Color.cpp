
#include "Color.h"
#include "Math.h"

void Color::setRandom()
{
	RNG rng;
	r = rng.Zero_to_One();
	g = rng.Zero_to_One();
	b = rng.Zero_to_One();
	a = 1.0f;
}

Color Red = Color(1.0f, 0.0f, 0.0f);
Color Green = Color(0.0f, 1.0f, 0.0f);
Color Blue = Color(0.0f, 0.0f, 1.0f);
Color Black = Color(0.0f, 0.0f, 0.0f);
Color White = Color(1.0f, 1.0f, 1.0f);