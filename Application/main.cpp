#include "Core/Engine.h"
#include "Graphics/Camera.h"
#include "Core/Keys.h"
#include "Core/Log.h"
#include "Graphics/Meshes/Sphere.h"
#include "Graphics/Model.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/Meshes/Plane.h"
#include "Graphics/Renderer.h"
#include "Gui/Text/Font.h"
#include "Gui/Text/Text2.h"
#include "Math/Utility.h"
#include "Gui/Text/TextMaterial.h"
#include "AssetManager/MaterialManager.h"

class TextDemo : public SnackerEngine::Scene
{
	SnackerEngine::FPSCamera camera;
	SnackerEngine::Model plane;
	SnackerEngine::Font font;
	SnackerEngine::Material materialBitmap;
	SnackerEngine::Material materialText;
	SnackerEngine::Vec3f positionLeft;
	SnackerEngine::Vec3f positionRight;
	SnackerEngine::Vec3f positionText1;
	SnackerEngine::Vec3f positionText2;
	float rightSize;
	float rightSizeChange;

	double fontSize;
	double textWidth;
	SnackerEngine::EditableText text1;
	SnackerEngine::DynamicText2 text2;

	SnackerEngine::Mat4f fontSizeIndicatorModelMatrix;
	SnackerEngine::Mat4f textWidthIndicatorModelMatrix;
	SnackerEngine::Mat4f cursorModelMatrix;
	SnackerEngine::Material fontSizeIndicatorMaterial;

public:

	void computeCursorModelMatrix()
	{
		cursorModelMatrix = SnackerEngine::Mat4f::TranslateAndScale(
			positionText1 + text1.getCursorPos() * SnackerEngine::pointsToMeters(fontSize), 
			SnackerEngine::Vec3f(text1.getCursorSize() * SnackerEngine::pointsToMeters(fontSize)));
	}

	TextDemo()
		: camera{}, plane(SnackerEngine::createMeshPlane(true, false, false)), font("fonts/arial.ttf"), materialBitmap(SnackerEngine::Shader("shaders/basicTexture.shader")),
		materialText(SnackerEngine::MaterialManager::createMaterial(std::make_unique<SnackerEngine::SimpleTextMaterialData>(SnackerEngine::Shader("shaders/basic3DText.shader"), font, SnackerEngine::Color4f(1.0f, 0.0f, 0.0f, 1.0f), 
			SnackerEngine::Color4f(0.0f, 0.0f, 0.0f, 0.0f)))), positionLeft({ -1.1f, -1.5f, 3.0f }), positionRight({ 0.1f, -1.5f, 3.0f }),
		positionText1({ -1.5f, 0.6f, 2.0f }), positionText2({ -1.5f, -1.3f, 2.0f }), rightSize(1.0f), rightSizeChange(0.5f), fontSize(1000.0), textWidth(10000.0),
		text1(u8""/*"aaa  aaa\naaa  aaa"*/, font, fontSize, textWidth, 0.05, SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText2::Alignment::LEFT),
		text2(u8"Hello Wörld! This is a test. And now we have a reallyreallyreallyreallylongword!ü yeah", font, fontSize, textWidth, SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD),
		fontSizeIndicatorModelMatrix(SnackerEngine::Mat4f::TranslateAndScale(positionText1 - SnackerEngine::Vec3f(0.5f, 0.0f, 0.0f), SnackerEngine::pointsToMeters(fontSize))),
		textWidthIndicatorModelMatrix(SnackerEngine::Mat4f::TranslateAndScale(positionText1 + SnackerEngine::Vec3f(0.0f, 0.5f, 0.0f), 
		SnackerEngine::Vec3f(SnackerEngine::pointsToMeters(textWidth), SnackerEngine::pointsToMeters(fontSize), 0.0f))),
		cursorModelMatrix{}, fontSizeIndicatorMaterial(SnackerEngine::Shader("shaders/basic.shader"))
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(186, 214, 229)));
		camera.setAngleSpeed(0.0125f);
		camera.setFarPlane(1000.0f);
		camera.setPosition({ 0.0f, 0.0f, -5.0f });
		materialBitmap.getShader().bind();
		materialBitmap.getShader().setUniform<int>("u_Texture", 0);
		materialText.getShader().bind();
		materialText.getShader().setUniform<int>("u_msdf", 0);
		materialText.getShader().setUniform<float>("u_pxRange", 2.0f);
		computeCursorModelMatrix();
	}

	void update(const double& dt) override
	{
		camera.update(dt);
	}

	void draw() override
	{
		camera.computeView();
		camera.computeProjection();

		materialBitmap.bind();
		materialBitmap.getShader().setModelViewProjection(SnackerEngine::Mat4f::Translate(positionLeft), camera.getViewMatrix(), camera.getProjectionMatrix());
		font.bind();
		SnackerEngine::Renderer::draw(plane, materialBitmap);

		materialText.bind();
		materialText.getShader().setModelViewProjection(SnackerEngine::Mat4f::TranslateAndScale(positionRight, rightSize), camera.getViewMatrix(), camera.getProjectionMatrix());
		materialText.getShader().setUniform<SnackerEngine::Vec2i>("u_msdfDims", SnackerEngine::Vec2i(1, 1));
		font.bind();
		SnackerEngine::Renderer::draw(plane, materialText);

		materialText.bind();
		materialText.getShader().setModelViewProjection(SnackerEngine::Mat4f::TranslateAndScale(positionText1, SnackerEngine::pointsToMeters(fontSize)), camera.getViewMatrix(), camera.getProjectionMatrix());
		font.bind();
		SnackerEngine::Renderer::draw(text1.getModel(), materialText);

		materialText.bind();
		materialText.getShader().setModelViewProjection(SnackerEngine::Mat4f::TranslateAndScale(positionText2, SnackerEngine::pointsToMeters(fontSize)), camera.getViewMatrix(), camera.getProjectionMatrix());
		font.bind();
		SnackerEngine::Renderer::draw(text2.getModel(), materialText);

		// Draw font size indicator
		fontSizeIndicatorMaterial.bind();
		fontSizeIndicatorMaterial.getShader().setModelViewProjection(fontSizeIndicatorModelMatrix, camera.getViewMatrix(), camera.getProjectionMatrix());
		SnackerEngine::Renderer::draw(plane, fontSizeIndicatorMaterial);

		fontSizeIndicatorMaterial.bind();
		fontSizeIndicatorMaterial.getShader().setModelViewProjection(textWidthIndicatorModelMatrix, camera.getViewMatrix(), camera.getProjectionMatrix());
		SnackerEngine::Renderer::draw(plane, fontSizeIndicatorMaterial);

		// Draw cursor
		fontSizeIndicatorMaterial.bind();
		fontSizeIndicatorMaterial.getShader().setModelViewProjection(cursorModelMatrix, camera.getViewMatrix(), camera.getProjectionMatrix());
		SnackerEngine::Renderer::draw(plane, fontSizeIndicatorMaterial);
	}

	void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override
	{
		if (key == KEY_M && action == ACTION_PRESS)
		{
			camera.toggleMouseMovement();
			camera.toggleMovement();
		}
		else if ((action == ACTION_PRESS || action == ACTION_REPEAT) && !camera.isEnableMouse())
		{
			if (key == KEY_LEFT) {
				if (mods & MOD_CONTROL) {
					text1.moveCursorToLeftWordBeginning();
					computeCursorModelMatrix();
				}
				else {
					text1.moveCursorToLeft();
					computeCursorModelMatrix();
				}
			}
			else if (key == KEY_RIGHT) {
				if (mods & MOD_CONTROL) {
					text1.moveCursorToRightWordEnd();
					computeCursorModelMatrix();
				}
				else {
					text1.moveCursorToRight();
					computeCursorModelMatrix();
				}
			}
			else if (key == KEY_BACKSPACE) {
				if (mods & MOD_CONTROL) {
					text1.deleteWordBeforeCursor();
					computeCursorModelMatrix();
				}
				else {
					text1.deleteCharacterBeforeCursor();
					computeCursorModelMatrix();
				}
			}
			else if (key == KEY_ENTER) {
				text1.inputNewlineAtCursor();
				computeCursorModelMatrix();
			}
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
		if (!camera.isEnableMouse()) {
			rightSize += rightSizeChange * static_cast<float>(offset.y);
		}
	}

	void callbackMouseMotion(const SnackerEngine::Vec2d& position) override
	{
		camera.callbackMouseMotion(position);
	}

	void callbackCharacterInput(const unsigned int& codepoint) override
	{
		if (codepoint != KEY_M && !camera.isEnableMouse())
		{
			// font.addNewGlyph(codepoint);
			text1.inputAtCursor(codepoint);
			computeCursorModelMatrix();
		}
	}

};

int main(int argc, char** argv)
{
	if (!SnackerEngine::Engine::initialize(1200, 700, "Demo: Textured Models")) {
		SnackerEngine::errorLogger << SnackerEngine::LOGGER::BEGIN << "startup failed!" << SnackerEngine::LOGGER::ENDL;
	}

	{
		TextDemo scene;
		SnackerEngine::Engine::setActiveScene(scene);
		SnackerEngine::Engine::startup();
	}

	SnackerEngine::Engine::terminate();

}