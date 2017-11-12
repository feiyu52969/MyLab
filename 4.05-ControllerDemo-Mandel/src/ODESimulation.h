//ODESimulation.cpp
//--------------------** Description ** -------------------------------
// Create basic ODE simulation and manage rendering and updating of simulated primitives
//-----------------------------------------------------------------
//Copyright 2004 Michael Mandel
/*
History:
10/9/04 - created MJM
*/

#ifndef _ODE_SIMULATION_H__
#define _ODE_SIMULATION_H__

#include <ode/ode.h>

//the simulator performs better when you give it objects of roughly unit size
//increase this to match the scale of the real-world objects being simulated
#define WORLD_SCALE 60.0f
#define SIM_TO_WORLD(x) (WORLD_SCALE*x)
#define WORLD_TO_SIM(x) (x / WORLD_SCALE)


class ODESimulation
{
	public:
	ODESimulation(dReal gravity = -9.8);
	~ODESimulation();
    
	//utility methods
	dWorldID GetWorldID();     
	dSpaceID GetSpaceID(); 	
	dJointGroupID GetJointGroupID();     
	void setGravity(dReal gravity); 	
	int numObjectsSimulated();     
	
	//update loop for the simulation
	void Update(dReal dt);     
	//render the currently simulated bodies
    void Render(); 	
	
	void RemoveAllObjects();

	private:
	void initWorld(dReal gravity);

	dReal mGravity;     
	dWorldID mWorldID;
    dSpaceID mCollisionSpaceID;
    dJointGroupID mJointGroupID; 	
	float totalRemainder;
};

#endif
