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
const str_type::string NativeCommandAssembler::CMD_DETECT_JOYSTICKS = "detect_joysticks";

gs2d::str_type::string  NativeCommandAssembler::QuitApplication()
{
	return CMD_QUIT_APP;
}

gs2d::str_type::string  NativeCommandAssembler::DetectJoysticks()
{
	return CMD_DETECT_JOYSTICKS;
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
