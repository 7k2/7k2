Seven Kingdoms II: The Fryhtan Wars
===================================

Requirements
------------

* GCC 3.0 or later
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

Game music
----------

Playing from the CD is no longer supported, so you will need to rip the tracks
and copy them to the music directory. The tracks need to be renamed as follows:

	track 01 -> cbrief.wav
	track 02 -> cfrywin.wav
	track 03 -> menu.wav
	track 04 -> lose.wav
	track 05 -> win.wav
	track 06 -> bkmusic1.wav
	track 07 -> bkmusic2.wav
	track 08 -> bkmusic3.wav
	track 09 -> bkmusic4.wav
	track 10 -> bkmusic5.wav
	track 11 -> bkmusic6.wav
	track 12 -> bkmusic7.wav

There also needs to be a file named "norman.wav" in the music directory. The
easiest way to create one is to do a "`touch norman.wav`" in the music
directory.
