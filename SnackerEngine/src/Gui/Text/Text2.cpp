#include "Gui/Text/Text2.h"
#include "core/Log.h"
#include "AssetManager/MeshManager.h"

#include <optional>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	bool StaticText2::Line::operator<(const Line& other) const
	{
		return endIndex < other.beginIndex;
	}
	//--------------------------------------------------------------------------------------------------
	bool StaticText2::Line::operator>(const Line& other) const
	{
		return beginIndex > other.endIndex;
	}
	//--------------------------------------------------------------------------------------------------
	bool StaticText2::Line::operator==(const Line& other) const
	{
		return !(endIndex < other.beginIndex || beginIndex > other.endIndex);
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns the next codepoint and advances the position
	static std::optional<Unicode> getNextCodepoint(const std::u8string& text, unsigned int& position)
	{
		const char8_t* c = &text.at(position);
		if (!(*c & 0b10000000)) {
			position++;
			return static_cast<Unicode>(*c & 0b01111111);
		}
		// At least two chars
		if ((*c & 0b11100000) == 0b11000000) {
			position += 2;
			return (static_cast<Unicode>(*c & 0b00011111) << 6) |
				static_cast<Unicode>(*(c + 1) & 0b00111111);
		}
		// At least three chars
		if ((*c & 0b11110000) == 0b11100000) {
			position += 3;
			return (static_cast<Unicode>(*c & 0b00001111) << 11) |
				(static_cast<Unicode>(*(c + 1) & 0b00111111) << 6) |
				static_cast<Unicode>(*(c + 2) & 0b00111111);
		}
		// Four chars
		if ((*c & 0b11111000) == 0b11110000) {
			position += 4;
			return (static_cast<Unicode>(*c & 0b00000111) << 15) |
				(static_cast<Unicode>(*(c + 1) & 0b00111111) << 12) |
				(static_cast<Unicode>(*(c + 2) & 0b00111111) << 6) |
				static_cast<Unicode>(*(c + 3) & 0b00111111);
		}
		// Invalid Codepoint
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns true if the given codepoint is considered a whitespace character
	static bool isWhiteSpace(const Unicode& codepoint)
	{
		return
			codepoint == 0x0020 ||	// space
			codepoint == 0x0009 ||	// tab
			codepoint == 0x0085 ||	// newline
			codepoint == 0x000A;	// newline
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that resturns true if the given codepoint is the space character
	static bool isSpaceCharacter(const Unicode& codepoint)
	{
		return codepoint == 0x0020;
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns true if the given codepoint is the newline character '\n'
	static bool isNewline(const Unicode& codepoint)
	{
		return
			codepoint == 0x0085 ||
			codepoint == 0x000A;
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns the 'newline' codepoint
	static const Unicode& getNewlineCodepoint()
	{
		return 0x0085;
	}
	//--------------------------------------------------------------------------------------------------
	/// Struct that is used to hold data during the parsing of a text object
	struct ParseData
	{
		/// The font the text is written in
		Font& font;
		/// The current baseline
		Vec2d currentBaseline;
		/// The last codepoint (necessary for kerning)
		/// If lastCodepoint == 0, we are in a new line (currentBaseline.x == 0.0)
		Unicode lastCodepoint;
		/// The maximum allowed line width in pt
		const double& textWidth;
		/// The font size we'd like to parse in
		const double& fontSize;
		/// Vector of characters
		std::vector<StaticText2::Character>& characters;
		/// Vector of lines
		std::vector<StaticText2::Line>& lines;

		/// Constructor
		ParseData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, const double& textWidth, const double& fontSize, std::vector<StaticText2::Character>& characters, std::vector<StaticText2::Line>& lines);

		/// Checks if the given x position still is inside the maximum allowed line width
		bool isInsideTextWidth(const double& x);
		/// Goes to the next line, changing currentBaseline. Does not set lastCodepoint to zero!
		virtual void goToNextLine();
		/// Goes to the next line, changing currentBaseline. Does not set lastCodepoint to zero!
		/// firstCharacterOnNewlineIndex is the index of the first character of the new line
		virtual void goToNextLine(const unsigned int& firstCharacterOnNewlineIndex);
		/// Advances currentBaseline by using kerning from lastCodepoint to the given codepoint. 
		/// Does not update lastCodepoint
		void advanceBaselineWithKerning(const Unicode& newCodepoint);
		/// Adds a glyph to the given line by adding it to the back of the characters vector. 
		/// Also advances the baseline accordingly. Does not check if a new line is necessary. 
		/// Changes the left and right bounds of the glyph according to the baseline.
		void pushGlyph(Glyph& glyph, const Unicode& codepoint);
		/// Same as pushGlyph, but for newline character
		void pushNewlineGlyph();
		/// Edits a character in the characters vector given a glyph. 
		/// Also advances the baseline accordingly. Does not check if a new line is necessary. 
		/// Changes the left and right bounds of the glyph according to the baseline.
		void editGlyph(Glyph& glyph, const unsigned int& indexIntoCharactersVector);
		/// Same as editGlyph, but for newline character
		void editNewlineGlyph(const unsigned int& indexIntoCharactersVector);
		/// Reads in and pushes glyphs until a whitespace character is found. Also advances the baseline
		/// accordingly. Does not check if a new line is necessary. This function can be used for parsing whole
		/// words. After this function is called the baseline is at the beginning of the last glyph. 
		/// Advances the index into the utf8 string as well.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Also adds all glyphs to the back of the dynamicTextCharacters vector
		bool pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::u8string& text);
		/// Reads in and pushes glyphs until a whitespace character is found. Also advances the baseline
		/// accordingly. Does not check if a new line is necessary. This function can be used for parsing whole
		/// words. After this function is called the baseline is at the beginning of the last glyph. 
		/// Advances the index into the utf8 string as well.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		bool editGlyphsUntilWhitespace(unsigned& indexIntoCharactersVector);
		/// Parses the text character by character, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary. Does not care about word boundaries.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Assumes that the lines vector has at least one entry and is properly set up.
		bool parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::u8string& text);
		/// Parses the text character by character, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary. Does not care about word boundaries.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Assumes that the lines vector has at least one entry and is properly set up.
		bool parseCharacterByCharacter(const unsigned& beginIndexIntoCharactersVector, const unsigned& endIndexIntoCharactersVector);
		/// Parses the text word by word, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Assumes that the lines vector has at least one entry and is properly set up.
		bool parseWordByWord(const unsigned& beginIndexIntoUTF8String, const unsigned endIndexIntoUTF8String, const std::u8string& text);
		/// Parses the text word by word, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Assumes that the lines vector has at least one entry and is properly set up.
		bool parseWordByWord(const unsigned& beginIndexIntoCharactersVector, const unsigned endIndexIntoCharactersVector);
		/// Constructs the model from the characters and lines vectors. At this time a last alignment pass
		/// is made that shifts the text based on the alignment mode
		Model alignAndConstructModel(const StaticText2::Alignment& alignment, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices);
		/// Constructs the model from the characters and lines vectors. At this time a last alignment pass
		/// is made that shifts the text based on the alignment mode.
		/// Starts at the given line.
		Model alignAndConstructModel(const StaticText2::Alignment& alignment, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices, const unsigned int& startAtLine);
	};
	//--------------------------------------------------------------------------------------------------
	ParseData::ParseData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, const double& textWidth, const double& fontSize, std::vector<StaticText2::Character>& characters, std::vector<StaticText2::Line>& lines)
		: font(font), currentBaseline(currentBaseline), lastCodepoint(lastCodepoint), textWidth(textWidth), fontSize(fontSize), characters(characters), lines(lines) {}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::isInsideTextWidth(const double& x)
	{
		return x * fontSize <= textWidth;
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::goToNextLine()
	{
		// Change current baseline
		currentBaseline.x = 0;
		currentBaseline.y -= font.getLineHeight();
		// Update the lines vector!
		lines.back().endIndex = characters.size() - 1;
		lines.push_back(StaticText2::Line{ currentBaseline.y, static_cast<unsigned int>(characters.size()),
			static_cast<unsigned int>(characters.size()) });
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::goToNextLine(const unsigned int& firstCharacterOnNewlineIndex)
	{
		// Change current baseline
		currentBaseline.x = 0;
		currentBaseline.y -= font.getLineHeight();
		// Update the lines vector!
		lines.back().endIndex = firstCharacterOnNewlineIndex - 1;
		lines.push_back(StaticText2::Line{ currentBaseline.y, firstCharacterOnNewlineIndex,
			firstCharacterOnNewlineIndex });
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::advanceBaselineWithKerning(const Unicode& newCodepoint)
	{
		currentBaseline.x += font.getAdvance(lastCodepoint, newCodepoint);
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::pushGlyph(Glyph& glyph, const Unicode& codepoint)
	{
		glyph.left += currentBaseline.x;
		glyph.right += currentBaseline.x;
		glyph.top += currentBaseline.y;
		glyph.bottom += currentBaseline.y;
		characters.push_back(StaticText2::Character{ codepoint, glyph.left, glyph.right });
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::pushNewlineGlyph()
	{
		// Update characters vector
		if (!characters.empty()) {
			double position = 0.0;
			if (!isNewline(characters.back().codepoint)) {
				position = characters.back().left +
					font.getGlyph(characters.back().codepoint).advance;
			}
			characters.push_back({ getNewlineCodepoint(), position, position });
		}
		else characters.push_back({ getNewlineCodepoint(), 0.0, 0.0 });
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::editGlyph(Glyph& glyph, const unsigned int& indexIntoCharactersVector)
	{
		glyph.left += currentBaseline.x;
		glyph.right += currentBaseline.x;
		glyph.top += currentBaseline.y;
		glyph.bottom += currentBaseline.y;
		characters[indexIntoCharactersVector].left = glyph.left;
		characters[indexIntoCharactersVector].right = glyph.right;
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::editNewlineGlyph(const unsigned int& indexIntoCharactersVector)
	{
		// Update characters vector
		if (indexIntoCharactersVector > 0) {
			double position = 0.0;
			if (!isNewline(characters[indexIntoCharactersVector - 1].codepoint)) {
				position = characters[indexIntoCharactersVector - 1].left +
					font.getGlyph(characters[indexIntoCharactersVector - 1].codepoint).advance;
			}
			characters[indexIntoCharactersVector].left = position;
			characters[indexIntoCharactersVector].right = position;
		}
		else {
			characters[indexIntoCharactersVector].left = 0.0;
			characters[indexIntoCharactersVector].right = 0.0;
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::u8string& text)
	{
		// Save the index of the last parsed glyph st. we can return the correct index at the end
		// (and not the index of the whitespace character coming after the word!)
		unsigned lastIndexIntoUTF8String = indexIntoUTF8String;
		// Go through the text character by character
		while (indexIntoUTF8String < text.size())
		{
			auto codepoint = getNextCodepoint(text, indexIntoUTF8String);
			if (!codepoint) {
				warningLogger << LOGGER::BEGIN << "text was not formatted properly (can only read UTF-8 formatting)" << LOGGER::ENDL;
				return false;
			}
			// Check if the parsed character is whitespace, in which case we return
			if (isWhiteSpace(codepoint.value())) {
				indexIntoUTF8String = lastIndexIntoUTF8String;
				return true;
			}
			// We treat every character that is not a whitespace character equal: put it into the current line.
			// We don't check if we need a new line!
			if (lastCodepoint != 0) {
				// Advance to next glyph
				advanceBaselineWithKerning(codepoint.value());
			}
			Glyph glyph = font.getGlyph(codepoint.value());
			// Add the new glyph
			pushGlyph(glyph, codepoint.value());
			// Update lastCodepoint
			lastCodepoint = codepoint.value();
			// Update the lastIndex
			lastIndexIntoUTF8String = indexIntoUTF8String;
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::editGlyphsUntilWhitespace(unsigned& indexIntoCharactersVector)
	{
		// Go through the text character by character
		while (indexIntoCharactersVector < characters.size())
		{
			StaticText2::Character& character = characters[indexIntoCharactersVector];
			// Check if the parsed character is whitespace, in which case we return
			if (isWhiteSpace(character.codepoint)) {
				return true;
			}
			// We treat every character that is not a whitespace character equal: put it into the current line.
			// We don't check if we need a new line!
			if (lastCodepoint != 0) {
				// Advance to next glyph
				advanceBaselineWithKerning(character.codepoint);
			}
			Glyph glyph = font.getGlyph(character.codepoint);
			// Add the new glyph
			editGlyph(glyph, indexIntoCharactersVector);
			// Update lastCodepoint
			lastCodepoint = character.codepoint;
			// Update index
			indexIntoCharactersVector++;
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::u8string& text)
	{
		// Go through the text character by character
		unsigned indexIntoUTF8String = beginIndexIntoUTF8String;
		while (indexIntoUTF8String < endIndexIntoUTF8String)
		{
			auto codepoint = getNextCodepoint(text, indexIntoUTF8String);
			if (!codepoint) {
				warningLogger << LOGGER::BEGIN << "text was not formatted properly (can only read UTF-8 formatting)" << LOGGER::ENDL;
				return false;
			}
			// First check if the parsed codepoint was a newline character
			if (isNewline(codepoint.value())) {
				pushNewlineGlyph();
				goToNextLine();
				lastCodepoint = 0;
				continue;
			}
			// We treat every character that is not a newline character equal: Try to put it on the current line,
			// if it doesn't fit put it on the next line!
			if (lastCodepoint != 0) {
				// Advance to next glyph
				advanceBaselineWithKerning(codepoint.value());
			}
			// Update lastCodepoint
			lastCodepoint = codepoint.value();
			// Obtain glyph
			Glyph glyph = font.getGlyph(codepoint.value());
			// Check if we need a new line
			if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty()) {
				goToNextLine();
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (isSpaceCharacter(codepoint.value())) {
					lastCodepoint = 0;
				}
			}
			// Add the new glyph
			pushGlyph(glyph, codepoint.value());
		}
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::parseCharacterByCharacter(const unsigned& beginIndexIntoCharactersVector, const unsigned& endIndexIntoCharactersVector)
	{
		// Go through the text character by character
		unsigned indexIntoCharactersVector = beginIndexIntoCharactersVector;
		while (indexIntoCharactersVector  < endIndexIntoCharactersVector)
		{
			StaticText2::Character& character = characters[indexIntoCharactersVector];
			// First check if the parsed codepoint was a newline character
			if (isNewline(character.codepoint)) {
				editNewlineGlyph(indexIntoCharactersVector);
				indexIntoCharactersVector++;
				goToNextLine(indexIntoCharactersVector);
				lastCodepoint = 0;
				continue;
			}
			// We treat every character that is not a newline character equal: Try to put it on the current line,
			// if it doesn't fit put it on the next line!
			if (lastCodepoint != 0) {
				// Advance to next glyph
				advanceBaselineWithKerning(character.codepoint);
			}
			// Update lastCodepoint
			lastCodepoint = character.codepoint;
			// Obtain glyph
			Glyph glyph = font.getGlyph(character.codepoint);
			// Check if we need a new line
			if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty()) {
				goToNextLine(indexIntoCharactersVector);
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (isSpaceCharacter(character.codepoint)) {
					lastCodepoint = 0;
				}
			}
			// Add the new glyph
			editGlyph(glyph, indexIntoCharactersVector);
			// Advance to next character
			indexIntoCharactersVector++;
		}
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::parseWordByWord(const unsigned& beginIndexIntoUTF8String, const unsigned endIndexIntoUTF8String, const std::u8string& text)
	{
		unsigned indexIntoUTF8String = beginIndexIntoUTF8String;
		// Go through the text secharacter by character
		while (indexIntoUTF8String < text.size()) {
			/// Before we parse the next codepoint, we need to save the index into the UTF8 string, because we might
			/// need to backtrack if the parsed word does not fit into the current line
			unsigned int previousIndexIntoUTF8String = indexIntoUTF8String;
			// Now we can parse the next character
			auto codepoint = getNextCodepoint(text, indexIntoUTF8String);
			if (!codepoint) {
				warningLogger << LOGGER::BEGIN << "text was not formatted properly (can only read UTF-8 formatting)" << LOGGER::ENDL;
				return false;
			}
			// First check if the parsed codepoint was a newline character
			if (isNewline(codepoint.value())) {
				pushNewlineGlyph();
				goToNextLine();
				lastCodepoint = 0;
				continue;
			}
			// Next, check if we have a whitespace character
			if (isWhiteSpace(codepoint.value())) {
				/// Whitespaces are treated equally as in the CHARACTERS case: Try to put it on the current line,
				/// if it doesn't fit put it on the next line!
				if (lastCodepoint != 0) {
					// Advance to next glyph
					advanceBaselineWithKerning(codepoint.value());
				}
				// Update lastCodepoint
				lastCodepoint = codepoint.value();
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right)) {
					goToNextLine();
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(codepoint.value())) {
						lastCodepoint = 0;
					}
				}
				// Add the new glyph
				pushGlyph(glyph, codepoint.value());
			}
			else {
				/// We have a non-whitespace character. First: save some additional information that is necessary when
				/// we need to backtrack
				// Save the current baseline x position
				double previousBaselineX = currentBaseline.x;
				// Save the last codepoint
				Unicode previouseLastCodepoint = lastCodepoint;
				// Save the size of the characters vector
				unsigned int charactersSize = characters.size();
				// Now: place the character we just read in at the correct position
				if (lastCodepoint != 0) {
					// Advance to next glyph
					advanceBaselineWithKerning(codepoint.value());
				}
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty()) {
					goToNextLine();
					previousBaselineX = currentBaseline.x;
					lastCodepoint = codepoint.value();
					previouseLastCodepoint = 0;
				}
				else {
					// Update lastCodepoint
					lastCodepoint = codepoint.value();
				}
				// Add the new glyph
				pushGlyph(glyph, codepoint.value());
				/// Now, we want to read the remaining word.
				// First try to just parse the word in the current line
				pushGlyphsUntilWhitespace(indexIntoUTF8String, text);
				// Now, we can have three cases:
				double rightPositionX = currentBaseline.x + font.getGlyph(lastCodepoint).right;
				if (isInsideTextWidth(rightPositionX)) {
					// 1.)	The word fits, we can just move on!
				}
				else if (isInsideTextWidth(rightPositionX - previousBaselineX))
				{
					// 2.)	The word fits inside a single line, but not on the current line -> we have to move to the next line!
					characters.resize(charactersSize);
					goToNextLine();
					lastCodepoint = 0;
					// Now we can read in the word again, but this time it will fit.
					indexIntoUTF8String = previousIndexIntoUTF8String;
					pushGlyphsUntilWhitespace(indexIntoUTF8String, text);
				}
				else
				{
					// 3.)	The word does not even fit into a single line. In this case we fall back to the
					//		CHARACTERS behaviour: Try to put it on the current line,
					//		if it doesn't fit put it on the next line!
					characters.resize(charactersSize);
					currentBaseline.x = previousBaselineX;
					lastCodepoint = previouseLastCodepoint;
					parseCharacterByCharacter(previousIndexIntoUTF8String, indexIntoUTF8String, text);
				}
			}
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::parseWordByWord(const unsigned& beginIndexIntoCharactersVector, const unsigned endIndexIntoCharactersVector)
	{
		unsigned indexIntoCharactersVector = beginIndexIntoCharactersVector;
		// Go through the text secharacter by character
		while (indexIntoCharactersVector < endIndexIntoCharactersVector) {
			/// Before we parse the next codepoint, we need to save the index into the character vector, because we might
			/// need to backtrack if the parsed word does not fit into the current line
			unsigned int previousIndexIntoCharactersVector = indexIntoCharactersVector;
			// Now we can parse the next character
			StaticText2::Character& character = characters[indexIntoCharactersVector];
			// First check if the parsed codepoint was a newline character
			if (isNewline(character.codepoint)) {
				editNewlineGlyph(indexIntoCharactersVector);
				indexIntoCharactersVector++;
				goToNextLine(indexIntoCharactersVector);
				lastCodepoint = 0;
				continue;
			}
			// Next, check if we have a whitespace character
			if (isWhiteSpace(character.codepoint)) {
				/// Whitespaces are treated equally as in the CHARACTERS case: Try to put it on the current line,
				/// if it doesn't fit put it on the next line!
				if (lastCodepoint != 0) {
					// Advance to next glyph
					advanceBaselineWithKerning(character.codepoint);
				}
				// Update lastCodepoint
				lastCodepoint = character.codepoint;
				Glyph glyph = font.getGlyph(character.codepoint);
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right)) {
					goToNextLine(indexIntoCharactersVector);
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(character.codepoint)) {
						lastCodepoint = 0;
					}
				}
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (indexIntoCharactersVector == beginIndexIntoCharactersVector) {
					lastCodepoint = 0;
				}
				// Add the new glyph
				editGlyph(glyph, indexIntoCharactersVector);
				indexIntoCharactersVector++;
			}
			else {
				/// We have a non-whitespace character. First: save some additional information that is necessary when
				/// we need to backtrack
				// Save the current baseline x position
				double previousBaselineX = currentBaseline.x;
				// Save the last codepoint
				Unicode previouseLastCodepoint = lastCodepoint;
				// Save the size of the characters vector
				unsigned int charactersSize = characters.size();
				// Now: place the character we just read in at the correct position
				if (lastCodepoint != 0) {
					// Advance to next glyph
					advanceBaselineWithKerning(character.codepoint);
				}
				Glyph glyph = font.getGlyph(character.codepoint);
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty()) {
					goToNextLine(indexIntoCharactersVector);
					previousBaselineX = currentBaseline.x;
					lastCodepoint = character.codepoint;
					previouseLastCodepoint = 0;
				}
				else {
					// Update lastCodepoint
					lastCodepoint = character.codepoint;
				}
				// Add the new glyph
				editGlyph(glyph, indexIntoCharactersVector);
				/// Now, we want to read the remaining word.
				// First try to just parse the word in the current line
				indexIntoCharactersVector++;
				editGlyphsUntilWhitespace(indexIntoCharactersVector);
				// Now, we can have three cases:
				double rightPositionX = currentBaseline.x + font.getGlyph(lastCodepoint).right;
				if (isInsideTextWidth(rightPositionX)) {
					// 1.)	The word fits, we can just move on!
				}
				else if (isInsideTextWidth(rightPositionX - previousBaselineX))
				{
					// 2.)	The word fits inside a single line, but not on the current line -> we have to move to the next line!
					characters.resize(charactersSize);
					goToNextLine(previousIndexIntoCharactersVector - 1);
					lastCodepoint = 0;
					// Now we can read in the word again, but this time it will fit.
					indexIntoCharactersVector = previousIndexIntoCharactersVector;
					editGlyphsUntilWhitespace(indexIntoCharactersVector);
				}
				else
				{
					// 3.)	The word does not even fit into a single line. In this case we fall back to the
					//		CHARACTERS behaviour: Try to put it on the current line,
					//		if it doesn't fit put it on the next line!
					characters.resize(charactersSize);
					currentBaseline.x = previousBaselineX;
					lastCodepoint = previouseLastCodepoint;
					parseCharacterByCharacter(previousIndexIntoCharactersVector, indexIntoCharactersVector);
				}
			}
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	Model ParseData::alignAndConstructModel(const StaticText2::Alignment& alignment, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices)
	{
		return alignAndConstructModel(alignment, vertices, indices, 0);
	}
	//--------------------------------------------------------------------------------------------------
	Model ParseData::alignAndConstructModel(const StaticText2::Alignment& alignment, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices, const unsigned int& startAtLine)
	{
		if (startAtLine >= lines.size()) return Model();
		if (startAtLine > 0) {
			vertices.resize(lines[startAtLine].beginIndex * 4);
			indices.resize(lines[startAtLine].beginIndex * 6);
		}
		else
		{
			vertices.clear();
			indices.clear();
		}
		vertices.reserve(characters.size() * 4);
		indices.reserve(characters.size() * 6);
		for (unsigned int lineIndex = startAtLine; lineIndex < lines.size(); ++lineIndex) {
			const auto& line = lines[lineIndex];
			if (line.beginIndex >= characters.size() || line.endIndex >= characters.size()) {
				continue;
			}
			// Determine horizontal offset of the current line
			double horizontalOffset = 0.0;
			switch (alignment)
			{
			case StaticText2::Alignment::LEFT: break;
			case StaticText2::Alignment::CENTER:
			{
				double lineWidth = characters[line.endIndex].right - characters[line.beginIndex].left;
				horizontalOffset = (textWidth / fontSize - lineWidth) / 2.0;
				break;
			}
			case StaticText2::Alignment::RIGHT:
			{
				double lineWidth = characters[line.endIndex].right;
				horizontalOffset = textWidth / fontSize - lineWidth;
				break;
			}
			default: break;
			}
			// Go through line and push indices and vertices
			for (unsigned int i = line.beginIndex; i <= line.endIndex; ++i) {
				StaticText2::Character& c = characters[i];
				if (!isWhiteSpace(c.codepoint)) {
					// Push indices for two triangles
					indices.push_back(static_cast<unsigned int>(vertices.size())); indices.push_back(static_cast<unsigned int>(vertices.size() + 1)); indices.push_back(static_cast<unsigned int>(vertices.size() + 2));
					indices.push_back(vertices.size() + 2); indices.push_back(vertices.size() + 3); indices.push_back(vertices.size());
					// Push four vertices
					Glyph glyph = font.getGlyph(c.codepoint);
					glyph.bottom += line.baselineY;
					glyph.top += line.baselineY;
					vertices.push_back(Vec4f(c.left + horizontalOffset, glyph.bottom, glyph.texLeft, glyph.texBottom));
					vertices.push_back(Vec4f(c.left + horizontalOffset, glyph.top, glyph.texLeft, glyph.texTop));
					vertices.push_back(Vec4f(c.right + horizontalOffset, glyph.top, glyph.texRight, glyph.texTop));
					vertices.push_back(Vec4f(c.right + horizontalOffset, glyph.bottom, glyph.texRight, glyph.texBottom));
				}
				else
				{
					// We still need to push vertices and incices for the whitespace characters to make
					// the resizing of the indices and vertices vector reliable
					// Push indices for two triangles
					indices.push_back(static_cast<unsigned int>(vertices.size())); indices.push_back(static_cast<unsigned int>(vertices.size() + 1)); indices.push_back(static_cast<unsigned int>(vertices.size() + 2));
					indices.push_back(vertices.size() + 2); indices.push_back(vertices.size() + 3); indices.push_back(vertices.size());
					// Push four vertices
					vertices.push_back(Vec4f(0.0f));
					vertices.push_back(Vec4f(0.0f));
					vertices.push_back(Vec4f(0.0f));
					vertices.push_back(Vec4f(0.0f));
				}
			}
		}
		// Create layout and model
		VertexBufferLayout layout;
		layout.push<Vec2f>(1);
		layout.push<Vec2f>(1);
		Mesh mesh = MeshManager::createMesh<Vec4f>(layout, vertices, indices);
		return Model(mesh);
	}
	//--------------------------------------------------------------------------------------------------
	Model StaticText2::parseTextCharacters(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const Alignment& alignment)
	{
		// Create ParseDynamicTextData object
		std::vector<Character> characters;
		std::vector<Line> lines;
		lines.push_back({ 0.0, 0, 0 });
		Font tempFont(font);
		ParseData data{ tempFont, Vec2d{}, Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		return data.alignAndConstructModel(alignment, vertices, indices);
	}
	//--------------------------------------------------------------------------------------------------
	Model StaticText2::parseTextWordByWord(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const Alignment& alignment)
	{
		// Create ParseDynamicTextData object
		std::vector<Character> characters;
		std::vector<Line> lines;
		lines.push_back({ 0.0, 0, 0 });
		Font tempFont(font);
		ParseData data{ tempFont, Vec2d{}, Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseWordByWord(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		return data.alignAndConstructModel(alignment, vertices, indices);
	}
	//--------------------------------------------------------------------------------------------------
	void StaticText2::constructModel(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const ParseMode& parseMode, const Alignment& alignment)
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD: model = parseTextWordByWord(text, font, fontSize, textWidth, alignment); break;
		case SnackerEngine::StaticText2::ParseMode::CHARACTERS: model = parseTextCharacters(text, font, fontSize, textWidth, alignment); break;
		default: break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	StaticText2::StaticText2()
		: model{} {}
	//--------------------------------------------------------------------------------------------------
	StaticText2::StaticText2(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const ParseMode& parseMode, const Alignment& alignment)
		: model{}
	{
		constructModel(text, font, fontSize, textWidth, parseMode, alignment);
	}
	//--------------------------------------------------------------------------------------------------
	Model DynamicText2::parseTextCharacters()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		return data.alignAndConstructModel(alignment, vertices, indices);
	}
	//--------------------------------------------------------------------------------------------------
	Model DynamicText2::parseTextWordByWord()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseWordByWord(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		return data.alignAndConstructModel(alignment, vertices, indices);
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText2::constructModel()
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD: model = parseTextWordByWord(); break;
		case SnackerEngine::StaticText2::ParseMode::CHARACTERS: model = parseTextCharacters(); break;
		default: break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText2::DynamicText2() 
		: StaticText2(), font{}, fontSize(0), textWidth(0), text{}, parseMode(ParseMode::WORD_BY_WORD),
		alignment(Alignment::LEFT), characters{}, lines{}
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText2::DynamicText2(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const ParseMode& parseMode, const Alignment& alignment)
		: StaticText2(), font(font), fontSize(fontSize), textWidth(textWidth), text(text), parseMode(parseMode),
		alignment(alignment), characters{}, lines{}
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	const Font& DynamicText2::getFont() const
	{
		return font;
	}
	//--------------------------------------------------------------------------------------------------
	const double& DynamicText2::getFontSize() const
	{
		return fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	const double& DynamicText2::getTextWidth() const
	{
		return textWidth;
	}
	//--------------------------------------------------------------------------------------------------
	const std::u8string& DynamicText2::getText() const
	{
		return text;
	}
	//--------------------------------------------------------------------------------------------------
	const Model& DynamicText2::getModel() const
	{
		return model;
	}
	//--------------------------------------------------------------------------------------------------
	const StaticText2::ParseMode& DynamicText2::getParseMode() const
	{
		return parseMode;
	}
	//--------------------------------------------------------------------------------------------------
	const StaticText2::Alignment& DynamicText2::getAlignment() const
	{
		return alignment;
	}
	//--------------------------------------------------------------------------------------------------
	const double& DynamicText2::getTop()
	{
		return font.getAscender() * fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	const double& DynamicText2::getBottom()
	{
		return (lines.back().baselineY + font.getDescender()) * fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText2::setText(const std::u8string& text, bool recompute)
	{
		this->text = text;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText2::setTextWidth(const double& textWidth, bool recompute)
	{
		this->textWidth = textWidth;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText2::setFontSize(const double& fontSize, bool recompute)
	{
		this->fontSize = fontSize;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	unsigned int EditableText::getLineNumber(const unsigned int& characterIndex) const
	{
		auto result = std::lower_bound(lines.begin(), lines.end(), Line{ 0, characterIndex, characterIndex });
		if (result < lines.end()) return result - lines.begin();
		return lines.size() - 1;
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextCharacters()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		return data.alignAndConstructModel(alignment, vertices, indices);
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextWordByWord()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseWordByWord(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		return data.alignAndConstructModel(alignment, vertices, indices);
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextCharactersFrom(const unsigned int& lineIndex)
	{
		// Create ParseDynamicTextData object
		lines.resize(lineIndex);
		if (lineIndex == 0) lines.push_back({ 0.0, 0, 0 });
		else lines.push_back({ lines.back().baselineY - font.getLineHeight(), lines.back().endIndex + 1, lines.back().endIndex + 1 });
		ParseData data{ font, Vec2d(0.0, lines.back().baselineY), Unicode{0}, textWidth, fontSize, characters, lines};
		// Parse the text
		data.parseCharacterByCharacter(lines.back().beginIndex, characters.size());
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		return data.alignAndConstructModel(alignment, vertices, indices, lineIndex);
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextWordByWordFrom(const unsigned int& lineIndex)
	{
		// go one line back to catch certain edge cases
		unsigned int newLineIndex = lineIndex;
		if (newLineIndex > 0) newLineIndex--;
		// Create ParseDynamicTextData object
		lines.resize(newLineIndex);
		if (newLineIndex == 0) lines.push_back({ 0.0, 0, 0 });
		else lines.push_back({ lines.back().baselineY - font.getLineHeight(), lines.back().endIndex + 1, lines.back().endIndex + 1 });
		ParseData data{ font, Vec2d(0.0, lines.back().baselineY), Unicode{0}, textWidth, fontSize, characters, lines };
		// Parse the text
		data.parseWordByWord(lines.back().beginIndex, characters.size());
		// Finalize the lines vector
		lines.back().endIndex = characters.size() - 1;
		// Construct model
		return data.alignAndConstructModel(alignment, vertices, indices, newLineIndex);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::constructModelFrom(const unsigned int& lineIndex)
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD: model = parseTextWordByWordFrom(lineIndex); break;
		case SnackerEngine::StaticText2::ParseMode::CHARACTERS: model = parseTextCharactersFrom(lineIndex); break;
		default: break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::constructModel()
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD: model = parseTextWordByWord(); break;
		case SnackerEngine::StaticText2::ParseMode::CHARACTERS: model = parseTextCharacters(); break;
		default: break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	EditableText::EditableText()
		: DynamicText2(), textIsUpToDate(true), vertices{}, indices{}, cursorPosIndex(0), cursorPos{}, cursorSize{} 
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	EditableText::EditableText(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const double& cursorWidth, const ParseMode& parseMode, const Alignment& alignment)
		: DynamicText2(text, font, fontSize, textWidth, parseMode, alignment), textIsUpToDate(true), vertices{}, indices{}, cursorPosIndex(0), cursorPos{}, cursorSize(cursorWidth, font.getAscender() - font.getDescender())
	{
		// TODO: Define additional constructor such that DynamicText::constructModel() is not called on construction of EditableText object!
		constructModel();
		setCursorPos(0);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setText(const std::u8string& text, bool recompute)
	{
		this->text = text;
		textIsUpToDate = true;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setCursorPos(unsigned int characterIndex)
	{
		if (characterIndex > characters.size()) {
			characterIndex = characters.size();
		}
		unsigned int lineNumber = getLineNumber(characterIndex);
		if (characterIndex == 0) cursorPos = Vec2f(0.0f, lines[lineNumber].baselineY);
		else if (characterIndex == characters.size()) {
			// Cursor is at the end of the text
			const Unicode& codepoint = characters.back().codepoint;
			if (isNewline(codepoint)) {
				cursorPos = Vec2f(0.0f, lines[lineNumber].baselineY);
			}
			else {
				cursorPos = Vec2f(characters.back().left
					+ font.getGlyph(codepoint).advance, lines[getLineNumber(characters.size() - 1)].baselineY);
			}
		}
		else cursorPos = Vec2f(characters[characterIndex].left, lines[lineNumber].baselineY);
		cursorPos.x -= cursorSize.x;
		cursorPos.y += font.getDescender();
		cursorPosIndex = characterIndex;
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToLeft()
	{
		if (cursorPosIndex != 0) setCursorPos(cursorPosIndex - 1);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToRight()
	{
		if (cursorPosIndex != characters.size()) setCursorPos(cursorPosIndex + 1);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToLeftWordBeginning()
	{
		if (cursorPosIndex == 0) return;
		unsigned int newIndex = cursorPosIndex - 1;
		if (!isNewline(characters[newIndex].codepoint)) {
			while (newIndex > 0 && isWhiteSpace(characters[newIndex].codepoint) && !isNewline(characters[newIndex].codepoint)) {
				newIndex--;
			}
			while (newIndex > 0 && !isWhiteSpace(characters[newIndex - 1].codepoint)) {
				newIndex--;
			}
		}
		setCursorPos(newIndex);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToRightWordEnd()
	{
		if (cursorPosIndex == characters.size()) return;
		unsigned int newIndex = cursorPosIndex;
		if (isNewline(characters[cursorPosIndex].codepoint)) {
			newIndex++;
		}
		else {
			while (newIndex < characters.size() && !isWhiteSpace(characters[newIndex].codepoint)) {
				newIndex++;
			}
			while (newIndex < characters.size() && isWhiteSpace(characters[newIndex].codepoint) && !isNewline(characters[newIndex].codepoint)) {
				newIndex++;
			}
		}
		setCursorPos(newIndex);
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2f& EditableText::getCursorPos() const
	{
		return cursorPos;
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::inputAtCursor(const Unicode& codepoint)
	{
		characters.insert(characters.begin() + cursorPosIndex, { codepoint, 0.0, 0.0 });
		constructModelFrom(getLineNumber(cursorPosIndex));
		setCursorPos(cursorPosIndex + 1);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::inputNewlineAtCursor()
	{
		inputAtCursor(getNewlineCodepoint());
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteCharacters(unsigned int beginIndex, unsigned int endIndex)
	{
		if (characters.empty()) return;
		if (beginIndex > endIndex) return;
		if (endIndex > characters.size()) endIndex = characters.size() - 1;
		characters.erase(characters.begin() + beginIndex, characters.begin() + endIndex + 1);
		constructModelFrom(getLineNumber(beginIndex));
		if (cursorPosIndex > beginIndex) {
			if (cursorPosIndex < endIndex) {
				setCursorPos(beginIndex);
			}
			else {
				setCursorPos(cursorPosIndex + beginIndex - endIndex - 1);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteCharacterBeforeCursor()
	{
		if (cursorPosIndex > 0) deleteCharacters(cursorPosIndex - 1, cursorPosIndex - 1);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteWordBeforeCursor()
	{
		if (cursorPosIndex == 0) return;
		unsigned int beginIndex = cursorPosIndex - 1;
		if (!isNewline(characters[beginIndex].codepoint)) {
			while (beginIndex > 0 && isWhiteSpace(characters[beginIndex].codepoint) && !isNewline(characters[beginIndex].codepoint)) {
				beginIndex--;
			}
			while (beginIndex > 0 && !isWhiteSpace(characters[beginIndex - 1].codepoint)) {
				beginIndex--;
			}
		}
		deleteCharacters(beginIndex, cursorPosIndex - 1);
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2f& EditableText::getCursorSize() const
	{
		return cursorSize;
	}
	//--------------------------------------------------------------------------------------------------
}