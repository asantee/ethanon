#include "ArabicShaper.h"
#include <map>

namespace gs2d {
namespace rtl {

enum JoiningType
{
	JT_NON_JOINING,   
	JT_RIGHT_JOINING,
	JT_DUAL_JOINING,
	// (wikipedia) caractere diacrítico: sinal gráfico que se coloca sobre, sob ou através de uma letra
	// para alterar a sua realização fonética ou para marcar qualquer outra característica linguística.
	JT_TRANSPARENT
};

struct ArabicForms
{
	uint32_t isolated;   // caracteres sem conexão.
	uint32_t final_form; // caracteres com conexão com o caractere anterior.
	uint32_t initial;    // caracteres com conexão com o caractere seguinte.
	uint32_t medial;     // caracteres com conexão dupla.
	JoiningType joiningType;
};

static const std::map<uint32_t, ArabicForms>& GetFormsMap()
{
	static std::map<uint32_t, ArabicForms> forms;
	if (forms.empty())
	{
		// Letras árabes
		forms[0x0621] = { 0xFE80, 0xFE80, 0xFE80, 0xFE80, JT_NON_JOINING };   // Hamza
		forms[0x0622] = { 0xFE81, 0xFE82, 0xFE81, 0xFE82, JT_RIGHT_JOINING }; // Alef with Madda
		forms[0x0623] = { 0xFE83, 0xFE84, 0xFE83, 0xFE84, JT_RIGHT_JOINING }; // Alef with Hamza Above
		forms[0x0624] = { 0xFE85, 0xFE86, 0xFE85, 0xFE86, JT_RIGHT_JOINING }; // Waw with Hamza Above
		forms[0x0625] = { 0xFE87, 0xFE88, 0xFE87, 0xFE88, JT_RIGHT_JOINING }; // Alef with Hamza Below
		forms[0x0626] = { 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C, JT_DUAL_JOINING };  // Yaa with Hamza Above
		forms[0x0627] = { 0xFE8D, 0xFE8E, 0xFE8D, 0xFE8E, JT_RIGHT_JOINING }; // Alef
		forms[0x0628] = { 0xFE8F, 0xFE90, 0xFE91, 0xFE92, JT_DUAL_JOINING };  // Baa
		forms[0x0629] = { 0xFE93, 0xFE94, 0xFE93, 0xFE94, JT_RIGHT_JOINING }; // Taa Marbuta
		forms[0x062A] = { 0xFE95, 0xFE96, 0xFE97, 0xFE98, JT_DUAL_JOINING };  // Taa
		forms[0x062B] = { 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C, JT_DUAL_JOINING };  // Thaa
		forms[0x062C] = { 0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0, JT_DUAL_JOINING };  // Jeem
		forms[0x062D] = { 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4, JT_DUAL_JOINING };  // Haa
		forms[0x062E] = { 0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8, JT_DUAL_JOINING };  // Khaa
		forms[0x062F] = { 0xFEA9, 0xFEAA, 0xFEA9, 0xFEAA, JT_RIGHT_JOINING }; // Dal
		forms[0x0630] = { 0xFEAB, 0xFEAC, 0xFEAB, 0xFEAC, JT_RIGHT_JOINING }; // Dhal
		forms[0x0631] = { 0xFEAD, 0xFEAE, 0xFEAD, 0xFEAE, JT_RIGHT_JOINING }; // Raa
		forms[0x0632] = { 0xFEAF, 0xFEB0, 0xFEAF, 0xFEB0, JT_RIGHT_JOINING }; // Zay
		forms[0x0633] = { 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4, JT_DUAL_JOINING };  // Seen
		forms[0x0634] = { 0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8, JT_DUAL_JOINING };  // Sheen
		forms[0x0635] = { 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC, JT_DUAL_JOINING };  // Saad
		forms[0x0636] = { 0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0, JT_DUAL_JOINING };  // Daad
		forms[0x0637] = { 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4, JT_DUAL_JOINING };  // Taa (Emphatic)
		forms[0x0638] = { 0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8, JT_DUAL_JOINING };  // Zaa (Emphatic)
		forms[0x0639] = { 0xFEC9, 0xFECA, 0xFECB, 0xFECC, JT_DUAL_JOINING };  // Ain
		forms[0x063A] = { 0xFECD, 0xFECE, 0xFECF, 0xFED0, JT_DUAL_JOINING };  // Ghain
		forms[0x0640] = { 0x0640, 0x0640, 0x0640, 0x0640, JT_DUAL_JOINING };  // Tatweel / Kashida
		forms[0x0641] = { 0xFED1, 0xFED2, 0xFED3, 0xFED4, JT_DUAL_JOINING };  // Faa
		forms[0x0642] = { 0xFED5, 0xFED6, 0xFED7, 0xFED8, JT_DUAL_JOINING };  // Qaaf
		forms[0x0643] = { 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC, JT_DUAL_JOINING };  // Kaaf
		forms[0x0644] = { 0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0, JT_DUAL_JOINING };  // Lam
		forms[0x0645] = { 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4, JT_DUAL_JOINING };  // Meem
		forms[0x0646] = { 0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8, JT_DUAL_JOINING };  // Noon
		forms[0x0647] = { 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC, JT_DUAL_JOINING };  // Heh
		forms[0x0648] = { 0xFEED, 0xFEEE, 0xFEED, 0xFEEE, JT_RIGHT_JOINING }; // Waw
		forms[0x0649] = { 0xFEEF, 0xFEF0, 0xFBE8, 0xFBE9, JT_DUAL_JOINING };  // Alef Maksura
		forms[0x064A] = { 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4, JT_DUAL_JOINING };  // Yaa

		// Letras Persa/Urdu
		forms[0x067E] = { 0xFB56, 0xFB57, 0xFB58, 0xFB59, JT_DUAL_JOINING };  // Peh
		forms[0x0686] = { 0xFB7A, 0xFB7B, 0xFB7C, 0xFB7D, JT_DUAL_JOINING };  // Tcheh
		forms[0x0698] = { 0xFB8A, 0xFB8B, 0xFB8A, 0xFB8B, JT_RIGHT_JOINING }; // Zheh
		forms[0x06AF] = { 0xFB92, 0xFB93, 0xFB94, 0xFB95, JT_DUAL_JOINING };  // Gaf

		// Diacríticos transparentes (Harakat)
		for (uint32_t t = 0x064B; t <= 0x065F; ++t)
			forms[t] = { t, t, t, t, JT_TRANSPARENT };

		forms[0x0670] = { 0x0670, 0x0670, 0x0670, 0x0670, JT_TRANSPARENT }; // Alef Sobrescrito
	}
	return forms;
}

bool ArabicShaper::IsArabic(const uint32_t cp)
{
	return (cp >= 0x0600 && cp <= 0x06FF) || // Arabic
	       (cp >= 0x0750 && cp <= 0x077F) || // Extended-A
	       (cp >= 0x08A0 && cp <= 0x08FF) || // Extended-B
	       (cp >= 0xFB50 && cp <= 0xFDFF) || // Presentation Forms-A
	       (cp >= 0xFE70 && cp <= 0xFEFF);   // Presentation Forms-B
}

bool ArabicShaper::IsRTLChar(const uint32_t cp)
{
	// Blocos hebraico e arábico
	return (cp >= 0x0590 && cp <= 0x05FF) ||
	       (cp >= 0x0600 && cp <= 0x08FF) ||
	       (cp >= 0xFB1D && cp <= 0xFB4F) || // Hebraico
	       (cp >= 0xFB50 && cp <= 0xFDFF) ||
	       (cp >= 0xFE70 && cp <= 0xFEFF);
}

static JoiningType GetJoiningType(const uint32_t cp)
{
	const std::map<uint32_t, ArabicForms>& forms = GetFormsMap();
	std::map<uint32_t, ArabicForms>::const_iterator it = forms.find(cp);
	if (it != forms.end())
		return it->second.joiningType;
	return JT_NON_JOINING;
}

static bool IsLamAlefLigature(const uint32_t alef, uint32_t& isoForm, uint32_t& finalForm)
{
	switch (alef)
	{
	case 0x0622: // Alef com Madda
		isoForm = 0xFEF5;
		finalForm = 0xFEF6;
		return true;
	case 0x0623: // Alef com Hamza acima
		isoForm = 0xFEF7;
		finalForm = 0xFEF8;
		return true;
	case 0x0625: // Alef com Hamza abaixo
		isoForm = 0xFEF9;
		finalForm = 0xFEFA;
		return true;
	case 0x0627: // Alef seco
		isoForm = 0xFEFB;
		finalForm = 0xFEFC;
		return true;
	default:
		return false;
	}
}

std::vector<uint32_t> ArabicShaper::Shape(const std::vector<uint32_t>& input)
{
	const std::map<uint32_t, ArabicForms>& forms = GetFormsMap();
	std::vector<uint32_t> output;
	output.reserve(input.size());

	const size_t count = input.size();
	for (size_t i = 0; i < count; ++i)
	{
		const uint32_t cp = input[i];

		// Verifica a ligadura Lam-Alef
		if (cp == 0x0644 && (i + 1 < count))
		{
			// Pula diacríticos
			size_t nextIdx = i + 1;
			while (nextIdx < count && GetJoiningType(input[nextIdx]) == JT_TRANSPARENT)
				++nextIdx;

			if (nextIdx < count)
			{
				uint32_t isoLig, finLig;
				if (IsLamAlefLigature(input[nextIdx], isoLig, finLig))
				{
					// Verifica se Lam conecta
					bool prevConnects = false;
					if (i > 0)
					{
						size_t prevIdx = i;
						while (prevIdx > 0)
						{
							--prevIdx;
							JoiningType prevType = GetJoiningType(input[prevIdx]);
							if (prevType == JT_TRANSPARENT)
								continue;
							prevConnects = (prevType == JT_DUAL_JOINING);
							break;
						}
					}

					output.push_back(prevConnects ? finLig : isoLig);
					i = nextIdx;
					continue;
				}
			}
		}

		std::map<uint32_t, ArabicForms>::const_iterator it = forms.find(cp);
		if (it == forms.end() || it->second.joiningType == JT_NON_JOINING || it->second.joiningType == JT_TRANSPARENT)
		{
			output.push_back(cp);
			continue;
		}

		const ArabicForms& f = it->second;

		// Verifica se o caractere anterior conecta para a direita
		bool prevConnects = false;
		if (i > 0)
		{
			size_t prevIdx = i;
			while (prevIdx > 0)
			{
				--prevIdx;
				JoiningType prevType = GetJoiningType(input[prevIdx]);
				if (prevType == JT_TRANSPARENT)
					continue;
				prevConnects = (prevType == JT_DUAL_JOINING);
				break;
			}
		}

		// Verifica se o caractere seguinte conecta para a esquerda
		bool nextConnects = false;
		if (i + 1 < count)
		{
			size_t nextIdx = i + 1;
			while (nextIdx < count)
			{
				JoiningType nextType = GetJoiningType(input[nextIdx]);
				if (nextType == JT_TRANSPARENT)
				{
					++nextIdx;
					continue;
				}
				nextConnects = (nextType == JT_DUAL_JOINING || nextType == JT_RIGHT_JOINING);
				break;
			}
		}

		if (prevConnects && nextConnects && f.joiningType == JT_DUAL_JOINING)
			output.push_back(f.medial);
		else if (prevConnects)
			output.push_back(f.final_form);
		else if (nextConnects && f.joiningType == JT_DUAL_JOINING)
			output.push_back(f.initial);
		else
			output.push_back(f.isolated);
	}

	return output;
}

} // namespace rtl
} // namespace gs2d