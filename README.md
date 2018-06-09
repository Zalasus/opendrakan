
OpenDrakan
==========

[![Build Status](https://travis-ci.org/Zalasus/opendrakan.svg?branch=master)](https://travis-ci.org/Zalasus/opendrakan)

A project to reverse-engineer and recreate Surreal Software's Riot Engine with the ultimate goal to be
able to run the game *Drakan: Order of the Flame* on a modern GNU/Linux system.

__This project is still in an early stage__. Most things are still work-in-progress and the current state of
the engine differs from the original game in a significant way. OpenDrakan is definetly not a worthy replacement 
for the original engine yet. Until that is the case, I'd recommend to check out the AiO patch, found among other
downloads on [Arokh's Lair](http://www.arokhslair.net/wp/downloads/). It fixes some issues with the original
game that might prevent you from playing otherwise.

My documentation on the Riot Engine's SRSC file format can be found 
[here](https://github.com/Zalasus/opendrakan/blob/master/doc/riot_database_format.txt).
Additional documentation can be found in the [OpenDrakan wiki](https://github.com/Zalasus/opendrakan/wiki).


Copyright notice
----------------

Everything in this project was created specifically for OpenDrakan or was taken from other open-source
projects in accordance with their respective license. This project does not and will never distribute any 
copyrighted material by Surreal Software or Warner Bros. Interactive Entertainment (who aquired
Surreal Software in 2009). __OpenDrakan will not enable you to play Drakan without owning a legal copy!__

All information on the Riot Engine has been aquired through reverse engineering or by other, 
non-copyright-infringing means.


Compiling
---------

You'll need [zlib](https://www.zlib.net/), [Bullet](http://bulletphysics.org/wordpress/) and
[OpenSceneGraph](http://www.openscenegraph.org/) along with their respective dependencies, 
as well as [CMake](https://cmake.org/) to generate the Makefiles.
On Linux, these should be provided by your distro, so simply installing the appropriate packages
should suffice to compile OpenDrakan. This was tested successfully on Debian and Arch Linux.

Once you have all dependencies, fire up a terminal in the project root and do your usual CMake building routine:
```bash
$ mkdir build && cd build
$ cmake ..
$ make
```


Running
-------

Run the opendrakan executable in the root directory of the game (same directory that contains the Drakan.exe) to
load the intro level. To load a specific level, run OpenDrakan with the path to the *.lvl file as an argument.

Right now, OpenDrakan has a few command line options to inspect Drakan resource files and extract data from them to aid
in reverse-engineering. You can get an overview of these using the -h option.

*Depending on the current state of the project, your results may vary*. Right now, some levels load while others don't.
Most testing has been done on the "Ruined Village" level, so that's the one you probably want to try out OpenDrakan with.


License
-------

OpenDrakan is released under GPL 3.0 
(see [LICENSE](https://github.com/Zalasus/opendrakan/blob/master/LICENSE) for 
full license text).

