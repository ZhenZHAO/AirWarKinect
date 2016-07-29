#include "GestureControl.h"

bool GestureControl::loadGestureDesignFromFile()
{
	//whether the file exists
	std::fstream fp;
	fp.open(GAME_GESTURE_FILE_NAME,std::ios::in);

	if (!fp) // not exist
	{
		MessageBoxA(NULL,"Gesture design file doesn't exist!","ATTENTION!",MB_OK);
		return false;
	}
	else	// exist
	{
		std::ifstream fin(GAME_GESTURE_FILE_NAME);
		if (! fin.is_open())
		{
			MessageBoxA(NULL,"Failed to open design file","ATTENTION!",MB_OK);
			return false;
		}
		else
		{
			while(!fin.eof())
			{
				SingleGesture * gesNew = new SingleGesture();
				
				fin>>gestureNum;
				for (int i = 0; i < GESTURE_RECOGNITION_ANGLE_COUNT; i++)
				{
					fin>>gesNew->judgeAngles[i]>>gesNew->judgeErrors[i];
				}
				
				this->gestureArray.push_back(gesNew);

				//fin.get(); //
				//if(fin.peek() == '\n') break;
			}
		}
		gestureNum = gestureArray.size();

		fin.close();
		return true;
	}
	

}

GestureControl::GestureControl()
{
	currentActiveGestureID = 0;
	gestureNum = 0;
	
	loadGestureDesignFromFile(); // configure gestureArray

	for (int i = 0; i < GESTURE_RECOGNITION_ANGLE_COUNT; i++)
	{
		currentFrameAngles[i] = 0;
	}
}

GestureControl::~GestureControl()
{
	for (int i =0; i < gestureNum; i++)
	{
		SafeDelete(gestureArray[i]);
	}
}

int GestureControl::getActiveGesID()
{
	return this->currentActiveGestureID;
}

int GestureControl::getGestureNumber()
{
	return this->gestureNum;
}

bool GestureControl::gestureDataUpdate()
{
	calculateCurrentAngles();
	return findMatchGesId();
}

void GestureControl::calculateCurrentAngles()
{
	double vx1,vy1,vz1,vx2,vy2,vz2;
	for (int i = 0; i< GESTURE_RECOGNITION_ANGLE_COUNT; i++)
	{
		switch(i)
		{
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_LEFT: // vector direction: elbow->wrist
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).x;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;
			
			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_LEFT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).z;
			
			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;
			
			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).z;
			
			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_LEFT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).x;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;
			
			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_LEFT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		default:
			vx1 = vy1 = vz1 = vx2 = vy2 = vz2 = 0;
			break;
		}
	}
}

void GestureControl::calculateCurrentAngles( Vector4 _skeletonData[] )
{
	double vx1,vy1,vz1,vx2,vy2,vz2;
	for (int i = 0; i< GESTURE_RECOGNITION_ANGLE_COUNT; i++)
	{
		switch(i)
		{
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_LEFT: // vector direction: elbow->wrist
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).x;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_LEFT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndX(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_LEFT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).x;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_LEFT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_LEFT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_LEFT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_WRIST_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		case GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_RIGHT:
			vx1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).x ;
			vy1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).y;
			vz1 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_ELBOW_RIGHT).z;
			vx2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).x;
			vy2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).y;
			vz2 = myKinect.getSkeletonPointData(NUI_SKELETON_POSITION_SHOULDER_RIGHT).z;

			currentFrameAngles[i] = calAngleBetweenVecAndNegativeZ(vx1,vy1,vz1,vx2,vy2,vz2);
			break;
		default:
			vx1 = vy1 = vz1 = vx2 = vy2 = vz2 = 0;
			break;
		}
	}

}

bool GestureControl::findMatchGesId() //traverse gestureArray
{
	bool isAnyGestureTrigger = false;
	// 1.traverse 
	for (int i=0; i < gestureNum; i++) // all gestures
	{
		bool isSingleAngleJudgeOK = true;
		for (int j =0; j< GESTURE_RECOGNITION_ANGLE_COUNT; j ++) // one gesture i
		{
			// compare with currentFrameAngles
			if ( false == 
					isWithinErrorDistance(
						currentFrameAngles[j],
						gestureArray[i]->judgeAngles[j] ,
						gestureArray[i]->judgeErrors[j]  )
				)
			{
				isSingleAngleJudgeOK = false;
				break;
			}
		}

		if (isSingleAngleJudgeOK)
		{
			this->currentActiveGestureID = i + 1;
			isAnyGestureTrigger = true;
			break;
		}
	}

	// 2.judge 
	if ( !isAnyGestureTrigger )  
		currentActiveGestureID = 0;		// ID = 0, represent no trigger
	return isAnyGestureTrigger;

}

void GestureControl::clearAfterOneGame()
{
	currentActiveGestureID = 0;
		
	for (int i = 0; i < GESTURE_RECOGNITION_ANGLE_COUNT; i++)
	{
		currentFrameAngles[i] = 0;
	}
}

/************************************************************************/
/*		calculation functions */
/************************************************************************/

bool GestureControl::isWithinErrorDistance( double _angle1, double _angle2, double _error )
{
	double item1 = _angle1 > 0 ? _angle1 : - _angle1;
	double item2 = _angle2 > 0 ? _angle2 : - _angle2;
	if (fabs(item1 - item2) < _error)
	{
		return true;
	}
	else
	{
		return false;
	}
}

double GestureControl::calAngleBetween2Vec( double x1, double y1, double z1, double x2, double y2, double z2, double xx1, double yy1, double zz1, double xx2, double yy2, double zz2)
{
	// vector 1
	double v1X = x1 - x2;
	double v1Y = y1 - y2;
	double v1Z = z1	- z2;
	// vector 2
	double v2X = xx1 - xx2;
	double v2Y = yy1 - yy2;
	double v2Z = zz1 - zz2;
	//cal
	double dotRec = v1X * v2X + v1Y * v2Y + v1Z * v2Z;
	double V1M = sqrt(v1X * v1X + v1Y * v1Y + v1Z * v1Z);
	double V2M = sqrt(v2X * v2X + v2Y * v2Y + v2Z * v2Z);

	return acos(dotRec / (V1M * V2M) ) * 180.0 / PI ;
	
}

double GestureControl::calAngleBetween2Vec( double x1, double y1, double x2, double y2, double xx1, double yy1, double xx2, double yy2 )
{
	// vector 1
	double v1X = x1 - x2;
	double v1Y = y1 - y2;
	// vector 2
	double v2X = xx1 - xx2;
	double v2Y = yy1 - yy2;
	//cal
	double dotRec = v1X * v2X + v1Y * v2Y;
	double V1M = sqrt(v1X * v1X + v1Y * v1Y);
	double V2M = sqrt(v2X * v2X + v2Y * v2Y);

	return acos(dotRec / (V1M * V2M) ) * 180 / PI ;
}

double GestureControl::calAngleBetweenVecAndX( double x1, double y1,double z1, double x2, double y2, double z2 )
{
	return calAngleBetween2Vec(x1,y1,z1,x2,y2,z2,1,0,0,0,0,0);
}

double GestureControl::calAngleBetweenVecAndY( double x1, double y1,double z1, double x2, double y2, double z2 )
{
	return calAngleBetween2Vec(x1,y1,z1,x2,y2,z2,0,1,0,0,0,0);
}

double GestureControl::calAngleBetweenVecAndNegativeZ( double x1, double y1,double z1, double x2, double y2, double z2 )
{
	return calAngleBetween2Vec(x1,y1,z1,x2,y2,z2,0,0,-1,0,0,0);
}