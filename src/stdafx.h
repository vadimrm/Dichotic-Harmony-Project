/*
  stdafx.h : include file for standard system include files,
  or project specific include files that are used frequently, but
  are changed infrequently
*/

#pragma once

#ifndef UNICODE
#define UNICODE // надо для GW
#endif

// #define _WIN32_WINNT 0x0400 // need for WM_MOUSEWHEEL
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commdlg.h>
#include <commctrl.h>

// для midi надо прилинковать winmm.lib и включить этот хедер:
#pragma warning (disable: 4201) // nonstandard extension used: nameless struct/union
#include <mmsystem.h>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <stdlib.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <typeinfo>

using namespace std;

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

