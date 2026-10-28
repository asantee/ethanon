#include "BiDiReorder.h"
#include <algorithm>

namespace gs2d {
namespace rtl {

enum BidiType
{
	BT_L,   // LTR (Latin, CJK, ...)
	BT_R,   // RTL (Arabic, Hebrew)
	BT_EN,  // European Number ('0'-'9')
	BT_AN,  // Arabic-Indic Number
	BT_ES,  // European Separator ('+', '-')
	BT_ET,  // European Terminator ('%', '$')
	BT_CS,  // Common Separator ('.', ',', ':', '/')
	BT_WS,  // Whitespace (' ', '\t')
	BT_ON   // Other Neutral ('!', '?', '(', ')', ...)
};

static BidiType GetBidiType(const uint32_t cp)
{
	// Blocos Arabe e Hebraico
	if ((cp >= 0x0590 && cp <= 0x05FF) ||
	    (cp >= 0x0600 && cp <= 0x08FF) ||
	    (cp >= 0xFB1D && cp <= 0xFB4F) ||
	    (cp >= 0xFB50 && cp <= 0xFDFF) ||
	    (cp >= 0xFE70 && cp <= 0xFEFF))
	{
		// Números Arabes-Indic
		if (cp >= 0x0660 && cp <= 0x0669)
			return BT_AN;
		if (cp >= 0x06F0 && cp <= 0x06F9)
			return BT_AN;
		return BT_R;
	}

	// Numeros europeus
	if (cp >= '0' && cp <= '9')
		return BT_EN;

	// Espacos
	if (cp == ' ' || cp == '\t')
		return BT_WS;

	// Separadores
	if (cp == '+' || cp == '-')
		return BT_ES;
	if (cp == '%' || cp == '$')
		return BT_ET;
	if (cp == '.' || cp == ',' || cp == ':' || cp == '/')
		return BT_CS;

	// Latino, Grego, Cirílico, ...
	if ((cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z') ||
	    (cp >= 0x00C0 && cp <= 0x02AF) ||
	    (cp >= 0x0370 && cp <= 0x052F) ||
	    (cp >= 0x2E80 && cp <= 0x9FFF)) // CJK
	{
		return BT_L;
	}

	return BT_ON;
}

static uint32_t MirrorCharacter(const uint32_t cp)
{
	switch (cp)
	{
	case '(': return ')';
	case ')': return '(';
	case '[': return ']';
	case ']': return '[';
	case '{': return '}';
	case '}': return '{';
	case '<': return '>';
	case '>': return '<';
	case 0x00AB: return 0x00BB; // « <-> »
	case 0x00BB: return 0x00AB;
	default: return cp;
	}
}

bool BiDiReorder::HasRTL(const std::vector<uint32_t>& input)
{
	for (size_t i = 0; i < input.size(); ++i)
		if (GetBidiType(input[i]) == BT_R)
			return true;
	return false;
}

static void ReorderLine(std::vector<uint32_t>& line)
{
	if (line.empty())
		return;

	const size_t n = line.size();
	std::vector<BidiType> types(n);
	std::vector<int> levels(n, 1); // Padrão RTL (1)

	for (size_t i = 0; i < n; ++i)
		types[i] = GetBidiType(line[i]);

	// RTL (R): 1
	// LTR (L): 2
	// Números (EN, AN): 2
	for (size_t i = 0; i < n; ++i)
	{
		if (types[i] == BT_R)
			levels[i] = 1;
		else if (types[i] == BT_L || types[i] == BT_EN || types[i] == BT_AN)
			levels[i] = 2;
		else // Neutros, espaços, separadores
		{
			int prevStrong = 1; // caractere anterior RTL
			for (int p = static_cast<int>(i) - 1; p >= 0; --p)
			{
				if (types[p] == BT_R) { prevStrong = 1; break; }
				if (types[p] == BT_L || types[p] == BT_EN || types[p] == BT_AN) { prevStrong = 2; break; }
			}

			int nextStrong = 1;
			for (size_t f = i + 1; f < n; ++f)
			{
				if (types[f] == BT_R) { nextStrong = 1; break; }
				if (types[f] == BT_L || types[f] == BT_EN || types[f] == BT_AN) { nextStrong = 2; break; }
			}

			if (prevStrong == 2 && nextStrong == 2)
				levels[i] = 2;
			else
				levels[i] = 1;
		}
	}

	// Espelhamento de caractere para níveis ímpares (level 1)
	for (size_t i = 0; i < n; ++i)
		if (levels[i] == 1)
			line[i] = MirrorCharacter(line[i]);

	// --- Reversão ---
	// Reverte nível >= 2 (números, palavras LTR)
	size_t runStart = 0;
	bool inRun = false;
	for (size_t i = 0; i < n; ++i)
	{
		if (levels[i] >= 2)
		{
			if (!inRun)
			{
				runStart = i;
				inRun = true;
			}
		}
		else
		{
			if (inRun)
			{
				std::reverse(line.begin() + runStart, line.begin() + i);
				inRun = false;
			}
		}
	}
	if (inRun)
		std::reverse(line.begin() + runStart, line.end());

	// Reverte linha inteira
	std::reverse(line.begin(), line.end());
}

std::vector<uint32_t> BiDiReorder::Reorder(const std::vector<uint32_t>& input)
{
	if (!HasRTL(input))
		return input;

	std::vector<uint32_t> result;
	result.reserve(input.size());

	std::vector<uint32_t> currentLine;
	for (size_t i = 0; i < input.size(); ++i)
	{
		const uint32_t cp = input[i];
		if (cp == '\n' || cp == '\r')
		{
			ReorderLine(currentLine);
			result.insert(result.end(), currentLine.begin(), currentLine.end());
			currentLine.clear();
			result.push_back(cp);
		}
		else
			currentLine.push_back(cp);
	}

	if (!currentLine.empty())
	{
		ReorderLine(currentLine);
		result.insert(result.end(), currentLine.begin(), currentLine.end());
	}

	return result;
}

} // namespace rtl
} // namespace gs2d