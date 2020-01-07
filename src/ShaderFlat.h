#pragma once

#include "IShader.h"

/**
 * @brief Flat shader class
 */
class CShaderFlat : public IShader
{
public:
	/**
	 * @brief Constructor
	 * @details This is a texture-free and light-source-free shader
	 * @param color The color of the object
	 * @param if surface is opaque to allow reflection
	 */
	CShaderFlat(Vec3f color = RGB(0.5f, 0.5f, 0.5f), bool isOpaque = true) : IShader(isOpaque), m_color(color) {}

	virtual Vec3f Shade(const Ray& ray = Ray()) const override
	{
		return m_color;
	}

private:
	Vec3f m_color;
};
