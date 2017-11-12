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

#include <time.h>
#include <stdio.h>
#include <windows.h>


#include "TimeStamp.h"

TimeStamp::TimeStamp()
{
	start = 1;
	overflow = 0;

	isPaused = false;
}


void TimeStamp::Stamp()
{
	if(isPaused)
		UnPause();

	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
    prevTime = (double) time.QuadPart / (double) freq.QuadPart;

    //overflow = 0;
    if(start == 1)
    {
    	start = 0;
    	startTime = prevTime;
    }
}


double TimeStamp::TotalElapsed()
{
	if(start == 1)
	{
	   start = 0;
	   return 0; 
	}
	
	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
	double currTime = (double)time.QuadPart / (double) freq.QuadPart;

	return currTime-startTime;
}


double TimeStamp::Elapsed() 
{
	if(start == 1)
	{
	   start = 0;
	   return 0;
 
	}
	

	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
	double currTime = (double)time.QuadPart / (double) freq.QuadPart;
	
	double elapsed;
	if(isPaused)
	{
		UnPause();
		elapsed = currTime-prevTime;
		Pause();
	}
	else
		elapsed = currTime-prevTime;

	return elapsed;
}

//returns the # of frames that have elapsed since the last "stamp"
//frameTime is the time per frame in milliseconds
//factor is the scaling factor used to speed and slow the timer
int TimeStamp::ElapsedFrames(double frameTime, double factor)
{
  double total =  ((Elapsed() / (frameTime/1000)) + overflow)*factor;
  int result = (int) total;
  overflow = total - result;
  
  return result;
}

//allow timer to be pauses in between "stamps"
void TimeStamp::Pause()
{
	if(isPaused)
		return;

	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);

	pauseTime = (double)time.QuadPart / (double) freq.QuadPart;
	isPaused = true;
}

//unpause the timer...
void TimeStamp::UnPause()
{
	if(!isPaused)
		return;

	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
	double currTime = (double)time.QuadPart / (double) freq.QuadPart;

	prevTime += currTime - pauseTime;
	isPaused = false;
}