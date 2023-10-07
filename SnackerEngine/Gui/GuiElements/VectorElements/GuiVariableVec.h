#pragma once

#include "Gui\Layouts\HorizontalWeightedLayout.h"
#include "Gui\GuiElements\GuiTextBox.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Base class for all GuiElements that display or edit vectors of some kind. Base functionality included are basic setters/getters
	/// for GuiTextBox elements, and the handling of variableHandles.
	template<typename T, typename VecT, unsigned N>
	class GuiVariableVec : public GuiHorizontalWeightedLayout
	{
	public:
		/// Static default Attributes
		static unsigned defaultHorizontalBorder;
	private:
		/// variable Handle that updates the parent variableHandle
		friend class HelperVariableHandle;
		class HelperVariableHandle : public GuiVariableHandle<T>
		{
			friend class GuiVariableVec<T, VecT, N>;
		private:
			GuiVariableVec<T, VecT, N>* parentElement;
		protected:
			void onEvent() override
			{
				parentElement->updateValue();
			}
		public:
			/// Constructor
			HelperVariableHandle(GuiVariableVec<T, VecT, N>* parentElement, const T& value)
				: GuiVariableHandle<T>(value), parentElement(parentElement) {}
		};
	protected:
		/// The GuiTextBox elements that make up the individual components of the vector
		std::vector<std::unique_ptr<GuiTextBox>> components;
		/// The vector value
		VecT value{};
		/// The formatter used to format the text
		std::unique_ptr<Formatter<T>> formatter = nullptr;
		/// Variable handles to the components
		std::vector<HelperVariableHandle> variableHandles;
		/// The handle to the variable that is referenced in this element
		GuiVariableHandle<VecT>* variableHandle = nullptr;
		/// Helper function that computes the text from the current value
		void updateText();
		/// Helper function that updates the value
		void updateValue();
		/// Helper function that parses JSON after the components vector has been set by parent element
		void parseJSON(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "ERROR";
	protected:
		/// Default constructor
		GuiVariableVec(std::vector<std::unique_ptr<GuiTextBox>>&& components, const VecT& value = defaultValue);
		/// Constructor from JSON
		GuiVariableVec(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiVariableVec();
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
		StaticText::Alignment getAlignment() const { return components[0].getAlignment(); }
		const Font& getFont() const { return components[0].getFont(); }
		const VecT& getValue() const { return value; }
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
		void setAlignment(const StaticText::Alignment& alignment) {
			for (const auto& component : components) component->setAlignment(alignment);
		}
		void setFont(const Font& font) {
			for (const auto& component : components) component->setFont(font);
		}
		void setValue(const VecT& value) {
			this->value = value;
			updateText();
			if (variableHandle) variableHandle->set(value);
		}
	protected:
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;

		//==============================================================================================
		// GuiHandles
		//==============================================================================================

		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		void onHandleMove(GuiHandle& guiHandle) override;
		/// This function is called by a handle right before the handle is destroyed
		void onHandleDestruction(GuiHandle& guiHandle) override;
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		void onHandleUpdate(GuiHandle& guiHandle) override;
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiVariableVec<T, VecT, N>::updateText()
	{
		for (unsigned i = 0; i < N; ++i) variableHandles[i].set(value.values[i]);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiVariableVec<T, VecT, N>::updateValue()
	{
		for (unsigned i = 0; i < N; ++i) value.values[i] = variableHandles[i].get();
		if (variableHandle) variableHandle->set(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
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
		std::optional<StaticText::Alignment> alignment = parseJsonOrReadFromData<StaticText::Alignment>("alignment", json, data, parameterNames);
		if (alignment.has_value()) setAlignment(alignment.value());
		std::optional<Font> font = parseJsonOrReadFromData<Font>("font", json, data, parameterNames);
		if (font.has_value()) setFont(font.value());
		// JSON parsing for the value
		std::optional<VecT> value = parseJsonOrReadFromData<VecT>("value", json, data, parameterNames);
		if (value.has_value()) setValue(value.value());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(std::vector<std::unique_ptr<GuiTextBox>>&& components, const VecT& value)
		: GuiHorizontalWeightedLayout(), components(std::move(components)), value(value) 
	{
		// Initialize variableHandles
		variableHandles.reserve(N);
		for (unsigned i = 0; i < N; ++i) variableHandles.push_back(HelperVariableHandle(this, value.get(i)));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiHorizontalWeightedLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(value, "value", json, data, parameterNames);
		if (!json.contains("outerHorizontalBorder")) setOuterHorizontalBorder(0);
		if (!json.contains("horizontalBorder")) setHorizontalBorder(defaultHorizontalBorder);
		// Initialize variableHandles
		variableHandles.reserve(N);
		for (unsigned i = 0; i < N; ++i) variableHandles.push_back(HelperVariableHandle(this, value.values[i]));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>::~GuiVariableVec()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(const GuiVariableVec& other) noexcept
		: GuiHorizontalWeightedLayout(other), components{}, value(other.value),
		formatter(other.formatter == nullptr ? nullptr : std::make_unique<Formatter<T>>(*other.formatter)), 
		variableHandles{}, variableHandle(nullptr)
	{
		for (const auto& component : other.components) components.push_back(std::make_unique<GuiTextBox>(*component));
		variableHandles.reserve(N);
		for (unsigned i = 0; i < N; ++i) variableHandles.push_back(HelperVariableHandle(this, value.get(i)));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>& GuiVariableVec<T, VecT, N>::operator=(const GuiVariableVec& other) noexcept
	{
		GuiHorizontalWeightedLayout::operator=(other);
		components.clear();
		for (const auto& component : other.components) components.push_back(std::make_unique<GuiTextBox>(*component));
		value = other.value;
		formatter = other.formatter == nullptr ? nullptr : std::make_unique<Formatter<T>>(*other.formatter);
		variableHandles.clear();
		variableHandles.reserve(N);
		for (unsigned i = 0; i < N; ++i) variableHandles.push_back(HelperVariableHandle(this, value.get(i)));
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = nullptr;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>::GuiVariableVec(GuiVariableVec&& other) noexcept
		: GuiHorizontalWeightedLayout(std::move(other)), components(std::move(other.components)),
		value(std::move(other.value)), formatter(std::move(other.formatter)),
		variableHandles(std::move(other.variableHandles)), variableHandle(std::move(other.variableHandle))
	{
		for (unsigned i = 0; i < N; ++i) variableHandles[i].parentElement = this;
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline GuiVariableVec<T, VecT, N>& GuiVariableVec<T, VecT, N>::operator=(GuiVariableVec&& other) noexcept
	{
		GuiHorizontalWeightedLayout::operator=(std::move(other));
		components = std::move(other.components);
		value = std::move(other.value); 
		formatter = std::move(other.formatter);
		variableHandles.clear();
		variableHandles = std::move(other.variableHandles);
		for (unsigned i = 0; i < N; ++i) variableHandles[i].parentElement = this;
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = std::move(other.variableHandle);
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiVariableVec<T, VecT, N>::onRegister()
	{
		// Register all components with equal weight!
		for (const auto& component : components) registerChild(*component, 1.0f);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiVariableVec<T, VecT, N>::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle<VecT>*>(&guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiVariableVec<T, VecT, N>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, typename VecT, unsigned N>
	inline void GuiVariableVec<T, VecT, N>::onHandleUpdate(GuiHandle& guiHandle)
	{
		updateText();
	}
	//--------------------------------------------------------------------------------------------------
}