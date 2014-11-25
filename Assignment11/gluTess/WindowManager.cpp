#include "WindowManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

#include <iostream>
#include <vector>

#define WindowWidth 1000
#define WindowHeight 700
#define SIZE 4

class WindowState{
public:
    glm::vec3 lastPos;
    glm::vec3 cameraPos;
    glm::vec3 rotationAxis;
    glm::quat rotation;
    float angularSpeed;
    glm::mat4 rotMatrix;
    float x, y, z;
    float timer, timeDelta;
    static const int MaxTime = 5000;

    WindowState()
    {
        cameraPos = glm::vec3(0, 30, 250);
        timer = 0;
        timeDelta = 10;

    }
};

WindowState currentState;

void InitWindow(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab Exam");
}

bool lights[5] = {0, 0, 0, 0, 0};

void InitDisplay()
{
    glClearColor(0.5, 0.2, 0.9, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    GLfloat light_diffuse_sun[]  = { 1.0f, 1.0f, 0.0f, 1.0f };
    GLfloat light_diffuse_moon[]  = { 0.5f, 0.5f, 0.5, 1.0f };
    GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse_sun);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse_moon);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 3);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 3);

    GLfloat mat_ambient[] =   { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] =   { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] =  { 0.0f, 0.0f, 0.0f, 0.0f };
    GLfloat mat_emission[] =  { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f }; 
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,   mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,   mat_emission);
    glMaterialfv(GL_FRONT, GL_SHININESS,   mat_shininess);

    // Other Lights:
    GLfloat spot_direction[] = { 0.0f, 0.0f, -1.0f, 1.0f };
    GLfloat light2_diffuse[]  = { 0.0f, 0.0f, 1.0f, 1.0f };

    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 100.0);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
     
    GLfloat spot_direction2[] = { -1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat light3_diffuse[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
    
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 100.0);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 1);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction2);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);

    GLfloat light4_diffuse[]  = { 0.0f, 0.5f, 0.5f, 1.0f };
    GLfloat light4_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light4_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT4, GL_AMBIENT,  light4_ambient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE,  light4_diffuse);
    glLightfv(GL_LIGHT4, GL_POSITION, light4_position);

    // glEnable(GL_FOG);
    // float FogCol[4]={0.8f,0.8f,0.8f, 0.1f};
    // glFogfv(GL_FOG_COLOR,FogCol);
    // glFogf(GL_FOG_START, 0.f);
    // glFogf(GL_FOG_END, 10.f);
    // glFogi(GL_FOG_MODE, GL_EXP2);
    // glFogf(GL_FOG_DENSITY, 0.003f);

    loadTextures();
}


void InitScene()
{
}

void InitCallbacks()
{
    glutMouseFunc(Mouse);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    glutReshapeFunc(Reshape);
    glutMotionFunc(MouseMotion);
    glutKeyboardFunc(Keyboard);
}

void Keyboard(unsigned char button, int x, int y)
{
    float stepz = 10, stepx = 1, stepy = 1;
    float stepCamZ = 1;
    switch(button)
    {
    case 'w':
    case 'W':
        currentState.cameraPos.z -= stepCamZ;
        break;
    case 's':
    case 'S':
        currentState.cameraPos.z += stepCamZ;
        break;
    case 'X':
        currentState.x += stepx;
        break;
    case 'x':
        currentState.x -= stepx;
        break;
    case 'Y':
        currentState.y += stepy;
        break;
    case 'y':
        currentState.y -= stepy;
        break;
    case 'Z':
        currentState.z += stepz;
        break;
    case 'z':
        currentState.z -= stepz;
        break;
    case '+':
        currentState.timeDelta += 0.5;
        break;  
    case '-':
        currentState.timeDelta -= 0.5;
        break; 
    case '0':
        if(!lights[0])
            glEnable(GL_LIGHT0);
        else
            glDisable(GL_LIGHT0);
        lights[0] = !lights[0];
        for(int i = 0; i < 5; i++)
            std::cout << lights[i] << " ";
        std::cout << std::endl;
        break;
    case '1':
        if(!lights[1])
            glEnable(GL_LIGHT1);
        else
            glDisable(GL_LIGHT1);
        lights[1] = !lights[1];
        for(int i = 0; i < 5; i++)
            std::cout << lights[i] << " ";
        std::cout << std::endl;
        break;
    case '2':
        if(!lights[2])
            glEnable(GL_LIGHT2);
        else
            glDisable(GL_LIGHT2);
        lights[2] = !lights[2];
        for(int i = 0; i < 5; i++)
            std::cout << lights[i] << " ";
        std::cout << std::endl;
        break;
    case '3':
        if(!lights[3])
            glEnable(GL_LIGHT3);
        else
            glDisable(GL_LIGHT3);
        lights[3] = !lights[3];
        for(int i = 0; i < 5; i++)
            std::cout << lights[i] << " ";
        std::cout << std::endl;
        break;
    case '4':
        if(!lights[4])
            glEnable(GL_LIGHT4);
        else
            glDisable(GL_LIGHT4);
        lights[4] = !lights[4];
        for(int i = 0; i < 5; i++)
            std::cout << lights[i] << " ";
        std::cout << std::endl;
        break;
    default:
        ;
    }
}

void MouseMotion(int x, int y)
{
}

void Reshape(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    double aspect = (double)w / (double)(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const double zNear = 0.1, zFar = 2000.0, fov = 45.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Mouse(int button, int state, int x, int y)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        switch(state)
        {
        case GLUT_DOWN:
            currentState.lastPos = glm::vec3(x, y, 0);
            break;

        case GLUT_UP:
        {
            glm::vec3 diff = glm::vec3(x, y, 0) - currentState.lastPos;

            if(diff.x == 0 && diff.y == 0 && diff.z == 0)
                return;

            glm::vec3 n = glm::normalize(glm::vec3(diff.y, diff.x, 0.0));
            float acc = diff.length() / 5.0;
            currentState.rotationAxis = (currentState.rotationAxis * currentState.angularSpeed + n * acc);

            if(currentState.rotationAxis.length() != 0)
                glm::normalize(currentState.rotationAxis);
            currentState.angularSpeed += acc;
        }
            break;

        default:
            std::cout << "Unknown Mouse state for Left Button" << std::endl;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        switch(state)
        {
        case GLUT_UP:
            break;

        case GLUT_DOWN:
            break;

        default:
            std::cout << "Unknown Mouse state for Right Button" << std::endl;
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        switch(state)
        {
        case GLUT_UP:
            break;

        case GLUT_DOWN:
            break;

        default:
            std::cout << "Unknown Mouse state for Middle Button" << std::endl;
        }
        break;

    default:
        std::cout << "Unknown Mouse button pressed" << std::endl;
    }
}

void PreDisplayUpdate()
{
    // Update Time
    currentState.timer = currentState.timer + currentState.timeDelta;
    if(currentState.timer > currentState.MaxTime)
        currentState.timer -= currentState.MaxTime;

    // Decrease angular speed (friction)
    currentState.angularSpeed *= 0.95;

    // Stop rotation when speed goes below threshold
    if (currentState.angularSpeed < 0.01) {
        currentState.angularSpeed = 0.0;
    }

    // Update rotation
    currentState.rotation = glm::angleAxis(currentState.angularSpeed, currentState.rotationAxis) * currentState.rotation;
}

void beginCB(GLenum which)
{
    glBegin(which);
}

void endCB()
{
    glEnd();
}

GLuint grassTexture = 0, branchTexture = 0;

void loadTextures()
{
    grassTexture = SOIL_load_OGL_texture("cloud.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if(grassTexture == 0)
        exit(-1);
    branchTexture = SOIL_load_OGL_texture("branch.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if(branchTexture == 0)
        exit(-1);
}

void vertexCB(const GLvoid *data)
{
    // cast back to double type
    const GLdouble *ptr = (const GLdouble*)data;
    glTexCoord2f((ptr[0] + 50) / 100.0, (ptr[1] + 50) / 100.0);
    glVertex3dv(ptr);
}

void DrawTessJ()
{
    GLdouble quadJ[][3] = { {-50, 50, 50},
                            {-50, 30, 50},
                            {-10, 30, 50},
                            {-10, -30, 50},
                            {-50, -30, 50},
                            {-50, -50, 50},
                            { 10, -50, 50},
                            { 10, 30, 50},
                            { 50, 30, 50},
                            { 50, 50, 50}
                        };

    int size = sizeof(quadJ) / sizeof(quadJ[0]);
    // create tessellator
    GLUtesselator *tess = gluNewTess();

    // register callback functions
    gluTessCallback(tess, GLU_TESS_BEGIN,   (void (*)())beginCB);
    gluTessCallback(tess, GLU_TESS_END,     (void (*)())endCB);
    gluTessCallback(tess, GLU_TESS_VERTEX,  (void (*)())vertexCB);
    glColor4f(1, 1, 1, 1);
    
    // Front
    gluTessBeginPolygon(tess, 0);
        gluTessBeginContour(tess);
            for(int i = 0; i < size; i++)
                gluTessVertex(tess, quadJ[i], quadJ[i]);
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    // Back
    gluTessBeginPolygon(tess, 0);
        gluTessBeginContour(tess);
            for(int i = size - 1; i >= 0; i--)
            {
                quadJ[i][2] = 20;
                gluTessVertex(tess, quadJ[i], quadJ[i]);
            }
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    for(int j = 0; j < size; j++)
    {
        GLdouble quadBorder[4][3];
        int c = 0;
        for(int i = 0; i < 2; i++, c++)
        {
            quadBorder[c][0] = quadJ[(j + i) % size][0];
            quadBorder[c][1] = quadJ[(j + i) % size][1];
            quadBorder[c][2] = 20;
        }
        for(int i = 1; i >= 0; i--, c++)
        {
            quadBorder[c][0] = quadJ[(j + i) % size][0];
            quadBorder[c][1] = quadJ[(j + i) % size][1];
            quadBorder[c][2] = 50;
        }
        // Border
        gluTessBeginPolygon(tess, 0);
            gluTessBeginContour(tess);
                for(int i = 0; i < 4; i++)
                    gluTessVertex(tess, quadBorder[i], quadBorder[i]);
            gluTessEndContour(tess);
        gluTessEndPolygon(tess);    
    }
    
    // delete tessellator after processing
    gluDeleteTess(tess);
}

void DrawTessP()
{
   GLdouble quadP1[][3] = { {-15, 50, 50},
                            {-15, -50, 50},
                            {0, -50, 50},
                            {0, 0, 50},
                            {30, 0, 50},
                            {30, 50, 50},
                        };
    GLdouble quadP2[][3] = {
                            {0, 35, 50},
                            {0, 15, 50},
                            {15, 15, 50},
                            {15, 35, 50} 
                        };
    int size1 = sizeof(quadP1) / sizeof(quadP1[0]);
    int size2 = sizeof(quadP2) / sizeof(quadP2[0]);

    // create tessellator
    GLUtesselator *tess = gluNewTess();

    // register callback functions
    gluTessCallback(tess, GLU_TESS_BEGIN,   (void (*)())beginCB);
    gluTessCallback(tess, GLU_TESS_END,     (void (*)())endCB);
    gluTessCallback(tess, GLU_TESS_VERTEX,  (void (*)())vertexCB);
    glColor4f(1, 1, 1, 1);
    // describe non-convex polygon
    gluTessBeginPolygon(tess, 0);
        gluTessBeginContour(tess);
            for(int i = 0; i < size1; i++)
                gluTessVertex(tess, quadP1[i], quadP1[i]);
        gluTessEndContour(tess);
        
        gluTessBeginContour(tess);
            for(int i = 0; i < size2; i++)
                gluTessVertex(tess, quadP2[i], quadP2[i]);
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    gluTessBeginPolygon(tess, 0);
        gluTessBeginContour(tess);
            for(int i = size1 - 1; i >= 0; i--)
            {
                quadP1[i][2] = 20;
                gluTessVertex(tess, quadP1[i], quadP1[i]);
            }
        gluTessEndContour(tess);
        gluTessBeginContour(tess);
            for(int i = size2 - 1; i >= 0; i--)
            {
                quadP2[i][2] = 20;
                gluTessVertex(tess, quadP2[i], quadP2[i]);
            }
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    
    for(int j = 0; j < size1; j++)
    {
        GLdouble quadBorder[4][3];
        int c = 0;
        for(int i = 0; i < 2; i++, c++)
        {
            quadBorder[c][0] = quadP1[(j + i) % size1][0];
            quadBorder[c][1] = quadP1[(j + i) % size1][1];
            quadBorder[c][2] = 20;
        }
        for(int i = 1; i >= 0; i--, c++)
        {
            quadBorder[c][0] = quadP1[(j + i) % size1][0];
            quadBorder[c][1] = quadP1[(j + i) % size1][1];
            quadBorder[c][2] = 50;
        }
        // Border
        gluTessBeginPolygon(tess, 0);
            gluTessBeginContour(tess);
                for(int i = 0; i < 4; i++)
                    gluTessVertex(tess, quadBorder[i], quadBorder[i]);
            gluTessEndContour(tess);
        gluTessEndPolygon(tess);    
    }

    for(int j = 0; j < size2; j++)
    {
        GLdouble quadBorder[4][3];
        int c = 0;
        for(int i = 1; i >= 0; i--, c++)
        {
            quadBorder[c][0] = quadP2[(j + i) % size2][0];
            quadBorder[c][1] = quadP2[(j + i) % size2][1];
            quadBorder[c][2] = 20;
        }
        for(int i = 0; i < 2; i++, c++)
        {
            quadBorder[c][0] = quadP2[(j + i) % size2][0];
            quadBorder[c][1] = quadP2[(j + i) % size2][1];
            quadBorder[c][2] = 50;
        }
        // Border
        gluTessBeginPolygon(tess, 0);
            gluTessBeginContour(tess);
                for(int i = 0; i < 4; i++)
                    gluTessVertex(tess, quadBorder[i], quadBorder[i]);
            gluTessEndContour(tess);
        gluTessEndPolygon(tess);    
    }
    // delete tessellator after processing
    gluDeleteTess(tess);
}

void DrawTessText()
{   
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glPushMatrix();
    glTranslatef(-60, 0, 0);
    DrawTessJ();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, branchTexture);
    glPushMatrix();
    glTranslatef(60, 0, 0);
    DrawTessP();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

GLfloat ctrlpoints1[4][3] = {
        { 0.0, 0.0, 0.0}, { -50.0, 20.0, 0.0}, 
        {50.0, 10.0, 0.0}, {100.0, 40.0, 0.0}
    };

GLfloat ctrlpoints2[4][3] = {
        { 100.0, 40.0, 0.0}, { 70.0, 50.0, 0.0}, 
        {70.0, -90.0, 0.0}, { 10.0, -50.0, 0.0}
    };

GLfloat ctrlpoints3[4][3] = {
        { 80.0, 40.0, 0.0}, { 100.0, 20.0, 0.0}, 
        { 70.0, 10.0, 0.0}, { 50.0, -100.0, 0.0}
    };

GLfloat ctrlpoints4[4][3] = {
        { 80.0, 40.0, 0.0}, { 150.0, 20.0, 0.0}, 
        {150.0, -20.0, 0.0}, { 80.0, -50.0, 0.0}
    };

void DrawBezierText()
{
    glPushMatrix();
    glTranslatef(-100, 200, 0);
    glColor3f(1.0, 1.0, 1.0);

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints1[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 30; i++) 
            glEvalCoord1f((GLfloat) i/30.0);
    glEnd();

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints2[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 30; i++) 
            glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glTranslatef(100, 0, 0);
    
    glColor3f(1.0, 1.0, 1.0);

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints3[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 30; i++) 
            glEvalCoord1f((GLfloat) i/30.0);
    glEnd();

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints4[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 30; i++) 
            glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glPopMatrix();
}

void Display()
{
    PreDisplayUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(currentState.cameraPos.x, currentState.cameraPos.y, currentState.cameraPos.z,
              currentState.cameraPos.x, currentState.cameraPos.y, currentState.cameraPos.z - 1,
              0, 1, 0);

    float angle = (currentState.timer / currentState.MaxTime);
    
    float color;
    if(angle > 0.125 && angle <= 0.625)
        color = (0.625 - angle) * 2;
    else
    {
        float temp = angle + 0.375 > 1 ? angle + 0.375 - 1 : angle + 0.375;
        color = temp * 2;
    }

    glColor4f(color / 2, color / 2, color, 1);
    glBegin(GL_QUADS);
    glVertex3f(-1000, -1000, -500);
    glVertex3f(1000, -1000, -500);
    glVertex3f(1000, 1000, -500);
    glVertex3f(-1000, 1000, -500);
    glEnd();

    angle *= 360;

    glTranslatef(currentState.x, currentState.y, currentState.z);

    // Rotation of the entire scene wrt mouse movements
    glm::mat4 mat;
    mat = glm::toMat4(currentState.rotation);
    glMultMatrixf(glm::value_ptr(mat));

    GLfloat light2_position[] = { 0.0f, 0.0f, 100.0f, 1.0f };
    glTranslatef(0, 0, 100);
    glutSolidCube(5);
    glTranslatef(0, 0, -100);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);

    GLfloat light3_position[] = { 100.0f, 0.0f, 0.0f, 1.0f };
    glTranslatef(100, 0, 0);
    glutSolidCube(5);
    glTranslatef(-100, 0, 0);
    glLightfv(GL_LIGHT3, GL_POSITION, light3_position);

    glPushMatrix();
    glRotatef(angle, -0.5, 0, 1);
    glPushMatrix();
    glTranslatef(300, 0, 50);

    GLfloat light_position_sun[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position_sun);
    glColor4f(1, 1, 0, 1.0);
    glutSolidSphere(20, 25, 25);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-200, 0, -50);

    GLfloat light_position_moon[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light_position_moon);
    glColor4f(1, 1, 1, 1.0);
    glutSolidSphere(10, 25, 25);
    glPopMatrix();

    glPopMatrix();

    DrawTessText();
    DrawBezierText();

    glutSwapBuffers();
}

void Loop()
{
    glutMainLoop();
}

void Idle()
{
    glutPostRedisplay();
}
