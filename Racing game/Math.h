#pragma once

#include "MathGeoLib\MathBuildConfig.h"
#include "MathGeoLib\MathGeoLib.h"
#include "MathGeoLib\MathGeoLibFwd.h"
#include "p2Point.h"
#include "Globals.h"

using namespace math;

class RNG
{
private:
	LCG* lcg = nullptr;

public:

	RNG() { lcg = new LCG(); };
	~RNG() { delete lcg; }

	float Zero_to_One()					{ return lcg->Float(); }			// returns a random float between 0 and 1
	float Range(float min, float max)	{ return lcg->Float(min, max); }	// returns a random float between min and max
	float RangeInt(int min, int max)	{ return lcg->Int(min, max); }		// returns a random int between min and max

};






