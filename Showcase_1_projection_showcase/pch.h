#ifndef PCH_H
#define PCH_H

#include <sstream>
#include <vector>
#include <stdio.h>
#include <memory>
#include <cassert>
#include <stdexcept>

#define COLOR_BLACK 30
#define BG_COLOR_BLACK 40
#define COLOR_RED 31
#define BG_COLOR_RED 41
#define COLOR_GREEN 32
#define BG_COLOR_GREEN 42
#define COLOR_YELLOW 33
#define BG_COLOR_YELLOW 43
#define COLOR_BLUE 34
#define BG_COLOR_BLUE 44
#define COLOR_MAGENTA 35
#define BG_COLOR_MAGENTA 45
#define COLOR_CYAN 36
#define BG_COLOR_CYAN 46
#define COLOR_WHITE 37
#define BG_COLOR_WHITE 47
#define COLOR_BRIGHTBLACK 90
#define BG_COLOR_BRIGHTBLACK 100
#define COLOR_BRIGHTRED 91
#define BG_COLOR_BRIGHTRED 101
#define COLOR_BRIGHTGREEN 92
#define BG_COLOR_BRIGHTGREEN 102
#define COLOR_BRIGHTYELLOW 93
#define BG_COLOR_BRIGHTYELLOW 103
#define COLOR_BRIGHTBLUE 94
#define BG_COLOR_BRIGHTBLUE 104
#define COLOR_BRIGHTMAGENTA 95
#define BG_COLOR_BRIGHTMAGENTA 105
#define COLOR_BRIGHTCYAN 96
#define BG_COLOR_BRIGHTCYAN 106
#define COLOR_BRIGHTWHITE 97
#define BG_COLOR_BRIGHTWHITE 107

#define ERRORCOL COLOR_RED
#define WARNCOL COLOR_YELLOW
#define INFOCOL COLOR_BRIGHTBLUE
#define DEFAULTCOL COLOR_WHITE
#define ADDITIONALCOL COLOR_MAGENTA

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define printWColor(x, c) {\
std::stringstream ss;\
ss << x << '\n';\
printf("\x1B[" TOSTRING(c) "m%s\033[0m", ss.str().c_str());};
#define printWbgColor(x, c, bg) {\
std::stringstream ss; \
ss << x << '\n'; \
printf("\033[3;" TOSTRING(bg) ";" TOSTRING(c) "m%s\033[0m", x.str().c_str()); }


#ifdef DEBUGADDITIONAL

#define DebugPrint(x) printWColor(x, ADDITIONALCOL)

#else

#define DebugPrint(x)

#endif // DEBUGADDITIONAL
#endif // PCH_H

