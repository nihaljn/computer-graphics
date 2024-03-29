/**
 * @file main.cpp
 * @brief Main driver file
 */
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "chair.hpp"
#include "cupboard.hpp"
#include "dining.hpp"
#include "floor.hpp"
#include "screen.hpp"
#include "obj.hpp"
#include "table.hpp"
#include "tv.hpp"
#include "wall.hpp"
#include "parser.hpp"

Object sofa = Object((char*)"obj/sofas.obj");
Object tableTop = Object((char*)"obj/table_top.obj");
Object tableBottom = Object((char*)"obj/table_bottom.obj");
Object vase = Object((char*)"obj/vase.obj");
Object plant = Object((char*)"obj/plant.obj");
Object paint = Object((char*)"obj/paint.obj");
Object paintHolder = Object((char*)"obj/paint_holder.obj");
Object paintThread = Object((char*)"obj/paint_thread.obj");
Object cupboard = Object((char*)"obj/cupboard.obj");
Object cupboardDoor = Object((char*)"obj/cupboard_door.obj");
Object cupboardKnob = Object((char*)"obj/cupboard_knob.obj");
Object lamp = Object((char*)"obj/lamp.obj");
std::vector<Object> book, bookCover;

// screen settings
extern int WIDTH, HEIGHT;
// camera settings
glm::vec3 cameraPos, cameraUp, cameraRight, cameraDir, worldUp;
// yaw to be initialized
float yaw = -90.0f, pitch = 0, roll = 0;
// store for last mouse click coordinates
int lastX = WIDTH/2, lastY = HEIGHT/2, start = 0;
// field of view angle for perspective projection
float fov = 45.0f, near = 0.1f, far = 200.0f;
// various speeds
float zoomSpeed = 1.0f, panSpeed = 1.0f, dragSpeed = 0.005f;


void renderer();
void resizer(int, int);
/**
 * @brief callback to handle keyboard input
 * 'w': move along +up
 * 's': move along -up
 * 'a': move along +right
 * 'd': move along -right
 * 'z': zoom in
 * 'x': zoom out
 */
void keyHandler(unsigned char, int, int);
/**
 * @brief callback to handle special keys
 * 'up': move along +normal
 * 'down': move along -normal
 * 'left': roll along -normal
 * 'right': roll along +normal
 */
void specialKeyHandler(int, int, int);
void clickHandler(int, int, int, int);
/**
 * @brief callback to handle mouse drag.
 * horizontal drag: yaw
 * vertical drag: pitch
 */
void dragHandler(int, int);
void animate();
void initializeCamera();

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

void resizer(int width, int height)
{
    WIDTH = width, HEIGHT = height;
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (double)WIDTH/HEIGHT, near, far);
    glMatrixMode(GL_MODELVIEW);
}

void keyHandler(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:    exit(0);
        case 'q':   exit(0);
        case 'w':   { cameraPos += panSpeed*cameraUp; break; }
        case 's':   { cameraPos -= panSpeed*cameraUp; break; }
        case 'a':   { cameraPos -= panSpeed*cameraRight; break; }
        case 'd':   { cameraPos += panSpeed*cameraRight; break; }
        case 'x':
        {
            // zoom out
            if(fov < 45.0f)
                fov += zoomSpeed;
            else
                fov = 45.0f;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(fov, (double)WIDTH/HEIGHT, near, far);
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
            gluPerspective(fov, (double)WIDTH/HEIGHT, near, far);
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
        case GLUT_KEY_LEFT:     
        {
            roll += 1.0f;
            float x = cameraRight.x, y = cameraRight.y, z = cameraRight.z;
            glm::vec3 right;
            right.x = x*cosf(glm::radians(roll)) - y*sinf(glm::radians(roll));
            right.y = x*sinf(glm::radians(roll)) + y*cosf(glm::radians(roll));
            right.z = z;
            cameraRight = glm::normalize(right);
            cameraUp = glm::normalize(glm::cross(cameraRight, cameraDir));
            break; 
        }
        case GLUT_KEY_RIGHT:
        {
            roll -= 1.0f;
            float x = cameraRight.x, y = cameraRight.y, z = cameraRight.z;
            glm::vec3 right;
            right.x = x*cosf(glm::radians(roll)) - y*sinf(glm::radians(roll));
            right.y = x*sinf(glm::radians(roll)) + y*cosf(glm::radians(roll));
            right.z = z;
            cameraRight = glm::normalize(right);
            cameraUp = glm::normalize(glm::cross(cameraRight, cameraDir));
            break; 
        }
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
    cameraRight = glm::normalize(glm::cross(cameraDir, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraDir));
}

void animate()
{
    glutPostRedisplay();
}

void initializeCamera()
{
    // top view
    // cameraPos = {0, 90, 0};
    // cameraUp = {-1, 0, 0};
    // cameraDir = {0, -1, 0};
    // worldUp = {0, 1, 0};

    // // front view
    cameraPos = {0, 10, 90};
    cameraUp = {0, 1, 0};
    cameraDir = {0, 0, -1};
    worldUp = {0, 1, 0};

    // // left view
    // cameraPos = {50, 10, 0};
    // cameraUp = {0, 1, 0};
    // cameraDir = {-1, 0, 0};
    // worldUp = {0, 1, 0};

    yaw = -90.0f, pitch = 0, roll = 0;
}

void renderer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
        cameraDir.x + cameraPos.x, cameraDir.y + cameraPos.y, cameraDir.z + cameraPos.z,
        worldUp.x + roll, worldUp.y + 2.5, 0);

    draw_floor(-30, 30, 30, -30);
    draw_walls(-30, 30, 30, -30, 20);

    glPushMatrix();
    {
        glTranslatef(18, 0, -20);
        glScalef(1.3f, 1.3f, 1.3f);
        draw_dining();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-29, 10, 0);
        glRotatef(90, 0, 1, 0);
        draw_tv();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glColor3f(0.878, 0.619, 0.741);     // pinkish
        glTranslatef(0, 1.7, 0);
        sofa.draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-13, 0, 0);
        // glRotatef(90, -1, 0, 0);
        glScalef(0.5f, 0.5f, 0.5f);
        glColor3f(0.160f, 0.015f, 0.184f);  // dark purple
        tableTop.draw();
        glColor3f(1, 1, 1);
        tableBottom.draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-25, 2, 22);
        glScalef(0.5f, 0.5f, 0.5f);
        glColor3f(1, 0, 0);         // red
        vase.draw();
        glColor3f(0, 1, 0);         // green
        plant.draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(0, 10, -29.7);
        glScalef(0.5f, 0.5f, 0.5f);
        glColor3f(0.9f, 0.9f, 0.9f);        // grey
        paint.draw();
        glColor3f(0.521f, 0.298f, 0);       // brown
        paintHolder.draw();
        paintThread.draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(20, 9.2, 25);
        draw_cupboard(cupboard, cupboardDoor, cupboardKnob);
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-28, 12, 12);
        glRotatef(90, 0, 0, -1);
        glRotatef(90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glScalef(0.15f, 0.15f, 0.15f);
        glColor3f(1, 0, 0);
        bookCover[0].draw();
        glColor3f(0.5, 0.5, 0.5);
        book[0].draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-28.0f, 11.5f, 12.7f);
        glRotatef(90, 0, 0, -1);
        glRotatef(90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glScalef(0.1f, 0.1f, 0.1f);
        glColor3f(1, 0, 1);
        bookCover[1].draw();
        glColor3f(0.5, 0.5, 0.5);
        book[1].draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-11.0f, 5.46f, 5.2f);
        glScalef(0.1f, 0.2f, 0.1f);
        glColor3f(0.137f, 0.238f, 0.668f);
        bookCover[1].draw();
        glColor3f(0.8, 0.8, 0.8);
        book[1].draw();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(0, 15.9, 0);
        glScalef(0.028f, 0.028f, 0.028f);
        glColor3f(0.894f, 0.788f, 0.305f);
        lamp.draw();
    }
    glPopMatrix();
    
    glutSwapBuffers();
}

// load data into object variables from .obj files
void load_objects()
{
    sofa.load();
    tableTop.load();
    tableBottom.load();
    vase.load();
    plant.load();
    paint.load();
    paintHolder.load();
    paintThread.load();
    cupboard.load();
    cupboardDoor.load();
    cupboardKnob.load();
    lamp.load();
    
    for (int i = 0; i < 3; i++)
    {
        bookCover.push_back(Object((char*)"obj/book_outer.obj"));
        bookCover[i].load();
        book.push_back(Object((char*)"obj/book_inner.obj"));
        book[i].load();
    }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);

    glutCreateWindow("Window");
    initialize();

    load_objects();

    glutMainLoop();
}