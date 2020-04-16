/*

Perlin Noise Clouds

*/


#include <GL/gl.h>
#include <GL/glut.h>

#include "Perlin.h"
#include <iostream>
#include <cstdio>
#include <chrono>
#include <sstream>
#include <cmath>

#define WIDTH 500
#define HEIGHT 500
#define FPS 20
#define PI 3.1415926
#define CELL_SIZE 1


void generate_2D_noise();
void update_parameter(int val);
void offset_grid();
void update_min_max();
double to_scale(double n);
void draw_grid();
void parameter_color(int value);
void draw_parameters();

void display_callback();
void reshape_callback(int width, int height);
void timer_callback(int);
void keyboard_special_callback(int special, int x, int y);


double grid[HEIGHT][WIDTH];

int selectionRow(0); // currently selected row

double persistence = 0.8;
double deltaPersistence = 0.05;
int maxOctave = 8;
int minOctave = 3;
int primeIndex = 5;

// to simulate landscape scrolling
int xOff(2); // x offset at each step
int yOff(1); // y offset at each step
int step(std::max(HEIGHT, WIDTH)); // current step


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

void update_parameter(int val) {
    switch (selectionRow) {
        case 0: persistence += val * deltaPersistence; break;
        case 1: maxOctave += val; break;
        case 2: minOctave += val;
    }
    if (minOctave > maxOctave)
        minOctave = maxOctave;
    generate_2D_noise();
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

double to_scale(double n) {
    return (n - mini) / (maxi-mini);
}

void parameter_color(int value) {
    if (selectionRow == value)
        glColor3f(0, 1, 0);
    else
        glColor3f(1, 1, 1);
}

void draw_parameters() {
    std::stringstream minO;
    std::stringstream maxO;
    std::stringstream pers;
    minO << "Min octave: " << minOctave;
    maxO << "Max octave: " << maxOctave;
    pers << "Persistance: " << persistence;
    
    parameter_color(2);
    glRasterPos2f(2, 2);
    for (char c : minO.str())
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    
    parameter_color(1);
    glRasterPos2f(2, 10);
    for (char c : maxO.str())
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    
    parameter_color(0);
    glRasterPos2f(2, 18);
    for (char c : pers.str())
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
}

void draw_grid() {
    double val;
    glPointSize(CELL_SIZE);
    glBegin(GL_POINTS);
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            val = to_scale(grid[i][j]);
            glColor3f(val, val, val);
            glVertex2f(i, j);
            //glRecti(i, j, i+CELL_SIZE, j+CELL_SIZE);
        }
    }
    glEnd();
}

void display_callback() {
    glClear (GL_COLOR_BUFFER_BIT);

    draw_grid();
    draw_parameters();

    glFlush();
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int margin(0);
    glOrtho(0.0-margin, (double)WIDTH+margin,
            0.0-margin, (double)HEIGHT+margin,
            -1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
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
    // std::cout << duration.count() << std::endl;
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

void keyboard_special_callback(int special, int x, int y) {
    switch (special) {
        case GLUT_KEY_UP:
            selectionRow--; break;
        case GLUT_KEY_DOWN:
            selectionRow++; break;
    }
    if (selectionRow<0)
        selectionRow = 2;
    selectionRow %= 3;
    switch (special) {
        case GLUT_KEY_RIGHT:
            update_parameter(1); break;
        case GLUT_KEY_LEFT:
            update_parameter(-1);
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(WIDTH*CELL_SIZE, HEIGHT*CELL_SIZE);
    glutCreateWindow("Perlin Noise Clouds");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    glutSpecialFunc(keyboard_special_callback);
    // generate the whole grid once and then updates the sides
    generate_2D_noise();
    update_min_max();

    glutMainLoop();

    return 0;
}
