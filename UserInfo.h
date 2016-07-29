#pragma  once

#include "Control.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <time.h>


class UserInfo
{
public:
	int iScore;
	int iHighScore;

	UserInfo(int _score = 0);
	void SetCurrentScore(int _score);
	void SetCurrentScore(int _score, int _hScore);

	// file operations
	int LoadHistoryScore();
	bool SaveScoreInfo2File();
};