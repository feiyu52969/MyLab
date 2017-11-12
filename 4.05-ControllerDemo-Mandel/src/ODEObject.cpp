//ODEObject.cpp
//--------------------** Description ** -------------------------------
// An object that is simulated by the ODE simulation library
// Various primitives will derive from this class
//-----------------------------------------------------------------
//Copyright 2004 Michael Mandel
/*
History:
10/9/04 - created MJM
*/

#include "ODEObject.h"
#include "ODESimulation.h"

//all ODE objects added to this vector
std::vector<ODEObject*> ODEObjects;

ODEObject::ODEObject(ODESimulation *sim, bool hasBody)
: mEnabled(true), mBodyID(0), mGeomID(0),  mHasBody(hasBody), mRender(true), mID(rand()), mSimulator(sim)
{
	//add to list of simulated objects
	ODEObjects.push_back(this);

	if(mHasBody && mSimulator)
	{
		mBodyID = dBodyCreate(mSimulator->GetWorldID());
		SetEnabled(mEnabled);
	}

	//geom primitive MUST be created in subclasses
}

ODEObject::~ODEObject()
{

	if (mHasBody && mBodyID)
	{
		dBodyDestroy(mBodyID);
	}

	if(mGeomID)
	{
		dGeomDestroy(mGeomID);
	}
	
	//remove from list of simulated objects
	std::vector<ODEObject*>::iterator it2;
	for(it2 = ODEObjects.begin() ; it2 != ODEObjects.end() ; it2++)
	{
		if((*it2) == this)
		{
			ODEObjects.erase(it2);
			break;
		}
	}
}

void ODEObject::SetEnabled(bool b)
{ 
	mEnabled = b;

	//enable simulation of rigid body here...
	if(b)
	{
		if(mHasBody)
			dBodyEnable(mBodyID);

		if(mGeomID)
			dGeomEnable(mGeomID);
	}
	else
	{
		if(mHasBody)
			dBodyDisable(mBodyID);

		if(mGeomID)
			dGeomDisable(mGeomID);
	}

}

//setting the position manually of an ODE object
void ODEObject::SetPos(const ODEPosition& pos)
{
	dGeomSetPosition(GetGeomID(), WORLD_TO_SIM(pos[0]), WORLD_TO_SIM(pos[1]), WORLD_TO_SIM(pos[2]));

	//for stability...
	if(HasBody())
	{
		dBodySetLinearVel(GetBodyID(), 0, 0, 0);
	}	
}

//setting the quaternion rotation an ODE object
void ODEObject::SetRot(const ODERotation& rot)
{

	//load in the [w, x, y, z] order that ODE expects...
	dQuaternion dQuat;
	dQuat[0] = rot[3];
	dQuat[1] = rot[0];
	dQuat[2] = rot[1];
	dQuat[3] = rot[2];

	dGeomSetQuaternion(GetGeomID(), dQuat);
	
	//for stability...
	if(HasBody())
	{
		dBodySetAngularVel(GetBodyID(), 0, 0, 0);
	}	
}

//get position of simulated object
void ODEObject::GetPos(ODEPosition &p) const
{
	// getting body position
	const dReal* bodyPosition;
	bodyPosition = dGeomGetPosition(GetGeomID());

	//Set Position
	p[0] = SIM_TO_WORLD(bodyPosition[0]);
	p[1] = SIM_TO_WORLD(bodyPosition[1]);
	p[2] = SIM_TO_WORLD(bodyPosition[2]);
}

void ODEObject::GetRot(ODERotation &rot) const
{
	dQuaternion q;
	dGeomGetQuaternion(GetGeomID(), q);
	
	//ODE returns quaternions as [w x y z]
	//and this demo code expects [x y z w]
	//also need to invert the quaternion returned by ODE
	rot[0] = q[3]; 
	rot[1] = -q[0];
	rot[2] = -q[1];
	rot[3] = -q[2];
}

bool ODEObject::Update()
{
	if(mEnabled)
	{
		//nothing much to do here...

		//if rendering a 3D object associated with this simulated object, its position
		//and rotation could be extracted from the simulation here
	}

	return true;	
}

void ODEObject::SetVelocity(const ODEVector &velocity)
{
	if (mHasBody)
	{
		dBodySetLinearVel(mBodyID, velocity[0], velocity[1], velocity[2]);
	}
}

void ODEObject::DisableCollisionsWith(ODEObject *e)
{
	std::vector<ODEObject *>::iterator it;

	for(it = mNoCollideList.begin(); it != mNoCollideList.end() ; it++)
	{
		if(e == (*it))
			return;
	}

	mNoCollideList.push_back(e);
}

bool ODEObject::CanCollideWith(ODEObject *e) const
{
	std::vector<ODEObject *>::const_iterator it;

	for(it = mNoCollideList.begin();  it != mNoCollideList.end() ; it++)
	{
		if(e == (*it))
			return false;
	}

	return true;
}

void ODEObject::AddForce(const ODEPosition &f)
{
	if(mHasBody)
		dBodyAddForce(mBodyID, (dReal) f[0], (dReal) f[1], (dReal) f[2]);
}
