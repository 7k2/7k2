Seven Kingdoms II: The Fryhtan Wars
===================================

Requirements
------------

* GCC 3.0 or later
* JWasm 2.00 or later
* SDL2 2.0.0 or later
* CMake 2.8.0 or later
* libjpeg 6b or later
* OpenAL 1.13 or later

Building the game
-----------------

An out-of-tree build is the recommended way of compiling the source code:

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make

Installing the game
-------------------

The game data has *not* been released under the GPL, so you will need a copy of
the original game. Copy the file `7k2` from the build directory (`build/src` if
you followed the above example) to your installed game directory.

Running the game
----------------

Before starting the game, do a "`touch debug.sys`" in your game directory. This
tells the game to start in debug mode. If you're running the game on a
case-sensitive filesystem, convert the game data filenames to lowercase. Then,
run `7k2` to start the game.
