//ODEObject.h
//--------------------** Description ** -------------------------------
// represents a box primitive in an ODE simulation
//-----------------------------------------------------------------
//Copyright 2004 Michael Mandel
/*
History:
10/9/04 - created MJM
*/

#ifndef _ODE_BOX_OBJECT_H__
#define _ODE_BOX_OBJECT_H__

#include <vector>
#include "ODEObject.h"

class ODEBox : public ODEObject
{
	public:
		ODEBox(const ODEPosition &size, ODESimulation *sim,  bool hasBody = true);
		~ODEBox();

		virtual bool Update();
		virtual void Render();

		double* GetSize() { return (double*) mSize; }

		void SetMass(float m);
		float GetMass() { return mMass; }

		void SetColor(float cr, float cg, float cb) { r=cr; g=cg; b=cb; }

	protected:
	void DrawBox();

	ODEPosition mSize;
	float mMass;

	//render color
	float r, g, b;
};

#endif