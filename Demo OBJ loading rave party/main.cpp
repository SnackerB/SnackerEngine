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
#include "Utility/Random.h"

#include "Gui/GuiManager.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiWindow.h"
#include "Gui/GuiElements/GuiEditVariable.h"
#include "Gui/GuiElements/GuiSlider.h"
#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/Layouts/HorizontalListLayout.h"
#include "Gui/GuiElements/GuiCheckBox.h"

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

	SnackerEngine::GuiVariableHandle<bool> isNathanTime;
	SnackerEngine::GuiVariableHandleFloat nathanSpeed;
	SnackerEngine::GuiVariableHandleFloat nathanRadius;
	SnackerEngine::GuiVariableHandleFloat nathanRaveFactor;

	// GUI
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
		SnackerEngine::Vec3f randomOffset{};
		if (isNathanTime) {
			randomOffset = SnackerEngine::Vec3f{ SnackerEngine::randomFloat(0.0f, 1.0f), SnackerEngine::randomFloat(0.0f, 1.0f), SnackerEngine::randomFloat(0.0f, 1.0f) } * nathanRaveFactor;
		}
		modelMatrices[2] = SnackerEngine::Mat4f::Translate(SnackerEngine::Vec3f(2.5, 0.0f, 10.0f)
			+ nathanRadius.get() * SnackerEngine::Vec3f(sin(nathanTime), 0.0f, cos(nathanTime))
			+ randomOffset);
	}

	MyScene(unsigned int fps)
		: timer(fps), counter(0), models{}, material(SnackerEngine::Shader("shaders/basic.shader")), modelMatrices{},
		camera{}, clearColorA(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(226, 151, 67))),
		clearColorB(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(226, 67, 89))),
		nathanTime(0.0f), burgerTime(0.0f), planetTime(0.0f), maxWobbleBurger(-0.5f, 0.5f, -0.5f), burgerWobbleSpeed(5.0f), objPlanetRotationSpeed
		(0.2f), nathanSpeed(4.0f), nathanRadius(1.0f), guiManager{}
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
		SnackerEngine::GuiWindow window(SnackerEngine::Vec2i{ 325, 10 }, SnackerEngine::Vec2i{ 600, 250 }, "CNCU (central nathan controlling unit)");
		window.setBackgroundColor(SnackerEngine::Color4f(0.0f, 0.0f, 0.0f, 0.5f));
		guiManager.registerElement(window);
		SnackerEngine::GuiVerticalScrollingListLayout scrollingListLayout;
		window.registerChild(scrollingListLayout);
		{
			SnackerEngine::GuiHorizontalListLayout tempLayout;
			scrollingListLayout.registerChild(tempLayout);
			SnackerEngine::GuiCheckBox nathanTimeCheckBox;
			tempLayout.registerChild(nathanTimeCheckBox);
			nathanTimeCheckBox.setBoolHandle(isNathanTime);
			SnackerEngine::GuiTextBox nathanTimeText(std::string("nathan time"));
			tempLayout.registerChild(nathanTimeText);
			guiManager.moveElement(std::move(tempLayout));
			guiManager.moveElement(std::move(nathanTimeCheckBox));
			guiManager.moveElement(std::move(nathanTimeText));

			tempLayout = SnackerEngine::GuiHorizontalListLayout();
			tempLayout.setHorizontalLayoutGroupName("A");
			scrollingListLayout.registerChild(tempLayout);
			SnackerEngine::GuiTextBox nathanSpeedText(std::string("nathan speed:"));
			tempLayout.registerChild(nathanSpeedText);
			SnackerEngine::GuiSlider<float> nathanSpeedSlider(0.0f, 10.0f, 5.0f);
			tempLayout.registerChild(nathanSpeedSlider);
			nathanSpeedSlider.setVariableHandle(nathanSpeed);
			guiManager.moveElement(std::move(tempLayout));
			guiManager.moveElement(std::move(nathanSpeedText));
			guiManager.moveElement(std::move(nathanSpeedSlider));

			tempLayout = SnackerEngine::GuiHorizontalListLayout();
			tempLayout.setHorizontalLayoutGroupName("A");
			scrollingListLayout.registerChild(tempLayout);
			SnackerEngine::GuiTextBox nathanRadiusText(std::string("nathan radius"));
			tempLayout.registerChild(nathanRadiusText);
			SnackerEngine::GuiSlider<float> nathanRadiusSlider(0.0f, 10.0f, 2.0f);
			tempLayout.registerChild(nathanRadiusSlider);
			nathanRadiusSlider.setVariableHandle(nathanRadius);
			guiManager.moveElement(std::move(tempLayout));
			guiManager.moveElement(std::move(nathanRadiusText));
			guiManager.moveElement(std::move(nathanRadiusSlider));

			tempLayout = SnackerEngine::GuiHorizontalListLayout();
			tempLayout.setHorizontalLayoutGroupName("A");
			scrollingListLayout.registerChild(tempLayout);
			SnackerEngine::GuiTextBox nathanRaveFactorText(std::string("nathan rave factor"));
			tempLayout.registerChild(nathanRaveFactorText);
			SnackerEngine::GuiSlider<float> nathanRaveFactorSlider(0.0f, 1.0f, 0.0f);
			tempLayout.registerChild(nathanRaveFactorSlider);
			nathanRaveFactorSlider.setVariableHandle(nathanRaveFactor);
			guiManager.moveElement(std::move(tempLayout));
			guiManager.moveElement(std::move(nathanRaveFactorText));
			guiManager.moveElement(std::move(nathanRaveFactorSlider));
		}
		guiManager.moveElement(std::move(scrollingListLayout));
		guiManager.moveElement(std::move(window));
	}

	void update(const double& dt) override
	{
		//SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN << dt << SnackerEngine::LOGGER::ENDL;
		camera.update(dt);
		if (isNathanTime) {
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
			nathanTime += static_cast<float>(dt) * nathanSpeed;
			burgerTime += static_cast<float>(dt) * burgerWobbleSpeed;
			planetTime += static_cast<float>(dt) * objPlanetRotationSpeed;
		}
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

	if (!SnackerEngine::Engine::initialize(1200, 700, "Model Loading Rave Party")) {
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