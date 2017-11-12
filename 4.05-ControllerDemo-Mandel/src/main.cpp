/*
ControllerDemo: 
A demonstration of a PD controller based pose controller for Game Programming Gems 5
Open Dynamics Engine, GLUT, and OpenGL are used in this implementation...
Copyright (C) 2004  Michael J Mandel  
Contact the author at mmandel@gmail.com with any questions/comments
*/


#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#include "glut.h"
#include "ODEObject.h"
#include "ODEBox.h"
#include "ODESimulation.h"
#include "ODEHuman.h"
#include "WinGLFont.h"
#include "TimeStamp.h"

ODESimulation *gSimulator;
ODEHuman *gRagdoll;

struct Camera {
  double zoom;
  //euler angle rotation
  double rx;
  double ry;
  double rz;
  //translation
  double tx;
  double ty;
  double tz;
};

Camera cam;

TimeStamp worldTimer;

WinGLFont *glText;
int gWidth, gHeight;

bool displayHelpScreen = false;

enum DampingState
{
	CRITICAL_DAMP = 0,
	UNDER_DAMP,
	OVER_DAMP
};
DampingState dampingState = CRITICAL_DAMP;

int selectedBody = -1;
TimeStamp selectedTimer;

void ApplyCameraTransform()
{
    glTranslated(cam.tx, cam.ty, cam.tz);

    glRotated(cam.rx, 1.0, 0.0, 0.0);
    glRotated(cam.ry, 0.0, 1.0, 0.0); 
	glRotated(cam.rz, 0.0, 0.0, 1.0);  
    
	glScaled(cam.zoom, cam.zoom, cam.zoom);
}

void init (void)
{
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glEnable ( GL_COLOR_MATERIAL );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	cam.zoom = 1;
	cam.rx = 13;
	cam.ry = 0;
    cam.rz = 0;    
    cam.tx = 0;
    cam.ty = 0;
    cam.tz = 0;

	//set up some lights
    GLfloat white[] = {1., 1., 1., 1.};
	GLfloat light0_position[] = {1., 1., 5., 0.};
    GLfloat light1_position[] = {-3., 1., -1., 0.};
    GLfloat light2_position[] = {1., 8., -2., 0.};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);	
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
    glEnable(GL_LIGHT2);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);	
}

void reshape (int w, int h)
{
  gWidth = w;
  gHeight = h;

  glViewport     ( 0, 0, w, h );
  glMatrixMode   ( GL_PROJECTION );  
  glLoadIdentity ( );                
  if ( h==0 )
     gluPerspective ( 80, ( float ) w, .50, 1000.0 );
  else
     gluPerspective ( 80, ( float ) w / ( float ) h, .50, 1000.0 );
  glMatrixMode   ( GL_MODELVIEW ); 
  glLoadIdentity ( );   
}

//primary draw function
void display()
{
	//select the appropriate limb
	if(selectedBody >= 0)
	{
		if((selectedTimer.Elapsed() <= 5.0) && (selectedBody >= 0))
			gRagdoll->GetBody(selectedBody)->SetColor(.1, .8, .1);
		else
			gRagdoll->GetBody(selectedBody)->SetColor(.8, .1, .1);
	}
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0,5.0f,-65.0f);

	ApplyCameraTransform();

	//update the damping gains
	if(gRagdoll->isControllerEnabled())
	{
		switch(dampingState)
		{
			case CRITICAL_DAMP:
				for(int i = 0 ; i < NUM_BODY_PARTS ; i++)
					gRagdoll->ScalePDDamperGain(i, 1.0f);
			break;
			case UNDER_DAMP:
				for(int i = 0 ; i < NUM_BODY_PARTS ; i++)
					gRagdoll->ScalePDDamperGain(i, .25f);
			break;
			case OVER_DAMP:
				for(int i = 0 ; i < NUM_BODY_PARTS ; i++)
					gRagdoll->ScalePDDamperGain(i, 3.5f);
			break;
			default: break;
		}
	}

	//step the simulation and render the results
	//a constant time step is more stable
	gSimulator->Update(1.0f / 50.0f);
	gSimulator->Render();

	//on screen instructions
	if(displayHelpScreen)
	{
		glColor3f(23.0/255.0, 28.0/255.0, 224.0/255.0);
		glText->glPrintf(5,gHeight - glText->fontSize(),"Pose Controller Demo v1.0");
		glText->glPrintf(5,gHeight - 2*glText->fontSize() - 5,"--------------------------");

		glText->glPrintf(5,gHeight - 4*glText->fontSize() - 10,"Camera Controls:");
		glText->glPrintf(5,gHeight - 5*glText->fontSize() - 15,"Left Button + Mouse Movement - Rotation");
		glText->glPrintf(5,gHeight - 6*glText->fontSize() - 20,"Middle Button + Mouse Movement - Translation");
		glText->glPrintf(5,gHeight - 7*glText->fontSize() - 25,"Right Button + Up/Down Mouse Movement - Zoom");
	
		glText->glPrintf(5,gHeight - 9*glText->fontSize() - 25,"Press 'c' to toggle controller");
		glText->glPrintf(5,gHeight - 10*glText->fontSize() - 30,"When controller is enabled:");
		glText->glPrintf(5,gHeight - 11*glText->fontSize() - 35,"Press '1' for near critical damping");
		glText->glPrintf(5,gHeight - 12*glText->fontSize() - 40,"Press '2' for under damping (oscillating response)");
		glText->glPrintf(5,gHeight - 13*glText->fontSize() - 45,"Press '3' for over damping (slow response)");

		glText->glPrintf(5,gHeight - 15*glText->fontSize() - 45,"Interacting With Character:");
		glText->glPrintf(5,gHeight - 16*glText->fontSize() - 50,"Use arrow keys to select a body part");
		glText->glPrintf(5,gHeight - 17*glText->fontSize() - 55,"Press 'f' to apply a force at the selected the body part");

		glText->glPrintf(5,15 + glText->fontSize(),"Press ESC to quit");
		glText->glPrintf(5, 10,"Press 'h' to remove this screen...");
	}
	else
	{
		glColor3f(23.0/255.0, 28.0/255.0, 224.0/255.0);
		glText->glPrintf(5,gHeight - glText->fontSize(),"Press 'h' to toggle instructions...");
	
		//include some status text
		if(gRagdoll->isControllerEnabled())
		{
			glText->glPrintf(5, 10,"Controller Enabled (press 'c' to disable)");
			if(dampingState == CRITICAL_DAMP)
				glText->glPrintf(5,15 + glText->fontSize(), "Critical Damping Enabled (press '2' or '3')");
			else if(dampingState == UNDER_DAMP)
				glText->glPrintf(5,15 + glText->fontSize(), "Under Damping Enabled (press '1' or '3')");
			else if(dampingState == OVER_DAMP)
				glText->glPrintf(5,15 + glText->fontSize(), "Over Damping Enabled (press '1' or '2')");
		}
		else
			glText->glPrintf(5, 10,"Controller NOT Enabled (press 'c')");

		if((selectedBody != -1) && (selectedTimer.Elapsed() <= 5.0))
			glText->glPrintf(5,20 + 2*glText->fontSize(), "Press 'f' to apply force at selected body part");

	}
	//lock framerate at 60hz
	while(worldTimer.Elapsed() < (1.0f / 60.0f))
	{  }
	glutSwapBuffers();
	worldTimer.Stamp();
}

//just redisplay constantly to let the console update...
void idle()
{
	glutPostRedisplay();
}

//handle special key events
void special(int key, int px, int py)
{
	//handle allowing interactive selection of body part with arrow keys
	if(selectedBody == -1)
		selectedBody = lowerback;
	else
	{
		switch (key)
		{
			case GLUT_KEY_UP: 
				if((selectedBody >= lowerback) && (selectedBody < head))
					selectedBody++;
				else if(selectedBody == lfoot)
					selectedBody = ltibia;
				else if(selectedBody == rfoot)
					selectedBody = rtibia;
				else if(selectedBody == ltibia)
					selectedBody = lfemur;
				else if(selectedBody == rtibia)
					selectedBody = rfemur;
				else if(selectedBody == lfemur)
					selectedBody = lhipjoint;
				else if(selectedBody == rfemur)
					selectedBody = rhipjoint;
				else if(selectedBody == rhipjoint)
					selectedBody = lowerback;
				else if(selectedBody == lhipjoint)
					selectedBody = lowerback;
				else if((selectedBody > rclavicle) && (selectedBody <= rradius))
					selectedBody--;
				else if((selectedBody > lclavicle) && (selectedBody <= lradius))
					selectedBody--;
			break;
			case GLUT_KEY_DOWN: 
				if((selectedBody > lowerback) && (selectedBody <= head))
					selectedBody--;
				else if(selectedBody == lowerback)
					selectedBody = rhipjoint;
				else if(selectedBody == ltibia)
					selectedBody = lfoot;
				else if(selectedBody == rtibia)
					selectedBody = rfoot;
				else if(selectedBody == lfemur)
					selectedBody = ltibia;
				else if(selectedBody == rfemur)
					selectedBody = rtibia;
				else if(selectedBody == lhipjoint)
					selectedBody = lfemur;
				else if(selectedBody == rhipjoint)
					selectedBody = rfemur;
				else if((selectedBody >= rclavicle) && (selectedBody < rradius))
					selectedBody++;
				else if((selectedBody >= lclavicle) && (selectedBody < lradius))
					selectedBody++;
			break;
			case GLUT_KEY_LEFT: 
				if(selectedBody == lowerneck)
					selectedBody = rclavicle;
				else if(selectedBody == lowerback)
					selectedBody = rradius;
				else if(selectedBody == lclavicle)
					selectedBody = lowerneck;
				else if((selectedBody >= rclavicle) && (selectedBody < rradius))
					selectedBody++;
				else if((selectedBody > lclavicle) && (selectedBody <= lradius))
					selectedBody--;	
				else if(selectedBody == lhipjoint)
					selectedBody = rhipjoint;
				else if(selectedBody == ltibia)
					selectedBody = rtibia;
				else if(selectedBody == lfemur)
					selectedBody = rfemur;
				else if(selectedBody == lfoot)
					selectedBody = rfoot;
			break;
			case GLUT_KEY_RIGHT: 
				if(selectedBody == lowerneck)
					selectedBody = lclavicle;
				else if(selectedBody == lowerback)
					selectedBody = lradius;
				else if(selectedBody == rclavicle)
					selectedBody = lowerneck;
				else if((selectedBody >= lclavicle) && (selectedBody < lradius))
					selectedBody++;	
				else if((selectedBody > rclavicle) && (selectedBody <= rradius))
					selectedBody--;	
				else if(selectedBody == rhipjoint)
					selectedBody = lhipjoint;
				else if(selectedBody == rtibia)
					selectedBody = ltibia;
				else if(selectedBody == rfemur)
					selectedBody = lfemur;
				else if(selectedBody == rfoot)
					selectedBody = lfoot;
			break;
		}
	}

	//start timer that will indicate the selected joint for a small time period
	selectedTimer.Stamp();
	//reset the selection
	for(int i = 0 ; i < NUM_BODY_PARTS ; i++)
		gRagdoll->GetBody(i)->SetColor(.8, .1, .1);
}

//handle incoming key events and send to console
void key(unsigned char inkey, int px, int py)
{
	//ODEBox *box;
	//float u;
	float f;
	switch (inkey)
	{
		case 27:	//escape
			exit ( 0 );  
		break;  
		case 'h':
			displayHelpScreen = !displayHelpScreen;
		break;
		//leaving this out since applying forces directly is more useful...
		//feel free to play around with this
		/*case 'b': //launch a random box at the character
			ODEVector size;
			size[0] = size[1] = size[2] = 6.0;
			box = new ODEBox(size, gSimulator);
			box->SetMass(5000);
			ODEPosition pos;
			u = (float) rand() / (float) RAND_MAX;
			pos[0] = -20 + u*55;
			pos[1] = -30 + u*90;
			pos[2] = 100.0f;
			box->SetPos(pos);
			ODEVector vel;
			vel[0] = 0;
			vel[1] = 0;
			vel[2] = -10;
			box->SetVelocity(vel);

		break;*/
		case 'c': //enable the controller
			gRagdoll->EnableController(!gRagdoll->isControllerEnabled());
			gRagdoll->SetControllerState(PDSTATE_BASE_POSE);

			if(gRagdoll->isEnabled())
				dampingState = CRITICAL_DAMP;
		break;
		case '1': //enable near critical damping
			dampingState = CRITICAL_DAMP;
		break;
		case '2': //enable under damping
			dampingState = UNDER_DAMP;
		break;
		case '3': //enable over damping
			dampingState = OVER_DAMP;
		break;
		case 'f': //apply force at selected joint
			if(selectedBody == -1)
				selectedBody = lowerback;

			ODEVector force;
			f = -75*gRagdoll->GetBody(selectedBody)->GetMass();
			if(f > -225)
				f = -225;
			force[0] = force[1] = 0; force[2] = f;
			gRagdoll->AddForceToBody(selectedBody, force);
			
			selectedTimer.Stamp();	
		break;
		default:      
			break;
	}

}


static int left_down = 0;
static int middle_down = 0;
static int right_down = 0;
static int prev_x = 0;
static int prev_y = 0;

void mouse_motion(int x, int y)
{
    
	int delta_x= x - prev_x;
    int delta_y= y - prev_y; 

	if ((left_down && !middle_down && !right_down))
	{
		if(abs(delta_x) > abs(delta_y))
			cam.ry += (GLdouble) (delta_x*.35);          
        else
			cam.rx += (GLdouble) (delta_y*.35);	 
	}
	else if((right_down && !middle_down && !left_down))
	{
        if(abs(delta_y) > abs(delta_x))
        {
            cam.zoom -= (GLdouble) delta_y/100.0;
            if(cam.zoom < 0.) cam.zoom = 0;
		}
	}
	else if((middle_down && !left_down && !right_down))
	{
         cam.tx += (GLdouble) (delta_x/10.0);
         cam.ty -= (GLdouble) (delta_y/10.0); 
	}	

	prev_x = x;
	prev_y = y;
}

void mouse (int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		prev_x = x;
		prev_y = y;
	}

	if((state == GLUT_DOWN) && (button == GLUT_MIDDLE_BUTTON))
		middle_down = 1;

	if((state == GLUT_UP) && (button == GLUT_MIDDLE_BUTTON))
		middle_down = 0;

	if((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON))
		right_down = 1;

	if((state == GLUT_UP) && (button == GLUT_RIGHT_BUTTON))
		right_down = 0;

	if((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON))
		left_down = 1;

	if((state == GLUT_UP) && (button == GLUT_LEFT_BUTTON))
		left_down = 0;
}

int main (int argc, const char * argv[])
{
    glutInit(&argc, (char **)argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition (300, 50);
	glutInitWindowSize (800, 600);
	char *title = "Game Programming Gems 5: Feedback Controllers Demo by Michael Mandel";
	glutCreateWindow(title);

    init(); // standard GL init

    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
	glutKeyboardFunc (key);
	glutSpecialFunc (special);
	glutMouseFunc (mouse);
	glutMotionFunc (mouse_motion);
	glutIdleFunc(idle);

	//create the ODE simulator
	gSimulator = new ODESimulation();
	//create ragdoll character
	ODEPosition pos;
	pos[0] = pos[1] = pos[2] = 0.0f;
	gRagdoll = new ODEHuman(pos,gSimulator);

	HWND hWnd = FindWindow("GLUT", title);
	glText = new WinGLFont(hWnd, "Courier", 18);

	worldTimer.Stamp();

    glutMainLoop();

	delete gRagdoll;
	delete gSimulator;

	return 0;
}
