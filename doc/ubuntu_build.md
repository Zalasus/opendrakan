
Building on Ubuntu
===================

This is a step-by-step guide for building *OpenDrakan* on Ubuntu.

**Please note that OpenDrakan is still very much unfinished!** This guide only serves as a reference for developers and future build guides.

Installing dependencies
-----------------------

First we need to install all the dependencies. These are:
- libopenscenegraph-3.4-dev
- libbullet-dev
- libglm-dev
- zlib1g-dev
- cmake
- build-essential
- (git)

To install all of these at once, simply run the following code in a terminal:

```
$ sudo apt-get install libopenscenegraph-3.4-dev libbullet-dev libglm-dev zlib1g-dev cmake build-essential git
```

Enter your password and wait for the installation to finish.


Building
--------
Now we start the building process. Open a terminal and clone the *OpenDrakan* repository if you haven't done that already:

```
$ git clone https://github.com/Zalasus/opendrakan
```

Now you should have a directory called 'opendrakan' with all the sources in it.
For building, we create a dedicated build subdirectory in which all the build files and compiler output will be created:

```
$ cd opendrakan
$ mkdir build && cd build
$ cmake ..
```

Wait for CMake to do it's thing. If something's wrong (like missing dependencies), it will get reported here. In that case, try to fix
whatever went wrong and simply run `cmake ..` again.

Once CMake ran successfully, we can start the actual compilation process like this:

```
$ make
```
Optionally, to speed things up by compiling on multiple threads, use `make -j<number of threads>`, where `<number of threads>`
usually is equal to your CPU core count.

Wait for the compilation to finish. This might take a minute or two. This should leave you with a set of executables, among them *odOsg*, which contains both the engine and the renderer.


Running
-------

Depending on the state of the project, running *OpenDrakan* is as simple as running:

```
$ ./odOsg <path to lvl file>
```

Pass the `-h` option to odOsg to get a list of command line parameters.

