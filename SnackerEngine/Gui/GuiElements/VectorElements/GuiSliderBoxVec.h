#pragma once

#include "Gui\Layouts\HorizontalWeightedLayout.h"
#include "Gui\GuiElements\GuiSliderBox.h"
#include "Gui\GuiElements\VectorElements\GuiVariableVec.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	class GuiSliderBoxVec : public GuiVariableVec<T, VecT, N>
	{
	private:
		/// Helper function that creates a vector of GuiSliderBoxes from the given value
		void setupComponents();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "ERROR";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderBoxVec(const VecT& minValue = VecT{}, const VecT& maxValue = VecT{}, const VecT& value = VecT{});
		/// Constructor from JSON
		GuiSliderBoxVec(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
	public:
		/// Getters
		int getSliderButtonWidth() const { return static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[0])->getSliderButtonWidth(); }
		const Color4f& getSliderButtonColor() const { static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[0])->getSliderButtonColor(); }
		const Shader& getSliderButtonShader() const { static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[0])->getSliderButtonShader(); }
		VecT getMinValue() const {
			VecT result{};
			for (std::size_t i = 0; i < N; ++i) result.values[i] = static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[i])->getMinValue();
			return result;
		}
		VecT getMaxValue() const {
			VecT result{};
			for (std::size_t i = 0; i < N; ++i) result.values[i] = static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[i])->getMaxValue();
			return result;
		}
		const T& getValue(std::size_t index) const { return static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[index])->getValue(); }
		const T& getMinValue(std::size_t index) const { return static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[index])->getMinValue(); }
		const T& getMaxValue(std::size_t index) const { return static_cast<std::unique_ptr<GuiSliderBox<T>>>(this->components[index])->getMaxValue(); }
		/// Setters
		void setSliderButtonWidth(int sliderButtonWidth) {
			for (const auto& component : this->components) static_cast<GuiSliderBox<T>*>(component.get())->setSliderButtonWidth(sliderButtonWidth);
		}
		void setSliderButtonColor(const Color4f& sliderButtonColor) {
			for (const auto& component : this->components)  static_cast<GuiSliderBox<T>*>(component.get())->setSliderButtonColor(sliderButtonColor);
		}
		void setSliderButtonShader(const Shader& sliderButtonShader) {
			for (const auto& component : this->components) static_cast<GuiSliderBox<T>*>(component.get())->setSliderButtonShader(sliderButtonShader);
		}
		void setValue(const VecT& value) {
			for (std::size_t i = 0; i < N; ++i) static_cast<GuiSliderBox<T>*>(this->components[i].get())->setValue(value[i]);
		}
		void setMinValue(const VecT& minValue) {
			for (std::size_t i = 0; i < N; ++i)  static_cast<GuiSliderBox<T>*>(this->components[i].get())->setMinValue(minValue[i]);
		}
		void setMaxValue(const VecT& maxValue) {
			for (std::size_t i = 0; i < N; ++i)  static_cast<GuiSliderBox<T>*>(this->components[i].get())->setMaxValue(maxValue[i]);
		}
		void setMinValue(const T& minValue, std::size_t index) { static_cast<GuiSliderBox<T>*>(this->components[index].get())->setMinValue(minValue.values[index]); }
		void setMaxValue(const T& maxValue, std::size_t index) { static_cast<GuiSliderBox<T>*>(this->components[index].get())->setMaxValue(maxValue.values[index]); }
		/// Getters
		const Color4f& getSelectionBoxColor() const { return static_cast<GuiSliderBox<T>*>(this->components[0].get())->getSelectionBoxColor(); }
		bool isActive() const { for (const auto& component : this->components) if (static_cast<GuiSliderBox<T>*>(component.get())->isActive()) return true; return false; }
		float getCursorWidth() const { return static_cast<GuiSliderBox<T>*>(this->components[0].get())->getCursorWidth()(); }
		double getCursorBlinkTime() const { return static_cast<GuiSliderBox<T>*>(this->components[0].get())->getCursorBlinkTime(); }
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline void GuiSliderBoxVec<T, VecT, N>::setupComponents()
	{
		this->components.clear();
		this->components.reserve(N);
		for (std::size_t i = 0; i < N; ++i) this->components.push_back(std::make_unique<GuiSliderBox<T>>());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiSliderBoxVec<T, VecT, N>::GuiSliderBoxVec(const VecT& minValue, const VecT& maxValue, const VecT& value)
		: GuiVariableVec() 
	{
		setupComponents();
		setValue(value);
		setMinValue(minValue);
		setMaxValue(maxValue);
		this->setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		this->setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiSliderBoxVec<T, VecT, N>::GuiSliderBoxVec(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVariableVec<T, VecT, N>(json, data, parameterNames)
	{
		setupComponents();
		this->parseJSON(json, data, parameterNames);
		std::optional<int> sliderButtonWidth = parseJsonOrReadFromData<int>("sliderButtonWidth", json, data, parameterNames);
		if (sliderButtonWidth.has_value()) setSliderButtonWidth(sliderButtonWidth.value());
		std::optional<Color4f> sliderButtonColor = parseJsonOrReadFromData<Color4f>("sliderButtonColor", json, data, parameterNames);
		if (sliderButtonWidth.has_value()) setSliderButtonColor(sliderButtonColor.value());
		std::optional<VecT> minValue = parseJsonOrReadFromData<VecT>("minValue", json, data, parameterNames);
		if (minValue.has_value()) setMinValue(minValue.value());
		std::optional<VecT> maxValue = parseJsonOrReadFromData<VecT>("maxValue", json, data, parameterNames);
		if (maxValue.has_value()) setMaxValue(maxValue.value());
		if (!json.contains("SizeHintModePreferredSize")) this->setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (!json.contains("preferredSize") && !json.contains("preferredWidth")) this->setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
	 }
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSliderBoxVec2 : public GuiSliderBoxVec<T, Vec2<T>, 2>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER_VEC2";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderBoxVec2(const Vec2<T>& minValue = Vec2<T>{}, const Vec2<T>& maxValue = Vec2<T>{}, const Vec2<T>& value = Vec2<T>{})
			: GuiSliderBoxVec<T, Vec2<T>, 2>(minValue, maxValue, value) {}
		/// Constructor from JSON
		GuiSliderBoxVec2(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiSliderBoxVec<T, Vec2<T>, 2>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSliderBoxVec3 : public GuiSliderBoxVec<T, Vec3<T>, 3>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER_VEC3";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderBoxVec3(const Vec3<T>& minValue = Vec3<T>{}, const Vec3<T>& maxValue = Vec3<T>{}, const Vec3<T>& value = Vec3<T>{})
			: GuiSliderBoxVec<T, Vec3<T>, 3>(minValue, maxValue, value) {}
		/// Constructor from JSON
		GuiSliderBoxVec3(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiSliderBoxVec<T, Vec3<T>, 3>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSliderBoxVec4 : public GuiSliderBoxVec<T, Vec4<T>, 4>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER_VEC4";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderBoxVec4(const Vec4<T>& minValue = Vec4<T>{}, const Vec4<T>& maxValue = Vec4<T>{}, const Vec4<T>& value = Vec4<T>{})
			: GuiSliderBoxVec<T, Vec4<T>, 4>(minValue, maxValue, value) {}
		/// Constructor from JSON
		GuiSliderBoxVec4(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiSliderBoxVec<T, Vec4<T>, 4>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	using GuiSliderBoxVec2f = GuiSliderBoxVec2<float>;
	using GuiSliderBoxVec2d = GuiSliderBoxVec2<double>;
	using GuiSliderBoxVec2i = GuiSliderBoxVec2<int>;
	//--------------------------------------------------------------------------------------------------
	using GuiSliderBoxVec3f = GuiSliderBoxVec3<float>;
	using GuiSliderBoxVec3d = GuiSliderBoxVec3<double>;
	using GuiSliderBoxVec3i = GuiSliderBoxVec3<int>;
	//--------------------------------------------------------------------------------------------------
	using GuiSliderBoxVec4f = GuiSliderBoxVec4<float>;
	using GuiSliderBoxVec4d = GuiSliderBoxVec4<double>;
	using GuiSliderBoxVec4i = GuiSliderBoxVec4<int>;
	//--------------------------------------------------------------------------------------------------
}