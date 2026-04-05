import OpenGL.GL as gl
import OpenGL.GLUT as glut
import random
import time
import sys

W, H = 600, 800
CAT_W = 140
CAT_H = 20
CAT_S = 200.0
DIA_S = 20
DIA_S_INIT = 100.0
DIA_A = 6.0

gs = {
    "score": 0,
    "paused": False,
    "over": False,
    "cx": (W - CAT_W) / 2,
    "dx": 0,
    "dy": 0,
    "ds": DIA_S_INIT,
    "dc": (1.0, 1.0, 1.0),
    "lt": 0,
}

R_BTN = {"x": 20, "y": H - 50, "w": 40, "h": 40}
P_BTN = {"x": W // 2 - 20, "y": H - 50, "w": 40, "h": 40}
E_BTN = {"x": W - 60, "y": H - 50, "w": 40, "h": 40}


def init_gl():
    gl.glClearColor(0.0, 0.0, 0.0, 1.0)
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glLoadIdentity()
    gl.glOrtho(0, W, 0, H, -1, 1)
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glPointSize(2.0)


def fz(dx, dy):
    if abs(dx) > abs(dy):
        if dx >= 0 and dy >= 0: return 0
        if dx < 0 and dy >= 0: return 3
        if dx < 0 and dy < 0: return 4
        if dx >= 0 and dy < 0: return 7
    else:
        if dx >= 0 and dy >= 0: return 1
        if dx < 0 and dy >= 0: return 2
        if dx < 0 and dy < 0: return 5
        if dx >= 0 and dy < 0: return 6
    return 0

def cz0(x, y, zone):
    if zone == 0: return x, y
    if zone == 1: return y, x
    if zone == 2: return y, -x
    if zone == 3: return -x, y
    if zone == 4: return -x, -y
    if zone == 5: return -y, -x
    if zone == 6: return -y, x
    if zone == 7: return x, -y
    return x, y

def cfz0(x, y, zone):
    if zone == 0: return x, y
    if zone == 1: return y, x
    if zone == 2: return -y, x
    if zone == 3: return -x, y
    if zone == 4: return -x, -y
    if zone == 5: return -y, -x
    if zone == 6: return y, -x
    if zone == 7: return x, -y
    return x, y

def dp(x, y):
    gl.glBegin(gl.GL_POINTS)
    gl.glVertex2i(int(x), int(y))
    gl.glEnd()
    
def dl(x1, y1, x2, y2):
    dx = x2 - x1
    dy = y2 - y1
    
    zone = fz(dx, dy)
    
    zx1, zy1 = cz0(x1, y1, zone)
    zx2, zy2 = cz0(x2, y2, zone)

    if zx1 > zx2:
        zx1, zx2 = zx2, zx1
        zy1, zy2 = zy2, zy1

    zdx = zx2 - zx1
    zdy = zy2 - zy1

    d = 2 * zdy - zdx
    inc_e = 2 * zdy
    inc_ne = 2 * (zdy - zdx)
    
    x, y = zx1, zy1
    
    orig_x, orig_y = cfz0(x, y, zone)
    dp(orig_x, orig_y)

    while x < zx2:
        if d < 0:
            d += inc_e
            x += 1
        else:
            d += inc_ne
            x += 1
            y += 1
        
        orig_x, orig_y = cfz0(x, y, zone)
        dp(orig_x, orig_y)

def dcat():
    x = gs["cx"]
    y = 10
    
    if gs["over"]:
        gl.glColor3f(1.0, 0.0, 0.0)
    else:
        gl.glColor3f(1.0, 1.0, 1.0)

    inset = 15
    by = y
    ty = y + CAT_H
    blx = x + inset
    brx = x + CAT_W - inset
    tlx = x
    trx = x + CAT_W

    dl(blx, by, brx, by)
    dl(blx, by, tlx, ty)
    dl(brx, by, trx, ty)
    dl(tlx, ty, trx, ty)

def ddia():
    if gs["over"]:
        return
        
    x = int(gs["dx"])
    y = int(gs["dy"])
    
    w = DIA_S // 2
    h = int(DIA_S * 0.8)
    
    gl.glColor3f(*gs["dc"])

    tx, ty = x, y + h
    bx, by = x, y - h
    lx, ly = x - w, y
    rx, ry = x + w, y

    dl(tx, ty, rx, ry)
    dl(rx, ry, bx, by)
    dl(bx, by, lx, ly)
    dl(lx, ly, tx, ty)

def dui():
    gl.glColor3f(0.0, 1.0, 1.0)
    b = R_BTN
    dl(b['x'] + b['w'], b['y'] + b['h']//2, b['x'], b['y'] + b['h']//2)
    dl(b['x'], b['y'] + b['h']//2, b['x'] + b['w']//2, b['y'] + b['h'])
    dl(b['x'], b['y'] + b['h']//2, b['x'] + b['w']//2, b['y'])

    gl.glColor3f(1.0, 0.75, 0.0)
    b = P_BTN
    if gs["paused"]:
        dl(b['x'], b['y'], b['x'] + b['w'], b['y'] + b['h']//2)
        dl(b['x'] + b['w'], b['y'] + b['h']//2, b['x'], b['y'] + b['h'])
        dl(b['x'], b['y'] + b['h'], b['x'], b['y'])
    else:
        dl(b['x'] + 5, b['y'], b['x'] + 5, b['y'] + b['h'])
        dl(b['x'] + b['w'] - 5, b['y'], b['x'] + b['w'] - 5, b['y'] + b['h'])

    gl.glColor3f(1.0, 0.0, 0.0)
    b = E_BTN
    dl(b['x'], b['y'], b['x'] + b['w'], b['y'] + b['h'])
    dl(b['x'] + b['w'], b['y'], b['x'], b['y'] + b['h'])

def sdia():
    gs["dy"] = H - DIA_S
    gs["dx"] = random.randint(DIA_S, W - DIA_S)
    gs["dc"] = (random.random(), random.random(), random.random())
    while sum(gs["dc"]) < 1.5:
        gs["dc"] = (random.random(), random.random(), random.random())

def res(is_restart=True):
    if is_restart:
        print("Starting Over!")
    gs["score"] = 0
    gs["over"] = False
    gs["paused"] = False
    gs["ds"] = DIA_S_INIT
    gs["cx"] = (W - CAT_W) / 2
    gs["lt"] = time.time()
    sdia()

def coll():
    dbox = {
        "x": gs["dx"] - DIA_S / 2,
        "y": gs["dy"] - DIA_S / 2,
        "w": DIA_S,
        "h": DIA_S
    }
    cbox = {
        "x": gs["cx"],
        "y": 10,
        "w": CAT_W,
        "h": CAT_H
    }
    
    return (cbox["x"] < dbox["x"] + dbox["w"] and
            cbox["x"] + cbox["w"] > dbox["x"] and
            cbox["y"] < dbox["y"] + dbox["h"] and
            cbox["y"] + cbox["h"] > dbox["y"])

def upd(value):
    global gs
    
    ct = time.time()
    dt = ct - gs["lt"]
    gs["lt"] = ct

    if not gs["paused"] and not gs["over"]:
        gs["dy"] -= gs["ds"] * dt
        gs["ds"] += DIA_A * dt
        
        if coll():
            gs["score"] += 1
            print(f"Score: {gs['score']}")
            sdia()
        elif gs["dy"] < 0:
            gs["over"] = True
            print(f"Game Over! Score: {gs['score']}")

    glut.glutPostRedisplay()
    glut.glutTimerFunc(16, upd, 0)

def disp():
    gl.glClear(gl.GL_COLOR_BUFFER_BIT)
    gl.glLoadIdentity()
    dcat()
    ddia()
    dui()
    gl.glFlush()

def kbs(key, x, y):
    if gs["paused"] or gs["over"]:
        return

    md = CAT_S * 0.1
    if key == glut.GLUT_KEY_LEFT:
        gs["cx"] -= md
        if gs["cx"] < 0:
            gs["cx"] = 0
    elif key == glut.GLUT_KEY_RIGHT:
        gs["cx"] += md
        if gs["cx"] > W - CAT_W:
            gs["cx"] = W - CAT_W

def mcl(button, state, x, y):
    if button == glut.GLUT_LEFT_BUTTON and state == glut.GLUT_DOWN:
        my = H - y
        
        b = R_BTN
        if b['x'] <= x <= b['x'] + b['w'] and b['y'] <= my <= b['y'] + b['h']:
            res(is_restart=True)

        b = P_BTN
        if b['x'] <= x <= b['x'] + b['w'] and b['y'] <= my <= b['y'] + b['h']:
            if not gs["over"]:
                gs["paused"] = not gs["paused"]
                if gs["paused"]:
                    print("Paused")
                else:
                    gs["lt"] = time.time()
                    print("Resumed")

        b = E_BTN
        if b['x'] <= x <= b['x'] + b['w'] and b['y'] <= my <= b['y'] + b['h']:
            print(f"Goodbye! Score: {gs['score']}")
            glut.glutLeaveMainLoop()

glut.glutInit(sys.argv)
glut.glutInitDisplayMode(glut.GLUT_SINGLE | glut.GLUT_RGB)
glut.glutInitWindowSize(W, H)
glut.glutCreateWindow(b"Catch the Diamonds!")
init_gl()
res(is_restart=False)
glut.glutDisplayFunc(disp)
glut.glutTimerFunc(0, upd, 0)
glut.glutSpecialFunc(kbs)
glut.glutMouseFunc(mcl)
glut.glutMainLoop()