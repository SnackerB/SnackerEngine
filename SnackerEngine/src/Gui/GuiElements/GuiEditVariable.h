#pragma once

#include "Gui/GuiElement2.h"
#include "Gui/GuiElements/GuiTextBox2.h"
#include "Gui/GuiStyle.h"
#include "Gui/GuiManager2.h"
#include "Gui/GuiEventHandles/GuiEventHandle2.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariable : public GuiElement2
	{
		/// The handle to the variable that is shown in this textBox
		GuiVariableHandle2<T>* variableHandle;
		/// The GuiTextBox that shows the label
		std::unique_ptr<GuiDynamicTextBox2> label;
		/// The GuiEditBox that is used to edit the variable
		std::unique_ptr<GuiEditTextBox2> editBox;
		/// EventHandle that is used to detect when the text is edited
		struct EditTextEventHandle : public GuiEventHandle2
		{
			friend class GuiEditVariable<T>;
			GuiEditVariable<T>* guiEditVariable;
		protected:
			void onEvent() override
			{
				infoLogger << LOGGER::BEGIN << "edited!" << LOGGER::ENDL;
				if (guiEditVariable) guiEditVariable->checkEditBoxAndSetVariable();
			}
			/// Constructor
			EditTextEventHandle(GuiEditVariable<T> guiEditVariable)
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
		virtual void onHandleDestruction(GuiHandle2& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle2& guiHandle) override;
		/// Transforms the variable of the handle to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText();
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle2& guiHandle) override;
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const;
		/// Checks if the text in the editBox describes a valid variable. If this is the case, sets the variableHandle to the variable.
		/// If not, sets the text in the editBox to the variableHandle.
		void checkEditBoxAndSetVariable();
	public:
		/// Constructor
		GuiEditVariable(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const GuiElement2::ResizeMode& resizeMode = GuiElement2::ResizeMode::DO_NOT_RESIZE,
			const std::string& label = "", const Font& font = Font(), const double& fontSize = 0,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color4f& editTextColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& labelTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& editTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::Alignment& editTextAlignment = StaticText::Alignment::LEFT, const double& cursorWidth = 0.0);
		/// Constructor that already registers variable handle
		GuiEditVariable(GuiVariableHandle2<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode,
			const std::string& label, const Font& font, const double& fontSize,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color4f& editTextColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& labelTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& editTextBackgroundColor = = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::Alignment & editTextAlignment = StaticText::Alignment::LEFT, const double& cursorWidth = 0.0);
		/// Constructors using GuiStyle
		GuiEditVariable(const std::string& label, GuiVariableHandle2<T>& handle, const GuiStyle& style);
		GuiEditVariable(const std::string& label, const GuiStyle& style);
		GuiEditVariable(const std::string& label, const double& fontSize, GuiVariableHandle2<T>& handle, const GuiStyle& style);
		GuiEditVariable(const std::string& label, const double& fontSize, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle2<T>& variableHandle);
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
		Vec2i nextPosition = parentPosition + position;
		drawElement(label->getGuiID(), nextPosition);
		drawElement(editBox->getGuiID(), nextPosition);
		GuiElement2::draw(parentPosition);
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
		int editBoxWidth = getWidth() - label->getWidth();
		if (editBoxWidth < 0) editBoxWidth = 0;
		editBox->setPositionAndSize(Vec2i(label->getWidth(), 0), Vec2i(editBoxWidth, label->getHeight()));
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
			label->setText(toText());
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

		}
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& editTextColor, const Color4f& labelTextBackgroundColor, const Color4f& editTextBackgroundColor, const StaticText::Alignment& editTextAlignment, const double& cursorWidth)
		: GuiElement2(position, size, resizeMode),
		label(std::make_unique<GuiDynamicTextBox2>(Vec2i(0, 0), size, ResizeMode::DO_NOT_RESIZE, label, font, fontSize, labelTextColor, labelTextBackgroundColor, StaticText::ParseMode::CHARACTERS, StaticText::Alignment::LEFT, GuiDynamicTextBox2::TextBoxMode::SHRINK_TO_FIT, true)),
		editBox(std::make_unique<GuiEditTextBox2>(Vec2i(this->label->getWidth(), 0), Vec2i(this->label->getWidth() < size.x ? size.x - this->label->getWidth() : 0, this->label->getHeight()), ResizeMode::DO_NOT_RESIZE, "", font, fontSize, cursorWidth, editTextColor, editTextBackgroundColor, StaticText::ParseMode::CHARACTERS, StaticText::Alignment::CENTER, GuiDynamicTextBox2::TextBoxMode::FORCE_SIZE, true)),
		variableHandle(nullptr), editTextEventHandle(*this)
	{
		setSizeInternal(Vec2i(size.x, this->label->getHeight()));
		editBox->setEventHandleTextWasEdited(editTextEventHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(GuiVariableHandle2<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& editTextColor, const Color4f& labelTextBackgroundColor, const Color4f& editTextBackgroundColor, const StaticText::Alignment& editTextAlignment, const double& cursorWidth)
		: GuiEditVariable<T>(position, size, resizeMode, label, font, fontSize, labelTextColor, editTextColor, labelTextBackgroundColor, editTextBackgroundColor, editTextAlignment, cursorWidth)
	{
		setVariableHandle(handle);
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, GuiVariableHandle2<T>& handle, const GuiStyle& style)
		: GuiEditVariable<T>(handle, Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth) {}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, const GuiStyle& style)
		: GuiEditVariable<T>(Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth) {}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, const double& fontSize, GuiVariableHandle2<T>& handle, const GuiStyle& style)
		: GuiEditVariable<T>(Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth) {}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiEditVariable<T>(Vec2i(), style.guiEditVariableSize, style.guiEditVariableResizeMode, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiEditVariableEditTextAlignment, style.guiEditTextBoxCursorWidth) {}

	template<typename T>
	inline void GuiEditVariable<T>::setVariableHandle(GuiVariableHandle2<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			editBox->setText(toText());
		}
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const GuiEditVariable<T>& other) noexcept
		: GuiElement2(other), variableHandle(nullptr), label(std::make_unique<GuiDynamicTextBox2>(*other.label)), 
		editBox(std::make_unique<GuiEditTextBox2>(*other.editBox)), editTextEventHandle(*this) 
	{
		editBox->setEventHandleTextWasEdited(editTextEventHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(const GuiEditVariable<T>& other) noexcept
	{
		GuiElement2::operator=(other);
		variableHandle = nullptr;
		label = std::make_unique<GuiDynamicTextBox2>(*other.label);
		editBox = std::make_unique<GuiEditTextBox2>(*other.editBox);
		editTextEventHandle = EditTextEventHandle(*this);
		editBox->setEventHandleTextWasEdited(editTextEventHandle);
		return this;
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(GuiEditVariable<T>&& other) noexcept
		: GuiElement2(std::move(other)), variableHandle(nullptr), label(std::move(other.label)), editBox(std::move(other.editBox)), 
		editTextEventHandle(std::move(other.editTextEventHandle))
	{
		editTextEventHandle.guiEditVariable = this;
		other.variableHandle = nullptr;
		other.label = nullptr;
		other.editBox = nullptr;
		if (variableHandle) notifyHandleOnGuiElementMove(*variableHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(GuiEditVariable<T>&& other) noexcept
	{
		GuiElement2::operator=(std::move(other));
		variableHandle = other.variableHandle;
		label = std::move(other.label);
		editBox = std::move(other.editBox);
		editTextEventHandle = std::move(other.editTextEventHandle);
		editTextEventHandle.guiEditVariable = this;
		other.variableHandle = nullptr;
		other.label = nullptr;
		other.editBox = nullptr;
		if (variableHandle) notifyHandleOnGuiElementMove(*variableHandle);
		return *this;
	}

	template<typename T>
	inline GuiEditVariable<T>::~GuiEditVariable()
	{
		label = nullptr;
		editBox = nullptr;
		if (variableHandle) signOffHandle(*variableHandle);
	}

	template<typename T>
	inline GuiEditVariable<T>::GuiID GuiEditVariable<T>::getCollidingChild(const Vec2i& position)
	{
		IsCollidingResult result = GuiElement2::isColliding(editBox->getGuiID(), position - getPosition());
		if (result != IsCollidingResult::NOT_COLLIDING) {
			GuiID childCollision = GuiElement2::getCollidingChild(result, editBox->getGuiID(), position);
			if (childCollision > 0) return childCollision;
		}
		result = GuiElement2::isColliding(label->getGuiID(), position - getPosition());
		if (result != IsCollidingResult::NOT_COLLIDING) {
			GuiID childCollision = GuiElement2::getCollidingChild(result, label->getGuiID(), position);
			if (childCollision > 0) return childCollision;
		}
		return GuiElement2::getCollidingChild(position);
	}

	template<typename T>
	inline void GuiEditVariable<T>::onHandleDestruction(GuiHandle2& guiHandle)
	{
		variableHandle = nullptr;
	}

	template<typename T>
	inline void GuiEditVariable<T>::onHandleMove(GuiHandle2& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle2<T>*>(&guiHandle);
	}

	template<typename T>
	inline std::string GuiEditVariable<T>::toText()
	{
		return std::to_string(variableHandle->get());
	}

	template<typename T>
	inline void GuiEditVariable<T>::onHandleUpdate(GuiHandle2& guiHandle)
	{
		editBox->setText(toText());
	}

}