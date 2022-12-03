#pragma once

#include "Gui/Text/Font.h"
#include "Math/Vec.h"
#include "Graphics/Model.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	using Unicode = unsigned;
	//--------------------------------------------------------------------------------------------------
	/// A StaticText object computes the text model once during construction. No parameters of the text
	/// can be changed later, including fontSize, textwidth, the text itself, the font, etc.
	class StaticText
	{
	protected:
		/// friend declaration for data structure that is used during parsing
		friend struct ParseData;
		/// Helper struct that saves information about a single character
		struct Character
		{
			/// The unicode codepoint of the character
			Unicode codepoint;
			/// The position of the left and right edge of a character in a left aligned context
			double left;
			double right;
		};
		/// Helper struct that saves information about a single line
		struct Line
		{
			/// The y position of the baseline
			double baselineY;
			/// indices into the unicodeCharacters vector (begin and end, inclusive)
			/// line includes characters in [beginIndex, endIndex]
			unsigned beginIndex, endIndex;
			/// Comparison operators for searching a given index
			bool operator<(const Line& other) const;
			bool operator>(const Line& other) const;
			bool operator==(const Line& other) const;
		};
	public:
		/// Enum to specify how text is parsed and displayed
		enum class ParseMode
		{
			WORD_BY_WORD,	/// The parser tries to not split words and moves them to the next line if they are too long
			CHARACTERS,		/// The parser does not care about words and just parses each character
			SINGLE_LINE,	/// Same as characters, but never goes to a new line!
		};
		/// Enum for the different horitontal alignment modes a StaticText object can use
		enum class Alignment
		{
			LEFT,
			CENTER,
			RIGHT,
		};
	protected:
		/// The model used for rendering
		Model model;
		/// Constructs the model using the given parameters and using parse mode 'CHARACTERS'
		Model parseTextCharacters(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const Alignment& alignment = Alignment::LEFT);
		/// Constructs the model using the given parameters and using parse mode 'WORD_BY_WORD'
		Model parseTextWordByWord(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const Alignment& alignment = Alignment::LEFT);
		/// Constructs the model from the text member variable using parse mode 'SINGLE_LINE'
		Model parseTextSingleLine(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const Alignment& alignment = Alignment::LEFT);
		/// Constructs the model from the text member variable
		virtual void constructModel(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const ParseMode& parseMode = ParseMode::WORD_BY_WORD, const Alignment& alignment = Alignment::LEFT);
	public:
		/// Default constructor
		StaticText();
		/// Constuctor using a string and various parameters
		StaticText(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const ParseMode& parseMode = ParseMode::WORD_BY_WORD, const Alignment& alignment = Alignment::LEFT);
		/// Copy and Move constructors
		StaticText(const StaticText& other) noexcept;
		StaticText(StaticText&& other) noexcept;
	};
	//--------------------------------------------------------------------------------------------------
	/// A DynamicText object does everything a StaticText object does, but has functions that can be used
	/// to change the parameters of the text, in which case the text model may be recomputed
	class DynamicText : public StaticText
	{
	protected:
		/// The font in which the text is written
		Font font;
		/// The font size in pt
		double fontSize;
		/// The width of the text in pt
		double textWidth;
		/// The contents of the text
		std::string text;
		/// The mode used while parsing
		ParseMode parseMode;
		/// The alignment of the text
		Alignment alignment;
		/// Vector of unicode characters
		std::vector<Character> characters;
		/// Vector of lines
		std::vector<Line> lines;
		/// Constructs the model from the text member variable using parse mode 'CHARACTERS'
		virtual Model parseTextCharacters();
		/// Constructs the model from the text member variable using parse mode 'WORD_BY_WORD'
		virtual Model parseTextWordByWord();
		/// Constructs the model from the text member variable using parse mode 'SINGLE_LINE'
		virtual Model parseTextSingleLine();
		/// Constructs the model from the text member variable
		virtual void constructModel();
	public:
		/// Default constructor
		DynamicText();
		/// Constuctor using a string and various parameters
		DynamicText(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const ParseMode& parseMode = ParseMode::WORD_BY_WORD, const Alignment& alignment = Alignment::LEFT);
		/// Copy and Move constructors
		DynamicText(const DynamicText& other) noexcept;
		DynamicText(DynamicText&& other) noexcept;
		/// Getters
		const Font& getFont() const;
		const double& getFontSize() const;
		const double& getTextWidth() const;
		virtual const std::string& getText();
		const Model& getModel() const;
		const ParseMode& getParseMode() const;
		const Alignment& getAlignment() const;
		/// Returns the y coordinate of the top of the text (in pt)
		/// The 0 coordinate is the baseline of the first character
		double getTop();
		/// Returns the y coordinate of the bottom of the text (in pt)
		/// The 0 coordinate is the baseline of the first character
		double getBottom();
		/// Returns the x coordinate of the left-most point in the text (in pt)
		double getLeft();
		/// Returns the x coordinate of the right-most point in the text (in pt)
		double getRight();
		/// Sets the contents of the text. Needs to recompute the text model.
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setText(const std::string& text, bool recompute = true);
		/// Sets the textwidth. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setTextWidth(const double& textWidth, bool recompute = true);
		/// Sets the font size. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setFontSize(const double& fontSize, bool recompute = true);
		/// Sets the font. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setFont(const Font& font, bool recompute = true);
		/// Sets the parse mode. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setParseMode(const StaticText::ParseMode& parseMode, bool recompute = true);
		/// Sets the alignment.
		virtual void setAlignment(const StaticText::Alignment& alignment, bool recompute = true);
	};
	//--------------------------------------------------------------------------------------------------
	/// A EditableText object does everything a DynamicText object does, but has functions that can be used
	/// to control the cursor position and add/remove characters
	class EditableText : public DynamicText
	{
	public:
		/// Struct that is used as a return type for getSelectionBoxes().
		struct SelectionBox
		{
			/// position of the lower left corner of the selection box
			Vec2f position;
			/// size of the selection box
			Vec2f size;
		};
	protected:
		/// If this bool is set to true, the UTF-8 encoded variable text is up-to-date
		bool textIsUpToDate;
		/// The vector of vertices. Contains positions and texture coordinates
		std::vector<Vec4f> vertices;
		/// Vector of indices
		std::vector<unsigned int> indices;
		/// index of the character which currently comes after the cursor
		unsigned cursorPosIndex;
		/// index of one end of a text selection. Text selection goes from selectionIndex to cursorPosIndex
		unsigned selectionIndex;
		/// Position of the cursor
		Vec2f cursorPos;
		/// Size of the cursor
		Vec2f cursorSize;
		/// Finds the line number of the given unicode character
		unsigned int getLineNumber(const unsigned int& characterIndex) const;
		/// Constructs the model from the text member variable using parse mode 'CHARACTERS'
		Model parseTextCharacters() override;
		/// Constructs the model from the text member variable using parse mode 'WORD_BY_WORD'
		Model parseTextWordByWord() override;
		/// Constructs the model from the text member variable using parse mode 'CHARACTERS', starting at the line with the given index
		Model parseTextCharactersFrom(const unsigned int& lineIndex);
		/// Constructs the model from the text member variable using parse mode 'WORD_BY_WORD', starting at the line with the given index
		Model parseTextWordByWordFrom(const unsigned int& lineIndex);
		/// Constructs the model from the text member variable using parse mode 'SINGLE_LINE', starting at the line with the given index
		Model parseTextSingleLineFrom(const unsigned int& lineIndex);
		/// Constructs the model from the text member variable
		void constructModelFrom(const unsigned int& lineIndex);
		/// Constructs the model from the text member variable
		void constructModel() override;
		/// Helper function to construct the text variable from the characters vector
		void constructTextFromCharacters();
		/// Helper function used in getSelectionBoxes(), which computes the selection box for two
		/// characters [startCharacterIndex, endCharacterIndex] (inclusive) in the same line. 
		SelectionBox computeSelectionBox(const unsigned& startCharacterIndex, const unsigned& endCharacterIndex, const unsigned& lineIndex);
		/// Helper function that computes the cursorPosIndex and cursorPosition from a given characterIndex
		std::pair<unsigned int, Vec2f> computeCursorIndexAndPosition(unsigned int characterIndex);
	public:
		/// Default constructor
		EditableText();
		/// Constuctor using a string and various parameters
		EditableText(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const float& cursorWidth, const ParseMode& parseMode = ParseMode::WORD_BY_WORD, const Alignment& alignment = Alignment::LEFT);
		/// Copy and Move constructors
		EditableText(const EditableText& other) noexcept;
		EditableText(EditableText&& other) noexcept;
		/// Sets the cursor position
		/// index: index of unciode character in front of which the cursor is shown. Indices start at zero
		/// If moveSelection is set to false, a selection will be made/changed (eg. use when pressing shift)
		void setCursorPos(unsigned int characterIndex, const bool& moveSelection = true);
		/// Sets the cursor position to be as close as possible to the given 2D (mouse) position
		/// If moveSelection is set to false, a selection will be made/changed (eg. use when holding down the mouse)
		void computeCursorPosFromMousePos(Vec2d mousePos, const bool& moveSelection = true);
		/// Moves the cursor one character to the left
		/// If moveSelection is set to false, a selection will be made/changed (eg. use when pressing shift)
		void moveCursorToLeft(const bool& moveSelection = true);
		/// Moves the cursor one character to the right
		/// If moveSelection is set to false, a selection will be made/changed (eg. use when pressing shift)
		void moveCursorToRight(const bool& moveSelection = true);
		/// Moves the cursor to the beginning of the current/last word
		/// If moveSelection is set to false, a selection will be made/changed (eg. use when pressing shift)
		void moveCursorToLeftWordBeginning(const bool& moveSelection = true);
		/// Moves the cursor to the end of the current/next word. 
		/// If moveSelection is set to false, a selection will be made/changed (eg. use when pressing shift)
		void moveCursorToRightWordEnd(const bool& moveSelection = true);
		/// Sets the selectionIndex to the cursorPosIndex
		void setSelectionIndexToCursor();
		/// Computes and returns a vector of selection boxes using the current selection.
		std::vector<SelectionBox> getSelectionBoxes();
		/// Returns the cursor position
		const Vec2f getCursorPos() const;
		/// Inputs a unicode character at the current cursor position. If a selection
		/// was made, it is replaced by the character instead.
		void inputAtCursor(const Unicode& codepoint);
		/// Inputs the given UTF-8 encoded string at the current cursor position. If a selection
		/// was made, it is replaced by the string instead.
		void inputAtCursor(const std::string& text);
		/// Inputs the 'line break' or 'newline' unicode character at the current cursor position
		void inputNewlineAtCursor();
		/// Deletes all characters in [beginIndex, endIndex] (inclusive)
		void deleteCharacters(unsigned int beginIndex, unsigned int endIndex);
		/// Deletes the character before the current cursor pos. 
		/// If something is selected, this is deleted instead
		void deleteCharacterBeforeCursor();
		/// Deletes the complete word before the current cursor pos.
		/// If something is selected, this is deleted instead
		void deleteWordBeforeCursor();
		/// Deletes the character after the current cursor pos. 
		/// If something is selected, this is deleted instead
		void deleteCharacterAfterCursor();
		/// Deletes the complete word after the current cursor pos.
		/// If something is selected, this is deleted instead
		void deleteWordAfterCursor();
		/// Returns the size of the cursor
		const Vec2f getCursorSize() const;
		/// Returns the text
		const std::string& getText() override;
		/// Sets the contents of the text. Needs to recompute the text model.
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setText(const std::string& text, bool recompute = true) override;
		/// Sets the textwidth. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setTextWidth(const double& textWidth, bool recompute = true) override;
		/// Sets the font size. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setFontSize(const double& fontSize, bool recompute = true) override;
		/// Sets the font. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setFont(const Font& font, bool recompute = true) override;
		/// Sets the parse mode. May need to recompute the text model
		/// If you want to set multiple parameters and not yet want to recompute the text model,
		/// set recompute to false
		virtual void setParseMode(const StaticText::ParseMode& parseMode, bool recompute = true) override;
		/// Sets the alignment.
		virtual void setAlignment(const StaticText::Alignment& alignment, bool recompute = true) override;
		/// Sets the cursor width
		void setCursorWidth(const float& cursorWidth);
		/// Returns true if the selectionIndex is different from the cursor index, ie. if a selection
		/// selecting > 0 characters is currently active
		bool isSelecting() const;
	};
	//--------------------------------------------------------------------------------------------------
}