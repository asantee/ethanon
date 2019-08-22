#ifndef ETH_TYPES_H_
#define ETH_TYPES_H_

#include <Math/GameMath.h>

#include "../vendors/tinyxml_ansi/tinyxml.h"

using namespace gs2d::math;
using namespace gs2d;

#define ETH_MAX_PARTICLE_SYS_PER_ENTITY 2
#define ETH_DEFAULT_SHADOW_RANGE 512.0f

#define ETH_SMALL_NUMBER (0.1f)

#define ETH_DEFAULT_PARTICLE_BITMAP	("particle.png")
#define ETH_APP_PROPERTIES_FILE ("app.enml")
#define ETH_DEFAULT_BITMAP_FONT ("Verdana14_shadow.fnt")
#define ETH_DEFAULT_PROPERTY_APP_ENML_ENTITY ("default")
#define ETH_OLDER_DEFAULT_PROPERTY_APP_ENML_ENTITY ("window")

#define _ETH_EMPTY_SCENE_STRING ("empty")

#define ETH_WHITE_V3 (Vector3(1.0f,1.0f,1.0f))
#define ETH_BLACK_V3 (Vector3(0.0f,0.0f,0.0f))
#define ETH_WHITE_V4 (Vector4(1.0f,1.0f,1.0f,1.0f))
#define ETH_BLACK_V4 (Vector4(0.0f,0.0f,0.0f,1.0f))

#define _ETH_DEFAULT_LIGHT_INTENSITY (2.0f)
#define _ETH_DEFAULT_AMBIENT_LIGHT (0.3f)
#define _ETH_MIN_BUCKET_SIZE (128.0f)
#define _ETH_DEFAULT_BUCKET_SIZE (256.0f)
#define _ETH_SCENE_EDITOR_BUCKET_SIZE (Vector2(512.0f,512.0f))

#define ETH_STREAM_DECL(name) std::stringstream name; name

#define ETH_CALLBACK_PREFIX ("ETHCallback_")
#define ETH_CONSTRUCTOR_CALLBACK_PREFIX ("ETHConstructorCallback_")
#define ETH_DESTRUCTOR_CALLBACK_PREFIX ("ETHDestructorCallback_")

typedef long ETH_INT;
typedef unsigned char ETH_BOOL;

#define ETH_TRUE (1)
#define ETH_FALSE (0)

#include <boost/shared_ptr.hpp>

class ETHResourceProvider;
typedef boost::shared_ptr<ETHResourceProvider> ETHResourceProviderPtr;

enum ETH_MESSAGE
{
	ETH_ERROR = 0,
	ETH_WARNING = 1,
	ETH_INFO = 2
};

// namespace for global functions and variables
namespace ETHGlobal {

const char entityTypes[][64] =
{
	("horizontal"),
	("ground decal"),
	("vertical"),
	("overall"),
	("opaque decal"),
	("layerable")
};

} // namespace ETHGlobal

#endif
