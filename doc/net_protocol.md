
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

Basic packet structure
----------------------
u8 packet_type;
u16 packet_number; // growing steadily
u16 payload_length;
u8 payload_bytes[~];

UDP packet types
----------------

### 0 = compound
u8 packet_count;
packet packets[~];

### 1 = begin snapshot
u32 snapshot_id;
u32 number_of_updates;

### 2 = object moved
u64 tick_number;
u8 flags;
u32 object_id;
if(flags & 1) vec3 translation;
if(flags & 2) quat rotation;
if(flags & 4) vec3 scale;

### 3 = raw keyboard action
u32 snapshot_id;
u32 action_id;
u32 flags?;

### 4 = ACK snapshot
u32 snapshot_id;


TCP packet types
----------------

### 4 = Chat
u32 message_length;
