#include "GameObject.h"

/************************************************************************/
/*		Dynamic Gesture TimerProcess */
/************************************************************************/

// constructor
GameObject::GameObject( HWND & hd )
{
	this->hwnd = hd;
}

GameObject::GameObject()
{
}

// set window handle
void GameObject::SetHWND( HWND & hd )
{
	this->hwnd = hd;
}


HWND GameObject::getHWND()
{
	return this->hwnd;
}

// change game state
void GameObject::ChangeToState( GameState gs )
{
	// 1. clear info
	switch(myGameState)
	{
	case GS_Menu:
		myGameMenu.ClearBeforeLeave();
		break;
	case GS_Playing:
		myGamePlaying.ClearBeforeLeave();
		break;
	case GS_Result:
		myGameResult.ClearBeforeLeave();
		break;
	}
	// start
	myGameState = gs;
	switch(gs)
	{
	case GS_Menu:
		myGameMenu.Start();
		break;
	case GS_Playing:
		myGamePlaying.Start();
		break;
	case GS_Result:
		myGameResult.Start();
		break;
	}
}

// game start
void GameObject::GameStart()
{
	// create DC
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	HBITMAP whiteBmp = CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT);
	SelectObject(g_mdc,whiteBmp);
	g_bufdc = CreateCompatibleDC(g_hdc);

	// hwnd
	myGameMenu.setHWND(hwnd);
	myGamePlaying.setHWND(hwnd);
	myGameResult.setHWND(hwnd);

	// Initialization
	myGameMenu.Init();
	myGamePlaying.Init();
	myGameResult.Init();

	// set the initial state--- gameMenu
	myGameState = GS_Menu;
	myGameMenu.Start();
	//myGameState = GS_Result;
	//myGameResult.Start();
	//myGameState = GS_Playing;
	//myGamePlaying.Start();
}

// game update
// state change: 1) kinect event. in GameUpdate  2) mouse event . in OnWindowMessage
void GameObject::GameUpdate()
{
	// update each game state
	switch (myGameState)
	{
	case GS_Menu:
		myGameMenu.Update();
		if (myGameMenu.isGo2Play)
		{
			ChangeToState(GS_Playing);
		}
		break;
	case GS_Playing:
		myGamePlaying.Update();
		if (myGamePlaying.isGameOver)
		{
			ChangeToState(GS_Result);
		}
		break;
	case GS_Result:
		myGameResult.Update();
		if (myGameResult.isGo2Play)
		{
			ChangeToState(GS_Playing);
		}
		break;
	}
}

// game end
void GameObject::GameEnd()
{
	// state
	myGameMenu.Destroy();
	myGamePlaying.Destroy();
	myGameResult.Destroy();

	// release the DC
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd,g_hdc);
}

void GameObject::OnWindowMessage( HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam )
{
	switch(myGameState)
	{
	case GS_Menu:
		switch(message)
		{
		case WM_LBUTTONUP:
			ChangeToState(GS_Playing);
			break;
		default:
			break;
		}
		break;
	case GS_Playing:
		switch(message)
		{
		case WM_KINECT_GESTURE_TRIGGER:
			
			switch( (int) wParam )
			{
			case GAME_GESTURE_LEFT_MOVE:
				myGamePlaying.iPlayerPositionX -= HERO_MOVE_SPEED;
				if (myGamePlaying.iPlayerPositionX < 0)
				{
					myGamePlaying.iPlayerPositionX = 0;
				}
				break;
			case GAME_GESTURE_RIGHT_MOVE:
				myGamePlaying.iPlayerPositionX += HERO_MOVE_SPEED;
				if (myGamePlaying.iPlayerPositionX > WINDOW_WIDTH - HERO_SIZE_WIDTH)
				{
					myGamePlaying.iPlayerPositionX = WINDOW_WIDTH - HERO_SIZE_WIDTH;
				}
				break;
			case GAME_GESTURE_UP_MOVE:
				myGamePlaying.iPlayerPositionY -= HERO_MOVE_SPEED;
				if (myGamePlaying.iPlayerPositionY < 0)
				{
					myGamePlaying.iPlayerPositionY = 0;
				}
				break;
			case GAME_GESTURE_DOWN_MOVE:
				myGamePlaying.iPlayerPositionY += HERO_MOVE_SPEED;
				if (myGamePlaying.iPlayerPositionY > WINDOW_HEIGHT - HERO_SIZE_HEIGHT)
				{
					myGamePlaying.iPlayerPositionY = WINDOW_HEIGHT - HERO_SIZE_HEIGHT;
				}
				break;
			case GAME_GESTURE_GIVE_UP:
				//::SendMessage(this->hwnd,WM_CLOSE,0,0);
				myGamePlaying.isGiveUp = true;
				break;
			case GAME_GESTURE_ALL_KILL_1:
				if (!myGamePlaying.isAllKilledTemp1)
				{
					myGamePlaying.isAllKilledTemp1 = true;
					SetTimer(this->hwnd,GAME_DYNAMIC_GESTURE_TIMER,1200,NULL);
				}
				break;
			case GAME_GESTURE_ALL_KILL_2:
				if (myGamePlaying.isAllKilledTemp1)
				{
					myGamePlaying.isAllKilledTemp1 = false;
					myGamePlaying.isAllKilled = true;
				}
				break;
			default:
				break;			
			}
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 0x41:
				case VK_LEFT:
					myGamePlaying.iPlayerPositionX -= HERO_MOVE_SPEED;
					if (myGamePlaying.iPlayerPositionX < 0)
					{
						myGamePlaying.iPlayerPositionX = 0;
					}
					break;
				
				case  0x44:
				case VK_RIGHT:
					myGamePlaying.iPlayerPositionX += HERO_MOVE_SPEED;
					if (myGamePlaying.iPlayerPositionX > WINDOW_WIDTH - HERO_SIZE_WIDTH)
					{
						myGamePlaying.iPlayerPositionX = WINDOW_WIDTH - HERO_SIZE_WIDTH;
					}
					break;

				case 0x57:
				case VK_UP:
					myGamePlaying.iPlayerPositionY -= HERO_MOVE_SPEED;
					if (myGamePlaying.iPlayerPositionY < 0)
					{
						myGamePlaying.iPlayerPositionY = 0;
					}
					break;

				case 0x53:
				case VK_DOWN:
					myGamePlaying.iPlayerPositionY += HERO_MOVE_SPEED;
					if (myGamePlaying.iPlayerPositionY > WINDOW_HEIGHT - HERO_SIZE_HEIGHT)
					{
						myGamePlaying.iPlayerPositionY = WINDOW_HEIGHT - HERO_SIZE_HEIGHT;
					}
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			myGamePlaying.bIsMouseDown = true;
			GetCursorPos(&(myGamePlaying.pPreMousePoint));
			break;
		case WM_LBUTTONUP:
			myGamePlaying.bIsMouseDown = false;
			break;
		default:
			break;
		}
		break;
	case GS_Result:
		switch(message)
		{
		case WM_LBUTTONUP:
			ChangeToState(GS_Playing);
			break;
		default:
			break;
		}
		break;
	}
}

void GameObject::ProcessDynamicGesture()
{
	myGamePlaying.isAllKilledTemp1 = false;
	KillTimer(hwnd,GAME_DYNAMIC_GESTURE_TIMER);
}