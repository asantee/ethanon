# Ethanon engine

- [About the engine] [1]
- [Sample code] [2]

  [1]: http://doc.ethanonengine.com/manual/6
  [2]: https://github.com/asantee/ethanon-samples

### License

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

## Preresquisites

- Git
### Windows notes

At Windows, most libraries suggest installing it's headers/binaries at `%ProgramFiles%`, just
like any other application, but for building permission simplicity we convention to install
all libraries at `%UserProfile%\.usr\`.
For creating this directory, use mkdir ".usr" at windows console.

Example:
during libboost install on Windows, use:
`b2 install --prefix=%UserProfile%\.usr\`

## Dependencies

- [libboost ^1.69](https://boost.org)
  - `git clone --recursive https://github.com/boostorg/boost.git`
  - [or download archive](https://www.boost.org/users/download/)
  - [Getting Started](https://github.com/boostorg/boost/wiki/Getting-Started%3A-Overview)
- [libzip]()
- [tsl/hopscotch](https://github.com/Tessil/hopscotch-map)
- [glext](https://sourceforge.net/projects/glextwin32/)
- [SDL2](https://www.libsdl.org/download-2.0.php)
- OpenGL
- [fmod](https://www.fmod.com/)
- [Angelscript ^2.32](https://angelcode.com)

## Build Tools

### Windows
- MSVC 16

## Currently used IDEs

### Windows
- Visual Studio 2019

### Mac OS X

- Xcode

## How to build

- Unpack the following files located at **toolkit/Source/src/gs2d/vendors**:
  - vendors.zip
  - Cg.framework.zip
  - SDL2.framework.zip



- Use Visual Studio 2019 (16.0)
- Unpack the following files located at **toolkit/Source/src/gs2d/vendors**:
  - vendors.zip

