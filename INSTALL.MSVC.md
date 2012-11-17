Compiling 7K2 with Microsoft Visual C++
=======================================

First, create a new folder hierarchy to hold the libraries that 7K2 needs:

	3rdPartyLibs
	    \Include
	        \SDL2
	    \Lib
	    \Runtime

Then, build the libraries by following the steps below.

JPEG library
------------
* Download the source code from http://www.ijg.org/
* Rename `jconfig.vc` to `jconfig.h`
* Rename `makejsln.v10` to `makejsln.sln`
* Rename `makejvcx.v10` to `jpeg.vcxproj`
* Open `makejsln.sln` in MSVC and build the library
* The `jpeg.lib` file will be built in the `Release` folder
* Copy the following files to `3rdPartyLibs\Include`:
  `jpeglib.h jerror.h jconfig.h jmorecfg.h`
* Copy `jpeg.lib` to `3rdPartyLibs\Lib`

OpenAL library
--------------
* Download the source code from http://kcat.strangesoft.net/openal.html
* Launch the CMake GUI
* Clear the CMake cache (open the `File` menu, then `Delete Cache`)
* Click on `Browse Source...` to set the location of the OpenAL source code
* Click on `Browse Build...` to set the build directory
* Click on `Configure` and verify that the project generator is set to `Visual
  Studio`, then click on `Finish`
* Click on `Generate` (the MSVC solution file will be generated in the build
  directory)
* Open `OpenAL.sln` in MSVC and build the library by right-clicking on
  `ALL_BUILD` and selecting `Build`
* Copy `OpenAL32.lib` from the `Release` folder of the build directory to
  `3rdPartyLibs\Lib`
* Copy `OpenAL32.dll` from the `Release` folder of the build directory to
  `3rdPartyLibs\Runtime`
* Copy the `AL` header directory (in the `include` folder) to
  `3rdPartyLibs\Include`

SDL2 library
------------
* Download the source code from http://www.libsdl.org/hg.php
* SDL2 requires DirectX SDK
* Open the solution file (in the `VisualC` folder) that corresponds to your
  installed version of MSVC
* In the Solution Explorer, rename the `SDL` project to `SDL2`
* In the Solution Explorer, rename the `SDLmain` project to `SDL2main`
* Build both `SDL2` and `SDL2main`
* Copy `include\*.h` to `3rdPartyLibs\Include\SDL2`
* Copy `VisualC\SDL\Win32\Release\SDL2.lib` to `3rdPartyLibs\Lib`
* Copy `VisualC\SDL\Win32\Release\SDL2.dll` to `3rdPartyLibs\Runtime`
* Copy `VisualC\SDLmain\Win32\Release\SDL2main.lib` to `3rdPartyLibs\Lib`

7K2
---
* Launch the CMake GUI
* Clear the CMake cache (open the `File` menu, then `Delete Cache`)
* Click on `Browse Source...` to set the location of the 7K2 source code
* Click on `Browse Build...` to set the build directory (create a new folder if
  needed)
* Click on `Configure` and verify that the project generator is set to `Visual
  Studio`, then click on `Finish`
* CMake will stop with an error, saying that it cannot find the JPEG library.
  Look for `CMAKE_PREFIX_PATH` and enter the full path to `3rdPartyLibs` in the
  `Value` field, e.g., `C:\Users\7K2\3rdPartyLibs`
* Click on `Configure` again
* Click on `Generate` (the MSVC solution file will be generated in the build
  directory)
* Open `7K2.sln` in MSVC and build the game by right-clicking on `ALL_BUILD`
  and selecting `Build`
* Copy `src\Release\7k2.exe` from the build directory to the 7K2 game folder
* Copy `3rdPartyLibs\Runtime\*` to the 7K2 game folder
