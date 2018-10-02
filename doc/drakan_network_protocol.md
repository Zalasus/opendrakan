
Drakan Network Protocol research
================================

Here is some very early research concerning the Drakan network protocol.

The protocol uses both UDP and TCP packets.

The first byte could be some kind of "service identifier". When first joining a server,
these vary between packages. During gameplay, they are mostly 0x2a.

This is always followed by a 16 bit length field containing the length of the whole packet
data, including the identifier and the length field itself.

The packets starting with 0x2a contain two bytes after that which seem to indicate source
followed by destination. 0x01 seems to be the server, clients start at 0x02. Could be 0x00 means broadcast.

The next 5 bytes are unknown.
After that is another 16 length field containing the length of the rest of the packet, _not_ including that length
field.

```
uint8_t    packet_type;   // 0x2a = during gameplay, also for chat etc.
uint16_t   packet_length; // including type and length field (equal to UDP/TCP payload size)

// only for packet_type 0x2a: 
uint8_t    source;
uint8_t    destination;
uint8_t[5] unknown; // command type?

uint16_t   command_specifier_length; // not including this field
uint8_t[~] command_data;
``` 


(TCP) Chat message
------------------

```
[...]
uint32_t     message_length;
uint8_t[~]   message; // this includes player name. seems to be always formatted like this: "<player name><space><0x1a><space><space><message>"
uint8_t      terminator = 0; 
```

