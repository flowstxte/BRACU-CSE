from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import random
import time
import math
from OpenGL.GLUT import GLUT_BITMAP_HELVETICA_18

over = False
pts = 0
spd = 3.0
last_t = 0
hi = 0
file = "score.txt"
cd_total = 5.0
cd_start = 0.0
cd_active = False
start_t = time.time()

msgs = [
    "Nice flying - keep your eyes forward!",
    "Great control! You can do this!",
    "Stay focused. Victory is ahead!",
    "Amazing reflexes! Keep dodging!",
    "Full throttle - don't give up!",
    "You're a sky legend - push on!",
    "Smooth as silk! Keep it up!",
    "The canyon can't stop you!",
    "Master pilot in action!",
    "Threading the needle like a pro!",
    "Speed demon! Own the skies!",
    "Unstoppable force of nature!",
    "Flying ace - show them how it's done!",
    "Precision flying at its finest!",
    "You're built for this challenge!",
    "Soaring to new heights!",
    "The desert winds are with you!",
    "Nothing can ground you now!",
    "Pure piloting perfection!",
    "You're writing aviation history!"
]

ship = [0, 50, -400]
boost = False
boost_end = 0
hills = []
rocks = []
trees = []
patches = []
H_COUNT = 50
R_COUNT = 30
T_COUNT = 20

def init():
    global over, pts, spd, ship, boost, hills, rocks, trees, last_t, patches, start_t, cd_start, cd_active
    over = False
    pts = 0
    spd = 3.0
    ship = [0, 50, -400]
    boost = False
    hills = []
    rocks = []
    trees = []
    gen_terrain()
    load_hi()
    last_t = time.time()
    start_t = time.time()
    cd_start = time.time()
    cd_active = True
    patches = []
    for i in range(100):
        x = random.uniform(-600, 600)
        z = random.uniform(-2000, 500)
        sz = random.uniform(20, 50)
        patches.append({'x': x, 'z': z, 'size': sz})

def gen_terrain():
    for i in range(H_COUNT):
        side = random.choice([-1, 1])
        x = random.uniform(-400, 400) * side
        z = -i * 300
        h = random.uniform(100, 400)
        w = random.uniform(40, 80)
        hills.append({'x': x, 'z': z, 'height': h, 'width': w})

    for i in range(R_COUNT):
        side = random.choice([-1, 1])
        x = random.uniform(50, 120) * side
        z = random.uniform(-H_COUNT * 180, 0)
        h = random.uniform(30, 120)
        w = random.uniform(20, 60)
        rocks.append({'x': x, 'z': z, 'height': h, 'width': w})

    for i in range(T_COUNT):
        x = random.uniform(-400, 400)
        z = random.uniform(-H_COUNT * 180, 0)
        h = random.uniform(40, 80)
        trees.append({'x': x, 'z': z, 'height': h})

def text(x, y, txt, font=GLUT_BITMAP_HELVETICA_18):
    glColor3f(1, 1, 1)
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluOrtho2D(0, 1000, 0, 800)
    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()
    glRasterPos2f(x, y)
    for ch in txt:
        glutBitmapCharacter(font, ord(ch))
    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)

def load_hi():
    global hi
    try:
        with open(file, 'r') as f:
            hi = int(f.read().strip())
    except (FileNotFoundError, ValueError):
        hi = 0
        save_hi()

def save_hi():
    with open(file, 'w') as f:
        f.write(str(hi))

def draw_ship():
    glPushMatrix()
    glTranslatef(ship[0], ship[1], ship[2])

    glPushMatrix()
    glColor3f(0.3, 0.3, 0.3)
    glTranslatef(0, 0, 0)
    glScalef(8, 8, 70)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.2, 0.2, 0.2)
    glTranslatef(0, 0, 45)
    glRotatef(90, 1, 0, 0)
    glutSolidCone(8, 20, 8, 8)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.1, 0.1, 0.1)
    glTranslatef(0, 4, 15)
    glScalef(6, 4, 25)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.4, 0.4, 0.4)
    glTranslatef(-25, -2, -10)
    glRotatef(-15, 0, 0, 1)
    glScalef(35, 3, 40)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.4, 0.4, 0.4)
    glTranslatef(25, -2, -10)
    glRotatef(15, 0, 0, 1)
    glScalef(35, 3, 40)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.2, 0.2, 0.2)
    glTranslatef(-45, -2, -15)
    glRotatef(-45, 0, 1, 0)
    glutSolidCone(12, 25, 6, 6)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.2, 0.2, 0.2)
    glTranslatef(45, -2, -15)
    glRotatef(45, 0, 1, 0)
    glutSolidCone(12, 25, 6, 6)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.3, 0.3, 0.3)
    glTranslatef(-12, 2, -30)
    glScalef(8, 12, 15)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.3, 0.3, 0.3)
    glTranslatef(12, 2, -30)
    glScalef(8, 12, 15)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.25, 0.25, 0.25)
    glTranslatef(0, 8, -30)
    glScalef(3, 20, 20)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.15, 0.15, 0.15)
    glTranslatef(-8, -2, -35)
    glRotatef(90, 1, 0, 0)
    glutSolidCone(4, 8, 8, 8)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.15, 0.15, 0.15)
    glTranslatef(8, -2, -35)
    glRotatef(90, 1, 0, 0)
    glutSolidCone(4, 8, 8, 8)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.9, 0.9, 0.9)
    glTranslatef(-20, -1, -5)
    glScalef(15, 1, 20)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glColor3f(0.9, 0.9, 0.9)
    glTranslatef(20, -1, -5)
    glScalef(15, 1, 20)
    glutSolidCube(1)
    glPopMatrix()

    if boost:
        glPushMatrix()
        glColor3f(1.0, 0.2, 0.0)
        glTranslatef(-6, -2, 65)
        glutSolidCone(8, 35, 10, 10)
        glPopMatrix()

        glPushMatrix()
        glColor3f(1.0, 0.5, 0.0)
        glTranslatef(-6, -2, 68)
        glutSolidCone(6, 25, 8, 8)
        glPopMatrix()

        glPushMatrix()
        glColor3f(1.0, 0.2, 0.0)
        glTranslatef(6, -2, 65)
        glutSolidCone(8, 35, 10, 10)
        glPopMatrix()

        glPushMatrix()
        glColor3f(1.0, 0.5, 0.0)
        glTranslatef(6, -2, 68)
        glutSolidCone(6, 25, 8, 8)
        glPopMatrix()

        glPushMatrix()
        glColor3f(1.0, 0.3, 0.0)
        glTranslatef(0, -1, 70)
        glutSolidCone(10, 30, 8, 8)
        glPopMatrix()

        glPushMatrix()
        glColor3f(1.0, 1.0, 0.2)
        glTranslatef(0, -1, 75)
        glutSolidCone(5, 20, 6, 6)
        glPopMatrix()

    glPopMatrix()

def draw_wall(x, z, h, w):
    layers = int(h / 30)
    for i in range(layers):
        ly = 30
        y = i * ly
        lw = w * (1.0 - i * 0.05)

        glPushMatrix()
        glTranslatef(x, y, z)

        r = 0.6 + (i * 0.02)
        g = 0.25 + (i * 0.01)
        b = 0.1 + (i * 0.01)

        glColor3f(r, g, b)
        glScalef(lw, ly, lw * 0.8)
        glutSolidCube(1)
        glPopMatrix()

def draw_rock(x, z, h, w):
    glPushMatrix()
    glTranslatef(x, h/2, z)
    glColor3f(0.7, 0.3, 0.15)
    glScalef(w, h, w * 0.7)
    glutSolidCube(1)
    glPopMatrix()

def draw_tree(x, z, h):
    glPushMatrix()
    glTranslatef(x, h/2, z)
    glColor3f(0.3, 0.2, 0.1)
    glScalef(3, h, 3)
    glutSolidCube(1)
    glPopMatrix()

    bh = h * 0.7
    glPushMatrix()
    glTranslatef(x, bh, z)
    glColor3f(0.25, 0.15, 0.08)

    glPushMatrix()
    glRotatef(30, 0, 0, 1)
    glTranslatef(8, 0, 0)
    glScalef(16, 2, 2)
    glutSolidCube(1)
    glPopMatrix()

    glPushMatrix()
    glRotatef(-45, 0, 0, 1)
    glTranslatef(6, 0, 0)
    glScalef(12, 2, 2)
    glutSolidCube(1)
    glPopMatrix()

    glPopMatrix()

def draw_terrain():
    for h in hills:
        draw_wall(h['x'], h['z'], h['height'], h['width'])

    for r in rocks:
        draw_rock(r['x'], r['z'], r['height'], r['width'])

    for t in trees:
        draw_tree(t['x'], t['z'], t['height'])

def draw_sky():
    glPushMatrix()
    glTranslatef(300, 400, -800)
    glColor3f(1.0, 0.8, 0.4)
    glutSolidSphere(60, 20, 20)
    glPopMatrix()

def update():
    global pts, spd, over, boost, last_t, patches

    if over:
        return

    if time.time() - last_t >= 1:
        pts += 1
        last_t = time.time()
        if pts > 0 and pts % 5 == 0:
            spd += 0.2

    cur_spd = spd * 2 if boost else spd
    if boost and time.time() > boost_end:
        boost = False

    for h in hills:
        h['z'] += cur_spd

    for r in rocks:
        r['z'] += cur_spd

    for t in trees:
        t['z'] += cur_spd

    for h in hills:
        if h['z'] > 100:
            side = random.choice([-1, 1])
            h['x'] = random.uniform(-400, 400) * side
            h['z'] = -H_COUNT * 300
            h['height'] = random.uniform(100, 400)
            h['width'] = random.uniform(40, 80)

    for r in rocks:
        if r['z'] > 100:
            side = random.choice([-1, 1])
            r['x'] = random.uniform(50, 120) * side
            r['z'] = -H_COUNT * 150
            r['height'] = random.uniform(30, 120)
            r['width'] = random.uniform(20, 60)

    for t in trees:
        if t['z'] > 100:
            t['x'] = random.uniform(-400, 400)
            t['z'] = -H_COUNT * 150
            t['height'] = random.uniform(40, 80)

    for p in patches:
        p['z'] += cur_spd
        if p['z'] > 500:
            p['x'] = random.uniform(-600, 600)
            p['z'] = -2000
            p['size'] = random.uniform(20, 50)

    check_hit()

def check_hit():
    global over, hi
    sx = ship[0]
    sy = ship[1]
    sz = ship[2]

    sw = 50
    sl = 65
    sh = 10

    for h in hills:
        left = sx - sw/2
        right = sx + sw/2
        front = sz - sl/2
        back = sz + sl/2
        bottom = sy - sh/2
        top = sy + sh/2

        hleft = h['x'] - h['width']/2
        hright = h['x'] + h['width']/2
        hfront = h['z'] - h['width']/2
        hback = h['z'] + h['width']/2
        htop = h['height']

        if (right > hleft and left < hright and
            back > hfront and front < hback and
            bottom < htop):
            if pts > hi:
                hi = pts
                save_hi()
            over = True
            return

    for r in rocks:
        left = sx - sw/2
        right = sx + sw/2
        front = sz - sl/2
        back = sz + sl/2
        bottom = sy - sh/2
        top = sy + sh/2

        rleft = r['x'] - r['width']/2
        rright = r['x'] + r['width']/2
        rfront = r['z'] - r['width']/2
        rback = r['z'] + r['width']/2
        rtop = r['height']

        if (right > rleft and left < rright and
            back > rfront and front < rback and
            bottom < rtop):
            if pts > hi:
                hi = pts
                save_hi()
            over = True
            return

    for t in trees:
        left = sx - sw/2
        right = sx + sw/2
        front = sz - sl/2
        back = sz + sl/2
        bottom = sy - sh/2
        top = sy + sh/2

        tleft = t['x'] - 3
        tright = t['x'] + 3
        tfront = t['z'] - 3
        tback = t['z'] + 3
        ttop = t['height']

        if (right > tleft and left < tright and
            back > tfront and front < tback and
            bottom < ttop):
            if pts > hi:
                hi = pts
                save_hi()
            over = True
            return

def key(k, x, y):
    global boost, boost_end

    if k == b'r' or k == b'R':
        init()
    if k == b' ' and not over:
        if not boost:
            boost = True
            boost_end = time.time() + 3

    if not over:
        mv = 15
        alt = 10

        if k == b'a' or k == b'A':
            ship[0] -= mv
        if k == b'd' or k == b'D':
            ship[0] += mv
        if k == b'w' or k == b'W':
            ship[1] += alt
        if k == b's' or k == b'S':
            ship[1] -= alt

        if ship[0] > 380:
            ship[0] = 380
        if ship[0] <  -380:
            ship[0] = -380

        if ship[1] < 50:
            ship[1] = 50
        if ship[1] > 300:
            ship[1] = 300

def special(k, x, y):
    if over:
        return

    mv = 15
    alt = 10

    if k == GLUT_KEY_LEFT:
        ship[0] -= mv
    if k == GLUT_KEY_RIGHT:
        ship[0] += mv
    if k == GLUT_KEY_UP:
        ship[1] += alt
    if k == GLUT_KEY_DOWN:
        ship[1] -= alt

    if ship[0] > 380:
        ship[0] = 380
    if ship[0] < -380:
        ship[0] = -380

    if ship[1] < 50:
        ship[1] = 50
    if ship[1] > 300:
        ship[1] = 300

def cam():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(90, 1.25, 1.0, 2000.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(0, 150, 0, 0, 100, -400, 0, 1, 0)

def loop():
    global cd_active
    if not cd_active:
        update()
    else:
        if time.time() - cd_start >= cd_total:
            cd_active = False
    glutPostRedisplay()

def draw():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    cam()

    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    draw_sky()

    glColor3f(0.8, 0.4, 0.2)
    glBegin(GL_QUADS)
    glVertex3f(-600, 0, -2000)
    glVertex3f(600, 0, -2000)
    glVertex3f(600, 0, 500)
    glVertex3f(-600, 0, 500)
    glEnd()

    glColor3f(0.7, 0.35, 0.15)
    for p in patches:
        glPushMatrix()
        glTranslatef(p['x'], 1, p['z'])
        glScalef(p['size'], 2, p['size'])
        glutSolidCube(1)
        glPopMatrix()

    draw_terrain()
    draw_ship()

    glDisable(GL_BLEND)

    if over:
        text(400, 770, "GAME OVER")
        text(400, 740, f"Final Score: {pts}")
        text(400, 710, f"High Score: {hi}")
        if hi == pts and pts > 0:
            text(400, 650, "New High Score! Congratulations!")
        text(400, 280, "Press R to Restart")
    else:
        text(10, 770, f"Score: {pts}")
        text(10, 740, f"Speed: {spd:.1f}")
        text(10, 710, f"High Score: {hi}")
        if boost:
            text(850, 770, "BOOST!")
            rem = boost_end - time.time()
            if rem > 0:
                ms = rem * 1000
                text(850, 740, f"{ms:.0f} ms left")

        elapsed = time.time() - start_t
        if elapsed > 5 and not cd_active:
            idx = int((elapsed - 5) // 5) % len(msgs)
            text(300, 740, msgs[idx])

        if cd_active:
            text(300, 740, "You can adjust the ship's position using arrow/WASD keys.")
            cd_elapsed = time.time() - cd_start
            step = int((cd_elapsed / cd_total) * 4)
            if step < 0:
                step = 0
            if step > 3:
                step = 3

            labels = ["3...", "2...", "1...", "GO!"]
            text(480, 400, labels[step], GLUT_BITMAP_HELVETICA_18)

    glutSwapBuffers()

def main():
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(1000, 800)
    glutInitWindowPosition(0, 0)
    glutCreateWindow(b"Space Runner 3D")

    init()

    glutDisplayFunc(draw)
    glutKeyboardFunc(key)
    glutSpecialFunc(special)
    glutIdleFunc(loop)

    glEnable(GL_DEPTH_TEST)
    glClearColor(0.9, 0.5, 0.3, 1.0)

    glutMainLoop()

if __name__ == "__main__":
    main()