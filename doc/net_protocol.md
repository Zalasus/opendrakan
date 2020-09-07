
OpenDrakan network protocol (draft)
===================================

This document contains considerations for the network protocol I'm planning to
use in OpenDrakan. Initially, this will be incompatible with the original Drakan
protocol, but once the latter is sufficiently reverse-engineered, we might
specify a translation layer.

Tick number
-----------
At 60 ticks per second, a uint32 would overflow after about 2.2 years. uint64
would overflow after 9.8 billion years.

Snapshot transmission
---------------------
Snapshots are sent to a client in two steps: First, an arbitrary number of
change packets referring to the snapshot using it's tick number are sent. Then,
a *confirmation* packet is transmitted, which states how many discrete changes
the snapshot with the given tick should contain, at which server time it was
taken, and possibly more metadata.

Upon reception of a confirmation packet, a client marks it's local version of
the snapshots as confirmed and stores the expected number of changes for that
snapshot. Once a confirmed snapshot contains as many changes as stated by the
confirmation, it is complete and can be moved into the local timeline.

Packet loss will cause either the confirmation or a number of changes to be
missing, so the expected change count will never match the one recorded. Thus,
an incomplete snapshot will not be considered by the client.

This scheme is also independent of the packet's order upon arrival.

Object States
-------------
Objects possess *states*, which covers things like position, rotation, animation
etc. These basic states are defined by the engine. Additionally, the RFL class
of an object can define *extra states*, which are sent in a separate channel.

States are also how the engine handles persistence across play sessions by
writing the states that changed since loading the level to a savegame.

Basic packet structure
----------------------
u8 packet_type;
u16 payload_length;
u8 payload_bytes[~];

Basic packet types
------------------

### Compound
u8 packet_count;
packet packets[~];

Snapshot packets
----------------
These are used to send snapshots from the server to a client. Thus, they are
only valid in the downlink. A client does not send state updates to the server.

### Confirm Snapshot
u64 tick_number;
f64 realtime;
u32 number_of_discrete_changes;

### Object State Changed
u64 tick_number;
u32 object_id;
u32 state_flags;
if(state_flags & (1 << 1)) vec3 translation;
if(state_flags & (1 << 2)) quat rotation;
if(state_flags & (1 << 3)) vec3 scale;
if(state_flags & (1 << 4)) u8 visibility;
if(state_flags & (1 << 5)) f32 animation_frame;

### Object Extra State Changed
u64 tick_number;
u32 object_id;
u16 first_state_number;
u16 state_count;
u8 state_data[~];

Gameplay packets
----------------
These are sent as soon as the action happens on the transmitting side. They
don't use the timeline.

### Action (Uplink)
u32 action_code;
u8 action_state;

### Analog Action (Uplink)
u32 action_code;
u8  flags; // 1 == has_y_axis
f32 x;
if(flags & 1) f32 y;

### Chat
u8 message_chars[];
