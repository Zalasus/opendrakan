
Notes on Drakan music research
==============================

Drakan's music uses the DirectMusic library. Music data is found in the Music.rrc file,
which is an SRSC file.

All records in that file use the type 0x401. Records with IDs 0x1 to 0xb are segment files,
records with IDs 0x100 to 0x10a are DLS files (instrument data). Those records contain a RIFF
header with all the information one would find in a DirectMusic .sgt or .dls file. Only
the file names are missing.

IDs in the range 0x1001 to 0x100b all contain only 4 bytes, the meaning of which is unclear.


