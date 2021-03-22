#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
// #include <GL/glext.h>

#define pi (2*acos(0.0))

int drawgrid;
int drawaxes;
double angle;
double speed;
double prevSpeed;
int counter;
int nBubbles;

struct point
{
	double x,y,z;
};

struct velocity // velocity vector
{
    double vx,vy;
};

struct bubble //bubble type
{
    struct point centre;
    struct velocity v;
    bool inside; //inside the large circle or not
    bool insideIt;
    

};

bubble bbs[5];

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
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
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
	glBegin(GL_LINES);{
        //glVertex3f(0,a,0);
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
        
	}glEnd();

    glBegin(GL_LINES);{
        glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		
    }glEnd();

    glBegin(GL_LINES);{
        
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
        
    }glEnd();

    glBegin(GL_LINES);{
        
        glVertex3f(-a, a,0);
        glVertex3f( a, a,0);
    }glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
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
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
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
		}
	}
}


void drawSS()
{
	// set the color of skuare
	glColor3f(0,1,0);
    drawSquare(60); //green skuare
    glColor3f(1,0,0); //red big circle
    drawCircle(40,60); //radious 40

    //glRotatef(angle,0,0,1);

     
    for(int i =0;i<nBubbles;i++){  //five bubbles
        glTranslatef(bbs[i].centre.x,bbs[i].centre.y,0);
        glColor3f(0,0,1);
        drawCircle(6,60); //radius 10
        glTranslatef(-bbs[i].centre.x,-bbs[i].centre.y,0);
    }
    
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'p': //pause or resume
			//drawgrid=1-drawgrid;
		    
            if(speed >0){
                prevSpeed = speed;
                speed = 0;
            }
            else{
               // cout << prevSpeed << endl;
                speed = prevSpeed;
            }

			break;
		
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key//making speed half
			//cameraMove -= 3.0;
			speed = 0.5*speed;
			break;
		case GLUT_KEY_UP:		// up arrow key//making speed 1.5 times
			//cameraHeight += 3.0;
			speed = 1.1*speed;
			break;

	

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
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

    counter = counter + 1;
    if(counter ==10){
        nBubbles = 1;
    }
    if(counter ==70){
        nBubbles = 2;
    }
    if(counter ==120){
        nBubbles = 3;
    }
    if(counter ==170){
        nBubbles = 4;
    }
    if(counter ==220){
        nBubbles = 5;
    }
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

	gluLookAt(0,0,100,	0,0,0,	0,1,0);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	//drawAxes();
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
    for(int i =0;i<nBubbles;i++){

        
        // bbs[i].centre.x = bbs[i].centre.x + 0.7*bbs[i].v.vx;
        // bbs[i].centre.y = bbs[i].centre.y + 0.7*bbs[i].v.vy;
        struct point temp;
        temp.x = bbs[i].centre.x + speed*bbs[i].v.vx;
        temp.y = bbs[i].centre.y + speed*bbs[i].v.vy;

        double dist = sqrt(pow(0-temp.x,2) + pow(0-temp.y,2));
        
        if (bbs[i].inside== true && (dist)>34){
            //handling collision and reflection

            struct velocity u;
            u.vx = 0-bbs[i].centre.x;
            u.vy = 0-bbs[i].centre.y; // vector within two points (AB)
            struct velocity unitu;

            unitu.vx = u.vx/sqrt(pow(u.vx,2) + pow(u.vy,2)); //unit vector inline to AB
            unitu.vy = u.vy/sqrt(pow(u.vx,2) + pow(u.vy,2));

            double vcostheta = (u.vx * bbs[i].v.vx + u.vy*bbs[i].v.vy)/sqrt(pow(u.vx,2) + pow(u.vy,2));

            struct velocity vcomp; // velocity component vector inline to AB

            vcomp.vx = vcostheta*unitu.vx;
            vcomp.vy = vcostheta*unitu.vy;
            
            struct velocity vperp; // velocity component perpendicular to AB
            
            vperp.vx = bbs[i].v.vx - vcomp.vx;
            vperp.vy = bbs[i].v.vy - vcomp.vy;

            bbs[i].v.vx = -vcomp.vx + vperp.vx;
            bbs[i].v.vy = -vcomp.vy + vperp.vy;

            double unit = sqrt(pow(bbs[i].v.vx,2) + pow(bbs[i].v.vy,2));
            bbs[i].v.vx = bbs[i].v.vx/ unit;
            bbs[i].v.vy = bbs[i].v.vy/ unit;

        }

        else{

            bbs[i].centre = temp;

        }

        // bbs[i].centre.x = bbs[i].centre.x + 0.7*bbs[i].v.vx;
        // bbs[i].centre.y = bbs[i].centre.y + 0.7*bbs[i].v.vy;

        // double dist = sqrt(pow(0-bbs[i].centre.x,2) + pow(0-bbs[i].centre.y,2));

        if(abs(bbs[i].centre.x)+6 >= 60){
            bbs[i].v.vx = - bbs[i].v.vx;
        }
        // else if(bbs[i].centre.x-6 <= -60){
        //     bbs[i].v.vx = - bbs[i].v.vx;
        // }

        if(abs(bbs[i].centre.y)+6 >= 60){
            bbs[i].v.vy = - bbs[i].v.vy;
        }
        // else if(bbs[i].centre.y-6 <= -60){
        //     bbs[i].v.vy = - bbs[i].v.vy;
        // }
        
        if(abs(dist)<=34){
            bbs[i].inside = true;
        }
        // if (bbs[i].inside== true && (dist)>=34){
        //     //handling collision and reflection

        //     struct velocity u;
        //     u.vx = 0-bbs[i].centre.x;
        //     u.vy = 0-bbs[i].centre.y; // vector within two points (AB)
        //     struct velocity unitu;

        //     unitu.vx = u.vx/sqrt(pow(u.vx,2) + pow(u.vy,2)); //unit vector inline to AB
        //     unitu.vy = u.vy/sqrt(pow(u.vx,2) + pow(u.vy,2));

        //     double vcostheta = (u.vx * bbs[i].v.vx + u.vy*bbs[i].v.vy)/sqrt(pow(u.vx,2) + pow(u.vy,2));

        //     struct velocity vcomp; // velocity component vector inline to AB

        //     vcomp.vx = vcostheta*unitu.vx;
        //     vcomp.vy = vcostheta*unitu.vy;
            
        //     struct velocity vperp; // velocity component perpendicular to AB
            
        //     vperp.vx = bbs[i].v.vx - vcomp.vx;
        //     vperp.vy = bbs[i].v.vy - vcomp.vy;

        //     bbs[i].v.vx = -vcomp.vx + vperp.vx;
        //     bbs[i].v.vy = -vcomp.vy + vperp.vy;

        //     double unit = sqrt(pow(bbs[i].v.vx,2) + pow(bbs[i].v.vy,2));
        //     bbs[i].v.vx = bbs[i].v.vx/ unit;
        //     bbs[i].v.vy = bbs[i].v.vy/ unit;

        // }

        for(int j = 0;j<nBubbles;j++){
            if(i!=j){
                if(bbs[i].inside == true && bbs[j].inside == true){
                    double dx = bbs[j].centre.x-bbs[i].centre.x;
                    double dy = bbs[j].centre.y-bbs[i].centre.y;

                    double d = sqrt(pow(dx,2)+pow(dy,2));
                    // if(d<12){
                    //     bbs[i].insideIt = true;
                    //     bbs[j].insideIt = true;
                    // }
                    if ( d <= 12){
                        // double nx = dx/d;
                        // double ny = dy/d;

                        // double tdist = (d*(6/12));
                        // double contactX = bbs[i].centre.x + nx*tdist;
                        // double contactY = bbs[i].centre.y + ny*tdist;

                        // bbs[i].centre.x = contactX - nx*3;
                        // bbs[i].centre.y = contactY - ny*3;
                        // bbs[j].centre.x = contactX + nx*3;
                        // bbs[j].centre.y = contactY + ny*3;


                        struct velocity u1;
                        u1.vx = bbs[j].centre.x-bbs[i].centre.x;
                        u1.vy = bbs[j].centre.y-bbs[i].centre.y; // vector within two points (AB)
                        struct velocity unitu1;

                        unitu1.vx = u1.vx/sqrt(pow(u1.vx,2) + pow(u1.vy,2)); //unit vector inline to AB
                        unitu1.vy = u1.vy/sqrt(pow(u1.vx,2) + pow(u1.vy,2));

                        double vcostheta1 = (u1.vx * bbs[i].v.vx + u1.vy*bbs[i].v.vy)/sqrt(pow(u1.vx,2) + pow(u1.vy,2));
                        double vcostheta2 = (u1.vx * bbs[j].v.vx + u1.vy*bbs[j].v.vy)/sqrt(pow(u1.vx,2) + pow(u1.vy,2));

                        struct velocity vcomp1; // velocity component vector inline to AB
                        struct velocity vcomp2;

                        vcomp1.vx = vcostheta1*unitu1.vx;
                        vcomp1.vy = vcostheta1*unitu1.vy;

                        vcomp2.vx = vcostheta2*unitu1.vx;
                        vcomp2.vy = vcostheta2*unitu1.vy;
                        
                        struct velocity vperp1; // velocity component perpendicular to AB
                        struct velocity vperp2;
                        
                        vperp1.vx = bbs[i].v.vx - vcomp1.vx;
                        vperp1.vy = bbs[i].v.vy - vcomp1.vy;

                        vperp2.vx = bbs[j].v.vx - vcomp2.vx;
                        vperp2.vy = bbs[j].v.vy - vcomp2.vy;

                        bbs[i].v.vx = -vcomp1.vx + vperp1.vx;
                        bbs[i].v.vy = -vcomp1.vy + vperp1.vy;

                        bbs[j].v.vx = vcomp2.vx + vperp2.vx;
                        bbs[j].v.vy = vcomp2.vy + vperp2.vy;

                        double unit1 = sqrt(pow(bbs[i].v.vx,2) + pow(bbs[i].v.vy,2));
                        bbs[i].v.vx = bbs[i].v.vx/ unit1;
                        bbs[i].v.vy = bbs[i].v.vy/ unit1;

                        double unit2 = sqrt(pow(bbs[j].v.vx,2) + pow(bbs[j].v.vy,2));
                        bbs[j].v.vx = bbs[j].v.vx/ unit2;
                        bbs[j].v.vy = bbs[j].v.vy/ unit2;
                        
                    }
                
                    
                }
            }
        }


    }
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
    speed = 0.8;
    counter = 0;
	

    bbs[0].centre.x = -54;
    bbs[0].centre.y = -54;
    bbs[0].v.vx = (rand()*0% 1500*(0+1));
    bbs[0].v.vy = (rand() % 500*(0+1));
    double unit = sqrt(pow(bbs[0].v.vx,2) + pow(bbs[0].v.vy,2));
    bbs[0].v.vx = bbs[0].v.vx/ unit;
    bbs[0].v.vy = bbs[0].v.vy/ unit;
    bbs[0].inside = false;
    bbs[0].insideIt = false;

    for(int i =1;i<5;i++){
        bbs[i].centre.x = -54;
        bbs[i].centre.y = -54;
        bbs[i].v.vx = (rand()% 1500*(i+1));
        bbs[i].v.vy = (rand() % 500*(i+1));
        double unit = sqrt(pow(bbs[i].v.vx,2) + pow(bbs[i].v.vy,2));
        bbs[i].v.vx = bbs[i].v.vx/ unit;
        bbs[i].v.vy = bbs[i].v.vy/ unit;
        bbs[i].inside = false;
        bbs[i].insideIt = false;
    }
	



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
