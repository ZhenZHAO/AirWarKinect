#pragma once

#include <Windows.h>
#include <iostream>
#include <nuiapi.h>
#include "KinectSensor.h"
#include <opencv2/opencv.hpp> 
/************************************************************************/
/*  0. user-defined Message  */
/************************************************************************/
#define WM_KINECT_GESTURE_BEGIN		(WM_USER + 1)
#define WM_KINECT_GESTURE_TRIGGER	(WM_USER + 2)
//#define WM_KINECT_SPEECH_STOP		(WM_USER + 3)
//#define WM_KINECT_SPEECH_RESUME		(WM_USER + 4)

#define GAME_GESTURE_LEFT_MOVE		1
#define GAME_GESTURE_RIGHT_MOVE		2
#define GAME_GESTURE_UP_MOVE		3
#define GAME_GESTURE_DOWN_MOVE		4
#define GAME_GESTURE_GIVE_UP		5
#define GAME_GESTURE_ALL_KILL_1		6
#define GAME_GESTURE_ALL_KILL_2		7
#define GAME_DYNAMIC_GESTURE_TIMER	(WM_USER + 5)

#define GAME_GESTURE_CONTROL_SPEED	120		// frame speed

/************************************************************************/
/*	1. Game Section */
/************************************************************************/
#define WINDOW_TITLE L"Air War Game"
#define USER_FILE_NAME "ScoreInfo.txt"
#define GAME_GESTURE_FILE_NAME "MoveDesign.txt"

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 852
#define SKELETON_WINDOW_WIDTH 400
#define SKELETON_WINDOW_HEIGHT 426
#define PI	3.1415926

#define UPDATE_INTERVAL 20 // MS
#define DYNAMIC_BG_MOVE_SPEED 5

#define ENEMY_SIZE_WIDTH 51
#define ENEMY_SIZE_HEIGHT 39
#define ENEMY_DOWN_SPEED 2

#define HERO_SIZE_WIDTH 100
#define HERO_SIZE_HEIGHT 124
#define HERO_MOVE_SPEED 20

#define BULLET_SIZE_WIDTH 9
#define BULLET_SIZE_HEIGHT 21
#define BULLET_SPEED  20

enum GameState{
	GS_Menu,
	GS_Playing,
	GS_Result
};

class Bullet
{
public:
	int x;
	int y;
	bool isExist;

	Bullet()
	{
		x = 0;
		y = 0;
		isExist = false;
	}
};

class Enemy
{
public:
	int x;
	int y;
	bool isExist;
	bool isDie;
	int iDieAnimationIndex;
	int iDieAnimationTimer;

	Enemy()
	{
		x = 0; y = 0;
		isExist = false;
		isDie = false;
		iDieAnimationIndex = 0;
		iDieAnimationTimer = 0;
	}
};

/************************************************************************/
/*  global variables */
/************************************************************************/

// Global variables
extern HDC g_hdc;
extern HDC g_mdc;
extern HDC g_bufdc;

// Score Information
extern int iScore;
extern int iHighScore;


/************************************************************************/
/*  draw Info                                                                     */
/************************************************************************/

