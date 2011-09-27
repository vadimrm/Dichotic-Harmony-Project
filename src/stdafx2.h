/*
  stdafx2.h здесь заголовки, которые иногда меняются
*/

#pragma once

/*
#pragma warning (disable:  4003 ) // not enough args for macro
#pragma warning (disable:  4102 ) // unreferenced label
#pragma warning (disable:  4115 ) // named type definition in parentheses
#pragma warning (disable:  4127 ) // cond exp is const
#pragma warning (disable:  4214 ) // nonstandard extension used : bit field types other than int
#pragma warning (disable:  4309 ) // 'initializing' truncation of constant value
#pragma warning (disable:  4514 ) // unreferenced inline function has been removed
#pragma warning (disable:  4706 ) // assignment within conditional expression
*/

#pragma warning (disable:  4100 ) // unreferenced formal parameter
#pragma warning (disable:  4996 ) // 'itoa' was declared deprecated

// #define DDD // для дебаг-принта

#include "unicode.h"
#include "tools.h"
#include "ar.h"
#include "random.h"
#include "files.h"
#include "winfile.h"
#include "win_kb.h"
#include "win_tools.h"

#include "midi_interface.h"

#include "resource.h"
#include "amain.h"
#include "win.h"

#include "dichotic.h"
#include "music_files.h"
#include "music.h"

