#include "RTLProcessor.h"
#include "ArabicShaper.h"
#include "BiDiReorder.h"
#include "../utf8/utf8.h"

#include <vector>
#include <map>

namespace gs2d {
namespace rtl {

static const std::string COLOR_TAG_START = "!#color#";
static const std::string COLOR_TAG_END   = "##!";

bool RTLProcessor::ContainsRTL(const std::string& utf8Text)
{
	if (utf8Text.empty())
		return false;

	const char* ptr = utf8Text.data();
	const char* end = ptr + utf8Text.size();

	while (ptr < end)
	{
		uint32_t cp = 0;
		try
		{
			cp = utf8::next(ptr, end);
		}
		catch (...)
		{
			break;
		}

		if (ArabicShaper::IsRTLChar(cp))
			return true;
	}
	return false;
}

std::string RTLProcessor::Process(const std::string& utf8Text)
{
	if (utf8Text.empty() || !ContainsRTL(utf8Text))
		return utf8Text;

	const bool hasColorTags = (utf8Text.find(COLOR_TAG_START) != std::string::npos);

	std::string cleanInput = utf8Text;
	std::map<uint32_t, std::string> placeholderToTag;
	uint32_t placeholderCounter = 0xE000;

	if (hasColorTags)
	{
		std::string resultWithPlaceholders;
		resultWithPlaceholders.reserve(utf8Text.size());

		size_t pos = 0;
		while (pos < utf8Text.size())
		{
			if (utf8Text.compare(pos, COLOR_TAG_START.length(), COLOR_TAG_START) == 0)
			{
				size_t endPos = utf8Text.find(COLOR_TAG_END, pos);
				if (endPos != std::string::npos)
				{
					size_t tagLen = (endPos - pos) + COLOR_TAG_END.length();
					std::string tag = utf8Text.substr(pos, tagLen);

					placeholderToTag[placeholderCounter] = tag;
					std::vector<uint32_t> phVec;
					phVec.push_back(placeholderCounter);
					utf8::utf32to8(phVec.begin(), phVec.end(), std::back_inserter(resultWithPlaceholders));

					placeholderCounter++;
					pos = endPos + COLOR_TAG_END.length();
					continue;
				}
			}

			resultWithPlaceholders.push_back(utf8Text[pos]);
			pos++;
		}
		cleanInput = resultWithPlaceholders;
	}

	// Decodifica UTF-8 -> UTF-32
	std::vector<uint32_t> cps;
	try
	{
		const char* ptr = cleanInput.data();
		const char* end = ptr + cleanInput.size();
		while (ptr < end)
			cps.push_back(utf8::next(ptr, end));
	}
	catch (...)
	{
		return utf8Text;
	}

	// Formas árabes com contexto e ligaduras
	std::vector<uint32_t> shaped = ArabicShaper::Shape(cps);

	// Reordenação bidirecional
	std::vector<uint32_t> reordered = BiDiReorder::Reorder(shaped);

	// Codifica de volta para UTF-8
	std::string output;
	output.reserve(cleanInput.size());
	for (size_t i = 0; i < reordered.size(); ++i)
	{
		const uint32_t cp = reordered[i];
		std::map<uint32_t, std::string>::const_iterator tagIt = placeholderToTag.find(cp);
		if (tagIt != placeholderToTag.end())
			output += tagIt->second;
		else
		{
			std::vector<uint32_t> singleCp;
			singleCp.push_back(cp);
			utf8::utf32to8(singleCp.begin(), singleCp.end(), std::back_inserter(output));
		}
	}

	return output;
}

} // namespace rtl
} // namespace gs2d