//WinGLFont.cpp
//--------------------** Description ** -----------------------------------------------
// An OpenGL based font class.  Note this only works on the Windows platform
// Modified from Nehe Tutorial 13 (http://nehe.gamedev.net/tutorials/lesson.asp?l=13)
//-------------------------------------------------------------------------------------
//Copyright 2002 Michael J Mandel
/*
History:
7/9/02 - created MJM
*/

#include "WinGLFont.h"

WinGLFont::WinGLFont(HWND window, char* fontname, int size)
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-size,							// Height Of Font (negative means char. height)
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight (0-1000, FW_NORMAL, etc)
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision (forcing truetype fonts)
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality (DEFAULT, NONANTIALIASED, etc)
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						fontname);					// Font Name
	HDC tempHDC = ::GetDC(window);
	oldfont = (HFONT)SelectObject(tempHDC, font);           // Selects The Font We Want
	//note: I could generate all 256 chars below if I want
	wglUseFontBitmaps(tempHDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(tempHDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font

	fSize = size;
}

WinGLFont::~WinGLFont()
{
	glDeleteLists(base, 96);
}


//printf style print function
//NOTE: coordinates start from bottom left
void WinGLFont::glPrintf(int x, int y, const char *fmt, ...)	
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glDisable(GL_DEPTH_TEST); //causes text not to clip with geometry
	//position text correctly...
	
	// This saves our transform (matrix) information and our current viewport information.
	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
	// Use a new projection and modelview matrix to work with.
	glMatrixMode( GL_PROJECTION );				
	glPushMatrix();									
	glLoadIdentity();								
	glMatrixMode( GL_MODELVIEW );					
	glPushMatrix();										
	glLoadIdentity();									
	//create a viewport at x,y, but doesnt have any width (so we end up drawing there...)
	glViewport( x - 1, y - 1, 0, 0 );					
	//This actually positions the text.
	glRasterPos4f( 0, 0, 0, 1 );
	//undo everything
	glPopMatrix();										// Pop the current modelview matrix off the stack
	glMatrixMode( GL_PROJECTION );						// Go back into projection mode
	glPopMatrix();										// Pop the projection matrix off the stack
	glPopAttrib();										// This restores our TRANSFORM and VIEWPORT attributes
	
	//glRasterPos2f(x, y);
	
	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
	glEnable(GL_DEPTH_TEST);
}