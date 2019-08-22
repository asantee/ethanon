#include "NativeCommandAssembler.h"

#include <string>
#include <sstream>

namespace Platform {

const std::string NativeCommandAssembler::CMD_PLAY_SOUND = "play_sound";
const std::string NativeCommandAssembler::CMD_PLAY_MUSIC = "play_music";
const std::string NativeCommandAssembler::CMD_LOAD_SOUND = "load_sound";
const std::string NativeCommandAssembler::CMD_LOAD_MUSIC = "load_music";
const std::string NativeCommandAssembler::CMD_DELETE_SOUND = "delete_sound";
const std::string NativeCommandAssembler::CMD_DELETE_MUSIC = "delete_music";
const std::string NativeCommandAssembler::CMD_STOP_MUSIC = "stop_music";
const std::string NativeCommandAssembler::CMD_SET_GLOBAL_VOLUME = "set_global_volume";
const std::string NativeCommandAssembler::CMD_QUIT_APP = "quit_app";
const std::string NativeCommandAssembler::CMD_DETECT_JOYSTICKS = "detect_joysticks";

std::string  NativeCommandAssembler::QuitApplication()
{
	return CMD_QUIT_APP;
}

std::string  NativeCommandAssembler::DetectJoysticks()
{
	return CMD_DETECT_JOYSTICKS;
}

std::string NativeCommandAssembler::LoadSound(const std::string& fileName)
{
	std::stringstream ss;
	ss << CMD_LOAD_SOUND << " " << fileName;
	return ss.str();
}

std::string NativeCommandAssembler::LoadMusic(const std::string& fileName)
{
	std::stringstream ss;
	ss << CMD_LOAD_MUSIC << " " << fileName;
	return ss.str();
}

std::string NativeCommandAssembler::DeleteSound(const std::string& fileName)
{
	std::stringstream ss;
	ss << CMD_DELETE_SOUND << " " << fileName;
	return ss.str();
}

std::string NativeCommandAssembler::DeleteMusic(const std::string& fileName)
{
	std::stringstream ss;
	ss << CMD_DELETE_MUSIC << " " << fileName;
	return ss.str();
}

std::string NativeCommandAssembler::PlaySound(const std::string& fileName, const float volume, const bool loop, const float speed)
{
	std::stringstream ss;
	ss << CMD_PLAY_SOUND << " " << fileName << " " << volume << " " << ((loop) ? "1" : "0") << " " << speed;
	return ss.str();
}

std::string NativeCommandAssembler::PlayMusic(const std::string& fileName, const float volume, const bool loop, const float speed)
{
	std::stringstream ss;
	ss << CMD_PLAY_MUSIC << " " << fileName << " " << volume << " " << ((loop) ? "1" : "0") << " " << speed;
	return ss.str();
}

std::string NativeCommandAssembler::StopMusic(const std::string& fileName)
{
	std::stringstream ss;
	ss << CMD_STOP_MUSIC << " " << fileName;
	return ss.str();
}

std::string NativeCommandAssembler::SetGlobalVolume(const float volume)
{
	std::stringstream ss;
	ss << CMD_SET_GLOBAL_VOLUME << " " << volume;
	return ss.str();
}

} // namespace Platform
