//WinGLFont.h
//--------------------** Description ** -----------------------------------------------
// An OpenGL based font class.  Note this only works on the Windows platform
// Modified from Nehe Tutorial 13 (http://nehe.gamedev.net/tutorials/lesson.asp?l=13)
//-------------------------------------------------------------------------------------
//Copyright 2002 Michael J Mandel
/*
History:
7/9/02 - created MJM
*/

#ifndef WINGLFONT_H__
#define WINGLFONT_H__

#include <windows.h>
#include "glut.h"
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <string>

class WinGLFont
{
	public:
		WinGLFont(HWND window, char* fontname = "Courier New", int size = 24);
		~WinGLFont();
		
		//printf style function take position to print to as well
		//NOTE: coordinates start from bottom left
		void glPrintf(int x, int y, const char *fmt, ...);
		void glPrintf(int x, int y, const std::string fmt, ...){ glPrintf(x,y, fmt.c_str()); }

		int fontSize() { return fSize; }

	private:
		GLuint base; //base number for each display list
		int fSize;
};

#endif
