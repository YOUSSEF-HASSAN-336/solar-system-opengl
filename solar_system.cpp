#include <GL/glut.h>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <SOIL/SOIL.h>
#include <cstring>
#include <algorithm>

const float PI = 3.14159f;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
float cameraAngleX = 20.0f, cameraAngleY = 30.0f;
float cameraDistance = 30.0f;
bool paused = false;
float orbitSpeed = 0.1f;

const int NUM_STARS = 200;
float stars[NUM_STARS][3];

struct Planet {
    float distance, size, orbitSpeed, angle, rotation;
    float r, g, b;
    const char* name;
    GLuint texture;
};

Planet planets[] = {
    {4.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, "Mercury", 0},
    {6.0f, 0.6f, 0.8f, 0.0f, 0.0f, 1.0f, 0.3f, 0.3f, "Venus", 0},
    {8.0f, 0.7f, 0.6f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, "Earth", 0},
    {10.0f, 0.4f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, "Mars", 0},
    {15.0f, 1.2f, 0.4f, 0.0f, 0.0f, 0.8f, 0.8f, 0.0f, "Jupiter", 0},
    {20.0f, 1.0f, 0.3f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, "Saturn", 0},
    {25.0f, 0.8f, 0.2f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, "Uranus", 0},
    {30.0f, 0.7f, 0.15f, 0.0f, 0.0f, 0.2f, 0.2f, 0.7f, "Neptune", 0}
};

GLuint loadTexture(const char* filename) {
    GLuint textureID = SOIL_load_OGL_texture(
        filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (textureID == 0) {
        printf("Error loading texture: %s\n", SOIL_last_result());
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return textureID;
}

void drawOrbit(float radius) {
    glBegin(GL_LINE_LOOP);
    glColor3f(0.5f, 0.5f, 0.5f);
    for (int i = 0; i < 100; ++i) {
        float theta = 2.0f * PI * i / 100;
        glVertex3f(radius * cos(theta), 0.0f, radius * sin(theta));
    }
    glEnd();
}

void drawLabel(const char* text, float x, float y, float z) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y + 0.6f, z);
    for (int i = 0; text[i] != '\0'; ++i)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    glEnable(GL_LIGHTING);
}

void drawStars() {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; ++i)
        glVertex3f(stars[i][0], stars[i][1], stars[i][2]);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawSaturnRings(float size) {
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(0.8f, 0.8f, 0.8f);
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_LINE);
    gluDisk(quadric, size * 1.2f, size * 2.0f, 100, 1);
    gluDeleteQuadric(quadric);
    glPopMatrix();
}

void drawPlanet(Planet p) {
    glPushMatrix();
    glRotatef(p.angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(p.distance, 0.0f, 0.0f);
    glRotatef(p.rotation, 0.0f, 1.0f, 0.0f);

    if (p.texture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, p.texture);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(p.r, p.g, p.b);
    }

    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, p.size, 30, 30);
    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_2D);

    if (strcmp(p.name, "Saturn") == 0)
        drawSaturnRings(p.size);

    drawLabel(p.name, 0.0f, p.size, 0.0f);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

    GLfloat light_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    drawStars();

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(1.0f, 30, 30);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    for (auto& p : planets) {
        drawOrbit(p.distance);
        drawPlanet(p);
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!paused) {
        for (auto& p : planets) {
            p.angle += p.orbitSpeed * orbitSpeed;
            if (p.angle > 360.0f) p.angle -= 360.0f;
            p.rotation += 1.0f;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p': paused = !paused; break;
    case '+': orbitSpeed += 0.01f; break;
    case '-': orbitSpeed -= 0.01f; break;
    case 27: exit(0); break;
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT: cameraAngleY -= 5.0f; break;
    case GLUT_KEY_RIGHT: cameraAngleY += 5.0f; break;
    case GLUT_KEY_UP: cameraAngleX -= 5.0f; break;
    case GLUT_KEY_DOWN: cameraAngleX += 5.0f; break;
    case GLUT_KEY_PAGE_UP: cameraDistance = std::max(cameraDistance - 1.0f, 5.0f); break;
    case GLUT_KEY_PAGE_DOWN: cameraDistance += 1.0f; break;
    }
    glutPostRedisplay();
}

void initStars() {
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i][0] = ((rand() % 1000) - 500) / 10.0f;
        stars[i][1] = ((rand() % 1000) - 500) / 10.0f;
        stars[i][2] = ((rand() % 1000) - 500) / 10.0f;
    }
}

void loadPlanetTextures() {
    planets[0].texture = loadTexture("mercury.jpg");
    planets[1].texture = loadTexture("venus.jpg");
    planets[2].texture = loadTexture("earth.jpg");
    planets[3].texture = loadTexture("mars.jpg");
    planets[4].texture = loadTexture("jupiter.jpg");
    planets[5].texture = loadTexture("saturn.jpg");
    planets[6].texture = loadTexture("uranus.jpg");
    planets[7].texture = loadTexture("neptune.jpg");
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)WINDOW_WIDTH / WINDOW_HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    initStars();
    loadPlanetTextures();
}

void reshape(int width, int height) {
    if (height == 0) height = 1; 
    float aspect = (float)width / height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Solar System");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
