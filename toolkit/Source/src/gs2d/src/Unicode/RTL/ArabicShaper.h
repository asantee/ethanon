#ifndef GS2D_ARABIC_SHAPER_H_
#define GS2D_ARABIC_SHAPER_H_

#include <vector>
#include <cstdint>

namespace gs2d {
namespace rtl {

/**
 * \brief Arabic Contextual Shaper
 *
 * Converte caracteres Unicode Arabic (0x0600 ~ 0x06FF, + extensões persas/urdu)
 * em suas formas de apresentação contextual (isolado, inicial, medial, final) em
 * Arabic Presentation Forms-B (0xFE70 ~ 0xFEFF) e Forms-A (0xFB50 ~ 0xFDFF)
 * e combina ligaduras.
 * Isolado: caractere não tem conexão com o caractere anterior nem com o seguinte.
 * Inicial: caractere tem conexão com o caractere seguinte, mas não com o anterior.
 * Medial:  caractere tem conexão com o caractere anterior e com o seguinte.
 * Final:   caractere tem conexão com o caractere anterior, mas não com o seguinte.
 */
class ArabicShaper
{
public:
	static bool IsArabic(const uint32_t cp);
	static bool IsRTLChar(const uint32_t cp);
	static std::vector<uint32_t> Shape(const std::vector<uint32_t>& input);
};

} // namespace rtl
} // namespace gs2d

#endif // GS2D_ARABIC_SHAPER_H_