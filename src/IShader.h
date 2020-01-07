#pragma once

#include "types.h"

struct Ray;

/**
 * @brief Base shader abstract interface class
 */
class IShader
{
public:

	IShader(bool isOpaque = true): isOpaque(isOpaque) {}
	IShader(const IShader&) = delete;
	virtual ~IShader(void) = default;
	const IShader& operator=(const IShader&) = delete;


	/**
	 * @brief Calculates the color of the hit by the ray \ray object
	 * @param ray The ray
	 * @return The color of the hit objesct
	 */
	virtual Vec3f Shade(const Ray& ray) const = 0;
	virtual bool getisOpaque() const{
		return isOpaque;
	}

protected:
	bool isOpaque; //if surface is opaque
};
