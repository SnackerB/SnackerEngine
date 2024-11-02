#pragma once

#include "Gui\Layouts\HorizontalWeightedLayout.h"
#include "Gui\GuiElements\GuiEditVariable.h"
#include "Gui\GuiElements\VectorElements\GuiVariableVec.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	class GuiEditVariableVec : public GuiVariableVec<T, VecT, N>
	{
	private:
		/// Helper function that creates a vector of GuiEditVariables from the given value
		void setupComponents();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "ERROR";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiEditVariableVec(const VecT& value = VecT{});
		/// Constructor from JSON
		GuiEditVariableVec(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
	public:
		/// Getters
		const Color4f& getSelectionBoxColor() const { return static_cast<std::unique_ptr<GuiEditBox>>(this->components[0])->getSelectionBoxColor(); }
		bool isActive() const { for (const auto& component : this->components) if (static_cast<std::unique_ptr<GuiEditBox>>(component)->isActive()) return true; return false; }
		float getCursorWidth() const { return static_cast<std::unique_ptr<GuiEditBox>>(this->components[0])->getCursorWidth()(); }
		double getCursorBlinkTime() const { return static_cast<std::unique_ptr<GuiEditBox>>(this->components[0])->getCursorBlinkTime(); }
		/// Setters
		void setSelectionBoxColor(const Color4f& selectionBoxColor) {
			for (const auto& component : this->components) static_cast<std::unique_ptr<GuiEditBox>>(component)->setSelectionBoxColor(selectionBoxColor);
		}
		void setCursorWidth(float cursorWidth) {
			for (const auto& component : this->components) static_cast<std::unique_ptr<GuiEditBox>>(component)->setCursorWidth(cursorWidth);
		}
		void setCursorBlinkTime(double cursorBlinkTime) {
			for (const auto& component : this->components) static_cast<std::unique_ptr<GuiEditBox>>(component)->setCursorBlinkTime(cursorBlinkTime);
		}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiEditVariableVec<T, VecT, N>::setupComponents()
	{
		this->components.clear();
		this->components.reserve(N);
		for (unsigned i = 0; i < N; ++i) this->components.push_back(std::make_unique<GuiEditVariable<T>>(this->getValue()[i]));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiEditVariableVec<T, VecT, N>::GuiEditVariableVec(const VecT& value)
		: GuiVariableVec(value)
	{
		setupComponents();
		setValue(value);
		this->setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		this->setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiEditVariableVec<T, VecT, N>::GuiEditVariableVec(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVariableVec<T, VecT, N>(json, data, parameterNames)
	{
		setupComponents();
		GuiEditVariableVec<T, VecT, N>::parseJSON(json, data, parameterNames);
		if (!json.contains("SizeHintModePreferredSize")) this->setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (!json.contains("preferredSize") && !json.contains("preferredWidth")) this->setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariableVec2 : public GuiEditVariableVec<T, Vec2<T>, 2>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_EDIT_VARIABLE_VEC2";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiEditVariableVec2(const Vec2<T>& value = Vec2<T>{})
			: GuiEditVariableVec<T, Vec2<T>, 2>(value) {}
		/// Constructor from JSON
		GuiEditVariableVec2(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiEditVariableVec<T, Vec2<T>, 2>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariableVec3 : public GuiEditVariableVec<T, Vec3<T>, 3>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_EDIT_VARIABLE_VEC3";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiEditVariableVec3(const Vec3<T>& value = Vec3<T>{})
			: GuiEditVariableVec<T, Vec3<T>, 3>(value) {}
		/// Constructor from JSON
		GuiEditVariableVec3(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiEditVariableVec<T, Vec3<T>, 3>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariableVec4 : public GuiEditVariableVec<T, Vec4<T>, 4>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_EDIT_VARIABLE_VEC4";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiEditVariableVec4(const Vec4<T>& value = Vec4<T>{})
			: GuiEditVariableVec<T, Vec4<T>, 4>(value) {}
		/// Constructor from JSON
		GuiEditVariableVec4(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiEditVariableVec<T, Vec4<T>, 4>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	using GuiEditVariableVec2f = GuiEditVariableVec2<float>;
	using GuiEditVariableVec2d = GuiEditVariableVec2<double>;
	using GuiEditVariableVec2i = GuiEditVariableVec2<int>;
	//--------------------------------------------------------------------------------------------------
	using GuiEditVariableVec3f = GuiEditVariableVec3<float>;
	using GuiEditVariableVec3d = GuiEditVariableVec3<double>;
	using GuiEditVariableVec3i = GuiEditVariableVec3<int>;
	//--------------------------------------------------------------------------------------------------
	using GuiEditVariableVec4f = GuiEditVariableVec4<float>;
	using GuiEditVariableVec4d = GuiEditVariableVec4<double>;
	using GuiEditVariableVec4i = GuiEditVariableVec4<int>;
	//--------------------------------------------------------------------------------------------------
}