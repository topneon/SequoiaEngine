// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#define DEBUG
#define RENDERER_METHOD 0x11
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <memory>
#include <ppltasks.h>
#if RENDERER_METHOD == 0x11
#pragma comment(lib, "d3d11")
#include <d3d11_4.h>
#endif
#pragma comment(lib, "DXGI")
#include <DirectXMath.h>
#include <dxgi.h>
#include <thread>
#include <chrono>
#include <optional>
#include <d3dcompiler.h>