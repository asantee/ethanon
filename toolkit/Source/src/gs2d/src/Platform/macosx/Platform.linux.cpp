/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include <iostream>

#include "Platform.linux.h"

#include "../Platform.h"

#include "../../Application.h"

void gs2d::ShowMessage(str_type::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	#warning todo
}

gs2d::str_type::string gs2d::Application::GetPlatformName()
{
	return "linux";
}

namespace Platform {

gs2d::str_type::string ResourceDirectory()
{
	#warning todo
	return "";
}

gs2d::str_type::string GlobalExternalStorageDirectory()
{
	#warning todo
	return "";
}

gs2d::str_type::string ExternalStorageDirectory()
{
	#warning todo
	return "";
}

} // namespace
