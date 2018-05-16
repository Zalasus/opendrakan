
How to contribute to OpenDrakan
===============================

__Please note:__
As of this writing (2018/05/14), OpenDrakan is still in a very early stage. There are many features missing that are part of
the core game experience, and implementing them is our top priority. Because of this, there are many loose ends in the code
that need to be tied together first, and maintaining a larger number of contributers is really hard under these circumstances.
All willing contributers are appreciated, but please be aware that it might take some time before we can assign meaningful tasks
to everyone.

See the GitHub issue tracker for any required features and bugs that need fixing. Feel free to comment if you want to start working
on anything not already assigned to a developer. 

The easiest way of contributing is by forking the repository and creating pull requests. Just make sure that you don't work on
an issue somebody else is already working on.


Creating pull requests
======================
Please consider the following rules when creating pull requests:

 * Make sure to include a link to the issue that your PR is addressing (if any)
 * Summarize your changes
 * Explain your motivations for non-trivial changes 
 * Try addressing only one feature/bug per PR. Don't stuff too many unrelated changes into one request.
 * Stick to the [Policies and conventions](https://github.com/Zalasus/opendrakan/wiki/Policies-and-conventions), but don't commit
   just to fix style issues that are already there
 * Don't clutter your history with merge commits. Prefer rebasing your branches when applicable


Fixing Riot engine bugs and adding features
===========================================
OpenDrakan is a project to recreate Drakan as faithfully as possible, but same as with other projects of this kind, it is 
sometimes difficult to define what counts as a bug that needs fixing, and what needs to be recreated to preserve the original
feel of the game.

For now, we define everything as a bug that negatively impacts gameplay (like crashes or the buggy collision detection of the 
original Riot Engine). Every other bug should be discussed before deciding whether to recreate it or not.

Adding features is another thing. Some people have expressed their desire for gameplay improvements, like more detailed difficulty
settings or an improved editor. Development of these is low priority for now, and such features should always stay optional in the
final recreation. 


Branching and code conventions
==============================

The master branch should always reflect a recent, runnable version of the project. Don't push unfinished or highly 
experimental code here.

