#ifndef GS2D_RTL_PROCESSOR_H_
#define GS2D_RTL_PROCESSOR_H_

#include <string>

namespace gs2d {
namespace rtl {

/**
 * \brief Pipeline de processamento RTL (Right-To-Left)
 */
class RTLProcessor
{
public:
	static bool ContainsRTL(const std::string& utf8Text);
	static std::string Process(const std::string& utf8Text);
};

} // namespace rtl
} // namespace gs2d

#endif // GS2D_RTL_PROCESSOR_H_