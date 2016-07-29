#pragma  once

#include "Utility.h"
#include <iostream>
#include "Control.h"
#include "KinectDeclare.h"

/************************************************************************/
/* Idea: 1)Init 2) start 3)Update 4)destroy */
/************************************************************************/
class AB_GameState
{
public:
	HWND hwnd;
	virtual void setHWND(HWND & hd) = 0;
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;
	virtual void ClearBeforeLeave() = 0;

};


class GameMenu : public AB_GameState
{
public:
	HBITMAP hBackground;
	HBITMAP hTitle;
	HBITMAP hGameLoading;
	HBITMAP hHand;

	GameMenu(){angleTimer = 0; isGo2Play = false; isHandOver = false; handX = 20; handY = 600;}
	
	int angleTimer;
	bool isHandOver;
	bool isGo2Play;
	int handX;
	int handY;
	bool IsIncludeStartButton(int x, int y);

	void setHWND(HWND & hd);
	void Init();
	void Start();
	void Update();
	void Destroy();
	void ClearBeforeLeave();
};



class GamePlaying : public AB_GameState
{
public:
	HBITMAP hBackground;
	int iBackgroundOffset;

	HBITMAP hHeroArray[2];
	int iHeroIndex;
	int iHeroTimer;

	int iPlayerPositionX;
	int iPlayerPositionY;

	bool bIsMouseDown;
	POINT pPreMousePoint;

	Bullet bulletArray[30];
	HBITMAP hBullet;
	int iBulletTimer;

	Enemy enemyArray[30];
	int iEnemySpawnTimer;
	HBITMAP hEnemyArray[5];

	// game over flag
	bool isGameOver;
	bool isGiveUp;
	bool isAllKilled;
	bool isAllKilledTemp1;

	GamePlaying();
	void setHWND(HWND  & hd);
	void Init();
	void Start();
	void Update();
	void Destroy();

	bool IsCollisionEnemyBullet(Enemy emy, Bullet blt);
	bool IsCollisionEnemyHero(Enemy emy);
	bool IsInclude(Enemy emy,int x, int y);

	void BeginPlayMusic();
	void StopMusic();
	void RePlayMusic(HWND & hd);
	void PauseMusic(HWND & hd);
	void ResumeMusic(HWND & hd);
	
	void ClearBeforeLeave();
};

class GameResult : public AB_GameState
{
public:
	HBITMAP hGameOver;
	HBITMAP hHand;
	HBITMAP hRestart;
	
	int angleTimer;
	bool isHandOver;
	bool isGo2Play;
	int handX;
	int handY;
	bool IsIncludeStartButton(int x, int y);

	wchar_t scoreText[10];
	wchar_t scoreHighText[100];
	
	GameResult();
	void setHWND(HWND  & hd);

	void Init();
	void Start();
	void Update();
	void Destroy();

	void ClearBeforeLeave();
};


