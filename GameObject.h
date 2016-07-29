#pragma  once

#include "Utility.h"
#include "Control.h"
#include "GameState.h"
#include "KinectDeclare.h"

class GameObject
{
public:
	HWND hwnd;
	GameState myGameState;
	GameMenu myGameMenu;
	GamePlaying myGamePlaying;
	GameResult myGameResult;

	/** Kinect **/
	//SensorObject myKinect;

	GameObject(HWND & hd);
	GameObject();
	void SetHWND(HWND & hd);
	HWND getHWND();

	void ChangeToState(GameState gs);
	void GameStart();
	void GameUpdate();
	void GameEnd();
	void OnWindowMessage(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);
	void ProcessDynamicGesture();
	friend VOID CALLBACK DynamicGesTimerProc(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);

};