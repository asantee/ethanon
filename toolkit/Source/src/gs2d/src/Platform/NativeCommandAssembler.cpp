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

#include "NativeCommandAssembler.h"

namespace Platform {

using namespace gs2d;

const str_type::string NativeCommandAssembler::CMD_PLAY_SOUND = "play_sound";
const str_type::string NativeCommandAssembler::CMD_PLAY_MUSIC = "play_music";
const str_type::string NativeCommandAssembler::CMD_LOAD_SOUND = "load_sound";
const str_type::string NativeCommandAssembler::CMD_LOAD_MUSIC = "load_music";
const str_type::string NativeCommandAssembler::CMD_DELETE_SOUND = "delete_sound";
const str_type::string NativeCommandAssembler::CMD_DELETE_MUSIC = "delete_music";
const str_type::string NativeCommandAssembler::CMD_STOP_MUSIC = "stop_music";
const str_type::string NativeCommandAssembler::CMD_SET_GLOBAL_VOLUME = "set_global_volume";
const str_type::string NativeCommandAssembler::CMD_QUIT_APP = "quit_app";

gs2d::str_type::string  NativeCommandAssembler::QuitApplication()
{
	return CMD_QUIT_APP;
}

str_type::string NativeCommandAssembler::LoadSound(const str_type::string& fileName)
{
	str_type::stringstream ss;
	ss << CMD_LOAD_SOUND << " " << fileName;
	return ss.str();
}

str_type::string NativeCommandAssembler::LoadMusic(const str_type::string& fileName)
{
	str_type::stringstream ss;
	ss << CMD_LOAD_MUSIC << " " << fileName;
	return ss.str();
}

str_type::string NativeCommandAssembler::DeleteSound(const str_type::string& fileName)
{
	str_type::stringstream ss;
	ss << CMD_DELETE_SOUND << " " << fileName;
	return ss.str();
}

str_type::string NativeCommandAssembler::DeleteMusic(const str_type::string& fileName)
{
	str_type::stringstream ss;
	ss << CMD_DELETE_MUSIC << " " << fileName;
	return ss.str();
}

str_type::string NativeCommandAssembler::PlaySound(const str_type::string& fileName, const float volume, const bool loop, const float speed)
{
	str_type::stringstream ss;
	ss << CMD_PLAY_SOUND << " " << fileName << " " << volume << " " << ((loop) ? "1" : "0") << " " << speed;
	return ss.str();
}

str_type::string NativeCommandAssembler::PlayMusic(const str_type::string& fileName, const float volume, const bool loop, const float speed)
{
	str_type::stringstream ss;
	ss << CMD_PLAY_MUSIC << " " << fileName << " " << volume << " " << ((loop) ? "1" : "0") << " " << speed;
	return ss.str();
}

str_type::string NativeCommandAssembler::StopMusic(const str_type::string& fileName)
{
	str_type::stringstream ss;
	ss << CMD_STOP_MUSIC << " " << fileName;
	return ss.str();
}

str_type::string NativeCommandAssembler::SetGlobalVolume(const float volume)
{
	str_type::stringstream ss;
	ss << CMD_SET_GLOBAL_VOLUME << " " << volume;
	return ss.str();
}

} // namespace Platform