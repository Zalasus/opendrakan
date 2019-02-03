
OpenDrakan
==========

[![Build Status](https://travis-ci.org/Zalasus/opendrakan.svg?branch=master)](https://travis-ci.org/Zalasus/opendrakan)

A project to reverse-engineer and recreate Surreal Software's Riot Engine with the ultimate goal to be
able to run the game *Drakan: Order of the Flame* on a modern GNU/Linux system.

__This project is still in an early stage__. Most things are still work-in-progress and the current state of
the engine differs from the original game in a significant way. That means that you can't use this replacement 
to play the game yet. Until that is the case, I'd recommend to check out the AiO patch or Community Patch, found 
among other downloads on [Arokh's Lair](http://www.arokhslair.net/wp/downloads/). They fix some of the issues with the 
original engine that might otherwise prevent you from playing on Wine.

Contributions to the project are very welcome! Since I am mostly coding alone on OpenDrakan right now, I don't really
have a collaborative workflow set up yet. But should more people be interested in contributing, I'd gladly change that.
Feel free to open or comment on a ticket for anything you want to work on.

For discussions regarding OpenDrakan or contacting me, the safest option is the Arokh's Lair forum thread for this project, found 
[here](https://arokhslair.net/forum/viewtopic.php?f=4&t=4244). 

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

The engine base requires [zlib](https://www.zlib.net/), [GLM](https://glm.g-truc.net/) and [Bullet Physics](http://bulletphysics.org/wordpress/), as well as [CMake](https://cmake.org/) to generate the Makefiles.

For the OSG renderer (the only one available at the moment) you will additionally need 
[OpenSceneGraph](http://www.openscenegraph.org/) and an OpenAL implementation like 
[OpenAL Soft](http://kcat.strangesoft.net/openal.html), along with their respective dependencies.

On Linux, all of these dependencies should be provided by your distro, so simply installing the appropriate packages
should suffice to be able to compile OpenDrakan. This was tested successfully on Debian and Arch Linux.

Once you have all dependencies, fire up a terminal in the project root and do your usual CMake building routine:
```bash
$ mkdir build && cd build
$ cmake ..
$ make
```


Running
-------

__This is only useful for developers at the moment. You can't use OpenDrakan to play the game yet!__

Run the *odOsg* executable in the root directory of the game (same directory that contains the Drakan.exe) to
load the intro level. To load a specific level, run OpenDrakan with the path to the *.lvl file as an argument.

*Depending on the current state of the project, your results may vary*. Since not all levels have been tried, some may yet
crash the engine. Most testing has been done on the "Ruined Village" level, so that's the one you probably want to use when
testing your contributions.

OpenDrakan comes with the *srsced* tool, which can extract raw data from Drakan's SRSC files to aid in reverse-engineering.
You can get an overview of it's functions using the -h option. While this utility is limited to inspection and extraction at
the moment, editing functions will be added later.


License
-------

OpenDrakan is released under GPL 3.0 
(see [LICENSE](https://github.com/Zalasus/opendrakan/blob/master/LICENSE) for 
full license text).

