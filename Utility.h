#pragma once

#include <Windows.h>
#include <time.h>
#include <tchar.h>
#include <mmstream.h> // MCI audio

//importing lib
#pragma comment(lib,"Msimg32.lib")
#pragma comment(lib,"winmm.lib") // MCI audio

/************************************************************************/
/*	 algorithm section */
/************************************************************************/
#define CLOSING_FROM_STATUSCHANGED    1
#define READING_TEXT_BUFFER_SIZE    128

// command ID
	// color
#define ID_COLORSTREAM                  40001
#define ID_COLORSTREAM_PAUSE            40002
#define ID_COLORSTREAM_RESOLUTION       40003
#define ID_COLORSTREAM_RESOLUTION_START 40004
#define ID_RESOLUTION_RGBRESOLUTION640X480FPS30 40004
#define ID_RESOLUTION_RGBRESOLUTION1280X960FPS12 40005
#define ID_RESOLUTION_YUVRESOLUTION640X480FPS15 40006
#define ID_RESOLUTION_INFRAREDRESOLUTION640X480FPS30 40007
#define ID_RESOLUTION_RAWBAYERRESOLUTION640X480FPS30 40008
#define ID_RESOLUTION_RAWBAYERRESOLUTION1280X960FPS12 40009
#define ID_COLORSTREAM_RESOLUTION_END   40009

#define ID_COLORSTREAM_SHOW_SKELETON    40010
	// skeleton
#define ID_SKELETONSTREAM               40023
#define ID_SKELETONSTREAM_PAUSE         40024
#define ID_SKELETONSTREAM_TRACKINGMODE  40025
#define ID_SKELETONSTREAM_TRACKINGMODE_START 40026
#define ID_TRACKINGMODE_DEFAULT         40026
#define ID_TRACKINGMODE_SEATED          40027
#define ID_SKELETONSTREAM_TRACKINGMODE_END 40027
#define ID_SKELETONSTREAM_CHOOSERMODE   40028
#define ID_SKELETONSTREAM_CHOOSERMODE_START 40029
#define ID_CHOOSERMODE_DEFAULTSYSTEMTRACKING 40029
#define ID_CHOOSERMODE_CLOSEST1PLAYER   40030
#define ID_CHOOSERMODE_CLOSEST2PLAYER   40031
#define ID_CHOOSERMODE_STICKY1PLAYER    40032
#define ID_CHOOSERMODE_STICKY2PLAYER    40033
#define ID_CHOOSERMODE_MOSTACTIVE1PLAYER 40034
#define ID_CHOOSERMODE_MOSTACTIVE2PLAYER 40035
#define ID_SKELETONSTREAM_CHOOSERMODE_END 40035
#define ID_KINECT_CLOSE					 40036

#define ID_FORCE_OFF_IR					 40100

//algorithm 
#define ID_ALGORITHM_DIFFICULTY_CHOOSE_EASY  40037
#define ID_ALGORITHM_DIFFICULTY_CHOOSE_NORMAL 40038
#define ID_ALGORITHM_DIFFICULTY_CHOOSE_HARD	 40039
#define ID_ALGORITHM_CLEAR_COMPLETENESS_TIMES 40040


// struct
//enum DEPTH_TREATMENT

// Vector3D
#ifndef _VECTOR_3D_
#define _VECTOR_3D_
typedef
struct	_Vector_3D
{
	double	x;
	double	y;
	double	z;
}Vector3D;
#endif _VECTOR_3D_


// Ensure the font object has been created correctly
inline void EnsureFontCreated(HFONT& hFont, int fontSize, int fontWeight)
{
	if (nullptr == hFont)
	{
		hFont = CreateFontW(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE, 
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
	}
}


// Ensure the image object has been loaded correctly
inline void EnsureImageLoaded(HBITMAP& hBitmap, UINT resourceId)
{
	if (nullptr == hBitmap)
	{
		hBitmap = LoadBitmapW(GetModuleHandle(NULL), MAKEINTRESOURCEW(resourceId));
	}
}


// Ensure the icon object has been loaded correctly
inline void EnsureIconLoaded(HICON& hIcon, UINT resourceId)
{
	if (nullptr == hIcon)
	{
		hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCEW(resourceId));
	}
}

inline SIZE GetWindowSize(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);

	SIZE size = {rect.right - rect.left, rect.bottom - rect.top};
	return size;
}

inline SIZE GetClientSize(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	SIZE size = {rect.right, rect.bottom};
	return size;
}

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface*& pInterfaceToRelease)
{
	if (pInterfaceToRelease)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = nullptr;
	}
}

// Safe delete pointers
template<class T>
inline void SafeDelete(T*& ptr)
{
	if(ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}

// Safe delete array
template<class T>
inline void SafeDeleteArray(T*& pArray)
{
	if(pArray)
	{
		delete[] pArray;
		pArray = nullptr;
	}
}