#include "Core/Engine.h"
#include "Graphics/Camera.h"
#include "Core/Keys.h"
#include "Core/Log.h"
#include "Graphics/Meshes/Sphere.h"
#include "Graphics/Model.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/Meshes/Sphere.h"
#include "Graphics/Meshes/Cube.h"
#include "Graphics/Renderer.h"

class TextureDemo : public SnackerEngine::Scene
{
	SnackerEngine::FPSCamera camera;
	SnackerEngine::Model sphere;
	SnackerEngine::Model cube;
	SnackerEngine::Material simpleMaterial;

	SnackerEngine::Vec3f positionSphere;
	SnackerEngine::Vec3f positionCube;
	SnackerEngine::Vec3f positionSmallSphere;
	float angle;
	float rotationSpeed;
	float smallSphereScale;

	SnackerEngine::Texture earthTexture;
	SnackerEngine::Texture containerTexture;
	SnackerEngine::Texture dabbingPenguinTexture;

public:

	void computePositionSmallSphere()
	{
		positionSmallSphere = { -3.0f + 1.5f * sin(angle), 0.0f + 1.5f * cos(angle), 10.0f };
	}

	TextureDemo()
		: camera{}, sphere(SnackerEngine::createMeshUVSphere(100, 100)), cube(SnackerEngine::createMeshCube(true, true, false)),
		simpleMaterial(SnackerEngine::Shader("shaders/basicTexture.shader")), positionSphere({ -3.0f, 0.0f, 10.0f }), positionCube({ 3.0f, 0.0f, 10.0f }),
		positionSmallSphere(0.0f, 0.0f, 0.0f), angle(0.0f), rotationSpeed(0.7f), smallSphereScale(0.2f),
		earthTexture(SnackerEngine::Texture::Load2D("textures/earthmap1k.jpg")), containerTexture(SnackerEngine::Texture::Load2D("textures/container.jpg")),
		dabbingPenguinTexture(SnackerEngine::Texture::Load2D("textures/dab_penguin.jpg"))
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(186, 214, 229)));
		camera.setAngleSpeed(0.0125f);
		camera.setFarPlane(1000.0f);
		simpleMaterial.getShader().bind();
		simpleMaterial.getShader().setUniform<int>("u_Texture", 0);
	}

	void update(const double& dt) override
	{
		camera.update(dt);
		angle += dt * rotationSpeed;
	}

	void draw() override
	{
		camera.computeView();
		camera.computeProjection();
		simpleMaterial.bind();
		simpleMaterial.getShader().setModelViewProjection(SnackerEngine::Mat4f::Translate(positionSphere), camera.getViewMatrix(), camera.getProjectionMatrix());
		earthTexture.bind();
		SnackerEngine::Renderer::draw(sphere, simpleMaterial);
		computePositionSmallSphere();
		simpleMaterial.getShader().setModelViewProjection(SnackerEngine::Mat4f::TranslateAndScale(positionSmallSphere, SnackerEngine::Vec3f(smallSphereScale)), camera.getViewMatrix(), camera.getProjectionMatrix());
		dabbingPenguinTexture.bind();
		SnackerEngine::Renderer::draw(sphere, simpleMaterial);
		simpleMaterial.getShader().setModelViewProjection(SnackerEngine::Mat4f::Translate(positionCube), camera.getViewMatrix(), camera.getProjectionMatrix());
		containerTexture.bind();
		SnackerEngine::Renderer::draw(cube, simpleMaterial);
	}

	void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override
	{
		if (key == KEY_M && action == ACTION_PRESS)
		{
			camera.toggleMouseMovement();
			camera.toggleMovement();
		}
		camera.callbackKeyboard(key, scancode, action, mods);
	}

	void callbackWindowResize(const SnackerEngine::Vec2i& screenDims) override
	{
		camera.callbackWindowResize(screenDims);
	}

	void callbackMouseScroll(const SnackerEngine::Vec2d& offset) override
	{
		camera.callbackMouseScroll(offset);
	}

	void callbackMouseMotion(const SnackerEngine::Vec2d& position) override
	{
		camera.callbackMouseMotion(position);
	}

};

int main(int argc, char** argv)
{
	if (!SnackerEngine::Engine::initialize(1200, 700, "Demo: Textured Models")) {
		SnackerEngine::errorLogger << SnackerEngine::LOGGER::BEGIN << "startup failed!" << SnackerEngine::LOGGER::ENDL;
	}

	{
		TextureDemo scene;
		SnackerEngine::Engine::setActiveScene(scene);
		SnackerEngine::Engine::startup();
	}

	SnackerEngine::Engine::terminate();

}