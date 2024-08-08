#pragma once

#include "Gui\Layouts\HorizontalWeightedLayout.h"
#include "Gui\GuiElements\GuiSlider.h"
#include "Gui\GuiElements\VectorElements\GuiVariableVec.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	class GuiSliderVec : public GuiVariableVec<T, VecT, N>
	{
	private:
		/// Helper function that creates a vector of GuiSliders from the given value
		void setupComponents();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "ERROR";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderVec(const VecT& minValue = VecT{}, const VecT& maxValue = VecT{}, const VecT& value = VecT{});
		/// Constructor from JSON
		GuiSliderVec(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
	public:
		/// Getters
		int getSliderButtonWidth() const { return static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[0])->getSliderButtonWidth(); }
		const Color4f& getSliderButtonColor() const { static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[0])->getSliderButtonColor(); }
		const Shader& getSliderButtonShader() const { static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[0])->getSliderButtonShader(); }
		VecT getMinValue() const {
			VecT result{};
			for (std::size_t i = 0; i < N; ++i) result.values[i] = static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[i])->getMinValue();
			return result;
		}
		VecT getMaxValue() const {
			VecT result{};
			for (std::size_t i = 0; i < N; ++i) result.values[i] = static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[i])->getMaxValue();
			return result;
		}
		const T& getValue(std::size_t index) const { return static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[index])->getValue(); }
		const T& getMinValue(std::size_t index) const { return static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[index])->getMinValue(); }
		const T& getMaxValue(std::size_t index) const { return static_cast<std::unique_ptr<GuiSlider<T>>>(this->components[index])->getMaxValue(); }
		/// Setters
		void setSliderButtonWidth(int sliderButtonWidth) {
			for (const auto& component : this->components) static_cast<GuiSlider<T>*>(component.get())->setSliderButtonWidth(sliderButtonWidth);
		}
		void setSliderButtonColor(const Color4f& sliderButtonColor) {
			for (const auto& component : this->components)  static_cast<GuiSlider<T>*>(component.get())->setSliderButtonColor(sliderButtonColor);
		}
		void setSliderButtonShader(const Shader& sliderButtonShader) {
			for (const auto& component : this->components) static_cast<GuiSlider<T>*>(component.get())->setSliderButtonShader(sliderButtonShader);
		}
		void setValue(const VecT& value) {
			for (std::size_t i = 0; i < N; ++i) static_cast<GuiSlider<T>*>(this->components[i].get())->setValue(value.values[i]);
		}
		void setMinValue(const VecT& minValue) {
			for (std::size_t i = 0; i < N; ++i)  static_cast<GuiSlider<T>*>(this->components[i].get())->setMinValue(minValue.values[i]);
		}
		void setMaxValue(const VecT& maxValue) {
			for (std::size_t i = 0; i < N; ++i)  static_cast<GuiSlider<T>*>(this->components[i].get())->setMaxValue(maxValue.values[i]);
		}
		void setMinValue(const T& minValue, std::size_t index) { static_cast<GuiSlider<T>*>(this->components[index].get())->setMinValue(minValue.values[index]); }
		void setMaxValue(const T& maxValue, std::size_t index) { static_cast<GuiSlider<T>*>(this->components[index].get())->setMaxValue(maxValue.values[index]); }
		/// Getters
		const Color4f& getSelectionBoxColor() const { return static_cast<GuiSlider<T>*>(this->components[0].get())->getSelectionBoxColor(); }
		bool isActive() const { for (const auto& component : this->components) if (static_cast<GuiSlider<T>*>(component.get())->isActive()) return true; return false; }
		float getCursorWidth() const { return static_cast<GuiSlider<T>*>(this->components[0].get())->getCursorWidth()(); }
		double getCursorBlinkTime() const { return static_cast<GuiSlider<T>*>(this->components[0].get())->getCursorBlinkTime(); }
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline void GuiSliderVec<T, VecT, N>::setupComponents()
	{
		this->components.clear();
		this->components.reserve(N);
		for (std::size_t i = 0; i < N; ++i) this->components.push_back(std::make_unique<GuiSlider<T>>());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiSliderVec<T, VecT, N>::GuiSliderVec(const VecT& minValue, const VecT& maxValue, const VecT& value)
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
	inline GuiSliderVec<T, VecT, N>::GuiSliderVec(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
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
	class GuiSliderVec2 : public GuiSliderVec<T, Vec2<T>, 2>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER_VEC2";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderVec2(const Vec2<T>& minValue = Vec2<T>{}, const Vec2<T>& maxValue = Vec2<T>{}, const Vec2<T>& value = Vec2<T>{})
			: GuiSliderVec<T, Vec2<T>, 2>(minValue, maxValue, value) {}
		/// Constructor from JSON
		GuiSliderVec2(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiSliderVec<T, Vec2<T>, 2>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSliderVec3 : public GuiSliderVec<T, Vec3<T>, 3>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER_VEC3";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderVec3(const Vec3<T>& minValue = Vec3<T>{}, const Vec3<T>& maxValue = Vec3<T>{}, const Vec3<T>& value = Vec3<T>{})
			: GuiSliderVec<T, Vec3<T>, 3>(minValue, maxValue, value) {}
		/// Constructor from JSON
		GuiSliderVec3(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiSliderVec<T, Vec3<T>, 3>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSliderVec4 : public GuiSliderVec<T, Vec4<T>, 4>
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER_VEC4";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSliderVec4(const Vec4<T>& minValue = Vec4<T>{}, const Vec4<T>& maxValue = Vec4<T>{}, const Vec4<T>& value = Vec4<T>{})
			: GuiSliderVec<T, Vec4<T>, 4>(minValue, maxValue, value) {}
		/// Constructor from JSON
		GuiSliderVec4(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
			: GuiSliderVec<T, Vec4<T>, 4>(json, data, parameterNames) {}
	};
	//--------------------------------------------------------------------------------------------------
	using GuiSliderVec2f = GuiSliderVec2<float>;
	using GuiSliderVec2d = GuiSliderVec2<double>;
	using GuiSliderVec2i = GuiSliderVec2<int>;
	//--------------------------------------------------------------------------------------------------
	using GuiSliderVec3f = GuiSliderVec3<float>;
	using GuiSliderVec3d = GuiSliderVec3<double>;
	using GuiSliderVec3i = GuiSliderVec3<int>;
	//--------------------------------------------------------------------------------------------------
	using GuiSliderVec4f = GuiSliderVec4<float>;
	using GuiSliderVec4d = GuiSliderVec4<double>;
	using GuiSliderVec4i = GuiSliderVec4<int>;
	//--------------------------------------------------------------------------------------------------
}