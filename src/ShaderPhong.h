#pragma once

#include "ShaderFlat.h"
#include "Scene.h"

//not working to declare the std namespace, gives segmentation fault
//using namespace std;

const int nAreaSamples = 1;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 * @param isOpaque opaque surface flag
	 */
	CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke, bool isOpaque = true)
		: CShaderFlat(color, isOpaque) //adding the opaque property in the constructor
		, m_scene(scene)
		, m_ka(ka)
		, m_kd(kd)
		, m_ks(ks)
		, m_ke(ke)
	{}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray& ray) const override
	{
		// get shading normal
		Vec3f normal = ray.hit->getNormal(ray);

		// turn normal to front
		if (normal.dot(ray.dir) > 0)
			normal = -normal;

		// calculate reflection vector
		Vec3f reflect = normalize(ray.dir - 2 * normal.dot(ray.dir) * normal);

		// ambient term
		Vec3f ambientIntensity(1,1,1);

		Vec3f color = CShaderFlat::Shade();
		Vec3f ambientColor = m_ka * color;
		Vec3f res = ambientColor.mul(ambientIntensity);

		// shadow ray (up to now only for the light direction)
		Ray shadow;
		shadow.org = ray.org + ray.t * ray.dir;
	
		//refraction parameters
		
		// iterate over all light sources
		for (auto pLight : m_scene.m_vpLights)
			for(int s = 0; s < nAreaSamples; s++) {	// TODO: make the sampling to depend on the light source type
				// get direction to light, and intensity
				std::optional<Vec3f> lightIntensity = pLight->Illuminate(shadow);
				if (lightIntensity) {
					// diffuse term
					float cosLightNormal = shadow.dir.dot(normal);
					if (cosLightNormal > 0) {
						if (m_scene.Occluded(shadow)){
								continue;
						}
						Vec3f diffuseColor = m_kd * color;
						res += (diffuseColor * cosLightNormal).mul(lightIntensity.value());
					}

					// specular term
					float cosLightReflect = shadow.dir.dot(reflect);
					if (cosLightReflect > 0) {
						Vec3f specularColor = m_ks * RGB(1, 1, 1); // white highlight;
						res += (specularColor * powf(cosLightReflect, m_ke)).mul(lightIntensity.value());
					}
				}
			}

			if(nAreaSamples > 1)
				res /= nAreaSamples;

		float nDiamond = 2.4, nAir = 1; // we declare the air refraction index in case of swaping
		Vec3f resRefr(0,0,0);
		Vec3f resRef(0,0,0);
		Ray refrRay;
		Ray refRay;
		bool irefract = false;
		bool ireflect = false;

		if(!isOpaque && ray.refractD <= MAXREFRACT){
			ireflect = true;
			irefract = true;
			Vec3f rNorm = normal;
			float ndI = rNorm.dot(ray.dir);
			ndI = -ndI;
			//swap
			if(ray.refractD %2 == 1){
				swap(nDiamond, nAir);	
			}

			float n = nAir / nDiamond;
			
			Ray refrRay;
			refrRay.org = ray.org + ray.t * ray.dir;
			refrRay.t = std::numeric_limits<float>::infinity();
			// calculate refraction vector 
			Vec3f refraction = normalize(n*(ray.dir+rNorm * ndI) - rNorm * sqrt((1 - n*n) *(1 - pow(ndI, 2))));
			refrRay.dir = refraction;
			refrRay.refractD++;
			refrRay.hit = NULL;
		}	
			resRefr = m_scene.RayTrace(refrRay);

			if(irefract && ray.refractD != 0){
				return res * 0.2 + resRefr * 0.8;
			}
		
			// reflection
		if(!isOpaque && ray.reflectD <= MAXREF){
			ireflect = true;
			Ray refRay;
			refRay.org = ray.org + ray.t * ray.dir;
			refRay.dir = reflect;
			refRay.t = std::numeric_limits<float>::infinity();
			refRay.reflectD++;
			resRef = m_scene.RayTrace(refRay);
		}

		if(ireflect && ray.reflectD != 0){
			return resRef;
		}


		for (int i = 0; i < 3; i++)
			if (res.val[i] > 1)
				res.val[i] = 1;

		return res;
	}

private:
	CScene& m_scene;
	float 	m_ka;    ///< ambient coefficient
	float 	m_kd;    ///< diffuse reflection coefficients
	float 	m_ks;    ///< specular refelection coefficients
	float 	m_ke;    ///< shininess exponent
	const int MAXREF = 1; //depth of reflection, for 1 and 3 works
		//for MAXREF = 2 we get bus error.
	const int MAXREFRACT = 5; //depth of refraction
};
