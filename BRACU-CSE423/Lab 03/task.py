from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import random
import math
from OpenGL.GLUT import GLUT_BITMAP_HELVETICA_18

# Camera-related variables
camera_pos = (0,500,500)

fovY = 120  # Field of view
GRID_LENGTH = 600  # Length of grid lines
camera_target = [0, 0, 0]
first_person_mode = False
player_pos = [0, 0, 0]
player_gun_rot_y = 0
player_life = 5
game_score = 0
bullets_missed = 0
game_over = False
cheat_mode = False
auto_gun_follow = False
bullets = []
bullet_speed = 15
bullet_size = 10
num_enemies = 5
enemies = []
enemy_speed = 0.2
enemy_min_size = 40
enemy_max_size = 60
gun_height = 50
gun_length = 50
gun_radius = 5
quadric = None

def init_game():
    global player_pos, player_gun_rot_y, player_life, game_score, bullets_missed, game_over
    global bullets, enemies, cheat_mode, auto_gun_follow, camera_pos, first_person_mode

    player_pos = [0, 0, 0]
    player_gun_rot_y = 0
    player_life = 5
    game_score = 0
    bullets_missed = 0
    game_over = False
    bullets = []
    enemies = []
    cheat_mode = False
    auto_gun_follow = False
    camera_pos = [0, 500, 500]
    first_person_mode = False

    for _ in range(num_enemies):
        spawn_enemy()

def spawn_enemy():
    x = random.uniform(-GRID_LENGTH, GRID_LENGTH)
    y = random.uniform(-GRID_LENGTH, GRID_LENGTH)
    while -100 < x < 100 and -100 < y < 100:
        x = random.uniform(-GRID_LENGTH, GRID_LENGTH)
        y = random.uniform(-GRID_LENGTH, GRID_LENGTH)
    size_scale = random.uniform(0.8, 1.2)
    grow_dir = 1
    enemies.append([x, y, 0, size_scale, grow_dir])

def draw_text(x, y, text, font=GLUT_BITMAP_HELVETICA_18):
    glColor3f(1, 1, 1)
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()

    # Set up an orthographic projection that matches window coordinates
    gluOrtho2D(0, 1000, 0, 800)  # left, right, bottom, top

    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()

    # Draw text at (x, y) in screen coordinates
    glRasterPos2f(x, y)
    for ch in text:
        glutBitmapCharacter(font, ord(ch))
    
    # Restore original projection and modelview matrices
    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)

def draw_player():
    glPushMatrix()
    # Body
    glColor3f(0.33, 0.42, 0.18)
    glPushMatrix()
    glTranslatef(0, 0, 40)
    glScalef(25, 35, 45)
    glutSolidCube(1)
    glPopMatrix()
    # Head
    glColor3f(0, 0, 0)
    glPushMatrix()
    glTranslatef(0, 0, 85)
    glutSolidSphere(15, 20, 20)
    glPopMatrix()
    # Legs
    glColor3f(0.0, 0.0, 1.0)
    glPushMatrix()
    glTranslatef(18, 0, 20)
    glRotatef(180, 1, 0, 0)
    gluCylinder(quadric, 15, 0, 50, 15, 15)
    glPopMatrix()
    glPushMatrix()
    glTranslatef(-18, 0, 20)
    glRotatef(180, 1, 0, 0)
    gluCylinder(quadric, 15, 0, 50, 15, 15)
    glPopMatrix()

    glPopMatrix()

def draw_gun():
    global first_person_mode
    glPushMatrix()

    if first_person_mode:
        glTranslatef(0, -30, 35)
        # Gun
        glColor3f(0.5, 0.5, 0.5)
        glPushMatrix()
        glRotatef(-90, 1, 0, 0)
        gluCylinder(quadric, 3, 3, 70, 10, 10)
        glPopMatrix()
    
        glPushMatrix()
        glTranslatef(0, -15, -10)
        glScalef(8, 20, 15)
        glutSolidCube(1)
        glPopMatrix()
        # Hands
        glColor3f(0.8, 0.6, 0.4)
        glPushMatrix()
        glTranslatef(-8, -10, -5)
        glRotatef(10, 0, 1, 0)
        glScalef(2.5, 1.5, 1.5)
        glutSolidSphere(10, 50, 10)
        glPopMatrix()
        
        glPushMatrix()
        glTranslatef(8, -10, -5)
        glRotatef(-10, 0, 1, 0)
        glScalef(2.5, 1.5, 1.5)
        glutSolidSphere(10, 50, 10)
        glPopMatrix()

    else:
        glTranslatef(0, 15, 55)
        glColor3f(0.5, 0.5, 0.5)
        glPushMatrix()
        glTranslatef(0, 0, -50)
        glScalef(10, 10, 10)
        glutSolidCube(1)
        glPopMatrix()
        glPushMatrix()
        glRotatef(-90, 1, 0, 0)
        gluCylinder(quadric, gun_radius, gun_radius, gun_length, 10, 10)
        glPopMatrix()

        glColor3f(0.96, 0.96, 0.86)
        glPushMatrix()
        glTranslatef(18, -10, 0)
        glRotatef(-90, 1, 0, 0)
        gluCylinder(quadric, 6, 2, 30, 10, 10)
        glPopMatrix()

        glPushMatrix()
        glTranslatef(-18, -10, 0)
        glRotatef(-90, 1, 0, 0)
        gluCylinder(quadric, 6, 2, 30, 10, 10)
        glPopMatrix()

    glPopMatrix()

def draw_enemies():
    for enemy in enemies:
        x, y, _, size_scale, _ = enemy
        glPushMatrix()
        glTranslatef(x, y, 30 * size_scale)
        glScalef(size_scale, size_scale, size_scale)

        glColor3f(1.0, 0.0, 0.0)
        glutSolidSphere(30, 20, 20)

        glPushMatrix()
        glTranslatef(0, 0, 20)
        glColor3f(0.0, 0.0, 0.0)
        glutSolidSphere(15, 20, 20)
        glPopMatrix()

        glPopMatrix()

def draw_bullets():
    for bullet in bullets:
        x, y, z, _ = bullet
        glPushMatrix()
        glTranslatef(x, y, z)
        glColor3f(1, 0, 0)
        glutSolidCube(bullet_size)
        glPopMatrix()

def draw_floor():
    glBegin(GL_QUADS)
    grid_size = 50
    for i in range(-GRID_LENGTH, GRID_LENGTH, grid_size):
        for j in range(-GRID_LENGTH, GRID_LENGTH, grid_size):
            if (i // grid_size + j // grid_size) % 2 == 0:
                glColor3f(0.7, 0.5, 0.95)
            else:
                glColor3f(1, 1, 1)
            glVertex3f(i, j, 0)
            glVertex3f(i + grid_size, j, 0)
            glVertex3f(i + grid_size, j + grid_size, 0)
            glVertex3f(i, j + grid_size, 0)
    glEnd()
    wall_height = 100
    glColor3f(0, 1, 1)
    glBegin(GL_QUADS)
    glVertex3f(-GRID_LENGTH, GRID_LENGTH, 0)
    glVertex3f(GRID_LENGTH, GRID_LENGTH, 0)
    glVertex3f(GRID_LENGTH, GRID_LENGTH, wall_height)
    glVertex3f(-GRID_LENGTH, GRID_LENGTH, wall_height)
    glEnd()

    glColor3f(0.1, 0.1, 0.1)
    glBegin(GL_QUADS)
    glVertex3f(-GRID_LENGTH, -GRID_LENGTH, 0)
    glVertex3f(GRID_LENGTH, -GRID_LENGTH, 0)
    glVertex3f(GRID_LENGTH, -GRID_LENGTH, wall_height)
    glVertex3f(-GRID_LENGTH, -GRID_LENGTH, wall_height)
    glEnd()

    glColor3f(0, 0, 1)
    glBegin(GL_QUADS)
    glVertex3f(-GRID_LENGTH, -GRID_LENGTH, 0)
    glVertex3f(-GRID_LENGTH, GRID_LENGTH, 0)
    glVertex3f(-GRID_LENGTH, GRID_LENGTH, wall_height)
    glVertex3f(-GRID_LENGTH, -GRID_LENGTH, wall_height)
    glEnd()

    glColor3f(0, 1, 0)
    glBegin(GL_QUADS)
    glVertex3f(GRID_LENGTH, -GRID_LENGTH, 0)
    glVertex3f(GRID_LENGTH, GRID_LENGTH, 0)
    glVertex3f(GRID_LENGTH, GRID_LENGTH, wall_height)
    glVertex3f(GRID_LENGTH, -GRID_LENGTH, wall_height)
    glEnd()

def draw_game_over():
    glPushMatrix()
    glTranslatef(player_pos[0], player_pos[1], 15)
    glRotatef(90, 1, 0, 0)
    draw_player()
    glPopMatrix()
    draw_text(10, 770, f"Game is Over. Your Score is {game_score}")
    draw_text(10, 740, "Press 'R' to RESTART the Game")

def nearest_enemy():
    if not enemies:
        return None
    px, py = player_pos[0], player_pos[1]
    best, best_d = None, 1e9
    for e in enemies:
        ex, ey = e[0], e[1]
        d = math.hypot(ex - px, ey - py)
        if d < best_d:
            best_d, best = d, (ex, ey)
    return best

def enemy_in_los(max_angle_deg=8, max_dist=600):
    if not enemies:
        return False
    angle = math.radians(player_gun_rot_y)
    fx, fy = math.sin(angle), math.cos(angle)
    px, py = player_pos[0], player_pos[1]
    best_d = 1e9
    for ex, ey, _, _, _ in enemies:
        vx, vy = ex - px, ey - py
        d = math.hypot(vx, vy)
        if d == 0 or d > max_dist:
            continue

        dot = (fx * vx + fy * vy) / d
        dot = max(-1.0, min(1.0, dot))
        ang = math.degrees(math.acos(dot))
        if ang <= max_angle_deg and d < best_d:
            best_d = d
    return best_d < 1e9

def check_collisions():
    global bullets, enemies, game_score, player_life, game_over

    bullets_to_keep = []
    new_enemies = []
    for bullet in bullets:
        bx, by, bz, _ = bullet
        hit = False
        for enemy in enemies:
            ex, ey, _, size_scale, _ = enemy
            dist = math.hypot(bx - ex, by - ey)
            if dist < (bullet_size / 2) + (30 * size_scale):
                hit = True
                game_score += 1
                spawn_enemy()
            else:
                new_enemies.append(enemy)
        enemies[:] = new_enemies
        new_enemies = []
        if not hit:
            bullets_to_keep.append(bullet)
    bullets = bullets_to_keep

    keep_enemies = []
    for enemy in enemies:
        ex, ey, _, size_scale, _ = enemy
        dist = math.hypot(player_pos[0] - ex, player_pos[1] - ey)
        if dist < 20 + (30 * size_scale):
            player_life -= 1
            if player_life <= 0:
                game_over = True
            spawn_enemy()
        else:
            keep_enemies.append(enemy)
    enemies = keep_enemies

def keyboardListener(key, x, y):
    global player_pos, player_gun_rot_y, cheat_mode, auto_gun_follow, game_over

    if not game_over:
        move_speed = 10.0
        rot_speed = 5.0
        angle_rad = math.radians(player_gun_rot_y)
        fwd_x, fwd_y = math.sin(angle_rad), math.cos(angle_rad)

        if key == b'w':
            player_pos[0] += fwd_x * move_speed
            player_pos[1] += fwd_y * move_speed
        elif key == b's':
            player_pos[0] -= fwd_x * move_speed
            player_pos[1] -= fwd_y * move_speed
        elif key == b'd':
            player_gun_rot_y += rot_speed
        elif key == b'a':
            player_gun_rot_y -= rot_speed
        elif key == b'c':
            cheat_mode = not cheat_mode
            if not cheat_mode:
                auto_gun_follow = False
        elif key == b'v':
            if cheat_mode and first_person_mode:
                auto_gun_follow = not auto_gun_follow

        player_gun_rot_y %= 360
        player_pos[0] = max(-GRID_LENGTH, min(GRID_LENGTH, player_pos[0]))
        player_pos[1] = max(-GRID_LENGTH, min(GRID_LENGTH, player_pos[1]))

    if key == b'r':
        init_game()

def specialKeyListener(key, y, x):
    global camera_pos

    if not game_over and not first_person_mode:
        if key == GLUT_KEY_UP:
            camera_pos[2] += 10
        elif key == GLUT_KEY_DOWN:
            camera_pos[2] = max(10, camera_pos[2] - 10)
        elif key == GLUT_KEY_LEFT:
            angle_rad = math.atan2(camera_pos[0], camera_pos[1]) + 0.05
            dist = math.hypot(camera_pos[0], camera_pos[1])
            camera_pos[0] = dist * math.sin(angle_rad)
            camera_pos[1] = dist * math.cos(angle_rad)
        elif key == GLUT_KEY_RIGHT:
            angle_rad = math.atan2(camera_pos[0], camera_pos[1]) - 0.05
            dist = math.hypot(camera_pos[0], camera_pos[1])
            camera_pos[0] = dist * math.sin(angle_rad)
            camera_pos[1] = dist * math.cos(angle_rad)

def mouseListener(button, state, x, y):
    """Handles mouse inputs for firing and camera toggling."""
    global bullets, first_person_mode, camera_pos

    if not game_over:
        if button == GLUT_LEFT_BUTTON and state == GLUT_DOWN:
            angle_rad = math.radians(player_gun_rot_y)
            gun_offset_forward = 15
            gun_offset_z = 55
            spawn_dist = gun_offset_forward + gun_length
            
            bullet_x = player_pos[0] + math.sin(angle_rad) * spawn_dist
            bullet_y = player_pos[1] + math.cos(angle_rad) * spawn_dist
            bullet_z = player_pos[2] + gun_offset_z
            
            bullets.append([bullet_x, bullet_y, bullet_z, player_gun_rot_y])
        if button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN:
            first_person_mode = not first_person_mode
            if not first_person_mode:
                camera_pos = [0, 500, 500]

def setupCamera():
    """
    Configures the camera's projection and view settings.
    Uses a perspective projection and positions the camera to look at the target.
    """
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(fovY, 1.25, 0.1, 2000)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    if first_person_mode:
        cam_x = player_pos[0]
        cam_y = player_pos[1]
        cam_z = player_pos[2] + 55
        ang = math.radians(player_gun_rot_y)
        look_at_x = cam_x + math.sin(ang) * 100
        look_at_y = cam_y + math.cos(ang) * 100
        look_at_z = cam_z
        gluLookAt(cam_x, cam_y, cam_z,
                  look_at_x, look_at_y, look_at_z,
                  0, 0, 1)
    else:
        gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2],
                  camera_target[0], camera_target[1], camera_target[2],
                  0, 0, 1)

def idle():
    """
    Idle function that runs continuously:
    - Triggers screen redraw for real-time updates.
    """
    global enemies, bullets, bullets_missed, game_over, player_gun_rot_y
    if not game_over:
        for enemy in enemies:
            dx = player_pos[0] - enemy[0]
            dy = player_pos[1] - enemy[1]
            dist = math.hypot(dx, dy)
            if dist > 1:
                enemy[0] += (dx / dist) * enemy_speed
                enemy[1] += (dy / dist) * enemy_speed
            enemy[3] += enemy[4] * 0.005
            if enemy[3] > 1.2 or enemy[3] < 0.8:
                enemy[4] *= -1
        bullets_to_keep = []
        for x, y, z, angle in bullets:
            angle_rad = math.radians(angle)
            x += math.sin(angle_rad) * bullet_speed
            y += math.cos(angle_rad) * bullet_speed
            if abs(x) > GRID_LENGTH or abs(y) > GRID_LENGTH:
                if not cheat_mode:
                    bullets_missed += 1
            else:
                bullets_to_keep.append([x, y, z, angle])
        bullets = bullets_to_keep
        if player_life <= 0 or bullets_missed >= 10:
            game_over = True
        if cheat_mode and auto_gun_follow and first_person_mode:
            tgt = nearest_enemy()
            if tgt:
                dx = tgt[0] - player_pos[0]
                dy = tgt[1] - player_pos[1]
                player_gun_rot_y = math.degrees(math.atan2(dx, dy))
        elif cheat_mode:
            player_gun_rot_y = (player_gun_rot_y + 1.2) % 360

        if cheat_mode and enemy_in_los() and len(bullets) < 5:  # cap spam
            angle_rad = math.radians(player_gun_rot_y)
            gun_offset_forward = 15
            gun_offset_z = 55
            spawn_dist = gun_offset_forward + gun_length
            bullet_x = player_pos[0] + math.sin(angle_rad) * spawn_dist
            bullet_y = player_pos[1] + math.cos(angle_rad) * spawn_dist
            bullet_z = player_pos[2] + gun_offset_z
            bullets.append([bullet_x, bullet_y, bullet_z, player_gun_rot_y])

        check_collisions()
    glutPostRedisplay()

def showScreen():
    """
    Display function to render the game scene:
    - Clears the screen and sets up the camera.
    - Draws everything of the screen
    """
    # Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    glViewport(0, 0, 1000, 800)
    setupCamera()
    draw_floor()

    if not game_over:
        if not first_person_mode:
            glPushMatrix()
            glTranslatef(player_pos[0], player_pos[1], player_pos[2])
            glRotatef(-player_gun_rot_y, 0, 0, 1)
            draw_player()
            draw_gun()
            glPopMatrix()
        else:
            glPushMatrix()
            glTranslatef(player_pos[0], player_pos[1], player_pos[2])
            glRotatef(-player_gun_rot_y, 0, 0, 1)
            draw_gun()
            glPopMatrix()
        draw_enemies()
        draw_bullets()
    else:
        draw_game_over()

    # HUD
    if not game_over:
        draw_text(10, 770, f"Player Life Remaining: {player_life}")
        draw_text(10, 740, f"Game Score: {game_score}")
        draw_text(10, 710, f"Player Bullets Missed: {bullets_missed}")
        if cheat_mode:
            draw_text(820, 770, "CHEAT MODE: ON", GLUT_BITMAP_HELVETICA_18)

    glutSwapBuffers()

# Main function to set up OpenGL window and loop
def main():
    global quadric

    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)  # Double buffering, RGB color, depth test
    glutInitWindowSize(1000, 800)  # Window size
    glutInitWindowPosition(0, 0)  # Window position
    wind = glutCreateWindow(b"3D OpenGL Intro")  # Create the window
    quadric = gluNewQuadric()
    init_game()
    glutDisplayFunc(showScreen)  # Register display function
    glutKeyboardFunc(keyboardListener)  # Register keyboard listener
    glutSpecialFunc(specialKeyListener)
    glutMouseFunc(mouseListener)
    glutIdleFunc(idle)  # Register the idle function to move the bullet automatically

    glutMainLoop()  # Enter the GLUT main loop

if __name__ == "__main__":
    main()