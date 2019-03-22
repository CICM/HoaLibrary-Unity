## Hoa for [Unity 3D](https://unity3d.com/)

The [HoaLibrary](https://github.com/CICM/HoaLibrary-Light) as a native unity audio plugin spatializer (supports Unity 2017.1 or newer).

[![Travis](https://img.shields.io/travis/CICM/HoaLibrary-Unity.svg?label=travis)](https://travis-ci.org/CICM/HoaLibrary-Unity)
[![Appveyor](https://img.shields.io/appveyor/ci/CICM/HoaLibrary-Unity.svg?label=appveyor)](https://ci.appveyor.com/project/CICM/HoaLibrary-Unity/history)
[![Licence](https://img.shields.io/badge/license-GPL--v3-blue.svg)](https://github.com/CICM/HoaLibrary-Unity/blob/master/LICENSE)

You can download the latest version of the package in the [releases page](https://github.com/CICM/HoaLibrary-Unity/releases).
To install and use the plugin, please see [this page](https://github.com/CICM/HoaLibrary-Unity/wiki).

<a href="https://github.com/CICM/HoaLibrary-Unity/releases/latest">
  <img src="https://img.shields.io/github/release/CICM/HoaLibrary-Unity.svg" alt="Release">
</a>
<a href="https://github.com/CICM/HoaLibrary-Unity/wiki">
 <img src="https://img.shields.io/badge/@-documentation-blue.svg" alt="Documentation">
</a>

---

### Install :

Follow the instructions below if you want to build this plugin yourself.

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

You should see generated plugin in `/Assets/HoaLibrary/Plugins` directory.

### Organization

- [CICM](http://cicm.mshparisnord.org)
- [Université Paris 8](https://www.univ-paris8.fr)
- [(EUR) ArTeC](http://eur-artec.fr/)

### Author :

- [Eliott Paris](https://github.com/eliottparis)

### Credits :

- [HoaLibrary](https://github.com/CICM/HoaLibrary-Light)
- [Unity nativeaudioplugins SDK](https://bitbucket.org/Unity-Technologies/nativeaudioplugins).
- Sounds included in the demo from [freesound.org](https://freesound.org/).

### Licence :
HoaLibrary-Unity is under the terms of the [GNU Public License](http://www.gnu.org/copyleft/gpl.html). </br>
If you want to avoid the restrictions of the GPL and use HoaLibrary for a closed-source product, please [contact the CICM](http://cicm.mshparisnord.org/).

### Other implementations :
* [HoaLibrary-Max](https://github.com/CICM/HoaLibrary-Max) : HoaLibrary for Max.
* [HoaLibrary-Pd](https://github.com/CICM/HoaLibrary-PD) : HoaLibrary for Pure Data.
* [ofxHoa](https://github.com/CICM/ofxHoa) : HoaLibrary for openFrameworks.
* [HoaLibrary-Faust](https://github.com/CICM/HoaLibrary-Faust) : HoaLibrary for Faust.
* [Cinder-Hoa](https://github.com/saynono/Cinder-Hoa) - HoaLibrary for Cinder.
