#include "UserInfo.h"


UserInfo::UserInfo( int _score /*= 0*/ )
{
	this->iScore = 0;
}

void UserInfo::SetCurrentScore( int _score )
{
	if (this->iHighScore < _score)
	{
		this->iHighScore = _score;
	}
	iScore = _score;
}

void UserInfo::SetCurrentScore( int _score, int _hScore )
{
	iHighScore = iHighScore<_hScore ? _hScore : iHighScore;
	iScore = _score;
}

int UserInfo::LoadHistoryScore()
{
	std::string timeStr;
	int highscore;
	int lastscore;
	//whether the file exists
	std::fstream fp;
	fp.open(USER_FILE_NAME,std::ios::in);

	if (!fp) // not exist
	{
		// create
		std::ofstream inFile;
		inFile.open(USER_FILE_NAME);
		if (inFile.is_open())
		{
			std::cout<<"Create Successful!\n";
		}
		inFile.close();
		// assign
		this->iHighScore = this->iScore;
		return iHighScore;
	}
	else	// exist
	{
		std::ifstream fin(USER_FILE_NAME);
		if (! fin.is_open())
		{
			std::cout<<"Failed to open file!\n";
			return 0;
		}
		else
		{
			while(!fin.eof())
			{
				fin>>timeStr>>highscore>>lastscore; 
				fin.get(); //
				if(fin.peek() == '\n') break;
			}
		}
		fin.close();

		this->iHighScore = highscore;
		return highscore;
	}

}

bool UserInfo::SaveScoreInfo2File()
{
	std::ofstream fout(USER_FILE_NAME, std::ios::app); 
	// current time
	time_t t = time(0); 
	char tmp[64]; 
	strftime( tmp, sizeof(tmp), "%Y-%m-%d_%H:%M:%S",localtime(&t) ); 
	if (! fout.is_open())
	{
		return false;
	}
	//record
	fout<<std::setw(20)<<tmp<<"\t" <<this->iHighScore<<"\t"<<this->iScore<<std::endl;
	// close
	fout.close();
	return true;
}