
Notes on Drakan music research
==============================

Drakan's music uses the DirectMusic library. Music data is found in the Music.rrc file,
which is an SRSC file.

All records in that file use the type 0x401. Records with IDs 0x1 to 0xb are segment files,
records with IDs 0x100 to 0x10a are DLS files (instrument data). Those records contain a RIFF
header with all the information one would find in a DirectMusic .sgt or .dls file. Only
the file names are missing.

IDs in the range 0x1001 to 0x100b all contain only 4 bytes. These are simple int32 values which represent volume as pointed out in the riot_database_format.txt. (volume in millibels. 0 == full volume, -5000 == (basically) silent. Amplitude factor = 10^(volume / 2000).

Further investigation has shown that the structure for the Music.rrc is based on the following concept:
Each music entry consists of a segment, instrument and volume entry. 
Segment entries are running IDs from 1-255 (0x01 - 0xFF)
Instrument entries are running IDs starting from 256 (0x100 - 0x1FE)
Volume entries starting at 4097 (0x1001 - 0x10FF)

It can be seen that the most significant bit is used as range criterion and due to the fact that the segment files may only use the range 0x01 - 0xFF the other entries should also be limited down to a count of 255. 

The Drakan Level Editor then adresses each music entry by an ID which corresponds to the ID of the segment. The Volume and Instrument entries are combined according to their IDs. That way the first sgt gets combined with first instrument and first volume file, and so on.
