/*
 *  Copyleft (C) 2018, Pavel Masny
 *  This file is part of the TGA SDK (TGASdk).
 */

#pragma once

#ifndef TGA_EXPORT
#if defined(_WINDLL) || defined(TGASDK_BUILD_DLL)
#define TGA_EXPORT __declspec(dllexport)
#elif defined(TGASDK_USE_DLL)
#define TGA_EXPORT __declspec(dllimport)
#else
#define TGA_EXPORT
#endif
#endif
#define MPV_TGA_VER_MAJOR   1
#define MPV_TGA_VER_MINOR   0
#define MPV_TGA_VER_PATCH   0
#define MPV_TGA_VER_STRING  "1.0.0"