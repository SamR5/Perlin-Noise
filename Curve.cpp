
/*

Perlin Noise Curve


*/


#include <GL/gl.h>
#include <GL/glut.h>


#include <iostream>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <sstream>

#include "Perlin.h"

#define WIDTH 800
#define HEIGHT 400
#define FPS 50
#define CELL_SIZE 1


void generate_1D_noise();
void update_parameter(int val);
void update_min_max();
double to_scale(double n);
void parameter_color(int value);
void draw_grid();
void parameter_color(int value);
void draw_parameters();

void display_callback();
void reshape_callback(int w, int h);
void timer_callback(int);
void keyboard_special_callback(int special, int x, int y);

double grid[WIDTH];

int selectionRow(0); // currently selected row

// to simulate landscape scrolling
int xOff(2); // x offset at each step
int step(WIDTH); // current step


// used to find extrema in the grid
double mini;
double maxi;

double persistence = 0.2;
double deltaPerisistance = 0.01;
int maxOctave = 8;
int minOctave = 3;
int primeIndex = 5;

void generate_1D_noise() {
    int xSign = std::copysign(1, xOff);
    int deltaX = xOff*step;
    for (int i=0; i<WIDTH; i++) {
        grid[i] = noise_value(xSign*(i+deltaX), 0);
    }
}

void update_parameter(int val) {
    switch (selectionRow) {
        case 0: persistence += val * deltaPerisistance; break;
        case 1: maxOctave += val; break;
        case 2: minOctave += val;
    }
    if (minOctave > maxOctave)
        minOctave = maxOctave;
}

void update_min_max() {
    mini = 1e5;
    maxi = -1e5;
    for (int i=0; i<WIDTH; i++) {
        if (grid[i] > maxi) {
            maxi = grid[i];
            continue;
        } else if (grid[i] < mini) {
            mini = grid[i];
        }
    }
}

double to_scale(double n) {
    return n;// / 2;
    return n / (maxi-mini);
}

void draw_grid() {
    double val;
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<WIDTH; i++) {
        val = to_scale(grid[i]);
        glVertex2f(i, val);
    }
    glEnd();
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
    glRasterPos2f(2, -0.97);
    for (char c : minO.str())
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    
    parameter_color(1);
    glRasterPos2f(2, -0.92);
    for (char c : maxO.str())
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    
    parameter_color(0);
    glRasterPos2f(2, -0.87);
    for (char c : pers.str())
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
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
    glOrtho(0.0-margin, WIDTH+margin,
            -1, 1,
            0.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    generate_1D_noise();
    step++;

    glutPostRedisplay();
    
    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    //std::cout << duration.count() << std::endl;
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
    glutCreateWindow("Perlin Noise Curve");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    glutSpecialFunc(keyboard_special_callback);
    // generate the whole grid once and then updates the sides
    generate_1D_noise();
    update_min_max();

    glutMainLoop();

    return 0;
}
