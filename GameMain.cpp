#include <windows.h>
#include <string>
#include <process.h>  // for thread
#include "Utility.h"
#include "Control.h"
#include "resource.h"
#include "GameState.h"
#include "GameObject.h"
#include "UserInfo.h"
#include "KinectSensor.h"
#include "GestureControl.h"
#include <nuiapi.h>
#include <opencv2/opencv.hpp> 
using namespace std;  
using namespace cv;  

// create a game object
GameObject myGame;
UserInfo myUser;
SensorObject myKinect;
GestureControl myGesture;

HDC g_hdc;
HDC g_mdc;
HDC g_bufdc;
int iScore = 0;
int iHighScore = 0;




const bool isShowSkeletonImage = true;
const bool isPlayWithKinect = true; // whether to play with kinect
//const bool isPlayWithSpeechRecognition = false;
bool isQuitKinect = false;	// whether the kinect thread should be stopped. -- changed inside of main message loop. // or just at the end of the program


// callback function declaration 
LRESULT CALLBACK myMainWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void KinectThread(PVOID	pvoid);
void GestureThread(PVOID	pvoid);
void drawSkeleton(Mat &image);

// program entry
int WINAPI WinMain( 
	__in HINSTANCE hInstance, 
	__in_opt HINSTANCE hPrevInstance, 
	__in LPSTR lpCmdLine, 
	__in int nShowCmd )
{
	/***** 0. Load User Info  *****/
	int tmp= myUser.LoadHistoryScore();
	iHighScore = iHighScore > tmp ? iHighScore : tmp;
	myUser.SetCurrentScore(iScore);

	/***** 1. create game window  *****/
		// 1.1 design  window
	WNDCLASSEX myWndClass = {0};
	myWndClass.cbSize = sizeof(WNDCLASSEX);
	myWndClass.lpfnWndProc = myMainWndProc;
	myWndClass.lpszClassName = L"myClass";
	myWndClass.hInstance = hInstance;
	myWndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	//myWndClass.hIcon = LoadIcon(NULL,IDI_INFORMATION);
	myWndClass.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
	myWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		// 1.2.register window
	if (!RegisterClassEx(&myWndClass))
	{
		return -1;
	}
		// 1.3.create window
	HWND hwnd = CreateWindow(L"myClass",
		WINDOW_TITLE,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // fixed window size
		//WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,NULL,
		hInstance,NULL);
		// 1.4.show window
	ShowWindow(hwnd,nShowCmd);
	UpdateWindow(hwnd);
	
	/***** 2. create game object  *****/
	myGame.SetHWND(hwnd);
		// 2.1. Gaming begin
	myGame.GameStart();

	/***** 3. message processing loop  *****/
		//3.1 record the timer
	DWORD tNow = GetTickCount(); //ms
	DWORD tPre = GetTickCount();
		// 3.2 message loop
	MSG msg = {0};
	while(msg.message!=WM_QUIT)
	{
		if (PeekMessage(&msg,0,NULL,NULL,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //update the game through the time interval
		{
			tNow = GetTickCount();
			if (tNow - tPre > UPDATE_INTERVAL)
			{
            	myGame.GameUpdate(); // Game Update
				tPre = tNow;
			}
		}
	}

	/***** 4. Game END & Clear  & Save*****/
		//4.1 game object clear //4.2 global DC
	myGame.GameEnd();
		//4.3 discard  the window
	UnregisterClass(L"myClass",hInstance);
		//4.4 Save Score
	myUser.SetCurrentScore(iScore,iHighScore);
	myUser.SaveScoreInfo2File();
		//4.5 stop thread
	isQuitKinect = true;

	return 0;
}

// callback functions(Message processing functions)
LRESULT CALLBACK myMainWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
	case WM_CREATE:
		if (isPlayWithKinect)
		{
			_beginthread(KinectThread,0,NULL);
		}
		break;
	case WM_KINECT_GESTURE_BEGIN:
		if (isPlayWithKinect)
		{
			_beginthread(GestureThread,0,NULL);
		}
		break;
	case WM_TIMER:
		switch(wparam)
		{
		case GAME_DYNAMIC_GESTURE_TIMER:
			myGame.ProcessDynamicGesture();
			break;
		default:
			break;
		}
		break;
	case WM_KINECT_GESTURE_TRIGGER:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_KEYDOWN:
		myGame.OnWindowMessage(hwnd,message,wparam,lparam);
		break;
	
	case WM_CLOSE: // pause music and kinect
		// pause music
		if (myGame.myGameState == GS_Playing)
		{
			myGame.myGamePlaying.PauseMusic(hwnd);
		}
		// pause tracking of Kinect 
		if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect())
		{
			myKinect.PauseStream(true);
		}

		// remind whether to quit
			// messageBox pop out -> pause the main thread, but the sub-thread still work
		if (IDOK == MessageBoxA(hwnd,"Are you sure to Quit?","ATTENTION!",MB_OKCANCEL))
		{
			DestroyWindow(hwnd);
			// close music
			if (myGame.myGameState == GS_Playing)
				myGame.myGamePlaying.StopMusic();
			// close Kinect
			if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect())
				myKinect.ProcessCommand(ID_KINECT_CLOSE,false);
			// release thread
			isQuitKinect = true;
		}
		else
		{
			// resume music
			if (myGame.myGameState == GS_Playing)
				myGame.myGamePlaying.ResumeMusic(hwnd);

			// resume kinect
			if (isPlayWithKinect&& myKinect.isKinectLinkCorrect() && myKinect.isSkeletonCorrect())
				myKinect.PauseStream(false);
		}
		break;
	case MM_MCINOTIFY: // replay music
		if (myGame.myGameState == GS_Playing)
		{
			myGame.myGamePlaying.RePlayMusic(hwnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,message,wparam,lparam);
	}
	return 0;
}

void GestureRecognition()
{
	while(myGame.myGameState == GS_Playing)
	{
		if (isPlayWithKinect && myKinect.isSkeletonCorrect() && myKinect.isKinectLinkCorrect() && myKinect.isSkeletonFoundOK())
		{
			Sleep(GAME_GESTURE_CONTROL_SPEED);  // not to be that fast
			if (myGesture.gestureDataUpdate()) // there is one gesture trigger
			{
				int i = myGesture.getActiveGesID(); // get active gesture id
				SendMessage(myGame.getHWND(),WM_KINECT_GESTURE_TRIGGER, (WPARAM) i, 0);
			}
		}
	}
	myGesture.clearAfterOneGame();
}

void GestureThread(PVOID	pvoid)
{
	GestureRecognition();
	_endthread();
}

int KinectOperation()
{
	// open the Kinect
	if (!myKinect.Initialize())
	{
		MessageBox(NULL,L"Sorry, failed to initialize Kinect sensor!",L"Attention!", MB_OK);
		return -1;
	}
	if (!myKinect.StartStreams())
	{
		MessageBox(NULL,L"Sorry, failed to Start sensor data!",L"Attention!", MB_OK);
		return -1;
	}

	// Create image Window
	Mat skeletonImage; // draw the skeleton window 
	skeletonImage.create(SKELETON_WINDOW_HEIGHT, SKELETON_WINDOW_WIDTH, CV_8UC3);
	if (isShowSkeletonImage)
	{	
		namedWindow("ShowSkeletonImage", CV_WINDOW_AUTOSIZE);
		moveWindow("ShowSkeletonImage", 900,100);
		
	}
	// opencv will create two window !
	HWND hSkeletonChild = (HWND)cvGetWindowHandle("ShowSkeletonImage"); 
	HWND hSkeleton = ::GetParent(hSkeletonChild);	
	
	// update
	bool isWindowShow = isShowSkeletonImage;
	bool isAtBegining = true;
	while(TRUE)
	{
		isAtBegining = (myGame.myGameState == GS_Menu);
		
		if ( myKinect.UpdateStreams() )	// skeleton tracked
		{
			if (isShowSkeletonImage && isWindowShow)
			{
					// draw image
					skeletonImage.setTo(0);
					drawSkeleton(skeletonImage);
					// show window
					imshow("ShowSkeletonImage", skeletonImage); // show skeleton 
					cvWaitKey(1);	
			}
		}
		else
		{
			if (isShowSkeletonImage && isWindowShow)
			{
					skeletonImage.setTo(0);
					imshow("ShowSkeletonImage", skeletonImage); // show skeleton 
					cvWaitKey(1);
			}
		}

		// show hide Skeleton Window
		if (isShowSkeletonImage && isAtBegining)
		{
			if (cvWaitKey(50) == 27){
				if (isWindowShow){
					isWindowShow = false;
					SetWindowPos(hSkeleton,0,900,200,SKELETON_WINDOW_WIDTH,SKELETON_WINDOW_HEIGHT,SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
					//ShowWindow(hSkeleton,SW_HIDE);
				}else{
					isWindowShow = true;
					SetWindowPos(hSkeleton,0,900,200,SKELETON_WINDOW_WIDTH,SKELETON_WINDOW_HEIGHT,SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
					//ShowWindow(hSkeleton,SW_SHOW);
				}
			}
		}
		//if (isShowSkeletonImage)
		//{
		//	if (cvWaitKey(20) == 27){
		//		isNewShowImage = ~ isNewShowImage;
		//	}
		//	if (isNewShowImage){
		//		ShowWindow(hSkeleton,SW_SHOW);	// same result by using imshow
		//	}else{
		//		ShowWindow(hSkeleton,SW_HIDE); // imshow will override this hidden state
		//	}
		//}
		// whether quit
		if (isQuitKinect)
		{
			break;
		}

	}
	
	// close Kinect
	skeletonImage.release();
	if (isShowSkeletonImage)
	{
		cv::destroyWindow("ShowSkeletonImage");
	}
	//hSkeleton
	myKinect.ProcessCommand(ID_KINECT_CLOSE,false);

	return 0;
}

void KinectThread(PVOID	pvoid)
{
	KinectOperation();
	_endthread();
}

void drawSkeleton(Mat &image) 
{ 
	// Vector4 -> CvPoint
	CvPoint pointSet[NUI_SKELETON_POSITION_COUNT];
	for (int j=0; j<NUI_SKELETON_POSITION_COUNT ; j++)   
	{
		pointSet[j].x = myKinect.getSkeDataOnScreen(j).x * SKELETON_WINDOW_WIDTH / WINDOW_WIDTH;
		pointSet[j].y = myKinect.getSkeDataOnScreen(j).y * SKELETON_WINDOW_HEIGHT / WINDOW_HEIGHT;
		circle(image, pointSet[j], 6, cvScalar(0, 255, 255), 2, 8, 0); 			 
	} 

	CvScalar color =cvScalar(255); 
	if((pointSet[NUI_SKELETON_POSITION_HEAD].x!=0 || pointSet[NUI_SKELETON_POSITION_HEAD].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_HEAD], pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], color, 3); 

	if((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_SPINE].x!=0 || pointSet[NUI_SKELETON_POSITION_SPINE].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SPINE], color, 3); 

	if((pointSet[NUI_SKELETON_POSITION_SPINE].x!=0 || pointSet[NUI_SKELETON_POSITION_SPINE].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_SPINE], pointSet[NUI_SKELETON_POSITION_HIP_CENTER], color, 3); 

	//left arm
	if((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT], pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT], pointSet[NUI_SKELETON_POSITION_WRIST_LEFT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_HAND_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_HAND_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_WRIST_LEFT], pointSet[NUI_SKELETON_POSITION_HAND_LEFT], color, 3); 

	//right arm
	if((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT], pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT], pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_HAND_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_HAND_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT], pointSet[NUI_SKELETON_POSITION_HAND_RIGHT], color, 3); 

	//left leg
	if((pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_HIP_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_CENTER], pointSet[NUI_SKELETON_POSITION_HIP_LEFT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_HIP_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_LEFT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_LEFT], pointSet[NUI_SKELETON_POSITION_KNEE_LEFT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_KNEE_LEFT], pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].y!=0) &&  
		(pointSet[NUI_SKELETON_POSITION_FOOT_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_FOOT_LEFT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT], pointSet[NUI_SKELETON_POSITION_FOOT_LEFT], color, 3); 

	//right leg
	if((pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_CENTER], pointSet[NUI_SKELETON_POSITION_HIP_RIGHT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_RIGHT], pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT],color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT], pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT], color, 3); 
	if((pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].y!=0) && 
		(pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT].y!=0)) 
		line(image, pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT], pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT], color, 3); 
} 

// #total: 2642 lines