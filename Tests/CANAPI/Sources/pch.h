//
// pch.h
// Header for standard system include files.
//
#ifndef PRECOMPILED_HEADERS_INCLUDED
#define PRECOMPILED_HEADERS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif
#ifdef _MSC_VER
//no Microsoft extensions please!
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "gtest/gtest.h"
#include "Config.h"
#include "Driver.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif
#include "Settings.h"
#include "Options.h"
#include "Device.h"
#include "Bitrates.h"
#include "Properties.h"
#include "Progress.h"
#include "Timer64.h"
#include "Version.h"

#endif // PRECOMPILED_HEADERS_INCLUDED
