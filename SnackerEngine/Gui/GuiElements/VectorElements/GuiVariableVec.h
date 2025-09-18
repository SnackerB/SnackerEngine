#pragma once

#include "Gui\Layouts\HorizontalWeightedLayout.h"
#include "Gui\GuiElements\GuiTextBox.h"
#include "Utility\Handles\VariableHandle.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Base class for all GuiElements that display or edit vectors of some kind. Base functionality included are basic setters/getters
	/// for GuiTextBox elements, and the handling of variableHandles.
	template<typename T, typename VecT, std::size_t N>
	class GuiVariableVec : public GuiHorizontalWeightedLayout
	{
	public:
		/// Static default Attributes
		static unsigned defaultHorizontalBorder;
	private:
		/// Variable handle for the full vector
		class GuiVariableVecVariableHandle : public VariableHandle<VecT>
		{
		private:
			friend class GuiVariableVec;
			GuiVariableVec* parentElement;
		protected:
			void onEvent() override
			{
				VariableHandle<VecT>::onEvent();
				parentElement->updateChildElements();
			}
		public:
			GuiVariableVecVariableHandle(GuiVariableVec* parentElement)
				: VariableHandle<VecT>(), parentElement(parentElement) {}
			void set(const T& value, std::size_t index) {
				if (this->value[static_cast<unsigned int>(index)] != value) {
					this->value[static_cast<unsigned int>(index)] = value;
					this->notifyAllConnectedHandles();
				}
			}
		};
		/// Variable handle for individual components
		class GuiVariableVecComponentVariableHandle : public VariableHandle<T>
		{
		private:
			friend class GuiVariableVec;
			GuiVariableVec* parentElement;
		protected:
			void onEvent() override
			{
				parentElement->updateValue();
			}
		public:
			GuiVariableVecComponentVariableHandle(GuiVariableVec* parentElement, const T& value)
				: VariableHandle<T>(value), parentElement(parentElement) {}
		};
	protected:
		/// Variable handle for the full vector
		GuiVariableVecVariableHandle value{ this };
		/// The GuiTextBox elements that make up the individual components of the vector
		std::vector<std::unique_ptr<GuiTextBox>> components;
		/// The formatter used to format the text
		std::unique_ptr<Formatter<T>> formatter = nullptr;
		/// Variable handles to the components
		std::vector<GuiVariableVecComponentVariableHandle> componentVariableHandles;
		/// Helper function that updates all child elements when the value changes
		void updateChildElements();
		/// Helper function that updates the value
		void updateValue();
		/// Helper function that parses JSON after the components vector has been set by parent element
		void parseJSON(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "ERROR";
		virtual std::string_view getTypeName() const override { return typeName; }
	protected:
		/// Default constructor
		GuiVariableVec(const VecT& value = VecT{});
		/// Constructor from JSON
		GuiVariableVec(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Copy constructor and assignment operator
		GuiVariableVec(const GuiVariableVec& other) noexcept;
		GuiVariableVec& operator=(const GuiVariableVec& other) noexcept;
		/// Move constructor and assignment operator
		GuiVariableVec(GuiVariableVec&& other) noexcept;
		GuiVariableVec& operator=(GuiVariableVec&& other) noexcept;
	public:
		/// Getters
		Color4f getBackgroundColor() const { return components[0].getBackgroundColor(); }
		const Shader& getPanelShader() const { return components[0].getPanelShader(); }
		Vec2i getTextSize() const { return components[0].getTextSize(); }
		const Color4f& getTextColor() const { return components[0].getTextColor(); }
		GuiTextBox::TextScaleMode getTextScaleMode() const { return components[0].getTextScaleMode(); }
		const GuiTextBox::SizeHintModes& getSizeHintModes() const { return components[0].getSizeHintModes(); }
		const int getBorder() const { return components[0].getBorder(); }
		double getFontSize() const { return components[0].getFontSize(); }
		int getRecomputeTries() const { return components[0].getRecomputeTries(); }
		bool isDoRecomputeOnSizeChange() const { return components[0].isDoRecomputeOnSizeChange(); }
		StaticText::ParseMode getParseMode() const { return components[0].getParseMode(); }
		AlignmentHorizontal getAlignmentHorizontal() const { return components[0].getAlignmentHorizontal(); }
		AlignmentVertical getAlignmentVertical() const { return components[0].getAlignmentVertical(); }
		const Font& getFont() const { return components[0].getFont(); }
		const VecT& getValue() const { return value; }
		VariableHandle<VecT>& getVariableHandle() { return value; }
		/// Setters
		void setBackgroundColor(const Color4f& backgroundColor) {
			for (const auto& component : components) component->setBackgroundColor(backgroundColor);
		}
		void setTextColor(const Color4f& textColor) {
			for (const auto& component : components) component->setTextColor(textColor);
		}
		void setTextScaleMode(GuiTextBox::TextScaleMode textScaleMode) {
			for (const auto& component : components) component->setTextScaleMode(textScaleMode);
		}
		void setSizeHintModeMinSize(const GuiTextBox::SizeHintMode& sizeHintModeMinSize) {
			for (const auto& component : components) component->setSizeHintModeMinSize(sizeHintModeMinSize);
		}
		void setSizeHintModeMaxSize(const GuiTextBox::SizeHintMode& sizeHintModeMaxSize) {
			for (const auto& component : components) component->setSizeHintModeMaxSize(sizeHintModeMaxSize);
		}
		void setSizeHintModePreferredSize(const GuiTextBox::SizeHintMode& sizeHintModePreferredSize) {
			for (const auto& component : components) component->setSizeHintModePreferredSize(sizeHintModePreferredSize);
		}
		void setSizeHintModes(const GuiTextBox::SizeHintModes& sizeHintModes) {
			for (const auto& component : components) component->setSizeHintModes(sizeHintModes);
		}
		void setBorder(const int& border) {
			for (const auto& component : components) component->setBorder(border);
		}
		void setFontSize(const double& fontSize) {
			for (const auto& component : components) component->setFontSize(fontSize);
		}
		void setRecomputeTries(int recomputeTries) {
			for (const auto& component : components) component->setRecomputeTries(recomputeTries);
		}
		void setDoRecomputeOnSizeChange(const bool& doRecomputeOnSizeChange) {
			for (const auto& component : components) component->setDoRecomputeOnSizeChange(doRecomputeOnSizeChange);
		}
		void setText(const std::string& text) {
			for (const auto& component : components) component->setText(text);
		}
		void setParseMode(const StaticText::ParseMode& parseMode) {
			for (const auto& component : components) component->setParseMode(parseMode);
		}
		void setAlignmentHorizontal(AlignmentHorizontal alignment) {
			for (const auto& component : components) component->setAlignmentHorizontal(alignment);
		}
		void setAlignmentVertical(AlignmentVertical alignment) {
			for (const auto& component : components) component->setAlignmentVertical(alignment);
		}
		void setFont(const Font& font) {
			for (const auto& component : components) component->setFont(font);
		}
		void setValue(const VecT& value) {
			static_cast<VariableHandle<VecT>&>(this->value).set(value);
			updateChildElements();
		}
	protected:
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline void GuiVariableVec<T, VecT, N>::updateChildElements()
	{
		for (std::size_t i = 0; i < N; ++i) {
			componentVariableHandles[static_cast<unsigned int>(i)].set(value.get()[static_cast<unsigned int>(i)]);
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline void GuiVariableVec<T, VecT, N>::updateValue()
	{
		for (std::size_t i = 0; i < N; ++i) value.set(componentVariableHandles[i].get(), i);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline void GuiVariableVec<T, VecT, N>::parseJSON(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		// JSON parsing for GuiPanel
		std::optional<Color4f> backgroundColor = parseJsonOrReadFromData<Color4f>("backgroundColor", json, data, parameterNames);
		if (backgroundColor.has_value()) setBackgroundColor(backgroundColor.value());
		// JSON parsing for GuiTextBox
		std::optional<Color4f> textColor = parseJsonOrReadFromData<Color4f>("textColor", json, data, parameterNames);
		if (textColor.has_value()) setTextColor(textColor.value());
		std::optional<GuiTextBox::TextScaleMode> textScaleMode = parseJsonOrReadFromData<GuiTextBox::TextScaleMode>("textScaleMode", json, data, parameterNames);
		if (textScaleMode.has_value()) setTextScaleMode(textScaleMode.value());
		std::optional<GuiTextBox::SizeHintMode> sizeHintModeMinSize = parseJsonOrReadFromData<GuiTextBox::SizeHintMode>("sizeHintModeMinSize", json, data, parameterNames);
		if (sizeHintModeMinSize.has_value()) setSizeHintModeMinSize(sizeHintModeMinSize.value());
		std::optional<GuiTextBox::SizeHintMode> sizeHintModeMaxSize = parseJsonOrReadFromData<GuiTextBox::SizeHintMode>("sizeHintModeMaxSize", json, data, parameterNames);
		if (sizeHintModeMaxSize.has_value()) setSizeHintModeMaxSize(sizeHintModeMaxSize.value());
		std::optional<GuiTextBox::SizeHintMode> sizeHintModePreferredSize = parseJsonOrReadFromData<GuiTextBox::SizeHintMode>("sizeHintModePreferredSize", json, data, parameterNames);
		if (sizeHintModePreferredSize.has_value()) setSizeHintModePreferredSize(sizeHintModePreferredSize.value());
		std::optional<int> border = parseJsonOrReadFromData<int>("border", json, data, parameterNames);
		if (border.has_value()) setBorder(border.value());
		std::optional<double> fontSize = parseJsonOrReadFromData<double>("fontSize", json, data, parameterNames);
		if (fontSize.has_value()) setFontSize(fontSize.value());
		std::optional<unsigned> recomputeTries = parseJsonOrReadFromData<unsigned>("recomputeTries", json, data, parameterNames);
		if (recomputeTries.has_value()) setRecomputeTries(recomputeTries.value());
		std::optional<bool> doRecomputeOnSizeChange = parseJsonOrReadFromData<bool>("doRecomputeOnSizeChange", json, data, parameterNames);
		if (doRecomputeOnSizeChange.has_value()) setDoRecomputeOnSizeChange(doRecomputeOnSizeChange.value());
		std::optional<std::string> text = parseJsonOrReadFromData<std::string>("text", json, data, parameterNames);
		if (text.has_value()) setText(text.value());
		std::optional<StaticText::ParseMode> parseMode = parseJsonOrReadFromData<StaticText::ParseMode>("parseMode", json, data, parameterNames);
		if (parseMode.has_value()) setParseMode(parseMode.value());
		std::optional<AlignmentHorizontal> alignmentHorizontal = parseJsonOrReadFromData<AlignmentHorizontal>("alignmentHorizontal", json, data, parameterNames);
		if (alignmentHorizontal.has_value()) setAlignmentHorizontal(alignmentHorizontal.value());
		std::optional<AlignmentVertical> alignmentVertical = parseJsonOrReadFromData<AlignmentVertical>("alignmentVertical", json, data, parameterNames);
		if (alignmentVertical.has_value()) setAlignmentVertical(alignmentVertical.value());
		std::optional<Font> font = parseJsonOrReadFromData<Font>("font", json, data, parameterNames);
		if (font.has_value()) setFont(font.value());
		// JSON parsing for the value
		std::optional<VecT> value = parseJsonOrReadFromData<VecT>("value", json, data, parameterNames);
		if (value.has_value()) setValue(value.value());
		else setValue(this->value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(const VecT& value)
		: GuiHorizontalWeightedLayout(), components(), value(value) 
	{
		setOuterHorizontalBorder(0);
		setHorizontalBorder(defaultHorizontalBorder);
		setResizeMode(ResizeMode::RESIZE_RANGE);
		setShrinkHeightToChildren(true);
		// Initialize variableHandles
		componentVariableHandles.reserve(N);
		for (std::size_t i = 0; i < N; ++i) componentVariableHandles.push_back(GuiVariableVecComponentVariableHandle(this, value.get(i)));
		updateChildElements();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiHorizontalWeightedLayout(json, data, parameterNames)
	{
		std::optional<VecT> value = parseJsonOrReadFromData<VecT>("value", json, data, parameterNames);
		if (value.has_value()) static_cast<VariableHandle<VecT>&>(this->value).set(value.value());
		if (!json.contains("outerHorizontalBorder")) setOuterHorizontalBorder(0);
		if (!json.contains("horizontalBorder")) setHorizontalBorder(defaultHorizontalBorder);
		if (!json.contains("resizeMode")) setResizeMode(ResizeMode::RESIZE_RANGE);
		if (!json.contains("shrinkHeightToChildren")) setShrinkHeightToChildren(true);
		// Initialize variableHandles
		componentVariableHandles.reserve(N);
		for (std::size_t i = 0; i < N; ++i) componentVariableHandles.push_back(GuiVariableVecComponentVariableHandle(this, this->value.get()[static_cast<unsigned int>(i)]));
		updateChildElements();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(const GuiVariableVec& other) noexcept
		: GuiHorizontalWeightedLayout(other), value(other.value), components(other.components),
		formatter(other.formatter == nullptr ? nullptr : std::make_unique<Formatter<T>>(*other.formatter)), 
		componentVariableHandles{}
	{
		componentVariableHandles.reserve(N);
		for (std::size_t i = 0; i < N; ++i) componentVariableHandles.push_back(GuiVariableVecComponentVariableHandle(this, value.get(i)));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiVariableVec<T, VecT, N>& GuiVariableVec<T, VecT, N>::operator=(const GuiVariableVec& other) noexcept
	{
		GuiHorizontalWeightedLayout::operator=(other);
		value = other.value; 
		components = other.components;
		formatter = other.formatter == nullptr ? nullptr : std::make_unique<Formatter<T>>(*other.formatter);
		componentVariableHandles.reserve(N);
		for (std::size_t i = 0; i < N; ++i) componentVariableHandles.push_back(GuiVariableVecComponentVariableHandle(this, value.get(i)));
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(GuiVariableVec&& other) noexcept
		: GuiHorizontalWeightedLayout(std::move(other)), components(std::move(other.components)),
		value(std::move(other.value)), formatter(std::move(other.formatter)),
		componentVariableHandles(std::move(other.componentVariableHandles))
	{
		for (std::size_t i = 0; i < N; ++i) componentVariableHandles[i].parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline GuiVariableVec<T, VecT, N>& GuiVariableVec<T, VecT, N>::operator=(GuiVariableVec&& other) noexcept
	{
		GuiHorizontalWeightedLayout::operator=(std::move(other));
		components = std::move(other.components);
		value = std::move(other.value); 
		formatter = std::move(other.formatter);
		componentVariableHandles = std::move(other.variableHandles);
		for (std::size_t i = 0; i < N; ++i) componentVariableHandles[i].parentElement = this;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, std::size_t N>
	inline void GuiVariableVec<T, VecT, N>::onRegister()
	{
		// Register all components with equal weight!
		for (const auto& component : components) registerChild(*component, 1.0f);
	}
}