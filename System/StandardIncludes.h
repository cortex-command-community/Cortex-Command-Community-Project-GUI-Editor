#ifndef _RTEINCLUDES_
#define _RTEINCLUDES_

// Disable a bunch of unneeded crap in WinAPI (on top of WIN32_LEAN_AND_MEAN)
#ifdef _WIN32
#define NOMINMAX
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOMETAFILE
#define NOSCROLL
#define NOTEXTMETRIC
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NODRAWTEXT
#define NOCOLOR
#define NOMSG
#define NOOPENFILE
#define NOSERVICE
#define NOSOUND
#define NOWH
#define NOWINOFFSETS
#endif

// Inclusion of relevant C++ Standard Library headers.
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <iostream>
#include <sstream>
#include <fstream>
#include <istream>
#include <cmath>
#include <cassert>
#include <limits>
#include <array>
#include <filesystem>

// TODO: Get rid of these once alias qualifiers are added.
using std::string;
using std::list;
using std::pair;
using std::deque;
using std::map;
using std::set;
using std::vector;
using std::ios_base;
using std::array;
using std::make_pair;
using std::min;
using std::max;

#endif
