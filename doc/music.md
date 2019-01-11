
Notes on Drakan music research
==============================

Drakan's music uses the DirectMusic library. Music data is found in the Music.rrc file,
which is an SRSC file.

All records in that file use the type 0x401. Records with IDs 0x1 to 0xb are segment files,
records with IDs 0x100 to 0x10a are DLS files (instrument data). Those records contain a RIFF
header with all the information one would find in a DirectMusic .sgt or .dls file. Only
the file names are missing.

IDs in the range 0x1001 to 0x100b all contain only 4 bytes. These are simple int32 values which represent volume as pointed out in the riot_database_format.txt. (volume in millibels. 0 == full volume, -5000 == (basically) silent. Amplitude factor = 10^(volume / 2000)


