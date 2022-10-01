#include "Gui/Text/Text.h"
#include "core/Log.h"
#include "AssetManager/MeshManager.h"

#include <optional>

#include <iostream>
#include <iomanip>
#include <bitset>

namespace SnackerEngine
{

	/// Helper function that returns the next codepoint and advances the position
	std::optional<Unicode> getNextCodepoint(const std::u8string& text, unsigned int& position)
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

	/// Helper function that returns true if the given codepoint is considered a whitespace character
	bool isWhiteSpace(const Unicode& codepoint)
	{
		return
			codepoint == 0x0020 ||	// space
			codepoint == 0x0009 ||	// tab
			codepoint == 0x0085 ||	// newline
			codepoint == 0x000A;	// newline
	}

	/// Helper function that resturns true if the given codepoint is the space character
	bool isSpaceCharacter(const Unicode& codepoint) 
	{
		return codepoint == 0x0020;
	}

	/// Helper function that returns true if the given codepoint is the newline character '\n'
	bool isNewline(const Unicode& codepoint)
	{
		return
			codepoint == 0x0085 ||
			codepoint == 0x000A;
	}

	/// Helper function that returns the 'newline' codepoint
	const Unicode& getNewlineCodepoint() 
	{
		return 0x0085;
	}

	/// Struct that is used to hold data during the parsing of a static text object
	struct ParseStaticTextData
	{
		/// The font the text is written in
		Font& font;
		/// The current baseline
		Vec2d currentBaseline;
		/// The last codepoint (necessary for kerning)
		/// If lastCodepoint == 0, we are in a new line (currentBaseline.x == 0.0)
		Unicode lastCodepoint;
		/// The vertices of the text model
		std::vector<Vec4f>& vertices;
		/// The indices of the text model
		std::vector<unsigned int>& indices;
		/// The maximum allowed line width in pt
		const double& textWidth;
		/// The font size we'd like to parse in
		const double& fontSize;

		/// Constructor
		ParseStaticTextData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, std::vector<Vec4f>& vertices,
			std::vector<unsigned int>& indices, const double& textWidth, const double& fontSize);

		/// Checks if the given position still is inside the maximum allowed line width
		bool isInsideTextWidth(const double& position);
		/// Goes to the next line, changing currentBaseline. Does not set lastCodepoint to zero!
		virtual void goToNextLine(Font& font);
		/// Advances currentBaseline by using kerning from lastCodepoint to the given codepoint. 
		/// Does not update lastCodepoint
		void advanceBaselineWithKerning(const Unicode& newCodepoint);
		/// Creates vertices and indices for a single glyph. Also advances the baseline accordingly.
		/// Does not check if a new line is necessary. Changes the left and right bounds of the glyph
		/// according to the baseline
		void pushGlyph(Glyph& glyph);
		/// Reads in and pushes glyphs until a whitespace character is found. Also advances the baseline
		/// accordingly. Does not check if a new line is necessary. This function can be used for parsing whole
		/// words. After this function is called the baseline is at the beginning of the last glyph. 
		/// Advances the index into the utf8 string as well.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		virtual bool pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::u8string& text);
		/// Parses the text character by character, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary. Does not care about word boundaries.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		virtual bool parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::u8string& text);
		/// Constructs the model from the vertices and indices vectors
		Model constructModel();
	};

	void ParseStaticTextData::goToNextLine(Font& font)
	{
		currentBaseline.x = 0;
		currentBaseline.y -= font.getLineHeight();
	}

	ParseStaticTextData::ParseStaticTextData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices, const double& textWidth, const double& fontSize)
		: font(font), currentBaseline(currentBaseline), lastCodepoint(lastCodepoint), vertices(vertices),
		indices(indices), textWidth(textWidth), fontSize(fontSize) {}

	bool SnackerEngine::ParseStaticTextData::isInsideTextWidth(const double& position)
	{
		return position * fontSize <= textWidth;
	}

	void SnackerEngine::ParseStaticTextData::advanceBaselineWithKerning(const Unicode& newCodepoint)
	{
		currentBaseline.x += font.getAdvance(lastCodepoint, newCodepoint);
	}

	void ParseStaticTextData::pushGlyph(Glyph& glyph)
	{
		// Push indices for two triangles
		indices.push_back(static_cast<unsigned int>(vertices.size())); indices.push_back(static_cast<unsigned int>(vertices.size() + 1)); indices.push_back(static_cast<unsigned int>(vertices.size() + 2));
		indices.push_back(vertices.size() + 2); indices.push_back(vertices.size() + 3); indices.push_back(vertices.size());
		// Push four vertices
		glyph.left += currentBaseline.x;
		glyph.right += currentBaseline.x;
		glyph.top += currentBaseline.y;
		glyph.bottom += currentBaseline.y;
		vertices.push_back(Vec4f(glyph.left, glyph.bottom, glyph.texLeft, glyph.texBottom));
		vertices.push_back(Vec4f(glyph.left, glyph.top, glyph.texLeft, glyph.texTop));
		vertices.push_back(Vec4f(glyph.right, glyph.top, glyph.texRight, glyph.texTop));
		vertices.push_back(Vec4f(glyph.right, glyph.bottom, glyph.texRight, glyph.texBottom));
	}

	bool ParseStaticTextData::pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::u8string& text)
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
			pushGlyph(glyph);
			// Update lastCodepoint
			lastCodepoint = codepoint.value();
			// Update the lastIndex
			lastIndexIntoUTF8String = indexIntoUTF8String;
		}
		return true;
	}

	bool ParseStaticTextData::parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::u8string& text)
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
				goToNextLine(font);
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
			if (!isInsideTextWidth(currentBaseline.x + glyph.right)) {
				goToNextLine(font);
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (isSpaceCharacter(codepoint.value())) {
					lastCodepoint = 0;
				}
			}
			// Add the new glyph
			pushGlyph(glyph);
		}
		return true;
	}

	Model ParseStaticTextData::constructModel()
	{
		VertexBufferLayout layout;
		layout.push<Vec2f>(1);
		layout.push<Vec2f>(1);
		Mesh mesh = MeshManager::createMesh<Vec4f>(layout, vertices, indices);
		return Model(mesh);
	}

	Model StaticText::parseTextCharacters()
	{
		// Create ParseStaticTextData object
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		ParseStaticTextData data{ font, Vec2d{}, Unicode{0}, vertices, indices, textWidth, fontSize };
		// Parse the text
		data.parseCharacterByCharacter(0, text.size(), text);
		// Save bottom y coordinate
		bottom = data.currentBaseline.y + data.font.getDescender();
		// Construct model
		return data.constructModel();
	}

	Model StaticText::parseTextWordByWord()
	{
		// Create ParseStaticTextData object
		std::vector<Vec4f> vertices;
		std::vector<unsigned int> indices;
		ParseStaticTextData data{ font, Vec2d{}, Unicode{0}, vertices, indices, textWidth, fontSize };
		// Index into the text
		unsigned int indexIntoUTF8String = 0;
		// Go through the text character by character
		while (indexIntoUTF8String < text.size()) {
			/// Before we parse the next codepoint, we need to save the index into the UTF8 string, because we might
			/// need to backtrack if the parsed word does not fit into the current line
			unsigned int previousIndexIntoUTF8String = indexIntoUTF8String;
			// Now we can parse the next character
			auto codepoint = getNextCodepoint(text, indexIntoUTF8String);
			if (!codepoint) {
				warningLogger << LOGGER::BEGIN << "text was not formatted properly (can only read UTF-8 formatting)" << LOGGER::ENDL;
				return Model();
			}
			// First check if the parsed codepoint was a newline character
			if (isNewline(codepoint.value())) {
				data.goToNextLine(font);
				data.lastCodepoint = 0;
				continue;
			}
			// Next, check if we have a whitespace character
			if (isWhiteSpace(codepoint.value())) {
				/// Whitespaces are treated equally as in the CHARACTERS case: Try to put it on the current line,
				/// if it doesn't fit put it on the next line!
				if (data.lastCodepoint != 0) {
					// Advance to next glyph
					data.advanceBaselineWithKerning(codepoint.value());
				}
				// Update lastCodepoint
				data.lastCodepoint = codepoint.value();
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!data.isInsideTextWidth(data.currentBaseline.x + glyph.right)) {
					data.goToNextLine(font);
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(codepoint.value())) {
						data.lastCodepoint = 0;
					}
				}
				// Add the new glyph
				data.pushGlyph(glyph);
			}
			else {
				/// We have a non-whitespace character. First: save some additional information that is necessary when
				/// we need to backtrack
				// Save the current baseline x position
				double previousBaselineX = data.currentBaseline.x;
				// Save the last codepoint
				Unicode previouseLastCodepoint = data.lastCodepoint;
				// Save the size of the vertices and indices vector
				unsigned int verticesSize = vertices.size();
				unsigned int indicesSize = indices.size();
				// Now: place the character we just read in at the correct position
				if (data.lastCodepoint != 0) {
					// Advance to next glyph
					data.advanceBaselineWithKerning(codepoint.value());
				}
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!data.isInsideTextWidth(data.currentBaseline.x + glyph.right)) {
					data.goToNextLine(font);
					previousBaselineX = data.currentBaseline.x;
					data.lastCodepoint = 0;
					previouseLastCodepoint = 0;
				}
				else 
				{
					// Update lastCodepoint
					data.lastCodepoint = codepoint.value();
				}
				// Add the new glyph
				data.pushGlyph(glyph);
				/// Now, we want to read the remaining word.
				// First try to just parse the word in the current line
				data.pushGlyphsUntilWhitespace(indexIntoUTF8String, text);
				// Now, we can have three cases:
				double rightPositionX = data.currentBaseline.x + font.getGlyph(data.lastCodepoint).right;
				if (data.isInsideTextWidth(rightPositionX)) {
					// 1.)	The word fits, we can just move on!
				}
				else if (data.isInsideTextWidth(rightPositionX - previousBaselineX))
				{
					// 2.)	The word fits inside a single line, but not on the current line -> we have to move to the next line!
					data.goToNextLine(font);
					data.lastCodepoint = 0;
					// Now we can read in the word again, but this time it will fit.
					// We also have to delete the vertices and indices we have read in before; they are no longer valid.
					vertices.resize(verticesSize);
					indices.resize(indicesSize);
					indexIntoUTF8String = previousIndexIntoUTF8String;
					data.pushGlyphsUntilWhitespace(indexIntoUTF8String, text);
				}
				else
				{
					// 3.)	The word does not even fit into a single line. In this case we fall back to the
					//		CHARACTERS behaviour: Try to put it on the current line,
					//		if it doesn't fit put it on the next line!
					vertices.resize(verticesSize);
					indices.resize(indicesSize);
					data.currentBaseline.x = previousBaselineX;
					data.lastCodepoint = previouseLastCodepoint;
					data.parseCharacterByCharacter(previousIndexIntoUTF8String, indexIntoUTF8String, text);
				}
			}
		}
		// Save bottom y coordinate
		bottom = data.currentBaseline.y + data.font.getDescender();
		// Construct model
		return data.constructModel();
	}

	void StaticText::constructModel()
	{
		switch (parseMode)
		{
		case SnackerEngine::TextParseMode::WORD_BY_WORD: model = parseTextWordByWord(); break;
		case SnackerEngine::TextParseMode::CHARACTERS: model = parseTextCharacters(); break;
		default: break;
		}
	}

	/// Struct that is used to hold data duringthe parsing of a dynamic text object. Since different data structures
	/// Need to be managed, this holds additional member variables and functions but still has some functionality
	/// derived from struct ParseStaticTextData
	struct ParseDynamicTextData : public ParseStaticTextData
	{
		/// The vector in which the DynamicTextCharacter objects are stored, necessary for saving work when characters are inserted or deleted.
		std::vector<DynamicTextCharacter>& dynamicTextCharacters;
		/// The vector in which the lines are stored. Necessary for quickly finding the y position of a given character
		std::vector<DynamicTextLine>& lines;

		/// Constructor
		ParseDynamicTextData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, std::vector<Vec4f>& vertices,
			std::vector<unsigned int>& indices, const double& textWidth, const double& fontSize, 
			std::vector<DynamicTextCharacter>& dynamicTextCharacters, std::vector<DynamicTextLine>& lines);

		/// Goes to the next line, changing currentBaseline. Does not set lastCodepoint to zero!
		/// Also edits the lines vector. For this it is assumed that the character at the back of
		/// dynamicTextCharacters is the last character of the previous line!
		void goToNextLine(Font& font) override;
		/// The same as goToNextLine(), but uses the given index into the characters vector as the next
		/// character index instead of the size of the characters vector! Used when parsing from the 
		/// characters vector
		void goToNextLine(const unsigned& indexIntoCharacters);
		/// Creates vertices and indices for a single glyph. Also advances the baseline accordingly.
		/// Does not check if a new line is necessary. Changes the left and right bounds of the glyph
		/// according to the baseline. Also adds the glyph to the back of the dynamicTextCharacters vector
		void pushGlyph(Glyph& glyph, const Unicode& codepoint);
		/// Same as pushGlyph, but for newline character
		void pushNewlineGlyph();
		/// The same as pushGlyph(), but does not add new data to vectors and instead changes/edits existing
		/// data. This function can be used when constructing the text model from the existing characters
		/// vector.
		void editGlyph(Glyph& glyph, const unsigned& indexIntoCharacters);
		/// Same as editGlyph, but for newline character
		void editNewlineGlyph(const unsigned& indexIntoCharacters);
		/// Reads in and pushes glyphs until a whitespace character is found. Also advances the baseline
		/// accordingly. Does not check if a new line is necessary. This function can be used for parsing whole
		/// words. After this function is called the baseline is at the beginning of the last glyph. 
		/// Advances the index into the utf8 string as well.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Also adds all glyphs to the back of the dynamicTextCharacters vector
		bool pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::u8string& text) override;
		/// The same as pushGlyphsUntilWhitespace(), but parses from the characters vector instead of from text.
		/// Advances the index into the characters vector. In this variant of the function errors
		/// cannot occur since all unicode characters are already parsed from the text.
		void editGlyphsUntilWhitespace(unsigned& indexIntoCharacters);
		/// Parses the text character by character, starting from beginIndex (inclusive) until 
		/// endIndex is reached (exclusive). Updates the baseline accordingly and jumps to a 
		/// new line if necessary. Does not care about word boundaries.
		/// If false is returned, an error occured during parsing and an warning message was printed.
		/// Assumes that the lines vector has at least one entry and is properly set up.
		bool parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::u8string& text) override;
		/// Parses the text character by character, starting from beginIndex (inclusive) until endIndex
		/// is reached (exclusive). Indices are into the dynamicTextCharacters vector.
		/// Updates the baseline accordingly and jumps to a new line if necessary.
		/// Does not care about word boundaries. Works in the vectors and does not push anything (therefore 
		/// assumes all vectors have enough space).
		/// If false is returned, an error occured during parsing and an warning message was printed.
		///  Assumes that the lines vector has at least one entry and is properly set up.
		bool parseCharacterByCharacterFromUnicodeVector(const unsigned& beginc, const unsigned& endIndexIntoCharacters);
	};

	ParseDynamicTextData::ParseDynamicTextData(Font& font, const Vec2d& currentBaseline, const Unicode& lastCodepoint, std::vector<Vec4f>& vertices, std::vector<unsigned int>& indices, const double& textWidth, const double& fontSize, std::vector<DynamicTextCharacter>& dynamicTextCharacters, std::vector<DynamicTextLine>& lines)
		: ParseStaticTextData(font, currentBaseline, lastCodepoint, vertices, indices, textWidth, fontSize), dynamicTextCharacters(dynamicTextCharacters), lines(lines) {}

	void ParseDynamicTextData::goToNextLine(Font& font)
	{
		ParseStaticTextData::goToNextLine(font);
		/// we need to additionally update the lines vector!
		lines.back().endIndex = dynamicTextCharacters.size() - 1;
		lines.push_back(DynamicTextLine{ currentBaseline.y, static_cast<unsigned int>(dynamicTextCharacters.size()), 
			static_cast<unsigned int>(dynamicTextCharacters.size()) });
	}

	void ParseDynamicTextData::goToNextLine(const unsigned& indexIntoCharacters)
	{
		ParseStaticTextData::goToNextLine(font);
		/// we need to additionally update the lines vector!
		lines.back().endIndex = indexIntoCharacters - 1;
		lines.push_back(DynamicTextLine{ currentBaseline.y, indexIntoCharacters, indexIntoCharacters });
	}

	void ParseDynamicTextData::pushGlyph(Glyph& glyph, const Unicode& codepoint)
	{
		ParseStaticTextData::pushGlyph(glyph);
		dynamicTextCharacters.push_back({ codepoint, glyph.left, glyph.right });
	}

	void ParseDynamicTextData::pushNewlineGlyph()
	{
		// Push indices for two triangles
		indices.push_back(vertices.size()); indices.push_back(vertices.size() + 1); indices.push_back(vertices.size() + 2);
		indices.push_back(vertices.size() + 2); indices.push_back(vertices.size() + 3); indices.push_back(vertices.size());
		// Push four vertices (which will not get drawn)
		vertices.push_back(Vec4f());
		vertices.push_back(Vec4f());
		vertices.push_back(Vec4f());
		vertices.push_back(Vec4f());
		// Update characters vector
		if (!dynamicTextCharacters.empty()) {
			double position = 0.0;
			if (!isNewline(dynamicTextCharacters.back().codepoint)) {
				position = dynamicTextCharacters.back().leftPosition +
					font.getGlyph(dynamicTextCharacters.back().codepoint).advance;
			}
			dynamicTextCharacters.push_back({ getNewlineCodepoint(), position, position });
		}
		else
			dynamicTextCharacters.push_back({ getNewlineCodepoint(), 0.0, 0.0 });
	}

	void ParseDynamicTextData::editGlyph(Glyph& glyph, const unsigned& indexIntoCharacters)
	{
		// Edit indices for two triangles
		indices[6 * indexIntoCharacters + 0] = 4 * indexIntoCharacters + 0;
		indices[6 * indexIntoCharacters + 1] = 4 * indexIntoCharacters + 1;
		indices[6 * indexIntoCharacters + 2] = 4 * indexIntoCharacters + 2;
		indices[6 * indexIntoCharacters + 3] = 4 * indexIntoCharacters + 2;
		indices[6 * indexIntoCharacters + 4] = 4 * indexIntoCharacters + 3;
		indices[6 * indexIntoCharacters + 5] = 4 * indexIntoCharacters + 0;
		// Edit four vertices
		glyph.left += currentBaseline.x;
		glyph.right += currentBaseline.x;
		glyph.top += currentBaseline.y;
		glyph.bottom += currentBaseline.y;
		vertices[4 * indexIntoCharacters + 0] = Vec4f(glyph.left, glyph.bottom, glyph.texLeft, glyph.texBottom);
		vertices[4 * indexIntoCharacters + 1] = Vec4f(glyph.left, glyph.top, glyph.texLeft, glyph.texTop);
		vertices[4 * indexIntoCharacters + 2] = Vec4f(glyph.right, glyph.top, glyph.texRight, glyph.texTop);
		vertices[4 * indexIntoCharacters + 3] = Vec4f(glyph.right, glyph.bottom, glyph.texRight, glyph.texBottom);
		// Edit characters vector
		dynamicTextCharacters[indexIntoCharacters].leftPosition = glyph.left;
		dynamicTextCharacters[indexIntoCharacters].rightPosition = glyph.right;
	}

	void ParseDynamicTextData::editNewlineGlyph(const unsigned& indexIntoCharacters)
	{
		// Edit indices for two triangles
		indices[6 * indexIntoCharacters + 0] = 4 * indexIntoCharacters + 0;
		indices[6 * indexIntoCharacters + 1] = 4 * indexIntoCharacters + 1;
		indices[6 * indexIntoCharacters + 2] = 4 * indexIntoCharacters + 2;
		indices[6 * indexIntoCharacters + 3] = 4 * indexIntoCharacters + 2;
		indices[6 * indexIntoCharacters + 4] = 4 * indexIntoCharacters + 3;
		indices[6 * indexIntoCharacters + 5] = 4 * indexIntoCharacters + 0;
		// Edit four vertices
		vertices[4 * indexIntoCharacters + 0] = Vec4f();
		vertices[4 * indexIntoCharacters + 1] = Vec4f();
		vertices[4 * indexIntoCharacters + 2] = Vec4f();
		vertices[4 * indexIntoCharacters + 3] = Vec4f();
		// Update characters vector
		if (indexIntoCharacters > 0) {
			double position = 0.0;
			if (!isNewline(dynamicTextCharacters[indexIntoCharacters - 1].codepoint)) {
				position = dynamicTextCharacters[indexIntoCharacters - 1].leftPosition +
					font.getGlyph(dynamicTextCharacters[indexIntoCharacters - 1].codepoint).advance;
			}
			dynamicTextCharacters[indexIntoCharacters] = { getNewlineCodepoint(), position, position };
		}
		else
			dynamicTextCharacters.push_back({ getNewlineCodepoint(), 0.0, 0.0 });
	}

	bool ParseDynamicTextData::pushGlyphsUntilWhitespace(unsigned& indexIntoUTF8String, const std::u8string& text)
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

	void ParseDynamicTextData::editGlyphsUntilWhitespace(unsigned& indexIntoCharacters)
	{
		// Go through the text character by character
		while (indexIntoCharacters < dynamicTextCharacters.size())
		{
			DynamicTextCharacter& character = dynamicTextCharacters[indexIntoCharacters];
			// Check if the parsed character is whitespace, in which case we return
			if (isWhiteSpace(character.codepoint)) {
				return;
			}
			// We treat every character that is not a whitespace character equal: put it into the current line.
			// We don't check if we need a new line!
			if (lastCodepoint != 0) {
				// Advance to next glyph
				advanceBaselineWithKerning(character.codepoint);
			}
			Glyph glyph = font.getGlyph(character.codepoint);
			// Add the new glyph
			editGlyph(glyph, indexIntoCharacters);
			// Update lastCodepoint
			lastCodepoint = character.codepoint;
			// Update the lastIndex
			indexIntoCharacters++;
		}
		return;
	}

	bool ParseDynamicTextData::parseCharacterByCharacter(const unsigned& beginIndexIntoUTF8String, const unsigned& endIndexIntoUTF8String, const std::u8string& text)
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
				goToNextLine(font);
				pushNewlineGlyph();
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
			if (!isInsideTextWidth(currentBaseline.x + glyph.right)) {
				goToNextLine(font);
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
		lines.back().endIndex = dynamicTextCharacters.size() - 1;
		return true;
	}

	bool ParseDynamicTextData::parseCharacterByCharacterFromUnicodeVector(const unsigned& beginIndexIntoCharacters, const unsigned& endIndexIntoCharacters)
	{
		// Go through the text character by character
		unsigned indexIntoCharacters = beginIndexIntoCharacters;
		while (indexIntoCharacters < endIndexIntoCharacters)
		{
			DynamicTextCharacter& character = dynamicTextCharacters[indexIntoCharacters];
			// First check if the parsed codepoint was a newline character
			if (isNewline(character.codepoint)) {
				goToNextLine(indexIntoCharacters + 1);
				editNewlineGlyph(indexIntoCharacters);
				lastCodepoint = 0;
				// Advance to next codepoint
				indexIntoCharacters++;
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
			if (!isInsideTextWidth(currentBaseline.x + glyph.right)) {
				goToNextLine(indexIntoCharacters);
				// Special rule: If the first character on a new line is a space character, we dont 
				// want to advance the next character!
				if (isSpaceCharacter(character.codepoint)) {
					lastCodepoint = 0;
				}
			}
			// Add the new glyph
			editGlyph(glyph, indexIntoCharacters);
			// Advance to next codepoint
			indexIntoCharacters++;
		}
		// Finalize the lines vector
		lines.back().endIndex = dynamicTextCharacters.size() - 1;
		return true;
	}

	StaticText::StaticText()
		: font(Font()), fontSize(0), textWidth(0), text(u8""), model(), parseMode{}, alignment{}, bottom(0) {}

	StaticText::StaticText(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const TextParseMode& parseMode, const Alignment& alignment)
		: font(font), fontSize(fontSize), textWidth(textWidth), text(text), model(), parseMode(parseMode), alignment(alignment), bottom(0)
	{
		constructModel();
	}

	const std::u8string& StaticText::getText() const
	{
		return text;
	}

	void StaticText::setText(const std::u8string& text)
	{
		this->text = text;
		constructModel();
	}

	void StaticText::setTextWidth(const double& textWidth)
	{
		this->textWidth = textWidth;
		constructModel();
	}

	const double& StaticText::getFontSize() const
	{
		return fontSize;
	}

	const Model& StaticText::getModel()
	{
		return model;
	}

	const Font& StaticText::getFont()
	{
		return font;
	}

	const Alignment& StaticText::getAlignment() const
	{
		return alignment;
	}

	const double& StaticText::getTop()
	{
		return font.getAscender();
	}

	const double& StaticText::getBottom()
	{
		return bottom;
	}

	Model DynamicText::parseTextWordByWord()
	{
		// Create ParseDynamicTextData object
		unicodeCharacters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseDynamicTextData data{ font, Vec2d{}, Unicode{0}, vertices, indices, textWidth, fontSize, unicodeCharacters, lines };
		// Index into the text
		unsigned int indexIntoUTF8String = 0;
		// Go through the text character by character
		while (indexIntoUTF8String < text.size()) {
			/// Before we parse the next codepoint, we need to save the index into the UTF8 string, because we might
			/// need to backtrack if the parsed word does not fit into the current line
			unsigned int previousIndexIntoUTF8String = indexIntoUTF8String;
			// Now we can parse the next character
			auto codepoint = getNextCodepoint(text, indexIntoUTF8String);
			if (!codepoint) {
				warningLogger << LOGGER::BEGIN << "text was not formatted properly (can only read UTF-8 formatting)" << LOGGER::ENDL;
				return Model();
			}
			// First check if the parsed codepoint was a newline character
			if (isNewline(codepoint.value())) {
				data.goToNextLine(font);
				data.lastCodepoint = 0;
				continue;
			}
			// Next, check if we have a whitespace character
			if (isWhiteSpace(codepoint.value())) {
				/// Whitespaces are treated equally as in the CHARACTERS case: Try to put it on the current line,
				/// if it doesn't fit put it on the next line!
				if (data.lastCodepoint != 0) {
					// Advance to next glyph
					data.advanceBaselineWithKerning(codepoint.value());
				}
				// Update lastCodepoint
				data.lastCodepoint = codepoint.value();
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!data.isInsideTextWidth(data.currentBaseline.x + glyph.right)) {
					data.goToNextLine(font);
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(codepoint.value())) {
						data.lastCodepoint = 0;
					}
				}
				// Add the new glyph
				data.pushGlyph(glyph, codepoint.value());
			}
			else {
				/// We have a non-whitespace character. First: save some additional information that is necessary when
				/// we need to backtrack
				// Save the current baseline x position
				double previousBaselineX = data.currentBaseline.x;
				// Save the last codepoint
				Unicode previouseLastCodepoint = data.lastCodepoint;
				// Save the size of the vertices, indices and dynamicTextCharacters vector
				unsigned int verticesSize = vertices.size();
				unsigned int indicesSize = indices.size();
				unsigned int dynamicTextCharactersSize = data.dynamicTextCharacters.size();
				// Now: place the character we just read in at the correct position
				if (data.lastCodepoint != 0) {
					// Advance to next glyph
					data.advanceBaselineWithKerning(codepoint.value());
				}
				Glyph glyph = font.getGlyph(codepoint.value());
				// Check if we need a new line
				if (!data.isInsideTextWidth(data.currentBaseline.x + glyph.right)) {
					data.goToNextLine(font);
					previousBaselineX = data.currentBaseline.x;
					data.lastCodepoint = 0;
					previouseLastCodepoint = 0;
				}
				else {
					// Update lastCodepoint
					data.lastCodepoint = codepoint.value();
				}
				// Add the new glyph
				data.pushGlyph(glyph, codepoint.value());
				/// Now, we want to read the remaining word.
				// First try to just parse the word in the current line
				data.pushGlyphsUntilWhitespace(indexIntoUTF8String, text);
				// Now, we can have three cases:
				double rightPositionX = data.currentBaseline.x + font.getGlyph(data.lastCodepoint).right;
				if (data.isInsideTextWidth(rightPositionX)) {
					// 1.)	The word fits, we can just move on!
				}
				else if (data.isInsideTextWidth(rightPositionX - previousBaselineX))
				{
					// 2.)	The word fits inside a single line, but not on the current line -> we have to move to the next line!
					data.dynamicTextCharacters.resize(dynamicTextCharactersSize);
					data.goToNextLine(font);
					data.lastCodepoint = 0;
					// Now we can read in the word again, but this time it will fit.
					// We also have to delete the vertices and indices we have read in before; they are no longer valid.
					vertices.resize(verticesSize);
					indices.resize(indicesSize);
					indexIntoUTF8String = previousIndexIntoUTF8String;
					data.pushGlyphsUntilWhitespace(indexIntoUTF8String, text);
				}
				else
				{
					// 3.)	The word does not even fit into a single line. In this case we fall back to the
					//		CHARACTERS behaviour: Try to put it on the current line,
					//		if it doesn't fit put it on the next line!
					vertices.resize(verticesSize);
					indices.resize(indicesSize);
					data.dynamicTextCharacters.resize(dynamicTextCharactersSize);
					data.currentBaseline.x = previousBaselineX;
					data.lastCodepoint = previouseLastCodepoint;
					data.parseCharacterByCharacter(previousIndexIntoUTF8String, indexIntoUTF8String, text);
				}
			}
		}
		// Finalize the lines vector
		lines.back().endIndex = unicodeCharacters.size() - 1;
		// Align properly
		enforceAlignment();
		// Construct model
		return data.constructModel();
	}

	Model DynamicText::parseTextCharacters()
	{
		// Create ParseDynamicTextData object
		vertices.clear();
		indices.clear();
		unicodeCharacters.clear();
		lines.clear();
		lines.push_back({ 0.0, 0, 0 });
		ParseDynamicTextData data{ font, Vec2d{}, Unicode{0}, vertices, indices, textWidth, fontSize, unicodeCharacters, lines };
		// Parse the text
		data.parseCharacterByCharacter(0, text.size(), text);
		// Finalize the lines vector
		lines.back().endIndex = unicodeCharacters.size() - 1;
		// Align properly
		enforceAlignment();
		// Construct model
		return data.constructModel();
	}

	void DynamicText::constructNew()
	{
		switch (parseMode)
		{
		case SnackerEngine::TextParseMode::WORD_BY_WORD: model = parseTextWordByWord(); break;
		case SnackerEngine::TextParseMode::CHARACTERS: model = parseTextCharacters(); break;
		default: break;
		}
	}

	Model DynamicText::parseTextStartingAtIndexWordByWord(unsigned int characterIndex)
	{
		textIsUpToDate = false;
		// Resize vectors
		vertices.resize(unicodeCharacters.size() * 4);
		indices.resize(unicodeCharacters.size() * 6);
		if (unicodeCharacters.empty()) {
			return Model();
		}
		// We need to go back to the first character of the word we are currently in
		if (characterIndex == unicodeCharacters.size()) {
			characterIndex--;
		}
		if (characterIndex <= unicodeCharacters.size() && !isWhiteSpace(unicodeCharacters[characterIndex].codepoint)) {
			while (characterIndex > 0 && !isWhiteSpace(unicodeCharacters[characterIndex - 1].codepoint)) {
				characterIndex--;
			}
		}
		// Create ParseDynamicTextData object
		unsigned int currentLine = 0;
		double currentBaselineX = 0.0;
		Unicode lastCodepoint = 0;
		if (characterIndex > 0) {
			currentLine = getLineNumber(characterIndex - 1);
			currentBaselineX = unicodeCharacters[characterIndex - 1].leftPosition;
			lastCodepoint = unicodeCharacters[characterIndex - 1].codepoint;
		}
		lines.resize(currentLine + 1);
		ParseDynamicTextData data{ font, Vec2d(currentBaselineX, lines.back().baselineY), lastCodepoint,
			vertices, indices, textWidth, fontSize, unicodeCharacters, lines };
		if (isNewline(lastCodepoint)) {
			data.goToNextLine(characterIndex);
			data.lastCodepoint = 0;
		}
		// Go through the characters vector character by character
		while (characterIndex < unicodeCharacters.size()) {
			/// Before we parse the next codepoint, we need to save the current character index, because we might
			/// need to backtrack if the parsed word does not fit into the current line
			unsigned int previousCharacterIndex = characterIndex;
			// Now we can parse the next character
			DynamicTextCharacter& character = unicodeCharacters[characterIndex];
			// First check if the parsed codepoint was a newline character
			if (isNewline(character.codepoint)) {
				data.goToNextLine(characterIndex + 1);
				data.editNewlineGlyph(characterIndex);
				data.lastCodepoint = 0;
				lastCodepoint = 0;
				// Advance to next codepoint
				characterIndex++;
				continue;
			}
			// Next, check if we have a whitespace character
			if (isWhiteSpace(character.codepoint)) {
				/// Whitespaces are treated equally as in the CHARACTERS case: Try to put it on the current line,
				/// if it doesn't fit put it on the next line!
				if (data.lastCodepoint != 0) {
					// Advance to next glyph
					data.advanceBaselineWithKerning(character.codepoint);
				}
				// Update lastCodepoint
				data.lastCodepoint = character.codepoint;
				Glyph glyph = font.getGlyph(character.codepoint);
				// Check if we need a new line
				if (!data.isInsideTextWidth(data.currentBaseline.x + glyph.right)) {
					data.goToNextLine(characterIndex);
					// Special rule: If the first character on a new line is a space character, we dont 
					// want to advance the next character!
					if (isSpaceCharacter(character.codepoint)) {
						data.lastCodepoint = 0;
					}
				}
				// Add the new glyph
				data.editGlyph(glyph, characterIndex);
				// Advance to next character
				characterIndex++;
			}
			else {
				/// We have a non-whitespace character. First: save some additional information that is necessary when
				/// we need to backtrack
				// Save the current baseline x position
				double previousBaselineX = data.currentBaseline.x;
				// Save the last codepoint
				Unicode previouseLastCodepoint = data.lastCodepoint;
				// Now: place the character we just read in at the correct position
				if (data.lastCodepoint != 0) {
					// Advance to next glyph
					data.advanceBaselineWithKerning(character.codepoint);
				}
				Glyph glyph = font.getGlyph(character.codepoint);
				// Check if we need a new line
				if (!data.isInsideTextWidth(data.currentBaseline.x + glyph.right)) {
					data.goToNextLine(characterIndex);
					previousBaselineX = data.currentBaseline.x;
				}
				// Add the new glyph
				data.editGlyph(glyph, characterIndex);
				// Update lastCodepoint
				data.lastCodepoint = character.codepoint;
				characterIndex++;
				/// Now, we want to read the remaining word.
				// First try to just parse the word in the current line
				data.editGlyphsUntilWhitespace(characterIndex);
				// Now, we can have three cases:
				double rightPositionX = data.currentBaseline.x + font.getGlyph(data.lastCodepoint).right;
				if (data.isInsideTextWidth(rightPositionX)) {
					// 1.)	The word fits, we can just move on
				}
				else if (data.isInsideTextWidth(rightPositionX - previousBaselineX))
				{
					// 2.)	The word fits inside a single line, but not on the current line -> we have to move to the next line!
					data.goToNextLine(previousCharacterIndex);
					data.lastCodepoint = 0;
					// Now we can read in the word again, but this time it will fit.
					// We also have to delete the vertices and indices we have read in before; they are no longer valid.
					characterIndex = previousCharacterIndex;
					data.editGlyphsUntilWhitespace(characterIndex);
				}
				else
				{
					// 3.)	The word does not even fit into a single line. In this case we fall back to the
					//		CHARACTERS behaviour: Try to put it on the current line,
					//		if it doesn't fit put it on the next line!
					data.currentBaseline.x = previousBaselineX;
					data.lastCodepoint = previouseLastCodepoint;
					data.parseCharacterByCharacterFromUnicodeVector(previousCharacterIndex, characterIndex);
				}
				// We dont need to advance to the next character
			}
		}
		// Finalize the lines vector
		lines.back().endIndex = unicodeCharacters.size() - 1;
		// Align properly
		enforceAlignment();
		// Construct model
		return data.constructModel();
	}

	Model DynamicText::parseTextStartingAtIndexCharacters(unsigned int characterIndex)
	{
		textIsUpToDate = false;
		// Resize vectors
		vertices.resize(unicodeCharacters.size() * 4);
		indices.resize(unicodeCharacters.size() * 6);
		// Create ParseDynamicTextData object
		unsigned int currentLine = 0;
		double currentBaselineX = 0.0;
		Unicode lastCodepoint = 0;
		if (characterIndex > 0) {
			currentLine = getLineNumber(characterIndex - 1);
			currentBaselineX = unicodeCharacters[characterIndex - 1].leftPosition;
			lastCodepoint = unicodeCharacters[characterIndex - 1].codepoint;
		}
		lines.resize(currentLine + 1);
		ParseDynamicTextData data{ font, Vec2d(currentBaselineX, lines.back().baselineY), lastCodepoint,
			vertices, indices, textWidth, fontSize, unicodeCharacters, lines };
		if (isNewline(lastCodepoint)) {
			data.goToNextLine(characterIndex);
			data.lastCodepoint = 0;
		}
		// Parse the text
		data.parseCharacterByCharacterFromUnicodeVector(characterIndex, unicodeCharacters.size());
		// Finalize the lines vector
		lines.back().endIndex = unicodeCharacters.size() - 1;
		// Align properly
		enforceAlignment();
		// Construct model
		return data.constructModel();
	}

	void DynamicText::constructFrom(unsigned int characterIndex)
	{
		switch (parseMode)
		{
		case SnackerEngine::TextParseMode::WORD_BY_WORD: model = parseTextStartingAtIndexWordByWord(characterIndex); break;
		case SnackerEngine::TextParseMode::CHARACTERS: model = parseTextStartingAtIndexCharacters(characterIndex); break;
		default: break;
		}
	}

	unsigned int DynamicText::getLineNumber(const unsigned int& characterIndex) const
	{
		auto result = std::lower_bound(lines.begin(), lines.end(), DynamicTextLine{ 0, characterIndex, characterIndex });
		if (result < lines.end()) return result - lines.begin();
		return lines.size() - 1;
	}

	void DynamicText::enforceAlignment()
	{
		switch (alignment)
		{
		case SnackerEngine::Alignment::LEFT: 
		{
			// Text is already aligned properly
			break;
		}
		case SnackerEngine::Alignment::CENTER:
		{
			for (const DynamicTextLine& line : lines) 
			{
				double lineLength = unicodeCharacters[line.endIndex].rightPosition -
					unicodeCharacters[line.beginIndex].leftPosition;
				double offset = (textWidth / fontSize - lineLength) / 2.0;
				for (unsigned characterIndex = line.beginIndex; characterIndex <= line.endIndex; ++characterIndex) 
				{
					unicodeCharacters[characterIndex].leftPosition += offset;
					unicodeCharacters[characterIndex].rightPosition += offset;
					vertices[4 * characterIndex + 0].x += offset;
					vertices[4 * characterIndex + 1].x += offset;
					vertices[4 * characterIndex + 2].x += offset;
					vertices[4 * characterIndex + 3].x += offset;
				}
			}
			break;
		}
		case SnackerEngine::Alignment::RIGHT:
		{
			break;
		}
		default:
			break;
		}
	}

	DynamicText::DynamicText(const std::u8string& text, const Font& font, const double& fontSize, const double& textWidth, const TextParseMode& parseMode, const Alignment& alignment)
		: font(font), fontSize(fontSize), textWidth(textWidth), unicodeCharacters{}, lines{}, text(text), textIsUpToDate(false),
		model{}, vertices{}, indices{}, cursorPosIndex(0), cursorPos{}, cursorSize(0.05f, 1.0f), parseMode(parseMode), alignment(alignment)
	{
		constructNew();
	}

	const std::u8string& DynamicText::getText() const
	{
		if (textIsUpToDate) {
			return text;
		}
		else {
			errorLogger << LOGGER::BEGIN << "Not implemented!" << LOGGER::ENDL; // TODO: Implement
		}
	}

	void DynamicText::setText(const std::u8string& text)
	{
		this->text = text;
		constructNew();
	}

	const double& DynamicText::getFontSize() const
	{
		return fontSize;
	}

	const Model& DynamicText::getModel()
	{
		return model;
	}

	void DynamicText::setCursorPos(unsigned int characterIndex)
	{
		if (characterIndex > unicodeCharacters.size()) {
			characterIndex = unicodeCharacters.size();
		}
		unsigned int lineNumber = getLineNumber(characterIndex);
		if (characterIndex == 0) cursorPos = Vec2f(0.0f, lines[lineNumber].baselineY);
		else if (characterIndex == unicodeCharacters.size()) {
			// Cursor is at the end of the text
			const Unicode& codepoint = unicodeCharacters.back().codepoint;
			if (isNewline(codepoint)) {
				cursorPos = Vec2f(0.0f, lines[lineNumber].baselineY);
			}
			else {
				cursorPos = Vec2f(unicodeCharacters.back().leftPosition
					+ font.getGlyph(codepoint).advance, lines[lineNumber].baselineY);
			}
		}
		else cursorPos = Vec2f(unicodeCharacters[characterIndex].leftPosition, lines[lineNumber].baselineY);
		cursorPos.x -= cursorSize.x;
		cursorPosIndex = characterIndex;
	}

	void DynamicText::moveCursorToLeft()
	{
		if (cursorPosIndex != 0) setCursorPos(cursorPosIndex - 1);
	}

	void DynamicText::moveCursorToRight()
	{
		if (cursorPosIndex != unicodeCharacters.size()) setCursorPos(cursorPosIndex + 1);
	}

	void DynamicText::moveCursorToLeftWordBeginning()
	{
		if (cursorPosIndex == 0) return;
		unsigned int newIndex = cursorPosIndex - 1;
		if (!isNewline(unicodeCharacters[newIndex].codepoint)) {
			while (newIndex > 0 && isWhiteSpace(unicodeCharacters[newIndex].codepoint) && !isNewline(unicodeCharacters[newIndex].codepoint)) {
				newIndex--;
			}
			while (newIndex > 0 && !isWhiteSpace(unicodeCharacters[newIndex - 1].codepoint)) {
				newIndex--;
			}
		}
		setCursorPos(newIndex);
	}

	void DynamicText::moveCursorToRightWordEnd()
	{
		if (cursorPosIndex == unicodeCharacters.size()) return;
		unsigned int newIndex = cursorPosIndex;
		if (isNewline(unicodeCharacters[cursorPosIndex].codepoint)) {
			newIndex++;
		}
		else {
			while (newIndex < unicodeCharacters.size() && !isWhiteSpace(unicodeCharacters[newIndex].codepoint)) {
				newIndex++;
			}
			while (newIndex < unicodeCharacters.size() && isWhiteSpace(unicodeCharacters[newIndex].codepoint) && !isNewline(unicodeCharacters[newIndex].codepoint)) {
				newIndex++;
			}
		}
		setCursorPos(newIndex);
	}

	const Vec2f& DynamicText::getCursorPos() const
	{
		return cursorPos;
	}

	const Vec2f& DynamicText::getCursorSize() const
	{
		return cursorSize;
	}

	void DynamicText::inputAtCursor(const Unicode& codepoint)
	{
		unicodeCharacters.insert(unicodeCharacters.begin() + cursorPosIndex, { codepoint, 0.0, 0.0 });
		constructFrom(cursorPosIndex);
		setCursorPos(cursorPosIndex + 1);
	}

	void DynamicText::inputNewlineAtCursor()
	{
		inputAtCursor(0x0085);
	}

	void DynamicText::deleteCharacters(unsigned int beginIndex, unsigned int endIndex)
	{
		if (unicodeCharacters.empty()) return;
		if (beginIndex > endIndex) return;
		if (endIndex > unicodeCharacters.size()) endIndex = unicodeCharacters.size() - 1;
		unicodeCharacters.erase(unicodeCharacters.begin() + beginIndex, unicodeCharacters.begin() + endIndex + 1);
		constructFrom(beginIndex);
		if (cursorPosIndex > beginIndex) {
			if (cursorPosIndex < endIndex) {
				setCursorPos(beginIndex);
			}
			else {
				setCursorPos(cursorPosIndex + beginIndex - endIndex - 1);
			}
		}
	}

	void DynamicText::deleteCharacterBeforeCursor()
	{
		if (cursorPosIndex > 0) deleteCharacters(cursorPosIndex - 1, cursorPosIndex - 1);
	}

	void DynamicText::deleteWordBeforeCursor()
	{
		if (cursorPosIndex == 0) return;
		unsigned int beginIndex = cursorPosIndex - 1;
		if (!isNewline(unicodeCharacters[beginIndex].codepoint)) {
			while (beginIndex > 0 && isWhiteSpace(unicodeCharacters[beginIndex].codepoint) && !isNewline(unicodeCharacters[beginIndex].codepoint)) {
				beginIndex--;
			}
			while (beginIndex > 0 && !isWhiteSpace(unicodeCharacters[beginIndex - 1].codepoint)) {
				beginIndex--;
			}
		}
		deleteCharacters(beginIndex, cursorPosIndex - 1);
	}

	bool DynamicTextLine::operator<(const DynamicTextLine& other) const
	{
		return endIndex < other.beginIndex;
	}

	bool DynamicTextLine::operator>(const DynamicTextLine& other) const
	{
		return beginIndex > other.endIndex;
	}

	bool DynamicTextLine::operator==(const DynamicTextLine& other) const
	{
		return !(endIndex < other.beginIndex || beginIndex > other.endIndex);
	}

}