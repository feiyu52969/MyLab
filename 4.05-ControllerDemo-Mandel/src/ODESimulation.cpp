//ODESimulation.cpp
//--------------------** Description ** -------------------------------
// Create basic ODE simulation and manage rendering and updating of simulated primitives
//-----------------------------------------------------------------
//Copyright 2004 Michael Mandel
/*
History:
10/9/04 - created MJM
*/

#include "ODESimulation.h"
#include "ODEObject.h"

 // an oddity that is necessary to compile with MSDEV 6.0
 #if (_MSC_VER == 1200) && (WINVER < 0x0500)
	extern "C" long _ftol( double ); //defined by VC6 C libs
	extern "C" long _ftol2( double dblSource ) { return _ftol( 
 dblSource ); } 
#endif

//number of iterations used by the fast step function of ODE
#define STEP_ITERATIONS 10

//maximum # of contact points between two bodies
#define MAX_CONTACT_POINTS 16

void PotentialHitCallback(void *data, dGeomID o1, dGeomID o2);
void odeErrorHandler (int errnum, const char *msg, va_list ap);

ODESimulation::ODESimulation(dReal gravity) : mCollisionSpaceID(0), totalRemainder(0)
{
	mGravity = gravity;
	initWorld(gravity); 
}

ODESimulation::~ODESimulation()
{
	ODEObjects.clear();

	dJointGroupDestroy (mJointGroupID);
	dSpaceDestroy (mCollisionSpaceID);
	dWorldDestroy (mWorldID);

    dCloseODE();

}

void ODESimulation::initWorld(dReal gravity)
{
    mWorldID = dWorldCreate();
    dWorldSetGravity(mWorldID, 0, gravity, 0);

    //container for all the geoms (try using the hash or quad tree versions for performance increase)
    mCollisionSpaceID = dSimpleSpaceCreate(0);

    //joint group for adding and removing collision joints quickly
    mJointGroupID = dJointGroupCreate(0);    

	//Error Reduction Parameter: for joint constraints.  
	//0 gives "bouncy joints" and 1 gives hard contraints (unstable)
	//recommended values: .1 - .8 (.2 is default)
	//dWorldSetERP(mWorldID, (dReal)0.9);
	//dWorldSetERP(mWorldID, (dReal)0.1);

	//Constraint Force Mixing: Used t control softness of collisions
	//0 means hard contacts while 1 means more spongy
	//dWorldSetCFM(mWorldID, (dReal)0.1);
	dWorldSetCFM (mWorldID,1e-5);

	dSetErrorHandler(odeErrorHandler);
	dSetDebugHandler(odeErrorHandler);
	dSetMessageHandler(odeErrorHandler);
}

dWorldID ODESimulation::GetWorldID()
{
    return mWorldID;
}

dSpaceID ODESimulation::GetSpaceID()
{
    return mCollisionSpaceID;
}

dJointGroupID ODESimulation::GetJointGroupID()
{
	return mJointGroupID;
}

void ODESimulation::setGravity(dReal gravity)
{
    dWorldSetGravity(mWorldID, 0, gravity, 0);
}

int ODESimulation::numObjectsSimulated() { return (int)ODEObjects.size(); }

//update loop for the simulation
void ODESimulation::Update(dReal dt)
{
    //make sure time step is reasonable
    if (dt > 0.2 || dt <= 0)
    {
        return;
    }

	//don't do anything if no objects are being simulated
	if(ODEObjects.size() == 0)
		return;

    //Call collision detection and add contacts to contact joint group.
    dSpaceCollide(mCollisionSpaceID, this, &PotentialHitCallback);

    //Take a Simulation step
	//not using fast iterative solver because it appears less stable...
	//dWorldQuickStep(mWorldID, dt);
	dWorldStep(mWorldID, dt);

    //Remove all joints from the contact group.
    dJointGroupEmpty(mJointGroupID);

	//allow all simulated objects to update themselves (extract info from simulation)
	std::vector<ODEObject*>::iterator it;
	for(it = ODEObjects.begin() ; it != ODEObjects.end() ; it++)
		(*it)->Update();
}


//render appropriate simulated bodies
void ODESimulation::Render()
{
	std::vector<ODEObject*>::iterator it;
	for(it = ODEObjects.begin() ; it != ODEObjects.end() ; it++)
	{
		if((*it)->isEnabled())
			(*it)->Render();
	}
}

void ODESimulation::RemoveAllObjects()
{
	
	std::vector<ODEObject*>::iterator it;
	for(it = ODEObjects.begin() ; it != ODEObjects.end() ; it++)
	{
		delete (*it);
	}
	ODEObjects.clear();
}

//Collision Callback
void PotentialHitCallback(void *data, dGeomID o1, dGeomID o2)
{

	//check if these two should collide
	ODEObject *entity1 = (ODEObject*) dGeomGetData(o1);
	ODEObject *entity2 = (ODEObject*) dGeomGetData(o2);
	ODESimulation *simulator = entity1->simulator();
	if(entity1 && entity2)
	{
		if(!entity1->CanCollideWith(entity2) || !entity2->CanCollideWith(entity1))
			return;
	}


	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
	{ // colliding a space with someting
		dSpaceCollide2(o1, o2, data, &PotentialHitCallback);
		
		// now colliding all geoms internal to the space(s)
		if (dGeomIsSpace(o1))
		{
			dSpaceID o1_spaceID = (dSpaceID)o1; // this may not be valid
			dSpaceCollide(o1_spaceID, data, &PotentialHitCallback);
		}
		if (dGeomIsSpace(o2))
		{
			dSpaceID o2_spaceID = (dSpaceID)o2; // this may not be valid
			dSpaceCollide(o2_spaceID, data, &PotentialHitCallback);
		}
	}
	else
	{ // colliding two "normal" (non-space) geoms

		int num_cp; // number of contact points
		dContactGeom cp_array[MAX_CONTACT_POINTS];
		num_cp = dCollide(o1, o2, MAX_CONTACT_POINTS, cp_array, sizeof(dContactGeom));

		dBodyID o1BodyID = dGeomGetBody(o1);
		dBodyID o2BodyID = dGeomGetBody(o2);

		// filter out collisions between joined bodies (except for contact joints)
		if (o1BodyID !=0 && o2BodyID != 0)
			if (num_cp > 0 && dAreConnectedExcluding(o1BodyID, o2BodyID, dJointTypeContact))
				return;


			// add these contact points to the simulation
			for(int i=0; i<num_cp; i++)
			{
				dContact tempContact;// = new dContact;
				//tempContact.surface.mode = 0;

				//lots of ways to set friction properties...
				tempContact.surface.mode = dContactApprox1 | dContactSlip1 | dContactSlip2 | dContactSoftCFM;
				tempContact.surface.slip1 = (dReal).2;
				tempContact.surface.slip2 = (dReal).1;
				//tempContact.surface.mode = dContactApprox1;
				//tempContact.surface.mode = dContactBounce | dContactSoftCFM;
				tempContact.surface.bounce = 0.1;
				tempContact.surface.bounce_vel = (dReal)0.2;
				tempContact.surface.soft_cfm = (dReal)0.001;
				//tempContact.surface.mu = dInfinity;
				//tempContact.surface.mu2 = 0;

				//tempContact.surface.mu = 100;
				tempContact.geom = cp_array[i];


				dJointID j = dJointCreateContact(simulator->GetWorldID(), simulator->GetJointGroupID(), &tempContact);
				dJointAttach(j, dGeomGetBody(o1), dGeomGetBody(o2));
			}
	}

}

void odeErrorHandler (int errnum, const char *msg, va_list ap)
{
	char msgBuf[1024];
	vsprintf(msgBuf, msg, ap);
	printf("ODE Error #%d: %s", errnum, msgBuf);
}