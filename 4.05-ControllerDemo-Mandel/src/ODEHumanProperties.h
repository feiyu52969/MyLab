//ODEHumanProperties.h
//--------------------** Description ** -----------------------------------------------
// Defines all the data structures and raw data representing the simulated human.
// This is used by an ODEHuman object to set up the bodies, joints, and controller properties.
//-------------------------------------------------------------------------------------
//Copyright 2004 Michael J Mandel
/*
History:
10/15/04 - created MJM
*/

#include "ODEObject.h"

//each body part is referenced with the IDs below
enum BoneID	
{
	lhipjoint=0, lfemur, ltibia, lfoot, //left leg
	rhipjoint, rfemur, rtibia, rfoot, //right leg
	lowerback, upperback, thorax, lowerneck, upperneck, head, //spine + head
	lclavicle, lhumerus, lradius, //left arm
	rclavicle, rhumerus, rradius,//right arm
	NUM_BODY_PARTS
};

//simulated joints will be either hinge (1-DOF) or universal (2-DOF)
enum odeJointType
{
	JOINT_TYPE_HINGE = 0,
	JOINT_TYPE_UNIVERSAL
};

//properties related to creating a part of the body
struct odeBodyProperty
{
	BoneID boneID;

	//specified whether the body (and any possible joints its connected to) are enabled
	bool enabled;

	ODEVector size;
	ODEPosition pos;
	ODERotation rot;

	float mass;
};

//joint properties used to connect two body parts
struct odeJointProperty
{
	//inboard and outboard body parts connected by the joint
	BoneID attachedBody;
	BoneID parentBody;
	//specifies whether attachedBody is connected to the world instead of another body part
	bool attachToWorld;

	odeJointType jointType;

	//joint range of motion contraints for each axis (hinge joint only defined one range)
	float loJointRange1, highJointRange1;
	float loJointRange2, highJointRange2;

	//anchor position of the joint
	ODEPosition jointAnchor;
	//axes that the joint rotates about (hinge joint only defines one axis)
	ODEVector rotAxis1;
	ODEVector rotAxis2;
};

//set of controller gains for a particular joint
struct odeControlProperty
{
	BoneID boneID;

	bool isControllable;
	
	//ks and kd parameters for each axis (ks2, kd2 not applicable to hinge joints)
	float ks1, kd1;
	float ks2, kd2;
};

//controller states used by the demo pose controller
enum PDControlState
{
	PDSTATE_NONE = 0,
	PDSTATE_BASE_POSE,
	PDSTATE_ARMS_FORWARD_LEGS_TOGETHER,
	PDSTATE_ARMS_DOWN_KNEES_BENT,
};

//store target pose for controller (up to two DOFs per bone)
typedef float ControllerPose[2*NUM_BODY_PARTS];

//expose arrays of properties representing the human to other objects
extern odeBodyProperty bodyProps[];
extern odeJointProperty jointProps[];
extern odeControlProperty controlPropsDefault[];
