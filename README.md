## Hoa for [Unity 3D](https://unity3d.com/)

The [HoaLibrary](https://github.com/CICM/HoaLibrary-Light) as a native unity audio plugin.

[![Travis](https://img.shields.io/travis/CICM/HoaLibrary-Unity.svg?label=travis)](https://travis-ci.org/CICM/HoaLibrary-Unity)
[![Appveyor](https://img.shields.io/appveyor/ci/CICM/HoaLibrary-Unity.svg?label=appveyor)](https://ci.appveyor.com/project/CICM/HoaLibrary-Unity/history)

> This project is under development, there is no release yet.

### Install :

##### Clone project :

```shell
# clone project
$ git clone https://github.com/CICM/HoaLibrary-Unity.git
$ cd HoaLibrary-Unity
# init submodules
$ git submodule update --init --recursive
# create build directory
$ cd NativeCode
$ mkdir build && cd build
```

##### Generate project, build and install (MacOS) :

```shell
$ cmake -G Xcode -DCMAKE_BUILD_TYPE=Release ..
$ xcodebuild -configuration Release -target install
```

##### Generate project, build and install (Windows) :

```shell
$ cmake .. -G "Visual Studio 15 2017 Win64"
$ cmake --build . --config Release --target INSTALL
```

You should see generated plugin in `/Assets/Plugins` directory.

### Licence :
The HoaLibrary is under the terms of the <a title="GNU" href="http://www.gnu.org/copyleft/gpl.html">GNU Public License</a>. </br>
If you want to avoid the restrictions of the GPL and use HoaLibrary for a closed-source product, please [contact the CICM](http://cicm.mshparisnord.org/).

### Other implementations :
* [HoaLibrary-Max](https://github.com/CICM/HoaLibrary-Max) : HoaLibrary for Max.
* [HoaLibrary-Pd](https://github.com/CICM/HoaLibrary-PD) : HoaLibrary for Pure Data.
* [ofxHoa](https://github.com/CICM/ofxHoa) : HoaLibrary for openFrameworks.
* [HoaLibrary-Faust](https://github.com/CICM/HoaLibrary-Faust) : HoaLibrary for Faust.
* [Cinder-Hoa](https://github.com/saynono/Cinder-Hoa) - HoaLibrary for Cinder.
