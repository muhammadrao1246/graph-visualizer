// Graphing_Tool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <string>
#include <cmath>
#include <math.h>

#include <iomanip>

#include <GL\freeglut.h>
#include <Windows.h>
#include <windows.h>

using namespace std;

#define TITLE "Graph Maker"
#define GL_PI 3.14159

//window bounds
int windowWidth = 800;
int windowHeight = 600;

//calculating screen center for windowX and windowY
int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
int desktopHeight = GetSystemMetrics(SM_CYSCREEN);

int windowX = (desktopWidth - windowWidth) / 2;
int windowY = (desktopHeight - windowHeight) / 2;

//current window height width at the time of runtime
int currWindowWidth = windowWidth;
int currWindowHeight = windowHeight;

//Callbacks prototypes
void myDisplayFunc();
void myReshapeFunc(int , int);
void myMouseWheelFunc(int button, int wheel_direction, int x, int y);

void OpenGL_Configs();

auto loadModel = [](){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
};
auto unloadModel = [](){
	glPopMatrix();
};
double round(double value)
{
    return (value >= 0.0) ? std::floor(value + 0.5) : std::ceil(value - 0.5);
}



class GraphBuilder
{	
private:
	GLfloat xMin, xMax, yMin, yMax;
	GLfloat xStep, yStep;
	GLfloat XUNIT, YUNIT, aspect;
	

	GLfloat getX(GLfloat x)
	{
		return x * xStep;
	}
	GLfloat getY(GLfloat y)
	{
		return y * yStep;
	}
	string trimLastZeros(string input)
	{
		string temp = "";
		int search_flag = 1;
		for (int i = input.length()-1; i >= 0; i--)
		{
			if(input[i] != '0' && search_flag == 1) 
				search_flag = 0;
			if( search_flag == 0 ) 
				temp = input[i] + temp;
		}
		if( temp[temp.length()-1] == '.' ) temp = temp.substr(0, temp.length()-1);

		return temp;
	}

public:
	GLfloat scale_step;
	GLfloat boxSize;
	enum Style {DOTTED_PATTERN = 0xAAAA, LINED_PATTERN = 0xFFFF};
	
	GraphBuilder()
	{

		//configurations
		scale_step = 0.5f;
		boxSize = 2.0f;
		updateConfigs();

	}
	void updateConfigs()
	{
		aspect = (GLfloat) currWindowWidth / (GLfloat) currWindowHeight;
		XUNIT = (GLfloat) 2.0f / (GLfloat) currWindowWidth;		XUNIT *= boxSize;
		YUNIT = (GLfloat) 2.0f / (GLfloat) currWindowHeight;	YUNIT *= boxSize;
		xStep = ((XUNIT *((currWindowHeight/aspect)/20))/scale_step);
		yStep = ((XUNIT *((currWindowWidth/aspect)/20))/scale_step);
		glLineStipple(1, 0xFFFF);
		
		glLineWidth(1.0f);
		glPointSize(1.0f);
	}

	void plottingGrid()
	{
		updateConfigs();

		printf_s("\nXUNIT: %f\tYUNIT: %f",XUNIT,YUNIT);


		glColor3f(0.867,0.867,0.867);
		loadModel();
		//plotting grid lines
		glBegin(GL_LINES);
		//---- x-axis lines
		for (GLfloat i = 0.0f; i <= 1.0f; i+= XUNIT *((currWindowWidth/aspect)/20))
		{
			glVertex2f(-1, i);
			glVertex2f(+1, i);
			glVertex2f(-1, -i);
			glVertex2f(+1, -i);
		}


		//----- y-axis lines
		for (GLfloat i = 0.0f; i <= 1.0f; i+= XUNIT *((currWindowHeight/aspect)/20))
		{
			glVertex2f(i, -1);
			glVertex2f(i, +1);
			glVertex2f(-i, -1);
			glVertex2f(-i, +1);
		}

		glColor3f(0,0,0); //black lines
		//plotting base y-axis lines
		glVertex2f(0, -1);
		glVertex2f(0, +1);
		//plotting base x-axis lines
		glVertex2f(-1, 0);
		glVertex2f(+1, 0);
		glEnd();

		unloadModel();



	}
	void plottingPoints()
	{
		loadModel();

		glColor3f(0,0,0);

		//----  plotting origin 0

		string label = "0";

		GLfloat textX = 0 - ( ( (GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) * XUNIT ) / 2.0f);
		GLfloat textY = -((2.0f/(float)currWindowHeight)*10.0f);

		glRasterPos2f(textX, textY);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str());


		//---   plotting points of x-axis
		int step_iterator = 1;
		for (GLfloat i = XUNIT *((currWindowHeight/aspect)/20); i <= 1.0f; i+= XUNIT *((currWindowHeight/aspect)/20))
		{
			GLfloat value = scale_step * (float)step_iterator;
			
			label = to_string(round((scale_step * (float)step_iterator)*10000.0f)/10000.0f);
			label = trimLastZeros(label);
			

			textX = i - ( ( (GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) * XUNIT ) / 2.0f);
			textY = -((2.0f/(float)currWindowHeight)*10.0f);

			//printf(("\n"+label).c_str());
			//printf("\n Text Width: In PX %d and In Real %f",glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()), ((2.0f/(GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str())) / 2.0f));
			//printf(string("\n"+to_string((GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) / 2.0f)).c_str());
			//printf("\nText Bounds: %f\t%f",textX, textY);

			glRasterPos2f(textX, textY);
			glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str());

			//--- minus points
			{
				label = "-" + label;
				textX = - i - (((GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) * XUNIT) / 2.0f);
				glRasterPos2f(textX, textY);
				glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str());
			}

			//----- new X-min and X-max
			xMax = (round(abs(scale_step * step_iterator)));
			xMin = - xMax;

			++step_iterator;


			
		}


		//---   plotting points of y-axis
		step_iterator = 1;
		for (GLfloat i = XUNIT * ((currWindowWidth/aspect)/20); i <= 1.0f; i+= XUNIT * ((currWindowWidth/aspect)/20))
		{
			GLfloat value = scale_step * (float)step_iterator;
			
			label = to_string(round((scale_step * (float)step_iterator)*10000.0f)/10000.0f);
			label = trimLastZeros(label);

			textX = ((2.0f/(float)currWindowWidth)*1.0f);
			textY = i - ( ( (GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) * XUNIT ) / 2.0f);

			//printf(("\n"+label).c_str());
			//printf("\n Text Width: In PX %d and In Real %f",glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()), ((2.0f/(GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str())) / 2.0f));
			//printf(string("\n"+to_string((GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) / 2.0f)).c_str());
			//printf("\nText Bounds: %f\t%f",textX, textY);

			glRasterPos2f(textX, textY);
			glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str());

			//--- minus points
			{
				label = "-" + label;
				textY = -i - ( ( (GLfloat) glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str()) * XUNIT ) / 2.0f * 0.5);
				glRasterPos2f(textX, textY);
				glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*) label.c_str());
			}

			//----- new Y-min and Y-max
			yMax = (round(abs(scale_step * step_iterator)));
			yMin = - yMax;

			++step_iterator;
		}


		printf("\n\nX-MIN:\t%f\nX-MAX:\t%f\nY-MIN:\t%f\nY-MAX:\t%f", xMin, xMax, yMin, yMax);
		unloadModel();
	}

	void drawPoint(GLfloat x, GLfloat y, GLfloat pointSize = 1.0f)
	{
		glPointSize(pointSize);
		glBegin(GL_POINTS);
		{
			glVertex2f(getX(x), getY(y));	
		}
		glEnd();
		
		updateConfigs();
	}

	void drawCurvatures(GLfloat x, GLfloat y)
	{
		glVertex2f(getX(x), getY(y));	
	}

	void plotFunction(GLfloat (*func)(GLfloat), Style graphStyle = Style::LINED_PATTERN, int spaceFactor = 1)
	{

		
		//glColor3f(0,0,0);

		GLfloat max = xMax > yMax ? xMax : yMax;
		GLfloat min = xMin < yMin ? xMin : yMin;

		glLineStipple(spaceFactor, graphStyle);

		glBegin(GL_LINE_STRIP);
		for (GLfloat x = min; x <= max; x += xStep)
		{
			drawCurvatures(x, func(x));
		}
		glEnd();

		//--- reset configs
		updateConfigs();
	}

	// give values between 0 and 1
	// also call before providing function to "plotFunction"
	void setColor(GLfloat red, GLfloat green, GLfloat blue)
	{
		glColor3f(red, green, blue);
	}

	//set line width
	void setLineWidth(GLfloat size = 1.0f)
	{
		glLineWidth(size);
	}
};


// YOU CAN DEFINE YOUR FUNCTIONS LIKE THIS AND PASS THEM AS THE PARAMETER TO "plotFunction"
inline GLfloat sin_func(GLfloat x){ return sinf(x); }
inline GLfloat cos_func(GLfloat x){ return cosf(x); }
inline GLfloat polynomial_func(GLfloat x){ return (3.0f*x+3.0f*(x*x)+1.0f); }
inline GLfloat logarithmic_func(GLfloat x){ return (log(x)); }
inline GLfloat exponential_func(GLfloat x){ return (2*x); }


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(windowX, windowY);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutCreateWindow(TITLE);

	//fullscreen view
	HWND handle = FindWindow(NULL, L"Graph Maker");
	ShowWindow(handle, SW_MAXIMIZE);

	//REGISTERING THE CALLBACKS
	glutDisplayFunc(myDisplayFunc);
	glutReshapeFunc(myReshapeFunc);
	glutMouseWheelFunc(myMouseWheelFunc);

	//OPENGL CONFIGURATION SHOULD BE LOADED A
	OpenGL_Configs();

	glutMainLoop();
	return 0;
}

GraphBuilder graph = GraphBuilder();
void myDisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//callbacks calling begins for rendering the graph
	graph.plottingGrid();
	graph.plottingPoints();

	graph.setColor(1, 0, 0);
	graph.drawPoint(5,5, 10.0f);

	graph.setColor(0, 1, 0);
	graph.setLineWidth(2.0f);
	graph.plotFunction(sin_func, GraphBuilder::Style::DOTTED_PATTERN, 3.0f);

	graph.setColor(0, 0, 1);
	graph.setLineWidth(3.0f);
	graph.plotFunction(polynomial_func, GraphBuilder::Style::DOTTED_PATTERN, 5.0f);

	graph.setColor(0, 1, 1);
	graph.plotFunction(logarithmic_func);

	graph.setColor(1, 0, 1);
	graph.plotFunction(exponential_func);

	graph.setColor(1, 1, 0);
	graph.setLineWidth(10.0f);
	graph.plotFunction(cos_func);

	glutSwapBuffers();
}

void myMouseWheelFunc(int button, int wheel_direction, int x, int y)
{
	if( wheel_direction > 0 )
	{
		//cout<<"\n\nCamera Zoomed In";
			graph.scale_step /= 2.0f;
	}
	else
	{
		//cout<<"\n\nCamera Zoomed Out";
			graph.scale_step *= 2.0f;
	}
	
	printf("\n\n\nSCALE STEP:\t%f", graph.scale_step);

	glutPostRedisplay();
}

void myReshapeFunc(int width, int height)
{
	currWindowWidth = width;
	currWindowHeight = height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	//gluOrtho2D(0, width, 0, height);
	glViewport(0, 0, width, height);


	printf("reshaped");
}

void OpenGL_Configs()
{
	glClearColor(1,1,1,1);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_LINE_STIPPLE);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, windowWidth, windowHeight);
}