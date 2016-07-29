#pragma  once

#include <nuiapi.h>
#include <cmath>
#include <fstream>
#include <vector>
#include "Control.h"
#include "Utility.h"
#include "KinectDeclare.h"

#ifndef _GESTURE_RECOGNITION_ANGLE_INDEX_
#define _GESTURE_RECOGNITION_ANGLE_INDEX_
typedef 
	enum _GESTURE_RECOGNITION_ANGLE_INDEX_
{	// X AXIS
	GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_LEFT	= 0,
	GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_LEFT	= (GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_LEFT+1),
	GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_RIGHT	= (GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_LEFT+1),
	GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_RIGHT	= (GESTURE_RECOGNITION_ANGLE_FRONT_ARM_X_RIGHT+1),
	// Z AXIS
	GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_LEFT	= (GESTURE_RECOGNITION_ANGLE_BACK_ARM_X_RIGHT+1),
	GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_LEFT	= (GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_LEFT+1),
	GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_RIGHT = (GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_LEFT+1),
	GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_RIGHT	= (GESTURE_RECOGNITION_ANGLE_FRONT_ARM_Z_RIGHT+1),
	GESTURE_RECOGNITION_ANGLE_COUNT				= (GESTURE_RECOGNITION_ANGLE_BACK_ARM_Z_RIGHT+1)
} 	GESTURE_RECOGNITION_ANGLE_INDEX;

#endif _GESTURE_RECOGNITION_ANGLE_INDEX_


class SingleGesture
{
public:
	double	judgeAngles[GESTURE_RECOGNITION_ANGLE_COUNT]; // angles
	double	judgeErrors[GESTURE_RECOGNITION_ANGLE_COUNT]; // errors of angle
	bool	isTrigger;

	SingleGesture()
	{
		isTrigger = false;
	}

};

class GestureControl
{
public:
	GestureControl();
	~GestureControl();	// remember to free the pointers

	bool	gestureDataUpdate(); // return the gesture ID
	int		getActiveGesID();
	int		getGestureNumber();
	void	clearAfterOneGame(); //  clear info

private:
	// 1. invoked in Constructor
	bool	loadGestureDesignFromFile();	// obtain gestureArray, gestureNum
	// 2.1 invoked in update //
	void	calculateCurrentAngles(Vector4 _skeletonData[]);		// obtain currentFrameAngles From myKinect
	void	calculateCurrentAngles();
	// 2.2 traverse the gestureArray, find match one , set active id
	bool	findMatchGesId();
	// 3. calculate details
	bool	isWithinErrorDistance(double _angle1, double _angle2, double _error);
	double	calAngleBetween2Vec( double x1, double y1, double z1, double x2, double y2, double z2, double xx1, double yy1, double zz1, double xx2, double yy2, double zz2);
	double	calAngleBetween2Vec(double x1, double y1, double x2, double y2, double xx1, double yy1, double xx2, double yy2);
	double  calAngleBetweenVecAndX(double x1, double y1,double z1, double x2, double y2, double z2);
	double	calAngleBetweenVecAndY(double x1, double y1,double z1, double x2, double y2, double z2);
	double  calAngleBetweenVecAndNegativeZ(double x1, double y1,double z1, double x2, double y2, double z2);

private:
	std::vector <SingleGesture *>	gestureArray;	// array of gestures
	int								gestureNum;		// after obtainning gestrueArray from file -- gesture.Array.size()
	//SensorObject currentKinect;  // -- data could be obtained from myKinect directly
	double							currentFrameAngles[GESTURE_RECOGNITION_ANGLE_COUNT];
	int								currentActiveGestureID;
	
};