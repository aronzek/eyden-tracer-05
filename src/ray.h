// Basic ray structure
// Written by Sergey Kosov in 2005 for Rendering Competition
#pragma once

#include "types.h"

class CPrim;

/// Basic ray structure
struct Ray
{
	Vec3f			org;		///< Origin
	Vec3f			dir;		///< Direction
	float			t;			///< Current/maximum hit distance
	const CPrim*	hit;		///< Pointer to currently closest primitive
	float			u;			///< Barycentric u coordinate
	float			v;			///< Barycentric v coordinate
	int reflectD; //depth of reflection
	int refractD; //depth of refraction
};
