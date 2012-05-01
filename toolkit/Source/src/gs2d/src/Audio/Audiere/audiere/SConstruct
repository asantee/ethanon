import os, sys

# This is still fairly dirty, but it does work. -- treed, 2004-09-23
# TODO :
# It should probably have a builtin method to use doxygen.
# Entirely untested on Windows.
# Add wxplayer as a target.
# Add 'all' as a target to build audiere and wxplayer.
# Needs to check for pthreads and rt.

env = Environment(ENV=os.environ)

def addLib(lib):
	global env
	env.Append(LIBS="lib")

def addLibs(libs):
	for lib in libs:
		addLib(lib)

DEFINITIONS = {}

def define(name,what=True):
	global DEFINITIONS
	DEFINITIONS[name] = what
	env.Append(CFLAGS="-D" + name, CXXFLAGS="-D"+name)

def isdef(name):
	global DEFINITIONS
	return DEFINITIONS.has_key(name)

env.Append(CPPPATH=["#/src"])
env.Append(CXXFLAGS=['-Wall','-Wno-non-virtual-dtor']

# Sanity is good
if sys.platform == "win32":
	if ARGUMENTS.has_key('use_oss') and ARGUMENTS['use_oss'] == True:
		print "Detected Win32. Disabling OSS."
	ARGUMENTS['use_oss'] = False

if sys.platform != "win32":
	if ARGUMENTS.has_key('use_dsound') and ARGUMENTS['use_dsound'] == True:
		print "Detected POSIX system. Disabling DirectSound."
	ARGUMENTS['use_dsound'] = False

if sys.platform not in ["win32","cygwin"]:
	if ARGUMENTS.has_key('use_winmm') and ARGUMENTS['use_winmm'] == True:
		print "Detected POSIX system. Disabling WinMM."
	ARGUMENTS['use_winmm'] = False

conf = Configure(env)

# Parse command-line options.
if ARGUMENTS.get('use_oggvorbis','yes') == 'yes':
	conf.env.Append(LIBS=['vorbis','ogg','vorbisfile'])
	if not conf.CheckHeader("vorbis/vorbisfile.h"):
		print "Error: Ogg Vorbis support was not found. Either install libvorbis, or rebuild with use_oggvorbis=no."
		sys.exit(1)
		
	define("HAVE_OGG")
else:
	define("NO_OGG")

if ARGUMENTS.get('use_flac','yes') == 'yes':
	conf.env.Append(LIBS="m")
	if not conf.CheckLib('FLAC','FLAC__seekable_stream_decoder_process_single','FLAC/all.h'):
		print "Error: FLAC support was not found. Either install libFLAC, or rebuild with use_flac=no."
		sys.exit(1)
	define("HAVE_FLAC")
else:
	define("NO_FLAC")

if ARGUMENTS.get('use_dumb','yes') == 'yes':
	if not conf.CheckLib('dumb','duh_start_renderer','dumb.h'):
		print "Error: libdumb was not found. Either install it, or rebuild with use_dumb=no."
		sys.exit(1)
	define("HAVE_DUMB")
else:
	define("NO_DUMB")

	
if ARGUMENTS.get('use_oss','yes') == 'yes':
	if not (conf.CheckHeader("unistd.h") and conf.CheckHeader("sys/soundcard.h")):
		print "Error: OSS support was not found. Either enable it, or rebuild with use_oss=no."
		sys.exit(1)
	define("HAVE_OSS")
else:
	define("NO_OSS")

if ARGUMENTS.get('use_dsound', 'yes') == 'yes':
	conf.env.Append(LIBS=['dsound','ole32','rpcrt4'])
	if not conf.CheckHeader("dsound.h"):
		print "Error: DirectSound support was not found. Install DirectX or rebuild with use_dsound=no."
		sys.exit(1)
	define("HAVE_DSOUND")
else:
	define("NO_DSOUND")

PREFIX = ARGUMENTS.get('prefix','/usr/local')


# Handle source
source = """
	src/basic_source.cpp
	src/device_null.cpp
	src/square_wave.cpp
	src/debug.cpp
	src/input_wav.cpp
	src/threads_posix.cpp
	src/device.cpp
	src/loop_point_source.cpp
	src/timer_posix.cpp
	src/file_ansi.cpp
	src/memory_file.cpp
	src/tone.cpp
	src/input.cpp
	src/noise.cpp
	src/utility.cpp
	src/input_aiff.cpp
	src/resampler.cpp
	src/version.cpp
	src/sample_buffer.cpp
	src/device_mixer.cpp
	src/sound.cpp
	src/input_mp3.cpp
	src/sound_effect.cpp
	src/mpaudec/bits.c
	src/mpaudec/mpaudec.c
"""

if isdef("HAVE_FLAC"):
	source += " src/input_flac.cpp"

if isdef("HAVE_DUMB"):
	source += " src/input_mod.cpp"

if isdef("HAVE_OGG"):
	source += " src/input_ogg.cpp"

if isdef("HAVE_AL"): # NOTHING CHECKS FOR THIS YET
	source += " src/device_al.cpp"

if isdef("HAVE_OSS"):
	source += " src/device_oss.cpp"

if isdef("HAVE_DSOUND"):
	source += """
		src/device_ds.cpp 
		src/device_ds.h
		src/device_ds_buffer.cpp
		src/device_ds_buffer.h
		src/device_ds_stream.cpp
		src/device_ds_stream.h
		src/dxguid.cpp
	"""

if isdef("HAVE_WINMM"): # NOTHING CHECKS FOR THIS YET
	source += " src/device_mm.cpp"

env = conf.Finish()

env.Alias('audiere', env.SharedLibrary(target='audiere', source = Split(source)))
env.Default('audiere')

env.Install(dir = PREFIX + "/lib", source = ['libaudiere.so'])
env.Alias('install', [PREFIX + "/lib"])

env.Install(dir = PREFIX + "/include", source = ['src/audiere.h'])
env.Alias('install', [PREFIX + "/include"])
