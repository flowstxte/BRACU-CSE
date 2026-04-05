from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import random

bg = (0.0, 0.0, 0.0, 1.0)
tar_bg = (0.0, 0.0, 0.0, 1.0)
transition = 0.001
angle = 0.0
rain = []

for i in range(120):
    xp = random.uniform(0, 800)
    yp = random.uniform(150, 600)
    rain.append([xp, yp])

def ground():
    glColor3f(0.0, 0.5, 0.0)
    glLineWidth(4)
    glBegin(GL_LINES)
    glVertex2f(0, 150) 
    glVertex2f(800, 150)
    glEnd()

def house():
    # body
    glColor3f(0.8, 0.6, 0.3)
    glBegin(GL_TRIANGLES)
    glVertex2f(200, 150)
    glVertex2f(600, 150)
    glVertex2f(200, 350)
    
    glVertex2f(600, 150)
    glVertex2f(600, 350)
    glVertex2f(200, 350)
    glEnd()
    # roof
    glColor3f(0.7, 0.2, 0.2)
    glBegin(GL_TRIANGLES)
    glVertex2f(400, 450)
    glVertex2f(150, 350)
    glVertex2f(650, 350)
    glEnd()
    # door
    glColor3f(0.4, 0.2, 0.1)
    glBegin(GL_TRIANGLES)
    glVertex2f(350, 150)
    glVertex2f(450, 150)
    glVertex2f(350, 280)
    
    glVertex2f(450, 150)
    glVertex2f(450, 280)
    glVertex2f(350, 280)
    glEnd()
    
    # window
    glColor3f(0.4, 0.2, 0.1)
    glBegin(GL_TRIANGLES)
    glVertex2f(480, 200)
    glVertex2f(570, 200)
    glVertex2f(480, 280)
    
    glVertex2f(570, 200)
    glVertex2f(570, 280)
    glVertex2f(480, 280)
    glEnd()
    
def rain_drop():
    global angle
    glColor3f(0.4, 0.4, 1.0)
    glBegin(GL_LINES)
    
    for i in range(len(rain)):
        xp, yp = rain[i]
        
        glVertex2f(xp, yp)
        glVertex2f(xp + 2, yp - 4)
        xp += angle
        yp -= .5
        
        g = yp < 150
        b = (200 < xp < 600 and 150 < yp < 350)
        
        if g or b:
            xp = random.uniform(0, 800)
            yp = random.uniform(150, 600)
        rain[i] = [xp, yp]
    
    glEnd()

def display():
    global bg, tar_bg, transition
    r = bg[0] + (tar_bg[0] - bg[0]) * transition
    g = bg[1] + (tar_bg[1] - bg[1]) * transition
    b = bg[2] + (tar_bg[2] - bg[2]) * transition
    bg = (r, g, b, 1.0)

    glClearColor(*bg)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, 800, 0.0, 600, 0.0, 1.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    ground()
    house()
    rain_drop()
    glutSwapBuffers()

def handle_keyboard(key, x, y):
    global tar_bg
    
    if key == b'd':
        tar_bg = (0.9, 0.9, 0.9, 1.0)
        print("Day mode activated")
    elif key == b'n':
        tar_bg = (0.0, 0.0, 0.0, 1.0)
        print("Night mode activated")

def handle_special_keys(key, x, y):
    global angle
    
    if key == GLUT_KEY_RIGHT:
        angle += 0.2
        print("Rain bends right")
    elif key == GLUT_KEY_LEFT:
        angle -= 0.2
        print("Rain bends left")

    glutPostRedisplay()

def animate_scene():
    glutPostRedisplay()

glutInit()
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE)
glutInitWindowSize(800, 600)
glutInitWindowPosition(100, 100)
glutCreateWindow(b"House in Rainfall")

glutDisplayFunc(display)
glutKeyboardFunc(handle_keyboard)
glutSpecialFunc(handle_special_keys)
glutIdleFunc(animate_scene)
glutMainLoop()