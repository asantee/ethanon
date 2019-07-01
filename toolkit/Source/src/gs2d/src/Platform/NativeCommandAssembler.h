#ifndef NATIVE_COMMAND_ASSEMBLER_H_
#define NATIVE_COMMAND_ASSEMBLER_H_

#include "../Types.h"

namespace Platform {

class NativeCommandAssembler
{
public:
	static const gs2d::str_type::string CMD_PLAY_SOUND;
	static const gs2d::str_type::string CMD_LOAD_SOUND;
	static const gs2d::str_type::string CMD_PLAY_MUSIC;
	static const gs2d::str_type::string CMD_LOAD_MUSIC;
	static const gs2d::str_type::string CMD_DELETE_SOUND;
	static const gs2d::str_type::string CMD_DELETE_MUSIC;
    static const gs2d::str_type::string CMD_QUIT_APP;
    static const gs2d::str_type::string CMD_DETECT_JOYSTICKS;
	static const gs2d::str_type::string CMD_STOP_MUSIC;
	static const gs2d::str_type::string CMD_SET_GLOBAL_VOLUME;

	/*
	load_sound <string:sample_name>
	play_sound <string:sample_name> <float:volume> <int:loop> <float:speed>
	delete_sound <string:sample_name>
	*/

	static gs2d::str_type::string QuitApplication();
	static gs2d::str_type::string DetectJoysticks();
	static gs2d::str_type::string LoadSound(const gs2d::str_type::string& fileName);
	static gs2d::str_type::string LoadMusic(const gs2d::str_type::string& fileName);
	static gs2d::str_type::string DeleteSound(const gs2d::str_type::string& fileName);
	static gs2d::str_type::string DeleteMusic(const gs2d::str_type::string& fileName);
	static gs2d::str_type::string PlaySound(const gs2d::str_type::string& fileName, const float volume, const bool loop, const float speed);
	static gs2d::str_type::string PlayMusic(const gs2d::str_type::string& fileName, const float volume, const bool loop, const float speed);
	static gs2d::str_type::string StopMusic(const gs2d::str_type::string& fileName);
	static gs2d::str_type::string SetGlobalVolume(const float volume);
};

} // namespace Platform

#endif
