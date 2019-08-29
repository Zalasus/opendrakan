
The big OpenDrakan TODO list
============================

This list contains all the features and details of OpenDrakan that still need to be implemented or require more work.
Feel free to pick an item and open a ticket for it should you wish to work on a feature.

- Multiplayer
    - Biggest hurdle to progress yet. Nearly all game logic should interact with this, so until this stands, progress with game logic is stalled
    - Protocol compatibility has low priority. If the original protocol is the source of bugs, or simply can't be reverse-engineered sufficiently, scratch it
    - Since handling multiplayer probably involves some incremental state tracking, this can probably be leveraged for easily implementing savegames
- Drakan GUI
    - GUI framework needs refinement
        - Support for layout building blocks (boxes, scrollable widgets etc.)
        - Animations
    - Text rendering support
    - Widgets required:
        - ~~Health orb~~
        - ~~Main menu~~
        - Dragon power orb
        - Breath meter
        - Loading bar
        - Settings menu (pretty complex)
    - Main menu crystal buttons still look wrong (crystal motion and lighting)
- Savegames
    - Backwards compatibility is almost completely irrelevant here
    - Might be easier to leverage multiplayer code for this
- Skeletal animation code needs work
    - Needs support for inverse kinematics
    - Movements of skeleton need to apply to bounding data
    - ~~Needs support for multiple interpolation styles (mostly "no interpolation" for authentic Drakan animations)~~
- STOMP sequences
- Renderer synchronization
    - Access synchronization to the rendering subsystem is inconsistently used or even absent in many cases. This needs a
      new efficient and effective concept.
- Physics performance
    - The physics system is the biggest hog of game logic loop time right now. There might be some optimizations possible, like
      a broadphase that handles layers separately.
- RFL classes
    - Most of the class types still need to be implemented
        - Keep in mind that these will likely require close interaction with the multiplayer code
    - Many classes' original behaviour still needs to be researched (will add list of implemented/documented classes soon)
- Layer PVS
    - We shouldn't spawn objects and layers until they are deemed visible by the current layer
    - A stronger association between objects and the layers they are "on" might be necessary
        - Should consider objects that hang off the ceiling, too
- DLS loading in music subsystem
    - The FluidSynth based subsystem does not like Drakan's DLS files, also requires big dependency for DLS support
    - Might want to try out a different synth altogether
- Character controllers
    - Character controlled by player
    - Dragon controlled by player
    - Character controlled by AI
    - Dragon controlled by AI
- AI
        