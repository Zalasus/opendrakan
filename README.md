
OpenDrakan
==========

An effort te reverse-engineer and hopefully reimplement Surreal Software's
Riot Engine with the ultimate goal to be able to run the game 
"Drakan: Order of the Flame" on a modern GNU/Linux system.

This project is still in a very early stage. So far it includes only the 
documentation on the partially decoded database format and a bit of code to read
the Drakan data files. Everything is still WIP and no extensive project
planning has been done yet.

My documentation on the Riot Engine's SRSC file format can be found 
[here](https://github.com/Zalasus/opendrakan/blob/master/doc/riot_database_format.txt).


Compiling
---------

There are no makefiles yet, so you'd have to assemble these yourself or use
whatever way of building a project suits you best.

The only dependencies yet are zlib and this wrapper for it: 
https://github.com/mateidavid/zstr


License
-------

Despite it's early stage, I'm releasing this project under GPL 3.0 
(see [LICENSE](https://github.com/Zalasus/opendrakan/blob/master/LICENSE) for 
full license text).