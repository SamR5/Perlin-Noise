/*

Perlin Noise Terrain

*/


#include <GL/gl.h>
#include <GL/glut.h>
#include "Perlin.h"
#include <iostream>
#include <cstdio>
#include <chrono>
#include <cmath>

#define WIDTH 200
#define HEIGHT 200
#define FPS 25
#define PI 3.1415926
#define CELL_SIZE 3




double grid[HEIGHT][WIDTH];

// to simulate landscape scrolling
int xOff(1); // x offset at each step
int yOff(0); // y offset at each step
int step(std::max(HEIGHT, WIDTH)); // current step

double persistence = 1.2; // useless after 1.5
double deltaPerisistance = 0.01;
int maxOctave = 8;
int minOctave = 2;
int primeIndex = 5;

// used to find extrema in the grid
double mini;
double maxi;

void generate_2D_noise() {
    int ySign = std::copysign(1, yOff);
    int xSign = std::copysign(1, xOff);
    int deltaX = xOff*step;
    int deltaY = yOff*step;
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            grid[i][j] = noise_value(ySign*(j+deltaY), xSign*(i+deltaX));
        }
    }
}

void offset_grid() {
    int xStarting, xEnding, xStep, yStarting, yEnding, yStep;
    xStarting = (xOff>=0) ? 0 : HEIGHT-1;
    xEnding = (xOff>=0) ? HEIGHT-xOff : -xOff;
    xStep = (xOff>=0) ? 1 : -1;
    yStarting = (yOff>=0) ? 0 : WIDTH-1;
    yEnding = (yOff>=0) ? WIDTH-yOff : -yOff;
    yStep = (yOff>=0) ? 1 : -1;
    for (int i=xStarting; i!=xEnding; i+=xStep) {
        for (int j=yStarting; j!=yEnding; j+=yStep) {
            grid[i][j] = grid[i+xOff][j+yOff];
        }
    }
    //recompute the sides of the grid
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            if (i>HEIGHT-xOff-1 || j>WIDTH-yOff-1 || i<-xOff+1 || j<-yOff+1)
                grid[i][j] = noise_value(yStep*(j+(yOff*step)), xStep*(i+(xOff*step)));
        }
    }
}

void update_min_max() {
    mini = 1e5;
    maxi = -1e5;
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            if (grid[i][j] > maxi) {
                maxi = grid[i][j];
                continue;
            } else if (grid[i][j] < mini) {
                mini = grid[i][j];
            }
        }
    }
}

int coeff(10);

double to_scale(double n) {
    return coeff * (n - mini) / (maxi-mini);
}

float gridVal[HEIGHT][WIDTH];

void draw_grid() {
    double val;
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            gridVal[i][j] = to_scale(grid[i][j]);
        }
    }
    glBegin(GL_TRIANGLES);
    for (int i=0; i<HEIGHT-1; i++) {
        for (int j=0; j<WIDTH-1; j++) {
            glColor3f(0, gridVal[i][j]/coeff, 0);
            glVertex3f(i, j, gridVal[i][j]);
            glVertex3f(i+1, j, gridVal[i+1][j]);
            glVertex3f(i, j+1, gridVal[i][j+1]);
            
            glVertex3f(i+1, j+1, gridVal[i+1][j+1]);
            glVertex3f(i+1, j, gridVal[i+1][j]);
            glVertex3f(i, j+1, gridVal[i][j+1]);
        }
    }
    glEnd();
}

void display_callback() {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, HEIGHT/2, 20,
              WIDTH*2.0/3.0, HEIGHT/2, 0,
              0, 0, 1);

    draw_grid();

    glFlush();
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, double(WIDTH)/HEIGHT, 1, 1000);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    //generate_2D_noise();
    offset_grid();
    //update_min_max();
    step++;

    glutPostRedisplay(); // run the display_callback function
    //std::cout << persistence << std::endl;
    //persistence += 0.05;

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    //std::cout << duration.count() << std::endl;
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(WIDTH*CELL_SIZE, HEIGHT*CELL_SIZE);
    glutCreateWindow("Perlin Noise");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);

    // generate the whole grid once and then updates the sides
    generate_2D_noise();
    update_min_max();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return 0;
}
