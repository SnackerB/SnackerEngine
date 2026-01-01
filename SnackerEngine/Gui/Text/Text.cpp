#include "Gui/Text/Text.h"
#include "core/Log.h"
#include "AssetManager/MeshManager.h"
#include "Gui/Text/Unicode.h"

#include <optional>
#include <sstream>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	bool StaticText::Line::operator<(const Line& other) const
	{
		return endIndex < other.beginIndex;
	}
	//--------------------------------------------------------------------------------------------------
	bool StaticText::Line::operator>(const Line& other) const
	{
		return beginIndex > other.endIndex;
	}
	//--------------------------------------------------------------------------------------------------
	bool StaticText::Line::operator==(const Line& other) const
	{
		return !(endIndex < other.beginIndex || beginIndex > other.endIndex);
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
		double textWidth;
		/// The font size we'd like to parse in
		const double& fontSize;
		/// The line height of the text in pt
		double lineHeight;
		/// Vector of characters
		std::vector<StaticText::Character>& characters;
		/// Vector of lines
		std::vector<StaticText::Line>& lines;

		/// Constructor
		ParseData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, const double& textWidth, const double& fontSize, double lineHeightMultiplier, std::vector<StaticText::Character>& characters, std::vector<StaticText::Line>& lines);

		/// Checks if the given x position still is inside the maximum allowed line width
		bool isInsideTextWidth(const double& x) const;
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
		bool pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::string& text);
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
		bool parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::string& text);
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
		bool parseWordByWord(const unsigned& beginIndexIntoUTF8String, const unsigned endIndexIntoUTF8String, const std::string& text);
		/// Parses the text word by word, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Assumes that the lines vector has at least one entry and is properly set up.
		bool parseWordByWord(const unsigned& beginIndexIntoCharactersVector, const unsigned endIndexIntoCharactersVector);
		/// Constructs the model from the characters and lines vectors. At this time a last alignment pass
		/// is made that shifts the text based on the alignment mode. Returns the Model and the right border
		/// of the text in pt.
		std::pair<Model, double> alignAndConstructModel(AlignmentHorizontal alignment, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices);
	};
	//--------------------------------------------------------------------------------------------------
	ParseData::ParseData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, const double& textWidth, const double& fontSize, double lineHeightMultiplier, std::vector<StaticText::Character>& characters, std::vector<StaticText::Line>& lines)
		: font(font), currentBaseline(currentBaseline), lastCodepoint(lastCodepoint), textWidth(textWidth), 
		fontSize(fontSize), lineHeight( font.getLineHeight() * lineHeightMultiplier ),
		characters(characters), lines(lines) {}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::isInsideTextWidth(const double& x) const
	{
		return (textWidth == 0.0) || (x * fontSize <= textWidth);
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::goToNextLine()
	{
		// Change current baseline
		currentBaseline.x = 0;
		currentBaseline.y -= lineHeight;
		// Update the lines vector!
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		lines.push_back(StaticText::Line{ static_cast<double>(currentBaseline.y), static_cast<unsigned int>(characters.size()),
			static_cast<unsigned int>(characters.size()) });
	}
	//--------------------------------------------------------------------------------------------------
	void ParseData::goToNextLine(const unsigned int& firstCharacterOnNewlineIndex)
	{
		// We dont want newline if we're just on a single line!
		if (textWidth == 0.0) return;
		// Change current baseline
		currentBaseline.x = 0;
		currentBaseline.y -= lineHeight;
		// Update the lines vector!
		lines.back().endIndex = firstCharacterOnNewlineIndex - 1;
		lines.push_back(StaticText::Line{ static_cast<double>(currentBaseline.y), firstCharacterOnNewlineIndex,
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
		characters.push_back(StaticText::Character{ codepoint, glyph.left, glyph.right });
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
			if (!isNewline(characters[static_cast<std::size_t>(indexIntoCharactersVector) - 1].codepoint)) {
				position = characters[static_cast<std::size_t>(indexIntoCharactersVector) - 1].left +
					font.getGlyph(characters[static_cast<std::size_t>(indexIntoCharactersVector) - 1].codepoint).advance;
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
	bool ParseData::pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::string& text)
	{
		// Save the index of the last parsed glyph st. we can return the correct index at the end
		// (and not the index of the whitespace character coming after the word!)
		unsigned lastIndexIntoUTF8String = indexIntoUTF8String;
		// Go through the text character by character
		while (indexIntoUTF8String < text.size())
		{
			auto codepoint = getNextCodepointUTF8(text, indexIntoUTF8String);
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
			StaticText::Character& character = characters[indexIntoCharactersVector];
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
	bool ParseData::parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::string& text)
	{
		// Go through the text character by character
		unsigned indexIntoUTF8String = beginIndexIntoUTF8String;
		while (indexIntoUTF8String < endIndexIntoUTF8String)
		{
			auto codepoint = getNextCodepointUTF8(text, indexIntoUTF8String);
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
			// Obtain glyph
			Glyph glyph = font.getGlyph(codepoint.value());
			// Check if we need a new line
			if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty() && lastCodepoint != 0) {
				goToNextLine();
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (isSpaceCharacter(codepoint.value())) {
					lastCodepoint = 0;
				}
				else {
					// Update lastCodepoint
					lastCodepoint = codepoint.value();
				}
			}
			else {
				// Update lastCodepoint
				lastCodepoint = codepoint.value();
			}
			// Add the new glyph
			pushGlyph(glyph, codepoint.value());
		}
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size()) - 1;
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::parseCharacterByCharacter(const unsigned& beginIndexIntoCharactersVector, const unsigned& endIndexIntoCharactersVector)
	{
		// Go through the text character by character
		unsigned indexIntoCharactersVector = beginIndexIntoCharactersVector;
		while (indexIntoCharactersVector  < endIndexIntoCharactersVector)
		{
			StaticText::Character& character = characters[indexIntoCharactersVector];
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
			// Obtain glyph
			Glyph glyph = font.getGlyph(character.codepoint);
			// Check if we need a new line
			if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty() && lastCodepoint != 0) {
				goToNextLine(indexIntoCharactersVector);
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (isSpaceCharacter(character.codepoint)) {
					lastCodepoint = 0;
				}
				else {
					// Update lastCodepoint
					lastCodepoint = character.codepoint;
				}
			}
			else {
				// Update lastCodepoint
				lastCodepoint = character.codepoint;
			}
			// Add the new glyph
			editGlyph(glyph, indexIntoCharactersVector);
			// Advance to next character
			indexIntoCharactersVector++;
		}
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	bool ParseData::parseWordByWord(const unsigned& beginIndexIntoUTF8String, const unsigned endIndexIntoUTF8String, const std::string& text)
	{
		unsigned indexIntoUTF8String = beginIndexIntoUTF8String;
		// Go through the text secharacter by character
		while (indexIntoUTF8String < text.size()) {
			/// Before we parse the next codepoint, we need to save the index into the UTF8 string, because we might
			/// need to backtrack if the parsed word does not fit into the current line
			unsigned int previousIndexIntoUTF8String = indexIntoUTF8String;
			// Now we can parse the next character
			auto codepoint = getNextCodepointUTF8(text, indexIntoUTF8String);
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
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right) && lastCodepoint != 0) {
					goToNextLine();
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(codepoint.value())) {
						lastCodepoint = 0;
					}
					else {
						// Update lastCodepoint
						lastCodepoint = codepoint.value();
					}
				}
				else {
					// Update lastCodepoint
					lastCodepoint = codepoint.value();
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
				unsigned int charactersSize = static_cast<unsigned int>(characters.size());
				// Now: place the character we just read in at the correct position
				if (lastCodepoint != 0) {
					// Advance to next glyph
					advanceBaselineWithKerning(codepoint.value());
				}
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty() && lastCodepoint != 0) {
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
			StaticText::Character& character = characters[indexIntoCharactersVector];
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
				Glyph glyph = font.getGlyph(character.codepoint);
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right) && lastCodepoint != 0) {
					goToNextLine(indexIntoCharactersVector);
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(character.codepoint)) {
						lastCodepoint = 0;
					}
					else {
						// Update lastCodepoint
						lastCodepoint = character.codepoint;
					}
				}
				else {
					// Update lastCodepoint
					lastCodepoint = character.codepoint;
				}
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character! (only if the last character before was a newline character)
				if (indexIntoCharactersVector == beginIndexIntoCharactersVector &&
					(indexIntoCharactersVector == 0 || !isNewline(characters[static_cast<std::size_t>(indexIntoCharactersVector)-1].codepoint))) {
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
				unsigned int charactersSize = static_cast<unsigned int>(characters.size());
				// Now: place the character we just read in at the correct position
				if (lastCodepoint != 0) {
					// Advance to next glyph
					advanceBaselineWithKerning(character.codepoint);
				}
				Glyph glyph = font.getGlyph(character.codepoint);
				// Check if we need a new line
				if (!isInsideTextWidth(currentBaseline.x + glyph.right) && !characters.empty() && lastCodepoint!=0) {
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
					goToNextLine(previousIndexIntoCharactersVector);
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
	std::pair<Model, double> ParseData::alignAndConstructModel(AlignmentHorizontal alignment, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices)
	{
		if (characters.empty()) return std::make_pair(Model(), 0.0);
		vertices.clear();
		indices.clear();
		vertices.reserve(characters.size() * 4);
		indices.reserve(characters.size() * 6);
		double longestLineWidth = 0.0;
		double right = 0.0;
		for (auto& line : lines) {
			if (line.beginIndex >= characters.size() || line.endIndex >= characters.size()) continue;
			double lineWidth = characters[line.endIndex].right - characters[line.beginIndex].left;
			if (lineWidth > longestLineWidth) {
				longestLineWidth = lineWidth;
				right = characters[line.endIndex].right;
			}
		}
		for (unsigned int lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
			const auto& line = lines[lineIndex];
			if (line.beginIndex >= characters.size() || line.endIndex >= characters.size()) {
				continue;
			}
			// Determine horizontal offset of the current line
			double horizontalOffset = 0.0;
			switch (alignment)
			{
			case AlignmentHorizontal::LEFT: break;
			case AlignmentHorizontal::CENTER:
			{
				double lineWidth = characters[line.endIndex].right - characters[line.beginIndex].left;
				horizontalOffset = (longestLineWidth - lineWidth) / 2.0;
				break;
			}
			case AlignmentHorizontal::RIGHT:
			{
				double lineWidth = characters[line.endIndex].right;
				horizontalOffset = longestLineWidth - lineWidth;
				break;
			}
			default: break;
			}
			// Go through line and push indices and vertices
			for (unsigned int i = line.beginIndex; i <= line.endIndex; ++i) {
				StaticText::Character& c = characters[i];
				if (!isWhiteSpace(c.codepoint)) {
					// Push indices for two triangles
					indices.push_back(static_cast<unsigned int>(vertices.size())); indices.push_back(static_cast<unsigned int>(vertices.size() + 1)); indices.push_back(static_cast<unsigned int>(vertices.size() + 2));
					indices.push_back(static_cast<unsigned int>(vertices.size()) + 2); indices.push_back(static_cast<unsigned int>(vertices.size()) + 3); indices.push_back(static_cast<unsigned int>(vertices.size()));
					// Push four vertices
					Glyph glyph = font.getGlyph(c.codepoint);
					glyph.bottom += line.baselineY;
					glyph.top += line.baselineY;
					vertices.push_back(Vec4f(static_cast<float>(c.left + horizontalOffset), static_cast<float>(glyph.bottom), static_cast<float>(glyph.texLeft), static_cast<float>(glyph.texBottom)));
					vertices.push_back(Vec4f(static_cast<float>(c.left + horizontalOffset), static_cast<float>(glyph.top), static_cast<float>(glyph.texLeft), static_cast<float>(glyph.texTop)));
					vertices.push_back(Vec4f(static_cast<float>(c.right + horizontalOffset), static_cast<float>(glyph.top), static_cast<float>(glyph.texRight), static_cast<float>(glyph.texTop)));
					vertices.push_back(Vec4f(static_cast<float>(c.right + horizontalOffset), static_cast<float>(glyph.bottom), static_cast<float>(glyph.texRight), static_cast<float>(glyph.texBottom)));
				}
				else
				{
					// We still need to push vertices and incices for the whitespace characters to make
					// the resizing of the indices and vertices vector reliable
					// Push indices for two triangles
					indices.push_back(static_cast<unsigned int>(vertices.size())); indices.push_back(static_cast<unsigned int>(vertices.size() + 1)); indices.push_back(static_cast<unsigned int>(vertices.size() + 2));
					indices.push_back(static_cast<unsigned int>(vertices.size()) + 2); indices.push_back(static_cast<unsigned int>(vertices.size()) + 3); indices.push_back(static_cast<unsigned int>(vertices.size()));
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
		return std::make_pair(Model(mesh), right);
	}
	//--------------------------------------------------------------------------------------------------
	Model StaticText::parseTextCharacters(const std::string& text, const Font& font, const double& fontSize, double lineHeightMultiplier, const double& textWidth, AlignmentHorizontal alignment)
	{
		// Create ParseDynamicTextData object
		std::vector<Character> characters;
		std::vector<Line> lines;
		lines.push_back({ 0.0, 0, 0 });
		Font tempFont(font);
		ParseData data{ tempFont, Vec2d{}, Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		return data.alignAndConstructModel(alignment, vertices, indices).first;
	}
	//--------------------------------------------------------------------------------------------------
	Model StaticText::parseTextWordByWord(const std::string& text, const Font& font, const double& fontSize, double lineHeightMultiplier, const double& textWidth, AlignmentHorizontal alignment)
	{
		// Create ParseDynamicTextData object
		std::vector<Character> characters;
		std::vector<Line> lines;
		lines.push_back({ 0.0, 0, 0 });
		Font tempFont(font);
		ParseData data{ tempFont, Vec2d{}, Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseWordByWord(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		return data.alignAndConstructModel(alignment, vertices, indices).first;
	}
	//--------------------------------------------------------------------------------------------------
	Model StaticText::parseTextSingleLine(const std::string& text, const Font& font, const double& fontSize, double lineHeightMultiplier, const double& textWidth, AlignmentHorizontal alignment)
	{
		// Create ParseDynamicTextData object
		std::vector<Character> characters;
		std::vector<Line> lines;
		lines.push_back({ 0.0, 0, 0 });
		Font tempFont(font);
		ParseData data{ tempFont, Vec2d{}, Unicode{0}, 0.0, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		data.textWidth = textWidth;
		return data.alignAndConstructModel(alignment, vertices, indices).first;
	}
	//--------------------------------------------------------------------------------------------------
	void StaticText::constructModel(const std::string& text, const Font& font, const double& fontSize, double lineHeightMultiplier, const double& textWidth, const ParseMode& parseMode, AlignmentHorizontal alignment)
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText::ParseMode::WORD_BY_WORD: model = parseTextWordByWord(text, font, fontSize, lineHeightMultiplier, textWidth, alignment); break;
		case SnackerEngine::StaticText::ParseMode::CHARACTERS: model = parseTextCharacters(text, font, fontSize, lineHeightMultiplier, textWidth, alignment); break;
		case SnackerEngine::StaticText::ParseMode::SINGLE_LINE: model = parseTextSingleLine(text, font, fontSize, lineHeightMultiplier, textWidth, alignment); break;
		default: break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	StaticText::StaticText()
		: model{} {}
	//--------------------------------------------------------------------------------------------------
	StaticText::StaticText(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, double lineHeightMultiplier, const ParseMode& parseMode, AlignmentHorizontal alignment)
		: model{}
	{
		constructModel(text, font, fontSize, lineHeightMultiplier, textWidth, parseMode, alignment);
	}	
	//--------------------------------------------------------------------------------------------------
	StaticText::StaticText(StaticText&& other) noexcept
		: model(std::move(other.model))
	{
		other.model = Model();
	}
	//--------------------------------------------------------------------------------------------------
	StaticText& StaticText::operator=(StaticText&& other) noexcept
	{
		model = std::move(other.model);
		other.model = Model();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	Model DynamicText::parseTextCharacters()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	Model DynamicText::parseTextWordByWord()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseWordByWord(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	Model DynamicText::parseTextSingleLine()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, 0.0, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		data.textWidth = textWidth;
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::constructModel()
	{
		//infoLogger << LOGGER::BEGIN << "Reconstructing text model." <<
		//	" text: " << text <<
		//	", textWidth: " << textWidth << LOGGER::ENDL;
		switch (parseMode)
		{
		case SnackerEngine::StaticText::ParseMode::WORD_BY_WORD: model = parseTextWordByWord(); break;
		case SnackerEngine::StaticText::ParseMode::CHARACTERS: model = parseTextCharacters(); break;
		case SnackerEngine::StaticText::ParseMode::SINGLE_LINE: model = parseTextSingleLine(); break;
		default: break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText::DynamicText() 
		: StaticText()
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText::DynamicText(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, double lineHeightMultiplier, const ParseMode& parseMode, AlignmentHorizontal alignment)
		: StaticText(), font(font), fontSize(fontSize), lineHeightMultiplier{ lineHeightMultiplier }, textWidth(textWidth), text(text), parseMode(parseMode),
		alignment(alignment), right(0.0), characters{}, lines{}
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText::DynamicText(const DynamicText& other) noexcept
		: DynamicText(other.text, other.font, other.fontSize, other.textWidth, other.lineHeightMultiplier, other.parseMode, other.alignment) {}
	//--------------------------------------------------------------------------------------------------
	DynamicText::DynamicText(DynamicText&& other) noexcept
		: StaticText(std::move(other)), font(std::move(other.font)), fontSize(std::move(other.fontSize)),
		lineHeightMultiplier{ std::move(other.lineHeightMultiplier) }, textWidth(std::move(other.textWidth)), text(std::move(other.text)),
		parseMode(std::move(other.parseMode)), alignment(std::move(other.alignment)), 
		right(std::move(other.right)), characters(std::move(other.characters)), 
		lines(std::move(other.lines))
	{
		other.text = "";
		other.characters.clear();
		other.lines.clear();
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText& DynamicText::operator=(const DynamicText& other) noexcept
	{
		text = other.text;
		font = other.font;
		fontSize = other.fontSize;
		lineHeightMultiplier = other.lineHeightMultiplier;
		textWidth = other.textWidth;
		parseMode = other.parseMode;
		alignment = other.alignment;
		right = 0.0;
		characters.clear();
		lines.clear();
		constructModel();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	DynamicText& DynamicText::operator=(DynamicText&& other) noexcept
	{
		StaticText::operator=(std::move(other));
		font = std::move(other.font);
		fontSize = std::move(other.fontSize);
		lineHeightMultiplier = std::move(other.lineHeightMultiplier);
		textWidth = std::move(other.textWidth);
		text = std::move(other.text);
		parseMode = std::move(other.parseMode);
		alignment = std::move(other.alignment);
		right = std::move(other.right);
		characters = std::move(other.characters);
		lines = std::move(other.lines);
		other.text = "";
		other.characters.clear();
		other.lines.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	const Font& DynamicText::getFont() const
	{
		return font;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getFontSize() const
	{
		return fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getTextWidth() const
	{
		return getRight() - getLeft();
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getTextHeight() const
	{
		return getTop() - getBottom();
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getLineHeight() const
	{
		return font.getLineHeight() * lineHeightMultiplier;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2d DynamicText::getTextSize() const
	{
		return Vec2d(getRight() - getLeft(), getTop() - getBottom());
	}
	//--------------------------------------------------------------------------------------------------
	const std::string& DynamicText::getText()
	{
		return text;
	}
	//--------------------------------------------------------------------------------------------------
	const Model& DynamicText::getModel() const
	{
		return model;
	}
	//--------------------------------------------------------------------------------------------------
	const StaticText::ParseMode& DynamicText::getParseMode() const
	{
		return parseMode;
	}
	//--------------------------------------------------------------------------------------------------
	AlignmentHorizontal DynamicText::getAlignment() const
	{
		return alignment;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getTop() const
	{
		return font.getAscender() * fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getBottom() const
	{
		return (lines.back().baselineY + font.getDescender()) * fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getLeft() const
	{
		return 0.0;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getRight() const
	{
		return right * fontSize;
	}
	//--------------------------------------------------------------------------------------------------
	double DynamicText::getRight(const unsigned int& lineIndex) const
	{
		if (lineIndex >= lines.size()) return 0.0;
		const auto& line = lines[lineIndex];
		if (line.beginIndex >= characters.size() || line.endIndex >= characters.size()) return 0.0;
		return characters[line.endIndex].right;
	}
	//--------------------------------------------------------------------------------------------------
	std::size_t DynamicText::getNumCharacters()
	{
		return characters.size();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setText(const std::string& text, bool recompute)
	{
		this->text = text;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setTextWidth(const double& textWidth, bool recompute)
	{
		if (this->textWidth == textWidth) return;
		this->textWidth = textWidth;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setFontSize(const double& fontSize, bool recompute)
	{
		this->fontSize = fontSize;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setFont(const Font& font, bool recompute)
	{
		this->font = font;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setLineHeightMultiplier(double lineHeightMultiplier, bool recompute)
	{
		this->lineHeightMultiplier = lineHeightMultiplier;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setParseMode(const StaticText::ParseMode& parseMode, bool recompute)
	{
		this->parseMode = parseMode;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::setAlignment(AlignmentHorizontal alignment, bool recompute)
	{
		this->alignment = alignment;
		if (recompute) constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	void DynamicText::recompute()
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	unsigned int EditableText::getLineNumber(const unsigned int& characterIndex) const
	{
		auto result = std::lower_bound(lines.begin(), lines.end(), Line{ 0, characterIndex, characterIndex });
		if (result < lines.end()) return static_cast<unsigned int>(result - lines.begin());
		return static_cast<unsigned int>(lines.size() - 1);
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextCharacters()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextWordByWord()
	{
		// Create ParseDynamicTextData object
		characters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseData data{ font, Vec2d{}, Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseWordByWord(0, static_cast<unsigned int>(text.size()), text);
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextCharactersFrom(const unsigned int& lineIndex)
	{
		// Create ParseDynamicTextData object
		lines.resize(lineIndex);
		if (lineIndex == 0) lines.push_back({ 0.0, 0, 0 });
		else lines.push_back({ lines.back().baselineY - getLineHeight(), lines.back().endIndex + 1, lines.back().endIndex + 1});
		ParseData data{ font, Vec2d(0.0, lines.back().baselineY), Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines};
		// Parse the text
		data.parseCharacterByCharacter(lines.back().beginIndex, static_cast<unsigned int>(characters.size()));
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	Model EditableText::parseTextSingleLineFrom(const unsigned int& lineIndex)
	{
		// Create ParseDynamicTextData object
		lines.resize(lineIndex);
		if (lineIndex == 0) lines.push_back({ 0.0, 0, 0 });
		else lines.push_back({ lines.back().baselineY - getLineHeight(), lines.back().endIndex + 1, lines.back().endIndex + 1 });
		ParseData data{ font, Vec2d(0.0, lines.back().baselineY), Unicode{0}, 0.0, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseCharacterByCharacter(lines.back().beginIndex, static_cast<unsigned int>(characters.size()));
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		data.textWidth = textWidth;
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
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
		else lines.push_back({ lines.back().baselineY - getLineHeight(), lines.back().endIndex + 1, lines.back().endIndex + 1 });
		ParseData data{ font, Vec2d(0.0, lines.back().baselineY), Unicode{0}, textWidth, fontSize, lineHeightMultiplier, characters, lines };
		// Parse the text
		data.parseWordByWord(lines.back().beginIndex, static_cast<unsigned int>(characters.size()));
		// Finalize the lines vector
		lines.back().endIndex = static_cast<unsigned int>(characters.size() - 1);
		// Construct model
		auto result = data.alignAndConstructModel(alignment, vertices, indices);
		right = result.second;
		return result.first;
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::constructModelFrom(const unsigned int& lineIndex)
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText::ParseMode::WORD_BY_WORD: model = parseTextWordByWordFrom(lineIndex); break;
		case SnackerEngine::StaticText::ParseMode::CHARACTERS: model = parseTextCharactersFrom(lineIndex); break;
		case SnackerEngine::StaticText::ParseMode::SINGLE_LINE: model = parseTextSingleLineFrom(lineIndex); break;
		default: break;
		}
		setCursorPos(cursorPosIndex);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::constructModel()
	{
		switch (parseMode)
		{
		case SnackerEngine::StaticText::ParseMode::WORD_BY_WORD: model = parseTextWordByWord(); break;
		case SnackerEngine::StaticText::ParseMode::CHARACTERS: model = parseTextCharacters(); break;
		case SnackerEngine::StaticText::ParseMode::SINGLE_LINE: model = parseTextSingleLine(); break;
		default: break;
		}
		setCursorPos(cursorPosIndex);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::constructTextFromCharacters()
	{
		std::vector<char> chars;
		for (const auto& character : characters) {
			appendUnicodeCharacter(chars, character.codepoint);
		}
		text = std::string(chars.begin(), chars.end());
		textIsUpToDate = true;
	}
	//--------------------------------------------------------------------------------------------------
	EditableText::SelectionBox EditableText::computeSelectionBox(const unsigned& startCharacterIndex, const unsigned& endCharacterIndex, const unsigned& lineIndex)
	{
		if (characters.empty()) return SelectionBox{};
		SelectionBox result{};
		result.position = computeCursorIndexAndPosition(startCharacterIndex).second * static_cast<float>(fontSize);
		float endPosition;
		if (endCharacterIndex < lines[lineIndex].endIndex) {
			endPosition = computeCursorIndexAndPosition(endCharacterIndex + 1).second.x;
		}
		else {
			if (endCharacterIndex < characters.size()) {
				if (isNewline(characters[endCharacterIndex].codepoint)) {
					endPosition = computeCursorIndexAndPosition(endCharacterIndex).second.x;
				}
				else {
					endPosition = computeCursorIndexAndPosition(endCharacterIndex).second.x + static_cast<float>(font.getGlyph(characters[endCharacterIndex].codepoint).advance);
				}
			}
			else {
				if (isNewline(characters.back().codepoint)) {
					endPosition = computeCursorIndexAndPosition(endCharacterIndex).second.x;
				}
				else {
					endPosition = computeCursorIndexAndPosition(endCharacterIndex).second.x + static_cast<float>(font.getGlyph(characters.back().codepoint).advance);
				}
			}
		}
		result.size = Vec2f(endPosition * static_cast<float>(fontSize) - result.position.x, static_cast<float>(getLineHeight()*fontSize));
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	std::pair<unsigned int, Vec2f> EditableText::computeCursorIndexAndPosition(unsigned int characterIndex)
	{
		auto result = std::make_pair(0, Vec2f());
		if (characterIndex > static_cast<unsigned int>(characters.size())) {
			characterIndex = static_cast<unsigned int>(characters.size());
		}
		unsigned int lineNumber = getLineNumber(characterIndex);
		if (characterIndex == 0) result.second = Vec2f(0.0f, static_cast<float>(lines[lineNumber].baselineY));
		else if (characterIndex == characters.size()) {
			// Cursor is at the end of the text
			const Unicode& codepoint = characters.back().codepoint;
			if (isNewline(codepoint)) {
				result.second = Vec2f(0.0f, static_cast<float>(lines[lineNumber].baselineY));
			}
			else {
				result.second = Vec2f(static_cast<float>(characters.back().left + font.getGlyph(codepoint).advance), 
					static_cast<float>(lines[getLineNumber(static_cast<unsigned int>(characters.size() - 1))].baselineY));
			}
		}
		else {
			result.second = Vec2f(static_cast<float>(characters[characterIndex].left), 
				static_cast<float>(lines[lineNumber].baselineY));
		}
		result.second.x -= cursorWidth;
		result.second.y += static_cast<float>(font.getDescender() + font.getAscender() - font.getLineHeight() * lineHeightMultiplier) / 2.0f;
		result.first = characterIndex;
		switch (alignment)
		{
		case SnackerEngine::AlignmentHorizontal::LEFT:
			break;
		case SnackerEngine::AlignmentHorizontal::CENTER: 
		{
			double lineWidth = getRight(lineNumber);
			result.second.x += static_cast<float>((right - lineWidth) / 2.0);
			break;
		}
		case SnackerEngine::AlignmentHorizontal::RIGHT:
		{
			double lineWidth = getRight(lineNumber);
			result.second.x += static_cast<float>(right - lineWidth);
			break;
		}
		default:
			break;
		}
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	EditableText::EditableText()
		: DynamicText(), textIsUpToDate(true), vertices{}, indices{}, cursorPosIndex(0), selectionIndex(0), cursorPos{}, cursorWidth{}
	{
		constructModel();
	}
	//--------------------------------------------------------------------------------------------------
	EditableText::EditableText(const std::string& text, const Font& font, const double& fontSize, const double& textWidth, const float& cursorWidth, double lineHeightMultiplier, const ParseMode& parseMode, AlignmentHorizontal alignment)
		: DynamicText(text, font, fontSize, textWidth, lineHeightMultiplier, parseMode, alignment), textIsUpToDate(true), vertices{}, indices{}, cursorPosIndex(0), selectionIndex(0), cursorPos{}, cursorWidth(cursorWidth)
	{
		constructModel();
		setCursorPos(0);
	}
	//--------------------------------------------------------------------------------------------------
	EditableText::EditableText(const EditableText& other) noexcept
		: EditableText(other.text, other.font, other.fontSize, other.textWidth, other.cursorWidth, other.lineHeightMultiplier, other.parseMode, other.alignment) {}
	//--------------------------------------------------------------------------------------------------
	EditableText::EditableText(EditableText&& other) noexcept
		: DynamicText(std::move(other)), textIsUpToDate(std::move(other.textIsUpToDate)), 
		vertices(std::move(other.vertices)), indices(std::move(other.indices)),
		cursorPosIndex(std::move(other.cursorPosIndex)), cursorPos(std::move(other.cursorPos)), 
		selectionIndex(std::move(other.selectionIndex)), cursorWidth(std::move(other.cursorWidth))
	{
		other.textIsUpToDate = true;
		other.vertices.clear();
		other.indices.clear();
		other.cursorPosIndex = 0;
		other.cursorPos = Vec2f();
	}
	//--------------------------------------------------------------------------------------------------
	EditableText& EditableText::operator=(const EditableText& other) noexcept
	{
		DynamicText::operator=(other);
		textIsUpToDate = true;
		vertices.clear();
		indices.clear();
		cursorPosIndex = 0;
		selectionIndex = 0; 
		cursorPos = Vec2f();
		cursorWidth = other.cursorWidth;
		constructModel();
		setCursorPos(0);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	EditableText& EditableText::operator=(EditableText&& other) noexcept
	{
		DynamicText::operator=(std::move(other));
		textIsUpToDate = std::move(other.textIsUpToDate);
		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
		cursorPosIndex = std::move(other.cursorPosIndex);
		cursorPos = std::move(other.cursorPos);
		selectionIndex = std::move(other.selectionIndex);
		cursorWidth = std::move(cursorWidth);
		other.textIsUpToDate = true;
		other.vertices.clear();
		other.indices.clear();
		other.cursorPosIndex = 0;
		other.cursorPos = Vec2f();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setText(const std::string& text, bool recompute)
	{
		this->text = text;
		textIsUpToDate = true;
		constructModel();
		if (cursorPosIndex > characters.size()) {
			setCursorPos(static_cast<unsigned int>(characters.size()));
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setCursorPos(unsigned int characterIndex, const bool& moveSelection)
	{
		auto result = computeCursorIndexAndPosition(characterIndex);
		cursorPosIndex = result.first;
		cursorPos = result.second;
		if (moveSelection) {
			setSelectionIndexToCursor();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::computeCursorPosFromMousePos(Vec2d mousePos, const bool& moveSelection)
	{
		if (characters.empty() || lines.empty()) {
			setCursorPos(0, moveSelection);
			return;
		}
		// Find the correct line
		mousePos = mousePos / fontSize;
		// If the mouse is above the text, set cursorPos to text beginning
		if (mousePos.y > lines[0].baselineY + font.getAscender()) {
			setCursorPos(0, moveSelection);
			return;
		}
		double descender = font.getDescender();
		int lineNumber = -1;
		for (unsigned int tempLineNumber = 0; tempLineNumber < lines.size(); tempLineNumber++) {
			if (mousePos.y >= lines[tempLineNumber].baselineY + descender) {
				lineNumber = tempLineNumber;
				break;
			}
		}
		if (lineNumber == -1) {
			setCursorPos(static_cast<unsigned int>(characters.size()) + 1, moveSelection);
			return;
		}
		const auto& line = lines[lineNumber];
		// Find the correct character in the line
		double textOffsetX = 0.0;
		switch (alignment)
		{
		case SnackerEngine::AlignmentHorizontal::LEFT:
			break;
		case SnackerEngine::AlignmentHorizontal::CENTER:
		{
			textOffsetX = (right - getRight(lineNumber)) / 2.0;
			break;
		}
		case SnackerEngine::AlignmentHorizontal::RIGHT:
		{
			textOffsetX = right - getRight(lineNumber);
			break;
		}
		default:
			break;
		}
		int characterIndex = -1;
		for (unsigned int tempCharacterIndex = line.beginIndex; tempCharacterIndex <= line.endIndex; ++tempCharacterIndex) {
			if (mousePos.x <= characters[tempCharacterIndex].right + textOffsetX) {
				characterIndex = tempCharacterIndex;
				break;
			}
		}
		if (characterIndex == -1) {
			if (lines[lineNumber].endIndex < characters.size() && (isSpaceCharacter(characters[lines[lineNumber].endIndex].codepoint) ||
																   isNewline(characters[lines[lineNumber].endIndex].codepoint))) {
				setCursorPos(lines[lineNumber].endIndex, moveSelection);
			}
			else {
				setCursorPos(lines[lineNumber].endIndex + 1, moveSelection);
			}
		}
		else {
			// Check if the mouse is more to the left or more to the right of the character
			if (mousePos.x >= textOffsetX + (characters[characterIndex].right + characters[characterIndex].left) / 2.0) {
				setCursorPos(characterIndex + 1, moveSelection);
			}
			else {
				setCursorPos(characterIndex, moveSelection);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToLeft(const bool& moveSelection)
	{
		if (cursorPosIndex != 0) setCursorPos(cursorPosIndex - 1, moveSelection);
		if (moveSelection) setSelectionIndexToCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToRight(const bool& moveSelection)
	{
		if (cursorPosIndex != characters.size()) setCursorPos(cursorPosIndex + 1, moveSelection);
		if (moveSelection) setSelectionIndexToCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToLeftWordBeginning(const bool& moveSelection)
	{
		if (cursorPosIndex == 0) return;
		unsigned int newIndex = cursorPosIndex - 1;
		if (!isNewline(characters[newIndex].codepoint)) {
			while (newIndex > 0 && !isAlphaNumeric(characters[newIndex].codepoint) && !isNewline(characters[newIndex].codepoint)) {
				newIndex--; 
			}
			while (newIndex > 0 && isAlphaNumeric(characters[static_cast<std::size_t>(newIndex) - 1].codepoint)) {
				newIndex--;
			}
		}
		setCursorPos(newIndex, moveSelection);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::moveCursorToRightWordEnd(const bool& moveSelection)
	{
		if (cursorPosIndex == characters.size()) return;
		unsigned int newIndex = cursorPosIndex;
		if (isNewline(characters[cursorPosIndex].codepoint)) {
			newIndex++;
		}
		else {
			while (newIndex < characters.size() && isAlphaNumeric(characters[newIndex].codepoint)) {
				newIndex++;
			}
			while (newIndex < characters.size() && !isAlphaNumeric(characters[newIndex].codepoint) && !isNewline(characters[newIndex].codepoint)) {
				newIndex++;
			}
		}
		setCursorPos(newIndex, moveSelection);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setSelectionIndexToCursor()
	{
		selectionIndex = cursorPosIndex;
	}
	//--------------------------------------------------------------------------------------------------
	std::vector<EditableText::SelectionBox> EditableText::getSelectionBoxes()
	{
		std::vector<SelectionBox> result;
		// Start with the lower of the two indices
		unsigned startIndex = std::min(static_cast<unsigned>(characters.size()), std::min(cursorPosIndex, selectionIndex));
		unsigned endIndex = std::min(static_cast<unsigned>(characters.size()), std::max(cursorPosIndex, selectionIndex));
		if (startIndex == endIndex) return result;
		unsigned startLine = getLineNumber(startIndex);
		unsigned endLine = getLineNumber(endIndex);
		if (startLine == endLine) {
			result.push_back(computeSelectionBox(startIndex, endIndex - 1, startLine));
		}
		else {
			// First line
			result.push_back(computeSelectionBox(startIndex, lines[startLine].endIndex, startLine));
			// full lines in between
			for (unsigned int lineIndex = startLine + 1; lineIndex < endLine; ++lineIndex) {
				result.push_back(computeSelectionBox(lines[lineIndex].beginIndex, lines[lineIndex].endIndex, lineIndex));
			}
			// Last line
			result.push_back(computeSelectionBox(lines[endLine].beginIndex, endIndex - 1, endLine));
		}
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2f EditableText::getCursorPos() const
	{
		return cursorPos * static_cast<float>(fontSize);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::inputAtCursor(const Unicode& codepoint)
	{
		if (isSelecting()) {
			// If we have a selection, delete characters first
			unsigned endIndex = std::max(cursorPosIndex, selectionIndex) - 1;
			deleteCharacters(std::min(cursorPosIndex, selectionIndex), endIndex);
		}
		characters.insert(characters.begin() + cursorPosIndex, { codepoint, 0.0, 0.0 });
		constructModelFrom(getLineNumber(cursorPosIndex));
		textIsUpToDate = false;
		setCursorPos(cursorPosIndex + 1);
		setSelectionIndexToCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::inputAtCursor(const std::string& text)
	{
		unsigned position = 0;
		unsigned startingCursorPosIndex = cursorPosIndex;
		if (isSelecting()) {
			// If we have a selection, delete characters first
			unsigned endIndex = std::max(cursorPosIndex, selectionIndex) - 1;
			deleteCharacters(std::min(cursorPosIndex, selectionIndex), endIndex);
		}
		while (position < text.size()) {
			std::optional<Unicode> codepoint = getNextCodepointUTF8(text, position);
			if (!codepoint.has_value()) return;
			characters.insert(characters.begin() + cursorPosIndex, { codepoint.value(), 0.0, 0.0});
			cursorPosIndex++;
		}
		constructModelFrom(getLineNumber(startingCursorPosIndex));
		textIsUpToDate = false;
		setCursorPos(cursorPosIndex);
		setSelectionIndexToCursor();
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
		if (endIndex >= characters.size()) endIndex = static_cast<unsigned int>(characters.size() - 1);
		characters.erase(characters.begin() + beginIndex, characters.begin() + endIndex + 1);
		constructModelFrom(getLineNumber(beginIndex));
		textIsUpToDate = false;
		if (cursorPosIndex > beginIndex) {
			if (cursorPosIndex < endIndex) {
				setCursorPos(beginIndex);
			}
			else {
				setCursorPos(cursorPosIndex + beginIndex - endIndex - 1);
			}
		}
		setSelectionIndexToCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteCharacterBeforeCursor()
	{
		if (isSelecting()) {
			// If we have a selection, delete characters first
			unsigned endIndex = std::max(cursorPosIndex, selectionIndex) - 1;
			deleteCharacters(std::min(cursorPosIndex, selectionIndex), endIndex);
		}
		else if (cursorPosIndex > 0) {
			deleteCharacters(cursorPosIndex - 1, cursorPosIndex - 1);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteWordBeforeCursor()
	{
		if (isSelecting()) {
			// If we have a selection, delete characters first
			unsigned endIndex = std::max(cursorPosIndex, selectionIndex) - 1;
			deleteCharacters(std::min(cursorPosIndex, selectionIndex), endIndex);
		}
		else {
			if (cursorPosIndex == 0) return;
			unsigned int beginIndex = cursorPosIndex - 1;
			if (!isNewline(characters[beginIndex].codepoint)) {
				while (beginIndex > 0 && !isAlphaNumeric(characters[beginIndex].codepoint) && !isNewline(characters[beginIndex].codepoint)) {
					beginIndex--;
				}
				while (beginIndex > 0 && isAlphaNumeric(characters[static_cast<std::size_t>(beginIndex) - 1].codepoint)) {
					beginIndex--;
				}
			}
			deleteCharacters(beginIndex, cursorPosIndex - 1);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteCharacterAfterCursor()
	{
		if (isSelecting()) {
			// If we have a selection, delete characters first
			unsigned endIndex = std::max(cursorPosIndex, selectionIndex) - 1;
			deleteCharacters(std::min(cursorPosIndex, selectionIndex), endIndex);
		}
		else if (cursorPosIndex < characters.size()) {
			deleteCharacters(cursorPosIndex, cursorPosIndex);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::deleteWordAfterCursor()
	{
		if (isSelecting()) {
			// If we have a selection, delete characters first
			unsigned endIndex = std::max(cursorPosIndex, selectionIndex) - 1;
			deleteCharacters(std::min(cursorPosIndex, selectionIndex), endIndex);
		}
		else {
			if (cursorPosIndex >= characters.size()) return;
			unsigned int endIndex = cursorPosIndex;
			if (!isNewline(characters[endIndex].codepoint)) {
				while (endIndex < characters.size() && isAlphaNumeric(characters[endIndex].codepoint)) {
					endIndex++;
				}
				while (static_cast<std::size_t>(endIndex) + 1 < characters.size() && !isAlphaNumeric(characters[static_cast<std::size_t>(endIndex) + 1].codepoint) && !isNewline(characters[static_cast<std::size_t>(endIndex) + 1].codepoint)) {
					endIndex++;
				}
			}
			deleteCharacters(cursorPosIndex, endIndex);
		}
	}
	//--------------------------------------------------------------------------------------------------
	Vec2f EditableText::getCursorSize() const
	{
		return Vec2f(getCursorWidth(), getCursorHeight());
	}
	//--------------------------------------------------------------------------------------------------
	float EditableText::getCursorWidth() const
	{
		return cursorWidth * static_cast<float>(fontSize);
	}
	//--------------------------------------------------------------------------------------------------
	float EditableText::getCursorHeight() const
	{
		return getLineHeight() * static_cast<float>(fontSize);
	}
	//--------------------------------------------------------------------------------------------------
	const std::string& EditableText::getText()
	{
		if (textIsUpToDate) {
			return text;
		}
		else {
			// Create text from characters vector
			constructTextFromCharacters();
			return text;
		}
	}
	std::string EditableText::getText(unsigned start, unsigned end)
	{
		std::vector<char> chars;
		start = std::min(start, static_cast<unsigned>(characters.size() - 1));
		end = std::min(end, static_cast<unsigned>(characters.size() - 1));
		for (unsigned i = start; i <= end; i++) {
			appendUnicodeCharacter(chars, characters[i].codepoint);
		}
		if (chars.empty()) return "";
		else return std::string(chars.begin(), chars.end());
	}
	//--------------------------------------------------------------------------------------------------
	std::string EditableText::getSelectedText()
	{
		std::vector<char> chars;
		return getText(std::min(cursorPosIndex, selectionIndex), std::max(cursorPosIndex, selectionIndex) - 1);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setTextWidth(const double& textWidth, bool recompute)
	{
		this->textWidth = textWidth;
		if (recompute) constructModelFrom(0);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setFontSize(const double& fontSize, bool recompute)
	{
		this->fontSize = fontSize;
		if (recompute) constructModelFrom(0);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setFont(const Font& font, bool recompute)
	{
		this->font = font;
		if (recompute) constructModelFrom(0);
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setParseMode(const StaticText::ParseMode& parseMode, bool recompute)
	{
		if (this->parseMode != parseMode) {
			this->parseMode = parseMode;
			if (recompute) constructModelFrom(0);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setAlignment(AlignmentHorizontal alignment, bool recompute)
	{
		if (this->alignment != alignment) {
			this->alignment = alignment;
			if (recompute) constructModelFrom(0);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::setCursorWidth(const float& cursorWidth)
	{
		this->cursorWidth = cursorWidth;
	}
	//--------------------------------------------------------------------------------------------------
	bool EditableText::isSelecting() const
	{
		return cursorPosIndex != selectionIndex;
	}
	//--------------------------------------------------------------------------------------------------
	void EditableText::recompute()
	{
		constructModelFrom(0);
	}
	//--------------------------------------------------------------------------------------------------
}