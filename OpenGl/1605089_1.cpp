#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
// #include <GL/glext.h>

#define pi (2*acos(0.0))

double cameraMove;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double uppAngle;
double cylinderAngle;
double rotateAngle;
bool intersect;
double x3, y3,z3;


struct point
{
	double x,y,z;
};

point u;//upDirection
point r;//rightDirection
point l;//lookDirection
point pos;//position

struct point crossProduct(struct point u, struct point v)
{
	struct point result;
	result.x = u.y*v.z - u.z*v.y;
	result.y = u.z*v.x - u.x*v.z;
	result.z = u.x*v.y - u.y*v.x;
 
	return result;
}



// The formula is: vector(v)*cos(angle)+ vector(vector(about)crossProduct vector(v))*sin(angle)
struct point rotation(struct point about, struct point v, double a){

	struct point scalar;
	struct point cross2;
	struct point result;

	scalar.x = v.x*(cos(a*pi/180));
	scalar.y = v.y*(cos(a*pi/180));
	scalar.z = v.z*(cos(a*pi/180));
    
	struct point cross = crossProduct(about, v); 

	cross2.x = cross.x*(sin(a*pi/180));
	cross2.y = cross.y*(sin(a*pi/180));
	cross2.z = cross.z*(sin(a*pi/180));

	result.x = scalar.x + cross2.x;
	result.y = scalar.y + cross2.y;
	result.z = scalar.z + cross2.z;
	
	return result;
}


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 300,0,0);
			glVertex3f(-300,0,0);

			glVertex3f(0,-300,0);
			glVertex3f(0, 300,0);

			glVertex3f(0,0,300);
			glVertex3f(0,0,-300);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, 0, a);
		glVertex3f( a, 0, -a);
		glVertex3f(-a,0, -a);
		glVertex3f(-a , 0, a);
	}glEnd();
}



void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	int color = 0;
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        
		
		for(j=0;j<slices;j++)
		{
			glColor3f(color,color,color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();

			color = 1- color;
		}
		
	}
}

void drawCylinder(double radius,int height,int slices,int stacks){

	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=height*sin(((double)i/(double)stacks)*(pi/2));
		r=radius;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	int color = 0;
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        
		
		for(j=0;j<slices;j++)
		{
			glColor3f(color,color,color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                // //lower hemisphere
                // glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				// glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				// glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				// glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();

			color = 1- color;
		}
		
	}

}

void drawUpperHemesphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	int color = 0;
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        
		
		for(j=0;j<slices;j++)
		{
			glColor3f(color,color,color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                
			}glEnd();

			color = 1- color;
		}
		
	}

}

void drawLowerHemisphere(double radius,int slices,int stacks){

	
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	int color = 0;
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        
		
		for(j=0;j<slices;j++)
		{
			glColor3f(color,color,color);
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();

			color = 1- color;
		}
		
	}

}

void drawReverseLowerHemisphere(double radius,int slices,int stacks){

	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		r = 2*radius -r;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	int color = 0;
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        
		
		for(j=0;j<slices;j++)
		{
			glColor3f(color,color,color);
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();

			color = 1- color;
		}
		
	}

}

void drawSS()
{
	// set the color of skuare
	// glColor3f(1,0,0);
    // drawSquare(20);
	glRotatef(angle,0,0,1);//for rotating in respect to z axis
	glRotatef(90,1,0,0); //for 90 degree rotated dorakata
	drawUpperHemesphere(40,100,70); //drawn by spliting as sir shown
	glRotatef(uppAngle,1,0,0); // for rotating w.r.t y axis
	drawLowerHemisphere(40,100,70); //drawn by spliting as sir shown

	glTranslatef(0,0,-40);//translated to the body of lower hemisphere
	glRotatef(cylinderAngle,1,0,0); //rotated at this point
    glRotatef(rotateAngle,0,0,1); // rotatig w.r.t own axis
	glTranslatef(0,0,-30); // translated to the centre of cyclinder er matha
	drawUpperHemesphere(30,100,70); // drawn the matha
	glTranslatef(0,0,-150);// translated throgh the height
	drawCylinder(30,150,100,70);// drawn the cyclinder
	glTranslatef(0,0,0);
	drawReverseLowerHemisphere(30,100,70);//drawn the ulta matha
	glTranslatef(0,0,150);
	glTranslatef(0,0,30);
	glRotatef(-rotateAngle,0,0,1);
	glRotatef(-cylinderAngle,1,0,0);
	glTranslatef(0,0,40);

	glRotatef(-uppAngle,1,0,0);
	glRotatef(-90,1,0,0);
	glRotatef(-angle,0,0,1);

	glTranslatef(0,400,0);
	glColor3f(0,5,30);
	drawSquare(200);
	glTranslatef(0,-400,0);

	if(intersect == true){
		glTranslatef(x3,y3,z3);
		glColor3f(1,0,0);
		drawCircle(2,100);

	}

    // glRotatef(angle,0,0,1);
    // glTranslatef(110,0,0);
    // glRotatef(2*angle,0,0,1);
    // glColor3f(0,1,0);
    // drawSquare(15);

    // glPushMatrix();
    // {
    //     glRotatef(angle,0,0,1);
    //     glTranslatef(60,0,0);
    //     glRotatef(2*angle,0,0,1);
    //     glColor3f(0,0,1);
    //     drawSquare(10);
    // }
    // glPopMatrix();

    // glRotatef(3*angle,0,0,1);
    // glTranslatef(40,0,0);
    // glRotatef(4*angle,0,0,1);
    // glColor3f(1,1,0);
    // drawSquare(5);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			//drawgrid=1-drawgrid;
			l = rotation(u, l, cameraAngle); //rotate/look left
			r = crossProduct(l, u);
			break;
		case '2':
			l = rotation(u, l, -cameraAngle); //rotate/look right
			r = crossProduct(l, u);
			break;
		case '3':
			l = rotation(r, l, cameraAngle); //look up
			u = crossProduct(r, l);
			break;
		case '4':
			l = rotation(r, l, -cameraAngle); //look down
			u = crossProduct(r, l);
			break;
		case '5':
			r = rotation(l, r, -cameraAngle); //tilt clockwise
			u = crossProduct(r, l);
			break;
		case '6':
			r = rotation(l, r, cameraAngle); //tilt counter-clockwise
			u = crossProduct(r, l);
			break;
		case 'p':
		
			
			if(angle<=50){
				angle = angle + 5;

			}
			break;

		case 'w':
		
			
			if(angle>=-50){
				angle = angle - 5;

			}
			break;

		case 'e':
		
			
			if(uppAngle<=60){
				uppAngle = uppAngle + 5;

			}
			break;
		case 'r':
		
			
			if(uppAngle>=-60){
				uppAngle = uppAngle - 5;

			}
			break;

		case 'a':
		
			
			if(cylinderAngle<=20){
				cylinderAngle = cylinderAngle + 5;

			}
			break;
		case 's':
		
			
			if(cylinderAngle>=-20){
				cylinderAngle = cylinderAngle - 5;

			}
			break;

		case 'd':
		
			rotateAngle = rotateAngle + 3;

			
			break;
		case 'f':
		
		    rotateAngle = rotateAngle - 3;
			break;
			

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraMove -= 3.0;
			pos.x = pos.x - cameraMove*l.x; //move backward
			pos.y = pos.y - cameraMove*l.y;
			pos.z = pos.z - cameraMove*l.z;
			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			pos.x = pos.x + cameraMove*l.x; //move forward
			pos.y = pos.y + cameraMove*l.y;
			pos.z = pos.z + cameraMove*l.z;
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			pos.x = pos.x + cameraMove*r.x; //move right
			pos.y = pos.y + cameraMove*r.y;
			pos.z = pos.z + cameraMove*r.z;
			break;
		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;

			pos.x = pos.x - cameraMove*r.x; //move left
			pos.y = pos.y - cameraMove*r.y;
			pos.z = pos.z - cameraMove*r.z;
			break;

		case GLUT_KEY_PAGE_UP:

			pos.x = pos.x + cameraMove*u.x; //move up
			pos.y = pos.y + cameraMove*u.y;
			pos.z = pos.z + cameraMove*u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:

			pos.x = pos.x - cameraMove*u.x; //move down
			pos.y = pos.y - cameraMove*u.y;
			pos.z = pos.z - cameraMove*u.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			struct point A;
			struct point B;
			A.x = cos(90-uppAngle)*40;
			A.y = cos(angle)*40;
			A.z = sqrt((40*40) - (A.x*A.x) - (A.y*A.y));

			B.x = cos(90-(uppAngle+cylinderAngle))*220;
			B.y = cos(angle)*220;
			B.z = sqrt((220*220) - (B.x*B.x) - (B.y*B.y));

			double x2,y2,z2;
			x2 = B.x - A.x;
			y2 = B.y - A.y;
			z2 = B.z - A.z;

			double t;

			t = (400 - A.y)/y2; // not sure
			
			x3 = A.x + t*x2;
			y3 = A.y + t*y2;
			z3 = A.z + t*z2;

			printf("%f %f %f",&x3,&y3,&z3);

			if(x3>=-200.0 && x3<=200.0){
				printf("\n%d",intersect);
				if(z3>=-200.0 && z3<=200.0){
					intersect = true;
				}
			}

			printf("\n%d",intersect);

			
			
			break;

		case GLUT_RIGHT_BUTTON:

			//........
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	// resets display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraMove=150.0;
	cameraAngle=5.0;
	angle=0;
	uppAngle = 0;
	cylinderAngle = 0;
	rotateAngle = 0;
	intersect = false;

	pos.x = 100;
	pos.y = 100;
	pos.z = 100;

	u.x = 0;
	u.y = 0;
	u.z = 1;
    
	r.x = -1/sqrt(2);
	r.y = 1/sqrt(2);
	r.z = 0;

	l.x = -1/sqrt(2);
	l.y = -1/sqrt(2);
	l.z = 0;



	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error %s\n", glewGetErrorString(err));
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	if (GLEW_ARB_vertex_program)
		fprintf(stdout, "Status: ARB vertex programs available.\n");

	if (glewGetExtension("GL_ARB_fragment_program"))
		fprintf(stdout, "Status: ARB fragment programs available.\n");

	if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite"))
		fprintf(stdout, "Status: ARB point sprites available.\n");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
