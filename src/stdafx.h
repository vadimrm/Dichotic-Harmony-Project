/*
  stdafx.h здесь все заголовки, которые никогда не меняются
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

