from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

# Window size
width, height = 500, 500

# Line coordinates
x1, y1 = 50, 100
x2, y2 = 400, 250

# Toggle flag
use_midpoint = True

def init():
    glClearColor(0, 0, 0, 1)  # Black background
    gluOrtho2D(0, width, 0, height)  # 2D projection

def draw_pixel(x, y):
    glBegin(GL_POINTS)
    glVertex2f(x, y)
    glEnd()

def draw_midpoint_line(x1, y1, x2, y2):
    dx = x2 - x1
    dy = y2 - y1
    x, y = x1, y1
    p = 2 * dy - dx

    glColor3f(1, 1, 0)  # Yellow
    draw_pixel(x, y)

    while x < x2:
        x += 1
        if p < 0:
            p += 2 * dy
        else:
            y += 1
            p += 2 * dy - 2 * dx
        draw_pixel(x, y)

def draw_smooth_line(x1, y1, x2, y2):
    glColor3f(0, 1, 0)  # Green line for OpenGL smooth version
    glBegin(GL_LINES)
    glVertex2f(x1, y1)
    glVertex2f(x2, y2)
    glEnd()

def display():
    glClear(GL_COLOR_BUFFER_BIT)
    glPointSize(4)

    if use_midpoint:
        draw_midpoint_line(x1, y1, x2, y2)
    else:
        draw_smooth_line(x1, y1, x2, y2)

    glFlush()

def keyboard(key, x, y):
    global use_midpoint
    if key == b' ':
        use_midpoint = not use_midpoint
        glutPostRedisplay()

def main():
    glutInit()
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
    glutInitWindowSize(width, height)
    glutCreateWindow(b"Midpoint Line Toggle Demo")
    init()
    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    glutMainLoop()

if __name__ == '__main__':
    main()
