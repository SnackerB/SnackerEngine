#pragma once

#include "Gui/GuiElement.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiStyle.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"
#include "Math/Conversions.h"

#include <optional>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariable : public GuiElement
	{
		/// The handle to the variable that is shown in this textBox
		GuiVariableHandle<T>* variableHandle;
		/// The GuiTextBox that shows the label
		std::unique_ptr<GuiDynamicTextBox> label;
		/// The GuiEditBox that is used to edit the variable
		std::unique_ptr<GuiEditTextBox> editBox;
		/// EventHandle that is used to detect when the text is edited
		struct EditTextEventHandle : public GuiEventHandle
		{
			friend class GuiEditVariable<T>;
			GuiEditVariable<T>* guiEditVariable;
		protected:
			void onEvent() override
			{
				if (guiEditVariable) guiEditVariable->checkEditBoxAndSetVariable();
			}
			/// Constructor
			EditTextEventHandle(GuiEditVariable<T>& guiEditVariable)
				: guiEditVariable(&guiEditVariable) {}
		};
		/// The EditTextEventHandle of this GuiEditVariableObject
		EditTextEventHandle editTextEventHandle;
	protected:
		friend EditTextEventHandle;
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Returns how the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Returns the first colliding child which collides with the given position vector. The position
		/// vector is relative to the top left corner of the parent. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& position) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// Transforms the value to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText(const T& value);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) override;
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const;
		/// Checks if the text in the editBox describes a valid variable. If this is the case, sets the variableHandle to the variable.
		/// If not, sets the text in the editBox to the variableHandle.
		void checkEditBoxAndSetVariable();
		/// Takes a string and returns the value it represents, if possible
		virtual std::optional<T> toValue(const std::string& text);
	public:
		/// Constructor
		GuiEditVariable(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const GuiElement::ResizeMode& resizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE,
			const std::string& label = "", const Font& font = Font(), const double& fontSize = 0,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color4f& editTextColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& labelTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& editTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const Color4f& editTextSelectionBoxColor = { 0.0f, 0.0f, 1.0f, 1.0f },
			const StaticText::Alignment& editTextAlignment = StaticText::Alignment::LEFT, const double& cursorWidth = 0.0,
			const int& border = 0);
		/// Constructor that already registers variable handle
		GuiEditVariable(GuiVariableHandle<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode,
			const std::string& label, const Font& font, const double& fontSize,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color4f& editTextColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& labelTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& editTextBackgroundColor = = { 0.0f, 0.0f, 0.0f, 0.0f },
			const Color4f& editTextSelectionBoxColor = { 0.0f, 0.0f, 1.0f, 1.0f },
			const StaticText::Alignment & editTextAlignment = StaticText::Alignment::LEFT, const double& cursorWidth = 0.0,
			const int& border = 0);
		/// Constructors using GuiStyle
		GuiEditVariable(const std::string& label, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiEditVariable(const std::string& label, const GuiStyle& style);
		GuiEditVariable(const std::string& label, const double& fontSize, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiEditVariable(const std::string& label, const double& fontSize, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle<T>& variableHandle);
		/// Copy constructor and assignment operator
		GuiEditVariable(const GuiEditVariable<T>& other) noexcept;
		GuiEditVariable& operator=(const GuiEditVariable<T>& other) noexcept;
		/// Move constructor and assignment operator
		GuiEditVariable(GuiEditVariable<T>&& other) noexcept;
		GuiEditVariable& operator=(GuiEditVariable<T>&& other) noexcept;
		/// Destructor
		~GuiEditVariable();
	};


	template<typename T>
	inline void GuiEditVariable<T>::draw(const Vec2i& parentPosition)
	{
		Vec2i nextPosition = parentPosition + getPosition();
		drawElement(label->getGuiID(), nextPosition);
		drawElement(editBox->getGuiID(), nextPosition);
		GuiElement::draw(parentPosition);
	}

	template<typename T>
	inline void GuiEditVariable<T>::onRegister()
	{
		registerElementAsChild(*label);
		registerElementAsChild(*editBox);
	}

	template<typename T>
	inline void GuiEditVariable<T>::onSizeChange()
	{
		if (label && editBox) {
			label->setWidth(std::min(label->getPreferredWidth(), getWidth()));
			label->setHeight(std::min(label->getPreferredHeight(), getHeight()));
			int editBoxWidth = std::max(0, getWidth() - label->getWidth());
			editBox->setPositionAndSize(Vec2i(label->getWidth(), 0), Vec2i(editBoxWidth, label->getHeight()));
			setMinSize(label->getMinSize());
			setPreferredHeight(label->getPreferredHeight());
		}
		else if (editBox) {
			int editBoxHeight = std::min(editBox->getPreferredHeight(), getHeight());
			editBox->setPositionAndSize(Vec2i(0, 0), Vec2i(getWidth(), editBoxHeight));
			setPreferredHeight(editBox->getPreferredHeight());
		}
		setPreferredWidth(-1);
		setMaxSize(Vec2i(-1, -1));
	}

	template<typename T>
	inline GuiEditVariable<T>::IsCollidingResult GuiEditVariable<T>::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_CHILD : IsCollidingResult::NOT_COLLIDING;
	}

	template<typename T>
	inline void GuiEditVariable<T>::setVariable(const T& value)
	{
		if (variableHandle) {
			setVariableHandleValue<T>(*variableHandle, value);
		}
	}

	template<typename T>
	inline const T& GuiEditVariable<T>::getVariable() const
	{
		if (variableHandle) return variableHandle->get();
		return T{};
	}

	template<typename T>
	inline void GuiEditVariable<T>::checkEditBoxAndSetVariable()
	{
		if (editBox) {
			auto result = toValue(editBox->getText());
			if (result) {
				editBox->setText(toText(result.value()));
				setVariable(result.value());
			}
			else {
				editBox->setText(toText(getVariable()));
			}
		}
	}

	template<typename T>
	inline std::optional<T> GuiEditVariable<T>::toValue(const std::string& text)
	{
		return convertFromString<T>(text);
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& editTextColor, const Color4f& labelTextBackgroundColor, const Color4f& editTextBackgroundColor, const Color4f& editTextSelectionBoxColor, const StaticText::Alignment& editTextAlignment, const double& cursorWidth, const int& border)
		: GuiElement(position, size, resizeMode),
		label(std::make_unique<GuiDynamicTextBox>(Vec2i(0, 0), size, ResizeMode::DO_NOT_RESIZE, label, font, fontSize, labelTextColor, labelTextBackgroundColor, StaticText::ParseMode::SINGLE_LINE, StaticText::Alignment::LEFT, border, GuiEditTextBox::TextScaleMode::DONT_SCALE, GuiEditTextBox::SizeHintModes(GuiEditTextBox::SizeHintMode::SET_TO_TEXT_SIZE), false)),
		editBox(std::make_unique<GuiEditTextBox>(Vec2i(this->label->getWidth(), 0), Vec2i(this->label->getWidth() < size.x ? size.x - this->label->getWidth() : 0, this->label->getHeight()), ResizeMode::DO_NOT_RESIZE, "", font, fontSize, cursorWidth, editTextColor, editTextBackgroundColor, editTextSelectionBoxColor, StaticText::ParseMode::SINGLE_LINE, StaticText::Alignment::CENTER, border, GuiEditTextBox::TextScaleMode::DONT_SCALE, GuiEditTextBox::SizeHintModes(GuiEditTextBox::SizeHintMode::ARBITRARY), false)),
		variableHandle(nullptr), editTextEventHandle(*this)
	{
		editBox->setEventHandleTextWasEdited(editTextEventHandle);
		onSizeChange();
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(GuiVariableHandle<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& editTextColor, const Color4f& labelTextBackgroundColor, const Color4f& editTextBackgroundColor, const Color4f& editTextSelectionBoxColor, const StaticText::Alignment& editTextAlignment, const double& cursorWidth, const int& border)
		: GuiEditVariable<T>(position, size, resizeMode, label, font, fontSize, labelTextColor, editTextColor, labelTextBackgroundColor, editTextBackgroundColor, editTextSelectionBoxColor, editTextAlignment, cursorWidth, border)
	{
		setVariableHandle(handle);
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiEditVariable<T>(handle, Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditTextBoxSelectionBoxColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth, style.guiTextBoxBorder) {}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, const GuiStyle& style)
		: GuiEditVariable<T>(Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditTextBoxSelectionBoxColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth, style.guiTextBoxBorder) {}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, const double& fontSize, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiEditVariable<T>(Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditTextBoxSelectionBoxColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth, style.guiTextBoxBorder) {}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiEditVariable<T>(Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditTextBoxSelectionBoxColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth, style.guiTextBoxBorder) {}

	template<typename T>
	inline void GuiEditVariable<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			editBox->setText(toText(variableHandle.get()));
		}
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const GuiEditVariable<T>& other) noexcept
		: GuiElement(other), variableHandle(nullptr), label(std::make_unique<GuiDynamicTextBox>(*other.label)), 
		editBox(std::make_unique<GuiEditTextBox>(*other.editBox)), editTextEventHandle(*this) 
	{
		editBox->setEventHandleTextWasEdited(editTextEventHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(const GuiEditVariable<T>& other) noexcept
	{
		GuiElement::operator=(other);
		variableHandle = nullptr;
		label = std::make_unique<GuiDynamicTextBox>(*other.label);
		editBox = std::make_unique<GuiEditTextBox>(*other.editBox);
		editTextEventHandle = EditTextEventHandle(*this);
		editBox->setEventHandleTextWasEdited(editTextEventHandle);
		return this;
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(GuiEditVariable<T>&& other) noexcept
		: GuiElement(std::move(other)), variableHandle(other.variableHandle), label(std::move(other.label)), editBox(std::move(other.editBox)), 
		editTextEventHandle(std::move(other.editTextEventHandle))
	{
		editTextEventHandle.guiEditVariable = this;
		other.variableHandle = nullptr;
		other.label = nullptr;
		other.editBox = nullptr;
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(GuiEditVariable<T>&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		variableHandle = other.variableHandle;
		label = std::move(other.label);
		editBox = std::move(other.editBox);
		editTextEventHandle = std::move(other.editTextEventHandle);
		editTextEventHandle.guiEditVariable = this;
		other.variableHandle = nullptr;
		other.label = nullptr;
		other.editBox = nullptr;
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
		return *this;
	}

	template<typename T>
	inline GuiEditVariable<T>::~GuiEditVariable()
	{
		if (label) signOffWithoutNotifyingParents(label->getGuiID());
		label = nullptr;
		if (editBox) signOffWithoutNotifyingParents(editBox->getGuiID());
		editBox = nullptr;
		if (variableHandle) signOffHandle(*variableHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiID GuiEditVariable<T>::getCollidingChild(const Vec2i& position)
	{
		IsCollidingResult result = GuiElement::isColliding(editBox->getGuiID(), position - getPosition());
		if (result != IsCollidingResult::NOT_COLLIDING) {
			GuiID childCollision = GuiElement::getCollidingChild(result, editBox->getGuiID(), position);
			if (childCollision > 0) return childCollision;
		}
		result = GuiElement::isColliding(label->getGuiID(), position - getPosition());
		if (result != IsCollidingResult::NOT_COLLIDING) {
			GuiID childCollision = GuiElement::getCollidingChild(result, label->getGuiID(), position);
			if (childCollision > 0) return childCollision;
		}
		return GuiElement::getCollidingChild(position);
	}

	template<typename T>
	inline void GuiEditVariable<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}

	template<typename T>
	inline void GuiEditVariable<T>::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle<T>*>(&guiHandle);
	}

	template<typename T>
	inline std::string GuiEditVariable<T>::toText(const T& value)
	{
		return convertToString(value);
	}

	template<typename T>
	inline void GuiEditVariable<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		editBox->setText(toText(variableHandle->get()));
	}

}