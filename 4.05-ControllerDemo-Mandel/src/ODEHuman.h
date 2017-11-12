//ODEHuman.h
//--------------------** Description ** -----------------------------------------------
// A set of joints and rigid boxes representing a human figure
//-------------------------------------------------------------------------------------
//Copyright 2004 Michael J Mandel
/*
History:
10/9/04 - created MJM
*/

#ifndef _ODEHUMAN_H__
#define _ODEHUMAN_H__

#include <vector>
#include "ODEObject.h"
#include "TimeStamp.h"
#include "ODEHumanProperties.h"

class ODEBox;
class ODESimulation;


class ODEHuman : public ODEObject
{
	public:
	ODEHuman(const ODEPosition &humanPos, ODESimulation *sim, bool hasBody = true);
	~ODEHuman();

	virtual void Render();
	virtual bool Update();

	//enable/disable all bodies and joints
	void SetEnabled(bool b);
	bool isEnabled() { return mEnabled; }

	//add a force to a particular joint
	void AddForceToBody(int bodyID, const ODEVector &force);

	//access an individual body part
	ODEBox * GetBody(int i) { 
			return rigidLinks[i];
	}

	//enable PD-controller
	void EnableController(bool b);
	bool isControllerEnabled() { return mControllerEnabled; }
	
	//activates/deactivates the PD-controller
	//NOTE: application must call SetControllerState() after enabling controller
	void SetControllerState(PDControlState s);
	PDControlState GetControllerState() {
		return mControllerState;
	}

	//allows controller gains to be scaled during simulation
	void ScalePDGains(int i, float f);
	void ScalePDSpringGain(int i, float f);
	void ScalePDDamperGain(int i, float f);

	private:
	bool mEnabled;

	//set the controller gains used by the controller (a copy is made)
	void SetControllerProperties(odeControlProperty *props);

	//creates all the joints and bodies for a human in the reference pose
	void CreateBodiesAndJoints(const ODEPosition &humanPos, bool allowAttachToWorld = true);

	std::vector<ODEBox*> rigidLinks;
	std::vector<dJointID> rigidJoints;

	//current target pose used by the controller
	ControllerPose mControllerPose;
	bool mControllerEnabled;
	//compute and apply controller torques
	void applyControlTorques();
	//called every update to determine whether state transitions are necessary
	void handleControllerStateTransitions();
	PDControlState mControllerState;
	TimeStamp mControllerStateTimer;

	//current controller gain specification (copy is kept to support scaling during runtime)
	odeControlProperty *mCurControllerProps;
	odeControlProperty mCurControllerPropsCopy[NUM_BODY_PARTS];
};

#endif