#include "Scene.h"

#include "CameraPerspective.h"

#include "PrimSphere.h"
#include "PrimPlane.h"
#include "PrimTriangle.h"

#include "ShaderFlat.h"
#include "ShaderEyelight.h"
#include "ShaderPhong.h"
#include "ShaderPhongBumpMapped.h"

#include "SampleGeneratorRegular.h"
#include "SampleGeneratorRandom.h"
#include "SampleGeneratorStratified.h"

#include "LightPoint.h"
#include "LightArea.h"
#include "timer.h"

//resources:
// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
//http://www.cs.otago.ac.nz/cosc342/2017-notes/342-2017lect18.pdf
//http://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/reflection_refraction.pdf
// 

Mat RenderFrame(void)
{
	// Define a scene
	CScene scene;

	auto pShaderLight	= std::make_shared<CShaderFlat>(RGB(1, 1, 1));
	auto pShaderWhite	= std::make_shared<CShaderPhong>(scene, RGB(1, 1, 1), 0.5f, 0.5f, 0, 0);
	auto pShaderRed		= std::make_shared<CShaderPhong>(scene, RGB(1, 0, 0), 0.5f, 0.5f, 0, 0);

	// Load scene description
	scene.ParseOBJ("../data/Torus Knot.obj");

	scene.Add(std::make_shared<CPrimTriangle>(Vec3f(0, 0, 50), Vec3f(0, 0, -50), Vec3f(-50, 0, -50), pShaderWhite));
	scene.Add(std::make_shared<CPrimTriangle>(Vec3f(0, 0, -50), Vec3f(0, 0, 50), Vec3f(50, 0, 50), pShaderRed));
	scene.Add(std::make_shared<CPrimTriangle>(Vec3f(-10, 10.1f, -10), Vec3f(10, 10.1f, -10), Vec3f(-10, 10.1f, 10), pShaderLight));
	scene.Add(std::make_shared<CPrimTriangle>(Vec3f(10, 10.1f, 10), Vec3f(-10, 10.1f, 10), Vec3f(10, 10.1f, -10), pShaderLight));

	scene.Add(std::make_shared<CLightPoint>(Vec3f::all(50), Vec3f(0, 4, 10)));
	scene.Add(std::make_shared<CLightArea>(Vec3f::all(6), Vec3f(-10, 10, -10), Vec3f(10, 10, -10), Vec3f(10, 10, 10), Vec3f(-10, 10, 10)));
	scene.Add(std::make_shared<CLightPoint>(Vec3f::all(50), Vec3f(0, 10, 0)));

#ifdef ENABLE_BSP
	// Build BSPTree
	scene.BuildAccelStructure();
#endif

	Mat img(scene.m_pCamera->getResolution(), CV_32FC3);		// image array
	Ray ray;                                          			// primary ray


#ifdef ENABLE_SUPERSAMPLING
	//auto sampleGenerator = std::make_unique<CSampleGeneratorRegular>();
	auto sampleGenerator = std::make_unique<CSampleGeneratorRandom>();
	//auto sampleGenerator = std::make_unique<CSampleGeneratorStratified>();
	int nSamples = 16;
	float *u = new float[nSamples];
	float *v = new float[nSamples];
	float *weight = new float[nSamples];
	sampleGenerator->getSamples(nSamples, u, v, weight);

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			Vec3f color = Vec3f::all(0);
			for (int s = 0; s < nSamples; s++) {
				scene.m_pCamera->InitRay(x + u[s], y + v[s], ray); 	// initialize ray
				color += weight[s] * scene.RayTrace(ray);
			}
			img.at<Vec3f>(y, x) = color;
		}
	}
#else
	for (int y = 0; y < img.rows; y++)
		for (int x = 0; x < img.cols; x++) {
			scene.m_pCamera->InitRay(x, y, ray); // initialize ray
			img.at<Vec3f>(y, x) = scene.RayTrace(ray);
		}
#endif

	img.convertTo(img, CV_8UC3, 255);
	return img;
}

int main(int argc, char* argv[])
{
	Mat img = RenderFrame();
	imshow("Image", img);
	waitKey();
	imwrite("image.jpg", img);
	return 0;
}
