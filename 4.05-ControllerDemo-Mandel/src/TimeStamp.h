//timestamp.cpp
//--------------------** Description ** ---------------------------
// Implementation of basic timer that operates based on querying
// information since the last "stamp"
//-----------------------------------------------------------------
//Copyright 2001 Michael J Mandel
/*
History:
7/01/01 - created MJM
*/

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__


class TimeStamp
{
	public:
	TimeStamp();
	void Stamp();
	double Elapsed(); //in seconds since last "stamp"
	double TotalElapsed(); //total since first stamp
	int ElapsedFrames(double frameTime, double factor=1.0); //given target time in millisecs per frames

	//allow timer to be pauses in between "stamps"
	void Pause();
	//unpause the timer...
	void UnPause();

	private:
	int start;
	

	double prevTime, startTime;
	double pauseTime;
	bool isPaused;

	double overflow;

};

#endif