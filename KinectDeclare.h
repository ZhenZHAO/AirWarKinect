#pragma  once
#include "KinectSensor.h"

extern SensorObject myKinect; 
extern const bool isPlayWithKinect;

/************************************************************************/
/* 1. （此情况经常出现在大型工程项目中）如果存在两个类的头文件a.h和b.h,在a.h中有这样的语句：#include "b.h",
*  在b.h文件中有这样的语句：#include "a.h"   且在一个类中有另一个类的对象时   那么就会出现这样的错误。*/
/************************************************************************/