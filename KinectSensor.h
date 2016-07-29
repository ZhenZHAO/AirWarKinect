#pragma  once

#include <nuiapi.h>
#include <NuiSensor.h>
#include <Windows.h>
#include "Control.h"
#include "Utility.h"


class SensorObject
{
	static const int        cScreenWidth  = 640;
	static const int        cScreenHeight = 480;

public:
	SensorObject();
	~SensorObject();
	// procedure
	bool			Initialize();
	bool			StartStreams();
	bool			UpdateStreams();
	// control
	void			PauseStream(bool pause);
	void			ProcessCommand(WORD commandId, bool previouslyChecked);
	// Kinect state
	bool			getKinectState();
	bool			isKinectLinkCorrect();
	bool			isSkeletonCorrect();
	bool			isSkeletonFoundOK();
	HANDLE			getFrameReadyEvent();
	// get data
	Vector4			getSkeletonPointData(NUI_SKELETON_POSITION_INDEX _index);
	POINT			getSkeDataOnScreen(NUI_SKELETON_POSITION_INDEX _index);
	POINT			getSkeDataOnScreen(int _index);
	// view
	//void			drawSetImageMat(Mat &image, CvPoint pointSet[]);
private:

	bool			ProcessSkeleton();
	//void			assignSkeleton2DataArray();
	//void			changeSkeleton2Screen();
	void			clearAllSkeletonData();

	// Current Kinect
	INuiSensor*             m_pNuiSensor;
	// skeleton - only readyEvent , both for color and depth image
	HANDLE					m_hStreamHandle;
	HANDLE					m_hFrameReadyEvent;

	// all skeleton data --- just for one person
	Vector4					m_allSkeletonData[NUI_SKELETON_POSITION_COUNT];
	POINT					m_allSkeDataOnScreen[NUI_SKELETON_POSITION_COUNT];
	//NUI_SKELETON_FRAME		m_skeletonFrame;
	
	// states
	bool					m_paused;
	bool					isKinectFinded;
	bool					isSkeletonEnable;
	bool					isSkeletonFound;
};
