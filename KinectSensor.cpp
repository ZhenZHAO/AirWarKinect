
#include "KinectSensor.h"


SensorObject::SensorObject()
	: m_pNuiSensor(NULL)
	, m_hStreamHandle(INVALID_HANDLE_VALUE)
	//, m_hFrameReadyEvent(INVALID_HANDLE_VALUE)
	, m_paused(false)
	, isKinectFinded(false)
	, isSkeletonEnable(false)
	,isSkeletonFound(false)
{
	//ZeroMemory(m_allSkeletonData, sizeof(m_allSkeletonData));
	//ZeroMemory(m_allSkeDataOnScreen, sizeof(m_allSkeDataOnScreen));
	m_hFrameReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	clearAllSkeletonData();
}

SensorObject::~SensorObject()
{
	if (m_pNuiSensor)
	{
		m_pNuiSensor->NuiShutdown();
	}

	if (m_hFrameReadyEvent && (m_hFrameReadyEvent != INVALID_HANDLE_VALUE))
	{
		CloseHandle(m_hFrameReadyEvent);
	}

	 SafeRelease(m_pNuiSensor);
}

bool SensorObject::getKinectState()
{
	if (NULL != m_pNuiSensor)
	{
		return TRUE;
	}
	return FALSE;
}

bool SensorObject::isKinectLinkCorrect()
{
	return this->isKinectFinded;
}

bool SensorObject::isSkeletonCorrect()
{
	return this->isSkeletonEnable;
}

bool SensorObject::isSkeletonFoundOK()
{
	return this->isSkeletonFound;
}

HANDLE SensorObject::getFrameReadyEvent()
{
	return m_hFrameReadyEvent;
}


bool SensorObject::Initialize()
{
	INuiSensor * pNuiSensor = nullptr;
	HRESULT hr;
	int iSensorCount = 0;
	//--1 get how many kinects there is
	hr = NuiGetSensorCount(&iSensorCount);
	if (FAILED(hr))
	{
		this->isKinectFinded = FALSE;
		return FALSE;
	}

	//--2 Look at each Kinect sensor
	for (int i = 0; i < iSensorCount; ++i)
	{
		// Create the sensor so we can check status, if we can't create it, move on to the next
		hr = NuiCreateSensorByIndex(i, &pNuiSensor);
		if (FAILED(hr))
		{
			continue;
		}

		// Get the status of the sensor, and if connected, then we can initialize it
		hr = pNuiSensor->NuiStatus();
		if (S_OK == hr)
		{
			m_pNuiSensor = pNuiSensor;
			break;
		}

		// This sensor wasn't OK, so release it since we're not using it
		pNuiSensor->Release();
	}

	//--3 initialize
	if (NULL != m_pNuiSensor)
	{
		// Initialize the Kinect and specify that we'll be using color
		//hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON); 
		hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
		if (SUCCEEDED(hr))
		{
			m_pNuiSensor->NuiSetForceInfraredEmitterOff(FALSE);
		}
	}

	if (NULL == m_pNuiSensor || FAILED(hr))
	{
		this->isKinectFinded = FALSE;
		return	FALSE;
	}

	this->isKinectFinded = TRUE;
	return	TRUE;
}

bool SensorObject::StartStreams()
{
	if (NULL == m_pNuiSensor || !isKinectFinded )
	{
		return false;
	}

	if (HasSkeletalEngine(m_pNuiSensor))
	{
		if (m_paused)
		{
			// Disable tracking skeleton
			m_pNuiSensor->NuiSkeletonTrackingDisable();
			this->isSkeletonEnable = false;
			return false;
		}
		else
		{			
			HRESULT hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hFrameReadyEvent,0);
			
			if (SUCCEEDED(hr))
			{
				this->isSkeletonEnable = true;
				return true;
			}
			
		}
	}
	this->isSkeletonEnable = false;
	return false;

}

void SensorObject::PauseStream( bool pause )
{
	if (m_paused != pause) // state change -- > invoke StartStreams().
	{
		m_paused = pause;
		StartStreams();
	}
}

bool SensorObject::UpdateStreams()
{
	if (NULL == m_pNuiSensor || !isSkeletonEnable)
	{
		return false;
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(getFrameReadyEvent(), INFINITE))
	{
		return ProcessSkeleton();
	}
	return false;
}


bool SensorObject::ProcessSkeleton()
{
	NUI_SKELETON_FRAME		m_skeletonFrame = {0};
	this->isSkeletonFound= false;
	// Retrieve skeleton frame
	HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &m_skeletonFrame);
	if (FAILED(hr) || m_paused)
	{
		// If occur error when get skeleton data or pause tracking skeleton,
		// clear skeleton data in stream viewers
		isSkeletonFound = false;
		return false;
	}

	//---double check
		// we must get hasDataIndex
	int		hasDataIndex = -1;
	for (int i=0; i <NUI_SKELETON_COUNT; i++)
	{
		if (m_skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			m_skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			hasDataIndex = i;
			isSkeletonFound = true;
			break;
		}
	}
	if (hasDataIndex < 0)
	{
		isSkeletonFound = false;
		return false;
	}
	
	// smooth out the skeleton data
	m_pNuiSensor->NuiTransformSmooth(&m_skeletonFrame, nullptr);

	// record value for data and screen data
	LONG x,y;
	USHORT depth;
	for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)  
	{
		if (m_skeletonFrame.SkeletonData[hasDataIndex].eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			m_allSkeletonData[j] = m_skeletonFrame.SkeletonData[hasDataIndex].SkeletonPositions[j];
			NuiTransformSkeletonToDepthImage(m_allSkeletonData[j],&x,&y,&depth,NUI_IMAGE_RESOLUTION_640x480);
			//m_allSkeDataOnScreen[j].x = x; m_allSkeDataOnScreen[j].y = y;
			m_allSkeDataOnScreen[j].x = x * WINDOW_WIDTH / cScreenWidth;
			m_allSkeDataOnScreen[j].y = y * WINDOW_HEIGHT / cScreenHeight;
		}

	}
	return true;

}

void SensorObject::ProcessCommand( WORD commandId, bool previouslyChecked )
{
	if (ID_SKELETONSTREAM_PAUSE == commandId)
	{
		// Pause skeleton stream
		PauseStream(previouslyChecked);

	}else if (ID_KINECT_CLOSE == commandId)
	{
		if (this->m_pNuiSensor)
		{
			m_pNuiSensor->NuiShutdown();
		}
	}
	else
	{
		switch (commandId)
		{
		case ID_FORCE_OFF_IR:
			m_pNuiSensor->NuiSetForceInfraredEmitterOff(true);
			break;

		default:
			break;
		}
	}
}

Vector4 SensorObject::getSkeletonPointData( NUI_SKELETON_POSITION_INDEX _index )
{
	if ( _index < NUI_SKELETON_POSITION_COUNT)
	{
		return this->m_allSkeletonData[_index];
	}else{
		Vector4 re;
		re.x =0; re.y = 0; re.z = 0; re.w = 0;
		return re;
	}
}

POINT SensorObject::getSkeDataOnScreen( NUI_SKELETON_POSITION_INDEX _index )
{
	if ( _index < NUI_SKELETON_POSITION_COUNT){
		return this->m_allSkeDataOnScreen[_index];
	}else{
		POINT re;
		re.x =0; re.y =0;
		return re;
	}
}

POINT SensorObject::getSkeDataOnScreen( int _index )
{
	if ( _index < 20){
		return this->m_allSkeDataOnScreen[_index];
	}else{
		POINT re;
		re.x =0; re.y =0;
		return re;
	}
}
void SensorObject::clearAllSkeletonData()
{
	Vector4 v4; v4.x = 0; v4.y = 0; v4.z = 0; v4.w = 0;
	POINT pt2; pt2.x = 0; pt2.y = 0;
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		m_allSkeletonData[i] = v4;
		m_allSkeDataOnScreen[i] = pt2;
	}
}
