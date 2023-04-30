#include <iostream>
#include "Core/Engine.h"
#include "Graphics/Shader.h"
#include "AssetManager/ShaderManager.h"
#include "Core/Scene.h"
#include "Core/Timer.h"
#include "Graphics/Texture.h"
#include "Graphics/Meshes/Sphere.h"
#include "Graphics/Meshes/Cube.h"
#include "Graphics/Meshes/Plane.h"
#include "Graphics/Meshes/ScreenQuad.h"
#include "Graphics/Meshes/Square.h"
#include "Graphics/Model.h"
#include "Graphics/Renderer.h"
#include "Graphics/Camera.h"
#include "Core/Keys.h"
#include "Math/Utility.h"

#include "Gui/GuiManager.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiWindow.h"
#include "Gui/GuiElements/GuiEditVariable.h"
#include "Gui/GuiElements/GuiSlider.h"
#include "Gui/Layouts/VerticalScrollingListLayout.h"

class MyScene : public SnackerEngine::Scene
{
	SnackerEngine::Timer timer;
	unsigned counter;
	std::vector<SnackerEngine::Model> models;
	SnackerEngine::Material material;
	std::vector<SnackerEngine::Mat4f> modelMatrices;

	SnackerEngine::FPSCamera camera;
	SnackerEngine::Color3f clearColorA;
	SnackerEngine::Color3f clearColorB;

	float nathanTime;
	float burgerTime;
	float planetTime;

	SnackerEngine::Vec3f maxWobbleBurger;
	float burgerWobbleSpeed;

	float objPlanetRotationSpeed;

	SnackerEngine::GuiVariableHandleFloat nathanSpeed;
	SnackerEngine::GuiVariableHandleFloat nathanRadius;

	// GUI
	SnackerEngine::GuiButton nathanTimeButton;
	SnackerEngine::GuiManager guiManager;

public:

	void computeModelMatrices()
	{
		// Burger
		modelMatrices[0] = SnackerEngine::Mat4f::TranslateAndScale({ -7.5f, 0.0f, 10.0f },
			SnackerEngine::Vec3f(1.0f, 1.0f, 1.0f) + maxWobbleBurger * sin(burgerTime));
		// ObjPlanet
		modelMatrices[1] = SnackerEngine::Mat4f::Translate({ -2.5f, 0.0f, 10.0f }) 
			* SnackerEngine::Mat4f::RotateZ(planetTime);
		// Nathan
		modelMatrices[2] = SnackerEngine::Mat4f::Translate(SnackerEngine::Vec3f(2.5, 0.0f, 10.0f)
			+ nathanRadius.get() * SnackerEngine::Vec3f(sin(nathanTime), 0.0f, cos(nathanTime)));
	}

	MyScene(unsigned int fps)
		: timer(fps), counter(0), models{}, material(SnackerEngine::Shader("shaders/basic.shader")), modelMatrices{},
		camera{}, clearColorA(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(226, 151, 67))),
		clearColorB(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(226, 67, 89))),
		nathanTime(0.0f), burgerTime(0.0f), planetTime(0.0f), maxWobbleBurger(-0.5f, 0.5f, -0.5f), burgerWobbleSpeed(5.0f), objPlanetRotationSpeed
		(0.2f), nathanSpeed(4.0f), nathanRadius(1.0f), nathanTimeButton(), guiManager{}
	{
		camera.setAngleSpeed(0.0125f);
		camera.setFarPlane(1000.0f);
		camera.setPosition({-2.0f, 5.0f, 20.0f});
		camera.setYaw(SnackerEngine::degToRad(0.0f));
		camera.setPitch(SnackerEngine::degToRad(-20.0f));
		camera.computeProjection();
		camera.computeView();
		models.push_back(SnackerEngine::Model("models/burger/burgerWithFries.obj"));
		modelMatrices.push_back(SnackerEngine::Mat4f::Translate({ -7.5f, 0.0f, 10.0f }));
		models.push_back(SnackerEngine::Model("models/demo/objPlanet.obj"));
		modelMatrices.push_back(SnackerEngine::Mat4f::Translate({ -2.5f, 0.0f, 10.0f }));
		models.push_back(SnackerEngine::Model("models/nathan/nathan.obj"));
		modelMatrices.push_back(SnackerEngine::Mat4f::Translate({ 2.5f, 0.0f, 10.0f }));
		// Tree model takes too much memory apparently!
		//models.push_back(SnackerEngine::Model("models/trees/Tree1/Tree1.obj"));
		//modelMatrices.push_back(SnackerEngine::Mat4f::Translate({ 7.5f, 0.0f, 10.0f }));

		// Setup GUI
		SnackerEngine::GuiStyle style = SnackerEngine::getDefaultStyle();
		SnackerEngine::GuiWindow window(style);
		window.setPosition({ 350, 10 });
		window.setSize({ 500, 150 });
		guiManager.registerElement(window);
		SnackerEngine::VerticalScrollingListLayout layout(style);
		window.registerChild(layout);
		{
			SnackerEngine::GuiSlider<float> slider1("Nathan speed: ", 0.0f, 10.0f, nathanSpeed, style);
			layout.registerChild(slider1);
			guiManager.moveElement(std::move(slider1));
			SnackerEngine::GuiSlider<float> slider2("Nathan radius: ", 0.0f, 10.0f, nathanRadius, style);
			layout.registerChild(slider2);
			guiManager.moveElement(std::move(slider2));
		}
		guiManager.moveElement(std::move(layout));
		guiManager.moveElement(std::move(window));
	}

	void update(const double& dt) override
	{
		auto timerResult = timer.tick(dt);
		if (timerResult.first) {
			//std::cout << "update, counter = " << counter << std::endl;
			counter++;
			if (counter % 2 == 0) {
				SnackerEngine::Renderer::setClearColor(clearColorA);
			}
			else {
				SnackerEngine::Renderer::setClearColor(clearColorB);
			}
		}
		//SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN << dt << SnackerEngine::LOGGER::ENDL;
		camera.update(dt);
		nathanTime += dt * nathanSpeed;
		burgerTime += dt * burgerWobbleSpeed;
		planetTime += dt * objPlanetRotationSpeed;
		guiManager.update(dt);
	}

	void draw() override
	{
		const SnackerEngine::Shader& shader = material.getShader();
		shader.bind();
		computeModelMatrices();
		camera.computeView();
		camera.computeProjection();
		shader.setUniform<SnackerEngine::Mat4f>("u_view", camera.getViewMatrix());
		shader.setUniform<SnackerEngine::Mat4f>("u_projection", camera.getProjectionMatrix());
		for (unsigned int i = 0; i < models.size(); ++i) {
			shader.setUniform<SnackerEngine::Mat4f>("u_model",modelMatrices[i]);
			SnackerEngine::Renderer::draw(models[i], material);
		}
		guiManager.draw();
	}

	void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override
	{
		if (key == KEY_M && action == ACTION_PRESS)
		{
			camera.toggleMouseMovement();
			camera.toggleMovement();
		}
		camera.callbackKeyboard(key, scancode, action, mods);
		guiManager.callbackKeyboard(key, scancode, action, mods);
	}

	void callbackWindowResize(const SnackerEngine::Vec2i& screenDims) override
	{
		camera.callbackWindowResize(screenDims);
		guiManager.callbackWindowResize(screenDims);
	}

	void callbackMouseScroll(const SnackerEngine::Vec2d& offset) override
	{
		camera.callbackMouseScroll(offset);
		guiManager.callbackMouseScroll(offset);
	}

	void callbackMouseMotion(const SnackerEngine::Vec2d& position) override
	{
		camera.callbackMouseMotion(position);
		guiManager.callbackMouseMotion(position);
	}

	void callbackMouseButton(const int& button, const int& action, const int& mods) override
	{
		guiManager.callbackMouseButton(button, action, mods);
	};

	void callbackCharacterInput(const unsigned int& codepoint) override
	{
		guiManager.callbackCharacterInput(codepoint);
	};

};

int main(int argc, char** argv)
{

	if (!SnackerEngine::Engine::initialize(1200, 700, "deformed sphere rave party")) {
		std::cout << "startup failed!" << std::endl;
	}

	SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(226, 151, 67)));

	{
		MyScene scene(2);
		SnackerEngine::Engine::setActiveScene(scene);
		SnackerEngine::Engine::startup();
	}

	SnackerEngine::Engine::terminate();
}