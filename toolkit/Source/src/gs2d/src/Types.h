#ifndef GS2D_TYPES_H_
#define GS2D_TYPES_H_

#ifdef GS2D_EXPORT
 #define GS2D_API __declspec(dllexport)
#else
 #define GS2D_API
#endif

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp> 
#include <boost/any.hpp>

#define GS2D_UNUSED_ARGUMENT(argument) ((void)(argument))

#include <string>
#include <sstream>

namespace gs2d {

namespace str_type {
 typedef std::string string;
 typedef std::stringstream stringstream;
 typedef char char_t;
 typedef std::ifstream ifstream;
 typedef std::ofstream ofstream;
 typedef std::ostringstream ostringstream;
 #define GS_L(x) x
 #define GS2D_COUT std::cout
 #define GS2D_CERR std::cerr
} // namespace str_type

#ifdef _MSC_VER
  #define GS2D_SSCANF sscanf_s
#else
  #define GS2D_SSCANF sscanf
#endif

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;
typedef boost::weak_ptr<Texture> TextureWeakPtr;

class Sprite;
typedef boost::shared_ptr<Sprite> SpritePtr;
typedef boost::weak_ptr<Texture> TextureWeakPtr;

class Video;
typedef boost::shared_ptr<Video> VideoPtr;
typedef boost::shared_ptr<const Video> VideoConstPtr;
typedef boost::weak_ptr<Video> VideoWeakPtr;

enum GS_MESSAGE_TYPE
{
	GSMT_INFO = 0,
	GSMT_WARNING = 1,
	GSMT_ERROR = 2
};

} // namespace gs2d
#endif
