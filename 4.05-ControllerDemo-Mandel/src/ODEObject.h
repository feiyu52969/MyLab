//ODEObject.h
//--------------------** Description ** -------------------------------
// An object that is simulated by the ODE simulation library
// Various primitives will derive from this class
//-----------------------------------------------------------------
//Copyright 2004 Michael Mandel
/*
History:
10/9/04 - created MJM
*/

//#pragma once
#ifndef _ODE_OBJECT_H__
#define _ODE_OBJECT_H__

#include <ode/ode.h>
#include <vector>

//list of all the currently simulated ode objects (updated dynamically)
class ODEObject;
extern std::vector<ODEObject*> ODEObjects;

class ODESimulation;

#define DEG_TO_RAD (M_PI / 180.0)

//quaternion rotation
typedef dQuaternion ODERotation;
//normal 3d position
typedef dVector3 ODEPosition;
typedef dVector3 ODEVector;

class ODEObject
{
	public:
		ODEObject(ODESimulation *sim, bool hasBody = true);
		virtual ~ODEObject();

		//enable/disable simulation of object
		virtual void SetEnabled(bool b);
		virtual bool isEnabled() const { return mEnabled; }

		//returns true if this ODE object has a rigid body associated with it
		//otherwise, the object is statically attached to the world
		bool HasBody() const { return mHasBody; }
		dBodyID GetBodyID() const { return mBodyID; }

		dGeomID GetGeomID() const { return mGeomID; }

		void AddForce(const ODEPosition &f);
		void SetVelocity(const ODEVector &v);

		//sets position/rotation of ODE body
		void SetPos(const ODEPosition& pos);
		void SetRot(const ODERotation& rot);
		
		//get position and/or rotation of simulated object
		void GetPos(ODEPosition &pos) const;
		void GetRot(ODERotation &rot) const;		

		void SetID(int id) { mID = id; }
		int GetID() const { return mID; }

		virtual bool Update();

		//primitives deriving from ODEObject must implement this
		virtual void Render() = 0;

		//allow collisions to be disabled between two bodies
		void DisableCollisionsWith(ODEObject *e);
		bool CanCollideWith(ODEObject *e) const;

		ODESimulation * simulator() { return mSimulator; }

	protected:
	bool mEnabled;


	dBodyID mBodyID;
	dGeomID mGeomID;

	bool mHasBody;

	bool mRender;

	int mID;

	std::vector<ODEObject *> mNoCollideList;
	
	ODESimulation *mSimulator;
	
};

#endif
