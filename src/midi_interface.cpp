
#include "stdafx.h"
#include "stdafx2.h"

// имена стандартных midi инструментов
const wchar_t * MidiInterface::PatchNames[GM_INSTRUMENTS_NUMBER] =
{
  L"000 Acoustic Grand Piano",  L"001 Bright Acoustic Piano",  L"002 Electric Grand Piano",  L"003 Honky-tonk Piano",
  L"004 Electric Piano 1",  L"005 Electric Piano 2",  L"006 Harpsichord",  L"007 Clavinet",  L"008 Celesta",  L"009 Glockenspiel",
  L"010 Music Box",  L"011 Vibraphone",  L"012 Marimba",  L"013 Xylophone",  L"014 Tubular Bells",  L"015 Dulcimer",
  L"016 Drawbar Organ",  L"017 Percussive Organ",  L"018 Rock Organ",  L"019 Church Organ",  L"020 Reed Organ",  L"021 Accordion",
  L"022 Harmonica",  L"023 Tango Accordion",  L"024 Acoustic Guitar (nylon)",  L"025 Acoustic Guitar (steel)",
  L"026 Electric Guitar (jazz)",  L"027 Electric Guitar (clean)",  L"028 Electric Guitar (muted)",  L"029 Overdriven Guitar",
  L"030 Distortion Guitar",  L"031 Guitar Harmonics",  L"032 Acoustic Bass",  L"033 Electric Bass (finger)",
  L"034 Electric Bass (pick)",  L"035 Fretless Bass",  L"036 Slap Bass 1",  L"037 Slap Bass 2",  L"038 Synth Bass 1",
  L"039 Synth Bass 2",  L"040 Violin",  L"041 Viola",  L"042 Cello",  L"043 Contrabass",  L"044 Tremolo Strings",
  L"045 Pizzicato Strings",  L"046 Orchestral Harp",  L"047 Timpani",  L"048 String Ensemble 1",  L"049 String Ensemble 2",
  L"050 Synth Strings 1",  L"051 Synth Strings 2",  L"052 Choir Aahs",  L"053 Voice Oohs",  L"054 Synth Voice",
  L"055 Orchestra Hit",  L"056 Trumpet",  L"057 Trombone",  L"058 Tuba",  L"059 Muted Trumpet",  L"060 French Horn",
  L"061 Brass Section",  L"062 Synth Brass 1",  L"063 Synth Brass 2",  L"064 Soprano Sax",  L"065 Alto Sax",  L"066 Tenor Sax",
  L"067 Baritone Sax",  L"068 Oboe",  L"069 English Horn",  L"070 Bassoon",  L"071 Clarinet",  L"072 Piccolo",  L"073 Flute",
  L"074 Recorder",  L"075 Pan Flute",  L"076 Bottle Blow",  L"077 Shakuhachi",  L"078 Whistle",  L"079 Ocarina",
  L"080 Lead 1 (square)",  L"081 Lead 2 (sawtooth)",  L"082 Lead 3 (calliope)",  L"083 Lead 4 (chiff)",  L"084 Lead 5 (charang)",
  L"085 Lead 6 (voice)",  L"086 Lead 7 (fifths)",  L"087 Lead 8 (bass + lead)",  L"088 Pad 1 (new age)",  L"089 Pad 2 (warm)",
  L"090 Pad 3 (polysynth)",  L"091 Pad 4 (choir)",  L"092 Pad 5 (bowed)",  L"093 Pad 6 (metallic)",  L"094 Pad 7 (halo)",
  L"095 Pad 8 (sweep)",  L"096 FX 1 (rain)",  L"097 FX 2 (soundtrack)",  L"098 FX 3 (crystal)",  L"099 FX 4 (atmosphere)",
  L"100 FX 5 (brightness)",  L"101 FX 6 (goblins)",  L"102 FX 7 (echoes)",  L"103 FX 8 (sci-fi)",  L"104 Sitar",  L"105 Banjo",
  L"106 Shamisen",  L"107 Koto",  L"108 Kalimba",  L"109 Bagpipe",  L"110 Fiddle",  L"111 Shanai",  L"112 Tinkle Bell",
  L"113 Agogo",  L"114 Steel Drums",  L"115 Woodblock",  L"116 Taiko Drum",  L"117 Melodic Tom",  L"118 Synth Drum",
  L"119 Reverse Cymbal",  L"120 Guitar Fret Noise",  L"121 Breath Noise",  L"122 Seashore",  L"123 Bird Tweet",
  L"124 Telephone Ring",  L"125 Helicopter",  L"126 Applause",  L"127 Gunshot",
};

