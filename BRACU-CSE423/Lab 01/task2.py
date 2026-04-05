from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import random
import time
import math

w, h = 800, 600
points = []
speed = 0.1
mult = 1.0
paused = False
blink = False
bl_start = 0
bl_time = 1.0

def init():
    glClearColor(0, 0, 0, 1)
    gluOrtho2D(0, w, 0, h)
    glEnable(GL_POINT_SMOOTH)

def draw_border():
    glColor3f(0.3, 0.3, 0.3)
    glLineWidth(2)
    glBegin(GL_LINE_LOOP)
    for vx, vy in [(0, 0), (w, 0), (w, h), (0, h)]:
        glVertex2f(vx, vy)
    glEnd()

def get_blink_factor():
    if not blink:
        return 1.0
    
    cur_time = time.time()
    elapsed = cur_time - bl_start
    cp = (elapsed % bl_time) / bl_time
    blink_fac = 0.5 * (1 + math.sin(2 * math.pi * cp - math.pi/2))
    
    return blink_fac

def draw_points():
    glPointSize(8)
    glBegin(GL_POINTS)
    blink_fac = get_blink_factor()
    
    for x, y, _, _, oc in points:
        current_color = [c * blink_fac for c in oc]
        glColor3f(*current_color)
        glVertex2f(x, y)
    glEnd()

def update_points():
    if paused:
        return
    for i in range(len(points)):
        x, y, dx, dy, color = points[i]
        x += dx * speed * mult
        y += dy * speed * mult
        if x <= 0 or x >= w: dx = -dx
        if y <= 0 or y >= h: dy = -dy
        x = max(0, min(w, x))
        y = max(0, min(h, y))
        points[i] = (x, y, dx, dy, color)

def display():
    glClear(GL_COLOR_BUFFER_BIT)
    draw_border()
    update_points()
    draw_points()
    glutSwapBuffers()

def mouse(btn, state, x, y):
    global blink, bl_start
    if state != GLUT_DOWN or paused:
        return
    gx, gy = x, h - y
    if btn == GLUT_RIGHT_BUTTON:
        dx, dy = random.choice([(1, 1), (1, -1), (-1, 1), (-1, -1)])
        color = [random.uniform(0.2, 1.0) for _ in range(3)]
        points.append((gx, gy, dx, dy, color))
        print(f"Mouse clicked at ({gx}, {gy})")
    elif btn == GLUT_LEFT_BUTTON:
        blink = not blink
        if blink:
            bl_start = time.time()
        print(f"Blink mode: {blink}")

def keyboard(key, *_):
    global paused
    if key == b' ':
        paused = not paused
    print(f"Spacebar pressed. Paused: {paused}")

def special_keys(key, *_):
    global mult
    if key == GLUT_KEY_UP:
        mult += 0.2
    elif key == GLUT_KEY_DOWN:
        mult -= 0.2
    print(f"Speed: {mult}")

glutInit()
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
glutInitWindowSize(w, h)
glutCreateWindow(b"The Amazing Box")
init()
glutDisplayFunc(display)
glutMouseFunc(mouse)
glutKeyboardFunc(keyboard)
glutSpecialFunc(special_keys)
glutIdleFunc(glutPostRedisplay)
glutMainLoop()