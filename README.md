Ethanon engine
==============

- [About the engine] [1]
- [Sample codes] [2]

  [1]: http://doc.ethanonengine.com/manual/6
  [2]: https://github.com/asantee/ethanon-samples

How to build
============

Things that can't be missed in order to compile successfully.

Windows
-------

- Use Visual Studio 2008 (9.0)
- Run **make_deploy.bat** to deploy all platform libraries and binaries.

Mac OS X
--------

- Use Xcode
- Unpack the following files located at **toolkit/Source/src/gs2d/vendors**:
  - vendors.7z (for the BoostSDK/)
  - irrKlang.zip
  - Cg.framework.zip
  - SDL.framework.zip