/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef ETH_TYPES_H_
#define ETH_TYPES_H_

#include <Math/Math.h>

#ifdef GS2D_STR_TYPE_WCHAR
#	include "../vendors/tinyxml_utf16/tinyxml.h"
#else
#	include "../vendors/tinyxml_ansi/tinyxml.h"
#endif

using namespace gs2d::math;
using namespace gs2d;

#define ETH_MAX_PARTICLE_SYS_PER_ENTITY 2
#define ETH_DEFAULT_SHADOW_RANGE 512.0f

#define ETH_SMALL_NUMBER (0.1f)

#define ETH_DEFAULT_PARTICLE_BITMAP	GS_L("particle.png")
#define ETH_APP_PROPERTIES_FILE GS_L("app.enml")
#define ETH_DEFAULT_BITMAP_FONT GS_L("Verdana14_shadow.fnt")
#define ETH_DEFAULT_PROPERTY_APP_ENML_ENTITY GS_L("default")
#define ETH_OLDER_DEFAULT_PROPERTY_APP_ENML_ENTITY GS_L("window")

#define _ETH_EMPTY_SCENE_STRING GS_L("empty")

#define ETH_WHITE_V3 (Vector3(1.0f,1.0f,1.0f))
#define ETH_BLACK_V3 (Vector3(0.0f,0.0f,0.0f))
#define ETH_WHITE_V4 (Vector4(1.0f,1.0f,1.0f,1.0f))
#define ETH_BLACK_V4 (Vector4(0.0f,0.0f,0.0f,1.0f))

#define _ETH_SHADOW_SCALEX (0.8f)
#define _ETH_SHADOW_SCALEY (8.0f)
#define _ETH_SHADOW_FAKE_STRETCH (2.2f)
#define _ETH_SHADOW_SIZE_TOLERANCE (0.95f)
#define _ETH_DEFAULT_LIGHT_INTENSITY (2.0f)
#define _ETH_DEFAULT_AMBIENT_LIGHT (0.3f)
#define _ETH_MIN_BUCKET_SIZE (128.0f)
#define _ETH_DEFAULT_BUCKET_SIZE (256.0f)
#define _ETH_SCENE_EDITOR_BUCKET_SIZE (Vector2(512.0f,512.0f))

#define ETH_STREAM_DECL(name) gs2d::str_type::stringstream name; name

#ifdef _MSC_VER
	#ifdef GS2D_STR_TYPE_ANSI
		#define _ETH_SAFE_swscanf sscanf_s
	#else
		#define _ETH_SAFE_swscanf swscanf_s
	#endif
#else
	#ifdef GS2D_STR_TYPE_ANSI
		#define _ETH_SAFE_swscanf sscanf
	#else
		#define _ETH_SAFE_swscanf swscanf
	#endif
#endif

#define ETH_CALLBACK_PREFIX GS_L("ETHCallback_")
#define ETH_CONSTRUCTOR_CALLBACK_PREFIX GS_L("ETHConstructorCallback_")
#define ETH_DESTRUCTOR_CALLBACK_PREFIX GS_L("ETHDestructorCallback_")
#define ETH_BEGIN_CONTACT_CALLBACK_PREFIX GS_L("ETHBeginContactCallback_")
#define ETH_END_CONTACT_CALLBACK_PREFIX GS_L("ETHEndContactCallback_")
#define ETH_CONTACT_CALLBACK_ARGS   GS_L("ETHEntity@, ETHEntity@, const vector2 &in")

#define ETH_INLINE inline

typedef long ETH_INT;
typedef unsigned char ETH_BOOL;
#define ETH_TRUE (1)
#define ETH_FALSE (0)

// TO-DO/TODO: remove deprecation constants in near future
enum ETH_ENTITY_TYPE
{
	ETH_HORIZONTAL = 0,
	ETH_GROUND_DECAL = 1, // deprecated
	ETH_VERTICAL,
	ETH_OVERALL, // depracated
	ETH_OPAQUE_DECAL, // deprecated
	ETH_LAYERABLE
};

enum ETH_MESSAGE
{
	ETH_ERROR = 0,
	ETH_WARNING = 1,
	ETH_INFO = 2
};

enum ETH_SCENE_SORT_MODE
{
	ETH_HEIGHT = 0,
	ETH_LAYERDEPTH = 1,
};

enum ETH_BODY_SHAPE
{
	ETHBS_NONE = 0,
	ETHBS_BOX = 1,
	ETHBS_CIRCLE = 2,
	ETHBS_POLYGON = 3,
	ETHBS_COMPOUND = 4
};

struct ETH_VIEW_RECT
{
	Vector2 v2Min, v2Max;
};

// namespace for global functions and variables
namespace ETHGlobal {

const str_type::char_t entityTypes[][64] =
{
	GS_L("horizontal"),
	GS_L("ground decal"),
	GS_L("vertical"),
	GS_L("overall"),
	GS_L("opaque decal"),
	GS_L("layerable")
};

} // namespace ETHGlobal

#endif
