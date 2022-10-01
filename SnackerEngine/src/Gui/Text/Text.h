#pragma once

#include "Gui/Text/Font.h"
#include "Math/Vec.h"
#include "Graphics/Model.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace SnackerEngine
{

	/// A struct that saves important information about each character
	struct DynamicTextCharacter
	{
		/// The codepoint of the character
		Unicode codepoint;
		/// The position of the left edge of the character
		double leftPosition;
		/// The position od the right edge of the character
		double rightPosition;
	};

	/// A struct that saves important information about each line
	struct DynamicTextLine
	{
		/// The y position of the baseline
		double baselineY;
		/// indices into the unicodeCharacters vector (begin and end, inclusive)
		/// line includes characters in [beginIndex, endIndex]
		unsigned beginIndex, endIndex;
		/// Comparison operators for searching a given index
		bool operator<(const DynamicTextLine& other) const;
		bool operator>(const DynamicTextLine& other) const;
		bool operator==(const DynamicTextLine& other) const;
	};

	/// Enum to specify how text is parsed and displayed
	enum class TextParseMode
	{
		WORD_BY_WORD,	/// The parser tries to not split words and moves them to the next line if they are too long
		CHARACTERS,		/// The parser does not care about words and just parses each character
	};

	/// Enum for the different alignment modes a StaticText object can use
	enum class Alignment
	{
		LEFT,
		CENTER,
		RIGHT,
	};

	/// A class representing text in a scene. The text can only be changed as a whole, and
	/// cannot be changed at certain positions only.
	class StaticText
	{
	private:
		/// The font in which the text is written
		Font font;
		/// The font size in pt
		double fontSize;
		/// The width of the text in pt
		double textWidth;
		/// The contents of the text
		std::u8string text;
		/// The model used for rendering
		Model model;
		/// The mode used while parsing
		TextParseMode parseMode;
		/// The alignment of the text
		Alignment alignment;
		/// The y coordinate of the bottom of the text (in pt)
		/// The 0 coordinate is the baseline of the first character
		double bottom;
		/// Constructs the model from the text member variable using parse mode 'CHARACTERS'
		Model parseTextCharacters();
		/// Constructs the model from the text member variable using parse mode 'WORD_BY_WORD'
		Model parseTextWordByWord();
		/// Constructs the model from the text member variable
		void constructModel();
	public:
		/// Default constructor
		StaticText();
		/// Constuctor using a string
		StaticText(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const TextParseMode& parseMode, const Alignment& alignment = Alignment::LEFT);
		/// Returns a const reference to the contents of the text
		const std::u8string& getText() const;
		/// Sets the contents of the text
		void setText(const std::u8string& text);
		/// Sets the textwidth. May need to recompute the text model
		void setTextWidth(const double& textWidth);
		/// Returns the font size
		const double& getFontSize() const;
		/// Returns a const reference to the model
		const Model& getModel();
		/// Returns a const reference to the font
		const Font& getFont();
		/// Returns the alignment
		const Alignment& getAlignment() const;
		/// Returns the y coordinate of the top of the text (in pt)
		/// The 0 coordinate is the baseline of the first character
		const double& getTop();
		/// Returns the y coordinate of the bottom of the text (in pt)
		/// The 0 coordinate is the baseline of the first character
		const double& getBottom();
	};

	/// A class representing a more dynamic text, that can be edited on the fly
	class DynamicText
	{
	private:
		/// The font in which the text is written
		Font font;
		/// The font size in pt
		double fontSize;
		/// The width of the text in pt
		double textWidth;
		/// Vector of DynamicTextCharacters, representing the contents of the text.
		/// This vector is up-to-date at all times
		std::vector<DynamicTextCharacter> unicodeCharacters;
		/// Map that saves DynamicTextLine structs for each line. The index is the line number, starting at 0
		std::vector<DynamicTextLine> lines;
		/// The contents of the text as a UTF-8 encoded string. Might not be up-to-data at all times
		std::u8string text;
		/// If this bool is set to true, the UTF-8 encoded variable text is up-to-date
		bool textIsUpToDate;
		/// The model used for rendering
		Model model;
		/// The vector of vertices. Contains positions and texture coordinates
		std::vector<Vec4f> vertices;
		/// Vector of indices
		std::vector<unsigned int> indices;
		/// index of the character which currently comes after the cursor
		unsigned cursorPosIndex;
		/// Position of the cursor
		Vec2f cursorPos;
		/// Size of the cursor
		Vec2f cursorSize;
		/// Parse mode
		TextParseMode parseMode;
		/// The alignment of the text
		Alignment alignment;
		/// Constructs the full model and fills the unicodeCharacters and lines vectors from the UTF-8 encoded string (parseMode == WORD_BY_WORD)
		Model parseTextWordByWord();
		/// Constructs the full model and fills the unicodeCharacters and lines vectors from the UTF-8 encoded string (parseMode == CHARACTERS)
		Model parseTextCharacters();
		/// Constructs the full model and fills the unicodeCharacters and lines vectors from the UTF-8 encoded string
		void constructNew();
		/// Constructs the model, starting at the given index into the unicodeCharacters vector (inclusive).
		/// All vertices before the index are not changed. The UTF-8 encoded text is not updated (parseMode == WORD_BY_WORD)
		Model parseTextStartingAtIndexWordByWord(unsigned int characterIndex);
		/// Constructs the model, starting at the given index into the unicodeCharacters vector (inclusive).
		/// All vertices before the index are not changed. The UTF-8 encoded text is not updated (parseMode == CHARACTERS)
		Model parseTextStartingAtIndexCharacters(unsigned int characterIndex);
		/// Constructs the model, starting at the given index into the unicodeCharacters vector (inclusive).
		/// All vertices before the index are not changed. The UTF-8 encoded text is not updated
		void constructFrom(unsigned int characterIndex);
		/// Finds the line number of the given unicode character
		unsigned int getLineNumber(const unsigned int& characterIndex) const;
		/// Aligns the text model properly, should be called after the lines and unicodeCharacters vectors are constructed
		/// but before the model is created
		void enforceAlignment();
	public:
		/// Constuctor using a string
		DynamicText(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const TextParseMode& parseMode = TextParseMode::WORD_BY_WORD, const Alignment& alignment = Alignment::LEFT);
		/// Returns a const reference to the contents of the text
		const std::u8string& getText() const;
		/// Sets the contents of the text
		void setText(const std::u8string& text);
		/// Returns the font size
		const double& getFontSize() const;
		/// Returns a const reference to the model
		const Model& getModel();
		/// Sets the cursor position
		/// index: index of unciode character in front of which the cursor is shown. Indices start at zero
		void setCursorPos(unsigned int characterIndex);
		/// Moves the cursor one character to the left
		void moveCursorToLeft();
		/// Moves the cursor one character to the right
		void moveCursorToRight();
		/// Moves the cursor to the beginning of the current/last word
		void moveCursorToLeftWordBeginning();
		/// Moves the cursor to the end of the current/next word
		void moveCursorToRightWordEnd();
		/// Returns the cursor position
		const Vec2f& getCursorPos() const;
		/// Returns cursor size
		const Vec2f& getCursorSize() const;
		/// Inputs a unicode character at the current cursor position
		void inputAtCursor(const Unicode& codepoint);
		/// Inputs the 'line break' or 'newline' unicode character at the current cursor position
		void inputNewlineAtCursor();
		/// Deletes all characters in [beginIndex, endIndex] (inclusive)
		void deleteCharacters(unsigned int beginIndex, unsigned int endIndex);
		/// Deletes the character before the current cursor pos
		void deleteCharacterBeforeCursor();
		/// Deletes the complete word before the current cursor pos
		void deleteWordBeforeCursor();
	};
	
}