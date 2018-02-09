
OpenDrakan
==========

An effort te reverse-engineer and hopefully reimplement Surreal Software's
Riot Engine with the ultimate goal to be able to run the game 
"Drakan: Order of the Flame" on a modern GNU/Linux system.

This project is still in a very early stage. So far it includes only the 
documentation on the partially decoded database format, code to read the engine's assets and
a very basic level viewer. Everything is still WIP and no extensive project planning has been done yet.

My documentation on the Riot Engine's SRSC file format can be found 
[here](https://github.com/Zalasus/opendrakan/blob/master/doc/riot_database_format.txt).


Copyright notice
----------------

Everything in this project was created by me or was taken from other open-source projects in
accordance with their respective license. This project does not and will never distribute any 
copyrighted material by Surreal Software or Warner Bros. Interactive Entertainment (who aquired
Surreal Software in 2009).

All information on the Riot Engine has been aquired through reverse engineering or by other, 
non-copyright-infringing means.


Compiling
---------

There are no makefiles yet, so you'd have to assemble these yourself or use
whatever way of building a project suits you best.

Dependencies are [zlib](https://www.zlib.net/) and [OpenSceneGraph](http://www.openscenegraph.org/).


License
-------

Despite it's early stage, I'm releasing this project under GPL 3.0 
(see [LICENSE](https://github.com/Zalasus/opendrakan/blob/master/LICENSE) for 
full license text).