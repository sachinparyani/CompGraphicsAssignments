#include <GL/gl.h>
#include <GL/freeglut.h>

#include <iostream>

#define Window_Height 600
#define Window_Width 1200
#define Default_Polygon_Vertex_Count 3

class Vertex{
public:
	double x;
	double y;
	Vertex()
	{
		x = 0;
		y = 0;
	}
	Vertex(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
};

int PolygonVertexCount = Default_Polygon_Vertex_Count;
Vertex *PolygonData;
int StoredCount = 0;
int Polygon_Ready = 0;

void initWindow();
void initCallbacks();
void MouseClicked(int button, int state, int x, int y);
void Render();
void GetInput();

int main(int argc, char **argv)
{
	if(argc > 1)
		PolygonVertexCount = atoi(argv[1]);
	if(PolygonVertexCount < 3 || PolygonVertexCount > 10)
		PolygonVertexCount = Default_Polygon_Vertex_Count;
	PolygonData = new Vertex[PolygonVertexCount];

	glutInit(&argc, argv);
	glutInitWindowSize(Window_Width, Window_Height);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("Polygon");

	initWindow();
	initCallbacks();

	glutMainLoop();
}

void initWindow()
{
	glClearColor(0, 0, 0, 0);
}

void initCallbacks()
{
	glutDisplayFunc(Render);
	glutMouseFunc(MouseClicked);
	glutIdleFunc(glutPostRedisplay);
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(Polygon_Ready)
	{
		double r = (rand() % 1000) / 1000.0;
		glBegin(GL_TRIANGLES);
		for(int i = 1; i < PolygonVertexCount - 1; i++)
		{
			glVertex2f(PolygonData[0].x, PolygonData[0].y);
			glVertex2f(PolygonData[i].x, PolygonData[i].y);
			glVertex2f(PolygonData[i + 1].x, PolygonData[i + 1].y);
		}
		glEnd();
	}

	glutSwapBuffers();
}

void MouseClicked(int button, int state, int x, int y)
{
	if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
	{
		if(StoredCount < PolygonVertexCount)
		{
			PolygonData[StoredCount++] = Vertex(2 * (x - Window_Width / 2.0) / Window_Width, 2 * (Window_Height / 2.0 - y) / Window_Height);

		}
		if(StoredCount == PolygonVertexCount)
			Polygon_Ready = 1;
	}
	
}
