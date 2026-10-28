#ifndef GS2D_BIDI_REORDER_H_
#define GS2D_BIDI_REORDER_H_

#include <vector>
#include <cstdint>

namespace gs2d {
namespace rtl {

/**
 * \brief Unicode Bidirectional Algorithm (UAX #9) implementation
 *
 * Reordena textos RTL e LTR misturados da esquerda para a direita renders de bitmap.
 * Mantém a ordem da esquerda para a direita de números e palavras LTR dentro do texto RTL,
 * e inverte caracteres de pontuação espelháveis (), [], ...
 */
class BiDiReorder
{
public:
	static bool HasRTL(const std::vector<uint32_t>& input);
	static std::vector<uint32_t> Reorder(const std::vector<uint32_t>& input);
};

} // namespace rtl
} // namespace gs2d

#endif // GS2D_BIDI_REORDER_H_