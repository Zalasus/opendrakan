
Drakan Network Protocol research
================================

Here is some very early research concerning the Drakan network protocol.

The protocol uses both UDP and TCP packets.

Server discovery
----------------
The protocol used during server discovery differs from the one used during gameplay. It uses UDP, and all sent packets
consist of a sequence of backslash-separated keywords. This seems to be a standard format used by MPlayer/GameSpy. 
Documentation on that is scarce, but this is not an important feature anyway, so I'll keep this section brief.

See [this](https://www.oldunreal.com/wiki/index.php?title=All_you_ever_wanted_to_know_about_the_Master_Uplink_settings_in_Unreal_-_IpServer_Package_for_Unreal_v1.0) 
page for some info that might be related to the Drakan discovery.

For LAN-discovery, a client periodically sends a UPD broadcast (IP=255.255.255.255, port=ServerQueryPort from Drakan.cfg) 
containing only the payload `\status\`.

Any server receiving this responds directly to the client's IP with a player list packet, containing a payload that is formatted as such:

```
\player_0\Zal\ping_0\0\frags_0\0\team_0\-1\queryid\1.2\final\
```

Once the client received that list, it knows there is a server at the packets source IP, and will periodically send `\status\`
messages directly to the server. The server will then respond with more detailed information, formated as such (linebreaks added for readability):

```
\gamename\drakan
\gamever\375/445
\location\0
\hostname\Server von Zal
\hostport\27045
\mapname\Hill of Cleanroom
\gametype\Nur Boden
\numplayers\1
\maxplayers\8
\gamemode\openplaying
\Dedicated Server?\No
\Punktebegrenzung\20
\Zeitbegrenzung\Kein Limit
\Network Speed\T1LAN
\Num Teams\0
\Freundliches Feuer\Nein
\password\0
\queryid\7.1
```

followed by the same player list as during discovery. Note how some keys/values are german because I was using the german release of Drakan.


Gameplay protocol
-----------------

Starting with the handshake, a different protocol over different ports is used. The respective ports are the ones specified as
"ServerGamePort" and "Client_GamePort" in the Drakan.cfg.

The first thing that happens is the client opening a TCP session. After that, TCP packets are sometimes sent, but most are still UDP.

Packet structure varies, except for a two-field header which seems to be present for all packets. It looks like this:

```
uint8   packet_type    // possibly? denotes different packet contents, seems to increase steadily as connection progresses
uint16  packet_length  // including type and this length field (so exactly equal to UDP/TCP payload size)
```

Other fields depend on the packet type. See section 'Packet types' for a list of types and speculative field definitions.


### Handshake

The handshake is initiated by the client opening a TCP session with the server. After the TCP handshake the server sends a
packet with type ID 0x07. No idea what the fields in that packet mean, though.


### Extended header

It seems that starting with packet type 0x0e, packets consistently contain an "extended header" after the length field, formatted like so:

```
struct ext_header
{
    uint8   source           // 0x01 = server, 0x02.. = clients, 0x00 = broadcast(?)
    uint8   destination      //    "
    uint8   sequence_number
    uint32  unknown
}
```

The sequence number grows steadily per packet source, counting all packets sent by that participant, 
not differentiating between TCP and UPD packets. It rolls over after 0xff.


### Packet types

* [0x07] (TCP) Handshake?
    * First thing send by the server after a client connects.
    * Possible that this is used to tell the client the servers ID and assign a client ID to it for use in the extended header.
    * ```
      uint8     server_id // ?
      uint8     assigned_client_id // ?
      uint8[6]  unknown
      ```
* [0x09] (TCP) ???
    * Send by the client in response to a 0x07 packet
    * `uint8[24] unknown`
* [0x0a] (TCP) ???
    * `uint8[12]  unknown`
* [0x0b] (TCP) Sign on?
    * First packet to contain a client's player name 
* [0x2a] (TCP/UDP) Compound/Wrapper
    * This packet type seems to wrap another packet. It is the main packet type sent during gameplay.
    * The sequence number seems to grow by 2 after this packet, probably because of the packet it wraps. 
    * ```
      ext_header  eheader
      uint16      payload_length  // length of the following data
      packet      subpacket       // including header and potentially and extended header
      ```
* [0x32] Chat message
    *  Appears wrapped in a 0x2a packet
    * ```
      ext_header eheader
      uint8[3]   unknown
      uint32     message_length  // length of the following data
      uint8[~]   message         // includes zero-terminator. seems to be always formatted like this: 
                                 //   "<player name><space><0x1a><space><space><message>"
      ```
