#include <GL/glut.h>
#include <cmath>
#include <iostream>

// 光源位置
GLfloat lightPos[4] = { 2.0f, 3.0f, 5.0f, 1.0f }; // 点光源 (w=1)

// 材质模式：0=塑料，1=金属
int materialMode = 0;

// 材质定义
void setMaterialPlastic() {
    GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat shininess = 10.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void setMaterialMetal() {
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess = 100.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// 初始化 OpenGL 设置
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // 深蓝色背景
}

// 绘制场景
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 设置相机
    gluLookAt(0.0, 2.0, 8.0,   // eye
        0.0, 0.0, 0.0,   // center
        0.0, 1.0, 0.0);  // up

    // 更新光源位置（固定在世界坐标）
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 地面
    glColor3f(0.3f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-10, -1, -10);
    glVertex3f(10, -1, -10);
    glVertex3f(10, -1, 10);
    glVertex3f(-10, -1, 10);
    glEnd();

    // 球体（在原点）
    if (materialMode == 0) setMaterialPlastic();
    else setMaterialMetal();
    glutSolidSphere(1.0, 32, 32);

    // 立方体（右侧）
    glPushMatrix();
    glTranslatef(3.0f, 0.0f, 0.0f);
    if (materialMode == 0) setMaterialPlastic();
    else setMaterialMetal();
    glutSolidCube(1.5);
    glPopMatrix();

    glutSwapBuffers();
}

// 键盘回调（移动光源）
void specialKeys(int key, int x, int y) {
    float step = 0.5f;
    switch (key) {
    case GLUT_KEY_LEFT:  lightPos[0] -= step; break;
    case GLUT_KEY_RIGHT: lightPos[0] += step; break;
    case GLUT_KEY_UP:    lightPos[1] += step; break;
    case GLUT_KEY_DOWN:  lightPos[1] -= step; break;
    }
    glutPostRedisplay();
}

// 普通按键（切换材质）
void keyboard(unsigned char key, int x, int y) {
    if (key == ' ') {
        materialMode = 1 - materialMode; // 切换 0/1
        std::cout << "Material: " << (materialMode ? "Metal" : "Plastic") << std::endl;
        glutPostRedisplay();
    }
}

// 窗口大小改变回调
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL 光照演示 - 大一课堂汇报");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    std::cout << "操作说明：\n";
    std::cout << "方向键：移动光源\n";
    std::cout << "空格键：切换材质（塑料/金属）\n";

    glutMainLoop();
    return 0;
}