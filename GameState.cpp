#include "GameState.h"

// class GameMenu

void GameMenu::setHWND( HWND & hd )
{
		this->hwnd = hd;
}

void GameMenu::Init()
{
	hBackground = (HBITMAP) LoadImage(NULL,L"image/background.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);
	hTitle = (HBITMAP) LoadImage(NULL,L"image/title.bmp",IMAGE_BITMAP,429,84,LR_LOADFROMFILE);
	hGameLoading = (HBITMAP) LoadImage(NULL,L"image/start.bmp",IMAGE_BITMAP,140,140,LR_LOADFROMFILE);
	hHand = (HBITMAP)LoadImage(NULL,L"image/hand.bmp",IMAGE_BITMAP,100,80,LR_LOADFROMFILE);
}

void GameMenu::Start()
{
	angleTimer = 0; // max = 120
	isGo2Play = false;
	handX = 100;
	handY = 600;
	isHandOver = false;
}

// start button: (170,450) + 140*140
bool GameMenu::IsIncludeStartButton( int x, int y )
{
	int checkX = x;
	int checkY = y;
	if (checkX > 170 && checkX < 310 && checkY > 450 && checkY< 590)
	{
		return true;
	}
	checkX = x + 100; checkY = y;
	if (checkX > 170 && checkX < 310 && checkY > 450 && checkY< 590)
	{
		return true;
	}
	checkX = x ; checkY = y+80;
	if (checkX > 170 && checkX < 310 && checkY > 450 && checkY< 590)
	{
		return true;
	}
	checkX = x + 100; checkY = y+80;
	if (checkX > 170 && checkX < 310 && checkY > 450 && checkY< 590)
	{
		return true;
	}
	return false;
}

void GameMenu::Update()
{
	SelectObject(g_bufdc,hBackground);
	BitBlt(g_mdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);
	SelectObject(g_bufdc,hTitle);
	TransparentBlt(g_mdc,20,100,429,84,g_bufdc,0,0,429,84,RGB(0,0,0));
	SelectObject(g_bufdc,hGameLoading);
	TransparentBlt(g_mdc,170,450,140,140,g_bufdc,0,0,140,140,RGB(0,0,0));
	
	// hand position
	if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect() && myKinect.isSkeletonFoundOK() )
	//if (isPlayWithKinect && myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect() )
	{
		POINT handNow = myKinect.getSkeDataOnScreen(NUI_SKELETON_POSITION_HAND_RIGHT);
		handX = (int)handNow.x - 50;
		handY = (int)handNow.y - 40;
		//handX +=10; handY -= 10;
	}//otherwise, keep still

	// draw hand
	//SelectObject(g_bufdc,hHand);
	//TransparentBlt(g_mdc,handX,handY,100,80,g_bufdc,0,0,100,80,RGB(0,0,0));
	//BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);
	
	// Hand & Start collision
	// start button: (170,450) + 140*140
	if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect() && myKinect.isSkeletonFoundOK() )
	{
		if (IsIncludeStartButton(handX,handY))
		{
			isHandOver = true;
		}else{
			isHandOver = false;
		}
	}else{
		isHandOver = false;
	}
	
	// draw start button dynamic showing
	if (isHandOver)
	{
		angleTimer += 2;
	}
	else
	{
		angleTimer = 0;
	}

	if (angleTimer >= 120 )
	{
		this->isGo2Play = true;
		angleTimer = 120;
	}
	// http://www.169it.com/tech-cpp/article-8232484040262880964.html
	// startImage (170,450) r = 65 , rec.width = 140
	if (angleTimer > 0 && angleTimer <= 120)
	{
		{
			//HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); // http://www.yansedaquan.com/
			int iRed = (angleTimer +100) % 255;
			int iGreen = (255 - angleTimer) % 255;
			int iBlue = (250)%255;
			HBRUSH hBrush =CreateSolidBrush(RGB(iRed,iGreen,iBlue));
				//SolidBrush hBrush(Color(128, GetRValue(clrMask), GetGValue(clrMask), GetBValue(clrMask)));
			SelectObject(g_mdc,hBrush);
			SetBkMode(g_mdc,TRANSPARENT);

			int ridius = 50 * angleTimer / 120;
			int cirLeft = 240 - ridius;
			int cirTop = 520 - ridius;
			int cirRight = 240 + ridius;
			int cirBottom = 520 + ridius;
			//Ellipse(g_hdc,20,20,ridius,ridius);
			Ellipse(g_mdc,cirLeft,cirTop,cirRight,cirBottom);
			DeleteObject(hBrush);
		}
	}

	SelectObject(g_bufdc,hHand);
	TransparentBlt(g_mdc,handX,handY,100,80,g_bufdc,0,0,100,80,RGB(0,0,0));
	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);
}

void GameMenu::Destroy()
{

}

void GameMenu::ClearBeforeLeave()
{
	angleTimer = 0; isGo2Play = false; isHandOver = false; handX = 20; handY = 600;
}

// class GamePlaying

GamePlaying::GamePlaying()
{
	iBackgroundOffset = 0;
	iHeroIndex = 0;
	iHeroTimer = 0;

	iPlayerPositionX = 190;
	iPlayerPositionY = 600;

	bIsMouseDown = false;

	iBulletTimer = 0;
	iEnemySpawnTimer = 0;

	isGameOver	= false;
	isGiveUp	= false;
	isAllKilled = false;
}

void GamePlaying::setHWND( HWND & hd )
{
		this->hwnd = hd;
}

void GamePlaying::Init()
{
	hBackground = (HBITMAP) LoadImage(NULL,L"image/background.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);
	hHeroArray[0] = (HBITMAP) LoadImage(NULL,L"image/hero1.bmp",IMAGE_BITMAP,HERO_SIZE_WIDTH,HERO_SIZE_HEIGHT,LR_LOADFROMFILE);
	hHeroArray[1] = (HBITMAP) LoadImage(NULL,L"image/hero2.bmp",IMAGE_BITMAP,HERO_SIZE_WIDTH,HERO_SIZE_HEIGHT,LR_LOADFROMFILE);
	hBullet = (HBITMAP) LoadImage(NULL,L"image/bullet1.bmp",IMAGE_BITMAP,BULLET_SIZE_WIDTH,BULLET_SIZE_HEIGHT,LR_LOADFROMFILE);
	hEnemyArray[0] = (HBITMAP) LoadImage(NULL,L"image/enemy0.bmp",IMAGE_BITMAP,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,LR_LOADFROMFILE);
	hEnemyArray[1] = (HBITMAP) LoadImage(NULL,L"image/enemy1.bmp",IMAGE_BITMAP,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,LR_LOADFROMFILE);
	hEnemyArray[2] = (HBITMAP) LoadImage(NULL,L"image/enemy2.bmp",IMAGE_BITMAP,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,LR_LOADFROMFILE);
	hEnemyArray[3] = (HBITMAP) LoadImage(NULL,L"image/enemy3.bmp",IMAGE_BITMAP,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,LR_LOADFROMFILE);
	hEnemyArray[4] = (HBITMAP) LoadImage(NULL,L"image/enemy4.bmp",IMAGE_BITMAP,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,LR_LOADFROMFILE);
	
	// set the random seed
	srand((unsigned)time(NULL));
}

void GamePlaying::Start()
{
	// every time go to this page , clear the score
	iScore = 0;
	// open the gesture Recognition thread
	SendMessage(hwnd,WM_KINECT_GESTURE_BEGIN,0,0);

	// play music
	BeginPlayMusic();
}

void GamePlaying::Update()
{
	// background -- dynamic background: just use two same graph
	SelectObject(g_bufdc,hBackground);
	iBackgroundOffset += DYNAMIC_BG_MOVE_SPEED;
	if (iBackgroundOffset > WINDOW_HEIGHT)
	{
		iBackgroundOffset -=WINDOW_HEIGHT;
	}
	BitBlt(g_mdc,0,iBackgroundOffset,WINDOW_WIDTH,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);
	BitBlt(g_mdc,0,iBackgroundOffset-WINDOW_HEIGHT,WINDOW_WIDTH,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);

	// Hero 1: -- show frame animation
	iHeroTimer ++;
	if (iHeroTimer %5 ==0)
	{
		iHeroIndex++;
		iHeroIndex %= 2;

	}
	SelectObject(g_bufdc,hHeroArray[iHeroIndex]);
		//remove the white background
	TransparentBlt(g_mdc,iPlayerPositionX,iPlayerPositionY,HERO_SIZE_WIDTH,HERO_SIZE_HEIGHT,g_bufdc,0,0,HERO_SIZE_WIDTH,HERO_SIZE_HEIGHT,RGB(255,255,255));


	// Hero 2: Move Hero
	if (bIsMouseDown)
	{
		POINT pNowMousePoint;
		GetCursorPos(&pNowMousePoint);
		int xOffset = pNowMousePoint.x - pPreMousePoint.x;
		int yOffset = pNowMousePoint.y - pPreMousePoint.y;
		iPlayerPositionX += xOffset;
		iPlayerPositionY += yOffset;

		if (iPlayerPositionX < 0)
		{
			iPlayerPositionX = 0;
		}
		if (iPlayerPositionX > WINDOW_WIDTH - HERO_SIZE_WIDTH)
		{
			iPlayerPositionX = WINDOW_WIDTH - HERO_SIZE_WIDTH;
		}
		if (iPlayerPositionY < 0)
		{
			iPlayerPositionY = 0;
		}
		if (iPlayerPositionY > WINDOW_HEIGHT - HERO_SIZE_HEIGHT)
		{
			iPlayerPositionY = WINDOW_HEIGHT - HERO_SIZE_HEIGHT;
		}
	}

	// bullet 1: generation
	iBulletTimer ++;
	if (iBulletTimer % 10 == 0)
	{
		for (int i =0; i< 30; i++)
		{
			if (bulletArray[i].isExist == false)
			{
				int x = iPlayerPositionX + HERO_SIZE_WIDTH/2 - BULLET_SIZE_WIDTH/2;
				bulletArray[i].x = x;
				bulletArray[i].y = iPlayerPositionY;
				bulletArray[i].isExist = true;
				break;
			}
		}
	}
	// bullet 2: show
	SelectObject(g_bufdc,hBullet);
	for (int i = 0; i< 30; i++)
	{
		if (bulletArray[i].isExist )
		{
			bulletArray[i].y -= BULLET_SPEED;
			if (bulletArray[i].y < BULLET_SIZE_HEIGHT )
			{
				bulletArray[i].isExist = false;
				continue;
			}
			TransparentBlt(g_mdc,bulletArray[i].x,bulletArray[i].y,BULLET_SIZE_WIDTH,BULLET_SIZE_HEIGHT,g_bufdc,0,0,BULLET_SIZE_WIDTH,BULLET_SIZE_HEIGHT,RGB(255,255,255));
		}
	}

	// Enemy 1: generation
	iEnemySpawnTimer++;
	if (iEnemySpawnTimer % 20 == 0)
	{
		for (int i = 0; i< 30; i++)
		{
			if (enemyArray[i].isExist == false)
			{
				int y = ENEMY_SIZE_HEIGHT;
				int x = rand() % (WINDOW_WIDTH - HERO_SIZE_WIDTH);

				enemyArray[i].x = x;
				enemyArray[i].y = y;
				enemyArray[i].isExist = true;
				enemyArray[i].isDie = false;
				enemyArray[i].iDieAnimationTimer = 0;
				enemyArray[i].iDieAnimationIndex = 0;
				break;
			}
		}
	}

	// Enemy 2: movements
	for (int i = 0; i< 30; i++)
	{
		if (enemyArray[i].isExist && enemyArray[i].isDie == false)
		{
			enemyArray[i].y += ENEMY_DOWN_SPEED;
			if (enemyArray[i].y > WINDOW_HEIGHT)
			{
				enemyArray[i].isExist = false;
			}
		}
	}

	// Enemy 3: Show
	for (int i =0; i< 30; i++)
	{
		if (enemyArray[i].isExist)
		{
			if (enemyArray[i].isDie)
			{
				enemyArray[i].iDieAnimationTimer++;
				enemyArray[i].iDieAnimationIndex = enemyArray[i].iDieAnimationTimer / 5;
				if (enemyArray[i].iDieAnimationIndex > 4)
				{
					enemyArray[i].isExist = false;
					enemyArray[i].isDie = false;
					continue;
				}
				// show exploration
				SelectObject(g_bufdc,hEnemyArray[enemyArray[i].iDieAnimationIndex]);
				TransparentBlt(g_mdc,enemyArray[i].x,enemyArray[i].y,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,
					g_bufdc,0,0,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,RGB(255,255,255));
			}
			else
			{
				SelectObject(g_bufdc,hEnemyArray[0]);
				TransparentBlt(g_mdc,enemyArray[i].x,enemyArray[i].y,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,
					g_bufdc,0,0,ENEMY_SIZE_WIDTH,ENEMY_SIZE_HEIGHT,RGB(255,255,255));
			}
		}
	}

	// Collision between Bullet(exist, not die) and enemy
		// or isAllKilled is true
	for (int i=0;i<30;i++) // traverse the enemy array
	{
		if(enemyArray[i].isExist)
		{
			if ( enemyArray[i].isDie == false )// traverse the bullet array
			{
				for (int j = 0; j < 30; j++)
				{
					if (bulletArray[j].isExist || isAllKilled)
					{
						if (IsCollisionEnemyBullet(enemyArray[i], bulletArray[j]) || isAllKilled)
						{
							bulletArray[j].isExist = false;
							enemyArray[i].isDie = true;
														
							iScore ++;
							PlaySound(L"sound/enemy0_down.wav", NULL, SND_FILENAME | SND_ASYNC);

							break;
						}
					}
				}
			}
		}
	}
	// clear the isAllKilled flag
	if (isAllKilled)
	{
		isAllKilled = false;
	}

	// Collision between Bullet(exist, not die) and Hero
	for (int i = 0; i< 30; i++) // traverse the enemy array
	{
		if (enemyArray[i].isExist)
		{
			if (enemyArray[i].isDie == false)
			{
				if (IsCollisionEnemyHero(enemyArray[i]) || isGiveUp)
				{
					StopMusic();
					PlaySound(L"sound/game_over.wav",NULL,SND_FILENAME | SND_ASYNC);
					
					this->isGameOver = true; // change state
					iHighScore = iHighScore > iScore ? iHighScore : iScore;
					break;
				}
			}
		}
	}

	//draw on Screen
	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);
}

void GamePlaying::Destroy()
{

}

bool GamePlaying::IsCollisionEnemyBullet( Enemy emy, Bullet blt )
{
	if (IsInclude(emy, blt.x, blt.y))
	{
		return true;
	}
	if (IsInclude(emy, blt.x + BULLET_SIZE_WIDTH, blt.y))
	{
		return true;
	}
	if (IsInclude(emy, blt.x, blt.y + BULLET_SIZE_HEIGHT))
	{
		return true;
	}
	if (IsInclude(emy, blt.x + BULLET_SIZE_WIDTH, blt.y + BULLET_SIZE_HEIGHT))
	{
		return true;
	}
	return false;
}

bool GamePlaying::IsCollisionEnemyHero( Enemy emy )
{
	// hero is a triangle

	if(IsInclude(emy, iPlayerPositionX + HERO_SIZE_WIDTH / 2, iPlayerPositionY))
	{
		return true;
	}
	if(IsInclude(emy, iPlayerPositionX, iPlayerPositionY + HERO_SIZE_HEIGHT * 3 / 4))
	{
		return true;
	}
	if(IsInclude(emy, iPlayerPositionX + HERO_SIZE_WIDTH, iPlayerPositionY+ HERO_SIZE_HEIGHT *3 /4))
	{
		return true;
	}
	return false;
}

bool GamePlaying::IsInclude( Enemy emy,int x, int y )
{
	if (x > emy.x && y >emy.y && x < emy.x +51 && y< emy.y+39)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GamePlaying::BeginPlayMusic()
{
	mciSendString(L"open sound/game_music.wav alias media",NULL,0,NULL);
	//mciSendString(L"play sound/game_music.wav ",NULL,0,NULL);
	//mciSendString(L"play media ",NULL,0,NULL);
	//mciSendString(L"play media repeat ",NULL,0,NULL); -- doesn't work
	mciSendString(L"play media notify",NULL,0,this->hwnd);
}

void GamePlaying::RePlayMusic(HWND & hd)
{
	mciSendString(L"seek media to start", NULL, 0, NULL);
	mciSendString(L"play media notify", NULL, 0, hd);
}

void GamePlaying::PauseMusic(HWND & hd)
{
	mciSendString(L"pause media",NULL,0,0);
}

void GamePlaying::ResumeMusic(HWND & hd)
{
	mciSendString(L"resume media",NULL,0,0);
}

void GamePlaying::StopMusic()
{
	mciSendString(L"stop media",NULL,0,0);
}

void GamePlaying::ClearBeforeLeave()
{
	// cle

	//GamePlaying::GamePlaying();
	iBackgroundOffset = 0;
	iHeroIndex = 0;
	iHeroTimer = 0;

	iPlayerPositionX = 190;
	iPlayerPositionY = 600;

	bIsMouseDown = false;

	iBulletTimer = 0;
	iEnemySpawnTimer = 0;

	isAllKilled	= false;
	isGiveUp	= false;
	isGameOver = false;
	for (int i =0; i< 30 ; i++)
	{
		this->bulletArray[i].isExist = false;
		this->enemyArray[i].isExist = false;
		this->enemyArray[i].isDie = false;
		this->enemyArray[i].iDieAnimationIndex = 0;
		this->enemyArray[i].iDieAnimationTimer = 0;
	}
}
// class GameResult
void GameResult::setHWND( HWND & hd )
{
	this->hwnd = hd;
}

void GameResult::Init()
{
	hGameOver = (HBITMAP)LoadImage(NULL,L"image/gameover.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);
	//create a font
	HFONT font = CreateFont(45,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("Cooper Std"));
	SelectObject(g_mdc,font);
	SetBkMode(g_mdc,TRANSPARENT);
	hHand = (HBITMAP)LoadImage(NULL,L"image/hand.bmp",IMAGE_BITMAP,100,80,LR_LOADFROMFILE);
	hRestart = (HBITMAP)LoadImage(NULL,L"image/restart.bmp",IMAGE_BITMAP,120,120,LR_LOADFROMFILE);
}

void GameResult::Start()
{
	swprintf_s(scoreText,9,L"%d",iScore);
	swprintf_s(scoreHighText,99,L"%d",iHighScore);
	this->angleTimer =0;
	this->handX = 100;
	this->handY = 200;
	this->isGo2Play = false;
	this->isHandOver = false;
}

void GameResult::Update()
{
	SelectObject(g_bufdc,hGameOver);
	BitBlt(g_mdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);
	SelectObject(g_bufdc,hRestart);
	int restartIconX = WINDOW_WIDTH/2 - 60;
	int restartIconY = WINDOW_HEIGHT/2 -90;
	TransparentBlt(g_mdc,restartIconX, restartIconY,120,120,g_bufdc,0,0,120,120,RGB(0,0,0));
	//TransparentBlt(g_mdc,restartIconX, restartIconY,120,120,g_bufdc,0,0,120,120,RGB(255,255,255));
	

	// draw the fonts
	TextOut(g_mdc,220,620,scoreText,wcslen(scoreText));
	TextOut(g_mdc,210,200,scoreHighText,wcslen(scoreHighText));

	// hand position
	if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect() && myKinect.isSkeletonFoundOK() )
	{
		POINT handNow = myKinect.getSkeDataOnScreen(NUI_SKELETON_POSITION_HAND_RIGHT);
		handX = (int)handNow.x - 50;
		handY = (int)handNow.y - 40;
		//handX +=10; handY -= 10;
	}//otherwise, keep still
	
	// Hand & Start collision
	// RestartImage (180,336) + 120*120 r =50
	if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect() && myKinect.isSkeletonFoundOK() )
	{
		if (IsIncludeStartButton(handX,handY))
		{
			isHandOver = true;
		}else{
			isHandOver = false;
		}
	}else{
		isHandOver = false;
	}

	// draw start button dynamic showing
	if (isHandOver)
	{
		angleTimer += 2;
	}
	else
	{
		angleTimer = 0;
	}

	if (angleTimer >= 120 )
	{
		this->isGo2Play = true;
		angleTimer = 120;
	}
	// RestartImage (180,336) + 120*120  r =50
	if (angleTimer > 0 && angleTimer <= 120)
	{
		{
			int iRed = (angleTimer +100) % 255;
			int iGreen = (255 - angleTimer) % 255;
			int iBlue = (250)%255;
			HBRUSH hBrush =CreateSolidBrush(RGB(iRed,iGreen,iBlue));
			SelectObject(g_mdc,hBrush);
			SetBkMode(g_mdc,TRANSPARENT);

			int ridius = 50 * angleTimer / 120;
			int cirLeft = 240 - ridius;
			int cirTop = 396 - ridius;
			int cirRight = 240 + ridius;
			int cirBottom = 396 + ridius;
			//Ellipse(g_hdc,20,20,ridius,ridius);
			Ellipse(g_mdc,cirLeft,cirTop,cirRight,cirBottom);
			DeleteObject(hBrush);
		}
		// count down
		int countDown = 5 - angleTimer * 5  / 120 ;
		switch(countDown)
		{
		case 5:
			TextOut(g_mdc,restartIconX+48, restartIconY + 40,L"5",strlen("5"));
			break;
		case 4:
			TextOut(g_mdc,restartIconX+48, restartIconY + 40,L"4",strlen("5"));
			break;
		case 3:
			TextOut(g_mdc,restartIconX+48, restartIconY + 40,L"3",strlen("5"));
			break;
		case 2:
			TextOut(g_mdc,restartIconX+48, restartIconY + 40,L"2",strlen("5"));
			break;
		case 1:
			TextOut(g_mdc,restartIconX+48, restartIconY + 40,L"1",strlen("5"));
			break;
		case 0:
			TextOut(g_mdc,restartIconX+48, restartIconY + 40,L"0",strlen("5"));
			break;
		}

	}

	// draw hand
	SelectObject(g_bufdc,hHand);
	TransparentBlt(g_mdc,handX,handY,100,80,g_bufdc,0,0,100,80,RGB(0,0,0));

	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);
}

void GameResult::Destroy()
{

}

void GameResult::ClearBeforeLeave()
{
	this->angleTimer =0;
	this->handX = 100;
	this->handY = 200;
	this->isGo2Play = false;
	this->isHandOver = false;
}

GameResult::GameResult()
{
	this->angleTimer =0;
	this->handX = 100;
	this->handY = 200;
	this->isGo2Play = false;
	this->isHandOver = false;
}

// restart button: (WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT/2 -90)    (180,336) + 120*120
bool GameResult::IsIncludeStartButton( int x, int y )
{
	int checkX = x;
	int checkY = y;
	if (checkX > 180 && checkX < 300 && checkY > 336 && checkY< 456)
	{
		return true;
	}
	checkX = x + 100; checkY = y;
	if (checkX > 180 && checkX < 300 && checkY > 336 && checkY< 456)
	{
		return true;
	}
	checkX = x ; checkY = y+80;
	if (checkX > 180 && checkX < 300 && checkY > 336 && checkY< 456)
	{
		return true;
	}
	checkX = x + 100; checkY = y+80;
	if (checkX > 180 && checkX < 300 && checkY > 336 && checkY< 456)
	{
		return true;
	}
	return false;
}