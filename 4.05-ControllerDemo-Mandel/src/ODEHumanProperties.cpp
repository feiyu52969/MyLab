//ODEHumanProperties.cpp
//--------------------** Description ** -----------------------------------------------
// Defines all the data structures and raw data representing the simulated human.
// This is used by an ODEHuman object to set up the bodies, joints, and controller properties.
//-------------------------------------------------------------------------------------
//Copyright 2004 Michael J Mandel
/*
History:
10/15/04 - created MJM
*/

#include "ODEHumanProperties.h"

#pragma warning( disable : 4305 )

//specifies size, position, orientation, and mass of each body part in the simulated human
odeBodyProperty bodyProps[] = {
	//left leg
	{lhipjoint, true, {4.701126, 4.701126, 6.183650}, {2.210537, -1.881518, 1.064327}, {0.371142, 0.436043, 0.000000, 0.819829}, 3.0f},
	{lfemur, true, {4.819100, 4.819100, 16.384600}, {7.223005, -11.461282, 2.128654}, {0.696363, 0.122787, 0.122787, 0.696363}, 6.05098068f},
	{ltibia, true, {4.515653, 4.515653, 17.012524}, {12.930611, -27.142811, 1.703518}, {0.713552, 0.125818, 0.119679, 0.678738}, 3.54112135f},
	{lfoot, true, {4.082203, 4.082203, 5.017975}, {16.122274, -35.911842, 3.643932}, {0.166501, 0.029359, 0.171148, 0.970629}, 1.11162485f},
	//right leg
	{rhipjoint, true, {4.659840, 4.659840, 6.097925}, {-2.150189, -1.881512, 1.064323}, {0.375683, -0.429330, 0.000000, 0.821303}, 3.0f},
	{rfemur, true, {4.597451, 4.597451, 16.850399}, {-7.181965, -11.680123, 2.128645}, {0.696363, -0.122787, -0.122787, 0.696363}, 5.93510880f},
	{rtibia, true, {4.337556, 4.337556, 17.350225}, {-13.026905, -27.738977, 1.695070}, {0.713552, -0.125818, -0.119679, 0.678738}, 3.51998805f}, 
	{rfoot, true, {3.977424, 3.977424, 4.841075}, {-16.215303, -36.499031, 3.590887}, {0.135130, -0.023827, -0.172005, 0.975491}, 1.12088208f},
	//spine + head
	{lowerback, true, {4.073971, 4.073971, 5.003900}, {-0.031552, 2.493519, -0.202789}, {-0.735145, -0.009302, 0.000000, 0.677843}, 13.40858920f},
	{upperback, true, {4.014784, 4.014784, 4.903600}, {-0.003502, 7.432600, -0.241279}, {-0.682800, 0.016641, 0.000000, 0.730413}, 12.0f},
	{thorax, true, {4.066469, 4.066469, 4.991100}, {0.137105, 12.358345, 0.187403}, {-0.668244, 0.021825, 0.000000, 0.743619}, 10.0f},
	{lowerneck, true, {3.375753, 3.375753, 3.905550}, {0.300635, 16.769709, 0.729377}, {-0.654324, 0.027961, 0.000000, 0.755695}, 5.87688157f},
	{upperneck, true, {3.417371, 3.417371, 3.966475}, {0.314781, 20.627439, 0.541177},  {-0.785273, -0.027871, 0.000000, 0.618519}, 3.0f},
	{head, true, {3.450344, 3.450344, 4.015100}, {0.227126, 24.552254, -0.116532}, {-0.740099, -0.007140, 0.000000, 0.672457}, 5.87688157f},
	//left arm
	{lclavicle, true, {5.311928, 5.311928, 7.675075}, {3.854127, 16.065308, 0.418672}, {-0.227028, 0.672882, 0.000000, 0.704048}, 1.5f},
	{lhumerus, true, {5.942909, 5.942909, 11.028575}, {13.004432, 17.292088, 0.385557}, {-0.612372, 0.353553, -0.612372, 0.353553}, 2.21222229f}, 
	{lradius, true, {5.431472, 5.431472, 8.043400}, {22.535393, 17.392588, 0.559629}, {-0.603343, 0.338135, -0.621019, 0.368751}, 1.10030929f}, 
	//right arm
	{rclavicle, true, {5.237526, 5.237526, 7.463150}, {-3.247896, 16.217402, 0.350926}, {-0.264890, -0.665842, 0.000000, 0.697484}, 1.5f},
	{rhumerus, true, {5.949380, 5.949380, 11.344575}, {-12.386189, 17.596273, 0.250065}, {-0.612372, -0.353553, 0.612372, 0.353553}, 2.21222229f},
	{rradius, true, {5.410233, 5.410233, 7.975075}, {-22.041031, 17.695921, 0.422659}, {-0.603343, -0.338135, 0.621019, 0.368751}, 1.10030929f},
};

//specifies all the joints between each of the body parts and their properties
odeJointProperty jointProps[] = {
	//left leg
	{lhipjoint, lowerback, false, JOINT_TYPE_HINGE, 0, 0, 0, 0, {-0.000000,0.000001,-0.000000}, {0.619732,0.323668,-0.714962}, {0,0,0}},
	{lfemur, lhipjoint, false, JOINT_TYPE_UNIVERSAL, -165.0f * DEG_TO_RAD, 45.0f* DEG_TO_RAD, -120.0f * DEG_TO_RAD, 20.0f * DEG_TO_RAD, {4.421082,-3.763035,2.128654}, {0.939693,0.342019,-0.000001}, {0.000000,0.000000,1.000000}},
	{ltibia, lfemur, false, JOINT_TYPE_HINGE, 0.05f, 165.0f * DEG_TO_RAD, -M_PI, M_PI, {10.024943,-19.159527,2.128654}, {0.939693,0.342019,-0.000001}, {0,0,0}}, //left knee
	{lfoot, ltibia, false, JOINT_TYPE_HINGE, -.2, .2, -.2, .2, {15.836287,-35.126095,1.278382}, {0.939693,0.342020,0.000000}, {0,0,0}},
	//right leg
	{rhipjoint, lowerback, false, JOINT_TYPE_HINGE, 0, 0, 0, 0, {-0.000001,-0.000001,0.000001}, {0.631352,-0.322584,0.705220}, {0,0,0}},
	{rfemur, rhipjoint, false, JOINT_TYPE_UNIVERSAL, -165.0f * DEG_TO_RAD, 45.0f* DEG_TO_RAD, -120.0f * DEG_TO_RAD, 20.0f * DEG_TO_RAD, {-4.300385,-3.763023,2.128645}, {0.939693,-0.342019,0.000001}, {-0.000000,0.000000,1.000000}},
	{rtibia, rfemur, false, JOINT_TYPE_HINGE, 0.05f, 165.0f * DEG_TO_RAD, -M_PI, M_PI, {-10.063559,-19.597225,2.128645}, {0.939693,-0.342019,0.000001}, {0,0,0}}, //right knee
	{rfoot, rtibia, false, JOINT_TYPE_HINGE, -.2, .2, -.2, .2, {-15.990259,-35.880726,1.261497}, {0.939693,-0.342019,0.000000}, {0,0,0}},
	//spine + head
	{lowerback, upperback, false, JOINT_TYPE_HINGE, -45.0f * DEG_TO_RAD, 25.0f * DEG_TO_RAD, -M_PI, M_PI, {-0.063104,4.987037,-0.405578}, {0.999827,0.013677,0.012611}, {0,0,0}},
	{upperback, thorax, false, JOINT_TYPE_HINGE, -20.0f*DEG_TO_RAD, 10.0f*DEG_TO_RAD, 0,0, {0.056100,9.878162,-0.076980}, {0.999446,-0.022725,-0.024310}, {0,0,0}},
	{thorax, lowerneck, false, JOINT_TYPE_HINGE, -5.0f*DEG_TO_RAD,2.5f*DEG_TO_RAD,0,0, {0.218111,14.838529,0.451788}, {0.999047,-0.029170,-0.032460}, {0,0,0}},
	{lowerneck, upperneck, false, JOINT_TYPE_UNIVERSAL, -50.0f * DEG_TO_RAD, 89.0f * DEG_TO_RAD, -60.0f*DEG_TO_RAD, 60.0f*DEG_TO_RAD, {0.383159,18.700890,1.006970}, {0.998436,-0.036591,-0.042260}, {-0.036591,0.143717,-0.988942}},
	{upperneck, head, false, JOINT_TYPE_HINGE, -45.0f * DEG_TO_RAD, 45.0f * DEG_TO_RAD, 0, 0, {0.246403,22.553988,0.075381}, {0.998446,0.043773,0.034478}, {0,0,0}},
	{head, head, true, JOINT_TYPE_HINGE, -M_PI, M_PI, -M_PI, M_PI, {0.207849,26.550516,-0.308451}, {0.999898,0.010568,0.009602}, {0,0,0}},
	//left arm	
	{lclavicle, thorax, false, JOINT_TYPE_HINGE, 0,0,0,0, {0.218112,14.838528,0.451786}, {0.094455,-0.305528,-0.947487}, {0,0,0}},
	{lhumerus, lclavicle, false, JOINT_TYPE_UNIVERSAL, -40*DEG_TO_RAD, 120*DEG_TO_RAD, -85*DEG_TO_RAD, 85*DEG_TO_RAD, {7.490144,17.292086,0.385556}, {0.000000,-0.866026,0.500000}, {-0.000000,0.500000,0.866026}}, //l shoulder
	{lradius, lhumerus, false, JOINT_TYPE_HINGE, .05f, 120.0f*DEG_TO_RAD, 0,0, {18.518719,17.292088,0.385557}, {-0.000000,-0.866025,0.500000}, {0,0,0}}, //elbow
	//right arm
	{rclavicle, thorax, false, JOINT_TYPE_HINGE, 0,0,0,0, {0.218109,14.838530,0.451787}, {0.113305,0.352752,0.928832}, {0,0,0}},
	{rhumerus, rclavicle, false, JOINT_TYPE_UNIVERSAL, -40*DEG_TO_RAD, 120*DEG_TO_RAD, -85*DEG_TO_RAD, 85*DEG_TO_RAD, {-6.713902,17.596272,0.250064}, {0.000000,0.866026,-0.500000}, {-0.000000,-0.500000,-0.866026}},
	{rradius, rhumerus, false, JOINT_TYPE_HINGE, .05f, 120.0f*DEG_TO_RAD, 0,0, {-18.058479,17.596275,0.250065}, {-0.000000,0.866025,-0.500000}, {0,0,0}},
};

//Manually specify controller gains for near critically damped behavior.
//Computing the effective moment of interia, as detailed in the article,
//would eliminate all but one of these sets of gains.
odeControlProperty controlPropsDefault[] = {
	//left leg
	{lhipjoint, false, 0,0, 0,0}, //already constained not to move
	{lfemur, true, 60,15, 60,15},
	{ltibia, true, 10,2, 0,0}, 
	{lfoot, true, 3,1, 0,0},
	//right leg
	{rhipjoint, false, 0,0, 0,0}, //already constained not to move
	{rfemur, true, 60,15, 60,15},
	{rtibia, true, 10,2, 0,0}, 
	{rfoot, true, 3,1, 0,0},
	//spine + head
	{lowerback, true, 100,40, 0,0},
	{upperback, true, 40,15, 0,0},
	{thorax, true, 30,10, 0,0},
	{lowerneck, true, 30,10, 0,0}, 
	{upperneck, true, 25,4, 0,0}, 
	{head, true, 24,4, 0,0}, 
	//left arm
	{lclavicle, false, 0,0, 0,0}, //already constrained not to move
	{lhumerus, true, 40,12, 40,12}, 
	{lradius, true, 30,8, 0,0}, 
	//right arm
	{rclavicle, false, 0,0, 0,0}, //already constrained not to move
	{rhumerus, true, 40,12, 40,12},
	{rradius, true, 30,8, 0,0},
};