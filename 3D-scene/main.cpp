#include <GL/glut.h>
#include <glm/glm.hpp>
#include "screen.hpp"

glm::vec3 cameraPos, cameraUp, cameraDir, worldUp;
// yaw to be initialized
float yaw = -90.0f, pitch;
// store for last mouse click coordinates
int lastX, lastY;
// field of view angle for perspective projection
float fov = 45.0f;
// various speeds
float zoomSpeed = 1.0f, panSpeed = 0.2f, dragSpeed = 0.005f;

void renderer();
void resizer(int, int);
void keyHandler(unsigned char, int, int);
void specialKeyHandler(int, int, int);
void clickHandler(int, int, int, int);
void dragHandler(int, int);
void animate();

void initializeCamera()
{
    cameraPos = {0, 0, 5};
    cameraUp = {0, 1, 0};
    cameraDir = {0, 0, -1};
    worldUp = {0, 1, 0};
}

void initialize()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(renderer);
    glutReshapeFunc(resizer);
    glutKeyboardFunc(keyHandler);
    glutSpecialFunc(specialKeyHandler);
    glutMouseFunc(clickHandler);
    glutMotionFunc(dragHandler);
    glutIdleFunc(animate);

    initializeCamera();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);

    glutCreateWindow("Window");
    initialize();

    glutMainLoop();
}

void resizer(int width, int height)
{
    WIDTH = width, HEIGHT = height;
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (double)WIDTH/HEIGHT, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void renderer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
        cameraDir.x + cameraPos.x, cameraDir.y + cameraPos.y, cameraDir.z + cameraPos.z,
        cameraUp.x, cameraUp.y, cameraUp.z);

    {
        GLfloat floors[] = 
        {
            20, -2, -20,
            -20, -2, -20,
            -20, -2, 20,
            20, -2, 20
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, floors);
        int f[] = {0, 1, 2, 3}; // white
        glColor3f(1.0f, 1.0f, 1.0f);
        glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, f);
    }

    glutSwapBuffers();
}

void keyHandler(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:    exit(0);
        case 'q':   exit(0);
        case 'w':   { cameraPos += panSpeed*cameraUp; break; }
        case 's':   { cameraPos -= panSpeed*cameraUp; break; }
        case 'a':
        {   
            glm::vec3 cameraRight = glm::cross(cameraDir, cameraUp);
            cameraRight = glm::normalize(cameraRight);
            cameraPos -= panSpeed*cameraRight;
            break;
        }
        case 'd':
        {
            glm::vec3 cameraRight = glm::cross(cameraDir, cameraUp);
            cameraRight = glm::normalize(cameraRight);
            cameraPos += panSpeed*cameraRight;
            break;
        }
        case 'x':
        {
            // zoom out
            if(fov < 45.0f)
                fov += zoomSpeed;
            else
                fov = 45.0f;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(fov, (double)WIDTH/HEIGHT, 0.1f, 100.0f);
            glMatrixMode(GL_MODELVIEW);
            break;
        }
        case 'z':
        {
            // zoom in
            if(fov > 1.0f)
                fov -= zoomSpeed;
            else
                fov = 1.0f;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(fov, (double)WIDTH/HEIGHT, 0.1f, 100.0f);
            glMatrixMode(GL_MODELVIEW);
            break;
        }
    }
}

void specialKeyHandler(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP:       { cameraPos += panSpeed*cameraDir; break; }
        case GLUT_KEY_DOWN:     { cameraPos -= panSpeed*cameraDir; break; }
    }
}

void clickHandler(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        lastX = x, lastY = y;
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        initializeCamera();
}

void dragHandler(int x, int y)
{
    float dispY = lastY - y;
    float dispX = x - lastX;
    dispX *= dragSpeed;
    dispY *= dragSpeed;
    yaw += dispX;
    pitch += dispY;
    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDir = glm::normalize(direction);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDir, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraDir));
}

void animate()
{
    glutPostRedisplay();
}