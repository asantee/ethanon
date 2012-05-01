# SCons build script for SGI MIPSPro

import os

Help("""
Build parameters:
    prefix\tinstallation prefix [/usr/local]
    debug\twhether to build debug [0]
""")

# get installation prefix
PREFIX = os.path.abspath(ARGUMENTS.get('prefix', '/usr/local'))

base_env = Environment(
    CPPPATH = ['/usr/freeware/include'],
    LIBPATH = ['/usr/freeware/$LIBDIR'],
    LIBS = ['pthread', 'vorbisfile', 'vorbis', 'ogg', 'FLAC', 'audio', 'm'])
base_env.AppendENVPath('PATH', '/usr/freeware/bin')
base_env.Append(CXXFLAGS = ['-DHAVE_AL', '-DNO_DUMB', '-DNO_SPEEX',
                            '-DWORDS_BIGENDIAN'])

# build debug?
if ARGUMENTS.get('debug', 0):
    base_env.Append(CXXFLAGS = ['-g', '-DDEBUG'])
else:
    base_env.Append(CXXFLAGS = ['-O2'])

if ARGUMENTS.get('64'):
    base_env.Append(CCFLAGS = ['-64', '-mips4'],
                    LINKFLAGS = ['-64', '-mips4'])
    base_env['LIBDIR'] = 'lib64'
else:
    base_env['LIBDIR'] = 'lib32'

if base_env['CXX'] in ['g++', 'c++']:
    base_env.Append(CXXFLAGS = ['-Wall', '-Wno-non-virtual-dtor'])

Export('PREFIX base_env')

SConscript(dirs = ['doc', 'src', 'examples', 'test'])

base_env.Alias('install', PREFIX)
