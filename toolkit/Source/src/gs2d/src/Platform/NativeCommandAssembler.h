#ifndef NATIVE_COMMAND_ASSEMBLER_H_
#define NATIVE_COMMAND_ASSEMBLER_H_

#include <string>

namespace Platform {

class NativeCommandAssembler
{
public:
	static const std::string CMD_PLAY_SOUND;
	static const std::string CMD_LOAD_SOUND;
	static const std::string CMD_PLAY_MUSIC;
	static const std::string CMD_LOAD_MUSIC;
	static const std::string CMD_DELETE_SOUND;
	static const std::string CMD_DELETE_MUSIC;
    static const std::string CMD_QUIT_APP;
    static const std::string CMD_DETECT_JOYSTICKS;
	static const std::string CMD_STOP_MUSIC;
	static const std::string CMD_SET_GLOBAL_VOLUME;

	/*
	load_sound <string:sample_name>
	play_sound <string:sample_name> <float:volume> <int:loop> <float:speed>
	delete_sound <string:sample_name>
	*/

	static std::string QuitApplication();
	static std::string DetectJoysticks();
	static std::string LoadSound(const std::string& fileName);
	static std::string LoadMusic(const std::string& fileName);
	static std::string DeleteSound(const std::string& fileName);
	static std::string DeleteMusic(const std::string& fileName);
	static std::string PlaySound(const std::string& fileName, const float volume, const bool loop, const float speed);
	static std::string PlayMusic(const std::string& fileName, const float volume, const bool loop, const float speed);
	static std::string StopMusic(const std::string& fileName);
	static std::string SetGlobalVolume(const float volume);
};

} // namespace Platform

#endif
