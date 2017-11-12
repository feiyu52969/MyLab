//ODEObject.cpp
//--------------------** Description ** -------------------------------
// represents a box primitive in an ODE simulation
//-----------------------------------------------------------------
//Copyright 2004 Michael Mandel
/*
History:
10/9/04 - created MJM
*/

#include "ODEBox.h"
#include "ODESimulation.h"
#include "glut.h"


ODEBox::ODEBox(const ODEPosition &size, ODESimulation *sim, bool hasBody)
: ODEObject(sim, hasBody), r(.8), g(.1), b(.1)
{
	mSize[0] = size[0]; mSize[1] = size[1]; mSize[2] = size[2];

	dSpaceID spaceID = 0;
	if(mSimulator)
		spaceID = mSimulator->GetSpaceID();
		
	mGeomID = dCreateBox(spaceID, WORLD_TO_SIM(mSize[0]), WORLD_TO_SIM(mSize[1]), WORLD_TO_SIM(mSize[2]));

	if (mHasBody)
	{
		dGeomSetBody(mGeomID, mBodyID);

		//multiply volume by 8 to get mass as a default (user may change this by calling SetMass())
		dReal newMass = WORLD_TO_SIM(mSize[0])*WORLD_TO_SIM(mSize[1])*WORLD_TO_SIM(mSize[2])*8;
		SetMass(newMass);
	}
	else
	{
		dGeomSetBody(mGeomID, 0);
	}

	SetEnabled(mEnabled);

	//allow things like the collision callback to access this object
	dGeomSetData(mGeomID, this);
}

ODEBox::~ODEBox()
{
	//both geom and body are destroyed in the super class destructor
}

//convert quaternion returned by simulation to a standard rotation matrix GL can use
void QuatToMatrix(const ODERotation &quat, float mat[4][4]) 
{
	float x, y, z, w;
	x = quat[0];
	y = quat[1];
	z = quat[2];
	w = quat[3];

  	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  	// calculate coefficients
  	x2 = x + x;
  	y2 = y + y;
  	z2 = z + z;
  	xx = x * x2;
  	xy = x * y2;
  	xz = x * z2;
  	yy = y * y2;
  	yz = y * z2;
  	zz = z * z2;
  	wx = w * x2;
  	wy = w * y2;
  	wz = w * z2;
  	mat[0][0] = 1.0f - (yy + zz);
  	mat[0][1] = xy - wz;
  	mat[0][2] = xz + wy;
  	mat[0][3] = 0.0f;
  	mat[1][0] = xy + wz;
  	mat[1][1] = 1.0f - (xx + zz);
  	mat[1][2] = yz - wx;
  	mat[1][3] = 0.0f;
  	mat[2][0] = xz - wy;
  	mat[2][1]= yz + wx;
  	mat[2][2] = 1.0f - (xx + yy);
  	mat[2][3] = 0.0f;
  	mat[3][0] = 0.0f;
  	mat[3][1] = 0.0f;
  	mat[3][2] = 0.0f;
  	mat[3][3] = 1.0f;
}

//render a box using OpenGL
void ODEBox::Render()
{
	glPushMatrix();

	ODEPosition pos;
	GetPos(pos);
	glTranslatef(pos[0],pos[1],pos[2]);

	ODERotation rot;
	GetRot(rot);
	float mat[4][4];
	QuatToMatrix(rot, mat);
	glMultMatrixf(&mat[0][0]);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	glEnable(GL_BLEND);

	glColor4f(1.0, .1, .1, .1);

	DrawBox();

	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);

	glPopMatrix();
}

void ODEBox::DrawBox()
{
	dReal halfx = mSize[0]/2;
	dReal halfy = mSize[1]/2;
	dReal halfz = mSize[2]/2;

	glEnable(GL_LIGHTING);
	glColor3f(r, g, b);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

	glBegin(GL_QUADS);
		//left
		glNormal3f(-1, 0, 0);
		glVertex3f(-halfx, -halfy, halfz);
		glVertex3f(-halfx, halfy, halfz);
		glVertex3f(-halfx, halfy, -halfz);
		glVertex3f(-halfx, -halfy, -halfz);
		//back
		glNormal3f(0, 0, -1);
		glVertex3f(-halfx, -halfy, -halfz);
		glVertex3f(-halfx, halfy, -halfz);
		glVertex3f(halfx, halfy, -halfz);
		glVertex3f(halfx, -halfy, -halfz);
		//right
		glNormal3f(1, 0, 0);
		glVertex3f(halfx, -halfy, -halfz);
		glVertex3f(halfx, halfy, -halfz);
		glVertex3f(halfx, halfy, halfz);
		glVertex3f(halfx, -halfy, halfz);
		//front
		glNormal3f(0, 0, 1);
		glVertex3f(halfx, -halfy, halfz);
		glVertex3f(halfx, halfy, halfz);
		glVertex3f(-halfx, halfy, halfz);
		glVertex3f(-halfx, -halfy, halfz);
		//top
		glNormal3f(0, 1, 0);
		glVertex3f(halfx, halfy, halfz);
		glVertex3f(halfx, halfy, -halfz);
		glVertex3f(-halfx, halfy, -halfz);
		glVertex3f(-halfx, halfy, halfz);
		//bottom
		glNormal3f(0, -1, 0);
		glVertex3f(-halfx, -halfy, halfz);
		glVertex3f(-halfx, -halfy, -halfz);
		glVertex3f(halfx, -halfy, -halfz);
		glVertex3f(halfx, -halfy, halfz);
	glEnd();

	glDisable(GL_LIGHTING);
}


bool ODEBox::Update()
{
	ODEObject::Update();

	return true;
}

void ODEBox::SetMass(float m)
{
	if(mHasBody)
	{
		dReal newMass = m;
		dMass mass;
		dBodyGetMass(mBodyID, &mass);
		dMassAdjust(&mass, newMass);

		mMass = m;
	}
}
