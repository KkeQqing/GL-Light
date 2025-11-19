#include <GL/glut.h>
#include <cmath>
#include <iostream>
#define M_PI 3.14159265358979323846

// 光源位置
GLfloat lightPos[4] = { 0.0f, 5.0f, 10.0f, 1.0f }; // 主光源（可移动）

// 太阳光（聚光灯）
GLfloat sunLightPos[] = { -10.0f, 6.0f, 8.0f, 1.0f };
GLfloat sunDirection[] = { 1.0f, -0.5f, -0.5f }; // 指向教堂中心
GLfloat sunCutoff = 30.0f;

// 材质模式
int materialMode = 0; // 0=塑料, 1=金属
bool fogEnabled = true;

// 设置塑料材质（用于柱子/地面）
void setMaterialPlastic() {
    GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat shininess = 10.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// 设置金属材质（用于装饰）
void setMaterialMetal() {
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess = 100.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// 设置彩色玻璃材质（半透明）
void setStainedGlass() {
    GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 0.5f };
    GLfloat diffuse[] = { 0.6f, 0.3f, 0.9f, 0.8f }; // 紫色玻璃
    GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 1.0f);
}

// 绘制一根石柱
void drawColumn(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // 柱身（白色石柱）
    // 柱身（白色石柱）
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.4f, 0.4f, 6.0f, 16, 1);
    gluDeleteQuadric(quad);

    // 柱顶（方形基座）
    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    glScalef(0.8f, 0.2f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // 柱底（方形底座）
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glScalef(0.8f, 0.2f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

// 绘制彩色玻璃窗（竖直矩形）
void drawStainedWindow(float x, float y, float z, float width, float height, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    glColor4f(r, g, b, 0.7f); // 半透明彩色
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-width / 2, 0, 0);
    glVertex3f(width / 2, 0, 0);
    glVertex3f(width / 2, height, 0);
    glVertex3f(-width / 2, height, 0);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

// 绘制体积光束（太阳光）
void drawVolumetricLightShaft() {
    glPushMatrix();
    glTranslatef(sunLightPos[0], sunLightPos[1], sunLightPos[2]);

    // 计算方向角度
    GLfloat dx = -sunDirection[0];
    GLfloat dy = -sunDirection[1];
    GLfloat dz = -sunDirection[2];
    GLfloat len = sqrt(dx * dx + dy * dy + dz * dz);
    if (len > 0) { dx /= len; dy /= len; dz /= len; }

    GLfloat angleY = atan2(dx, dz) * 180.0f / M_PI;
    glRotatef(angleY, 0, 1, 0);
    GLfloat angleX = atan2(dy, sqrt(dx * dx + dz * dz)) * 180.0f / M_PI;
    glRotatef(-angleX, 1, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    glColor4f(1.0f, 0.9f, 0.7f, 0.2f); // 暖黄色光束
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.0f, 1.2f, 15.0f, 20, 1);
    gluDeleteQuadric(quad);

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

// 初始化
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // 全局环境光（偏暖）
    GLfloat global_ambient[] = { 0.1f, 0.08f, 0.12f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // 主光源（顶部补光）
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat light_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 太阳光（聚光灯）
    glLightfv(GL_LIGHT1, GL_POSITION, sunLightPos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, sunDirection);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, sunCutoff);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0f);
    GLfloat sunDiffuse[] = { 1.0f, 0.9f, 0.6f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, sunDiffuse);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // 雾效（增强体积感）
    GLfloat fogColor[4] = { 0.08f, 0.06f, 0.1f, 1.0f };
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.02f);
    glHint(GL_FOG_HINT, GL_NICEST);
    glEnable(GL_FOG);

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
}

// 显示函数
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 相机视角（低角度，看地面反射）
    gluLookAt(0.0, 1.0, 5.0,   // eye
        0.0, 2.0, 0.0,   // center
        0.0, 1.0, 0.0);  // up

    // 更新光源
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_POSITION, sunLightPos);

    // 地面（高反光）
// 定义材质属性数组
    GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 应用到当前物体的正面和背面
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-15, 0, -15);
    glVertex3f(15, 0, -15);
    glVertex3f(15, 0, 15);
    glVertex3f(-15, 0, 15);
    glEnd();

    // 绘制石柱（左右对称）
    for (int i = -3; i <= 3; ++i) {
        float x = i * 3.0f;
        drawColumn(x, 0.0f);
    }

    // 彩色玻璃窗（左右墙）
    for (int i = -3; i <= 3; ++i) {
        float x = i * 4.0f;
        drawStainedWindow(x, 3.0f, -12.0f, 1.0f, 4.0f, 0.5f, 0.3f, 0.9f); // 紫色
        drawStainedWindow(x, 3.0f, 12.0f, 1.0f, 4.0f, 0.3f, 0.6f, 0.9f);  // 蓝色
    }

    // 中央吊灯（简化）
    glPushMatrix();
    glTranslatef(0.0f, 5.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidSphere(0.3f, 12, 12);
    glPopMatrix();

    // 体积光束
    drawVolumetricLightShaft();

    glutSwapBuffers();
}

// 键盘控制
void specialKeys(int key, int x, int y) {
    float step = 0.5f;
    switch (key) {
    case GLUT_KEY_LEFT:  sunLightPos[0] -= step; break;
    case GLUT_KEY_RIGHT: sunLightPos[0] += step; break;
    case GLUT_KEY_UP:    sunLightPos[1] += step; break;
    case GLUT_KEY_DOWN:  sunLightPos[1] -= step; break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == ' ') {
        materialMode = 1 - materialMode;
        std::cout << "Material: " << (materialMode ? "Metal" : "Plastic") << std::endl;
    }
    else if (key == 'f' || key == 'F') {
        fogEnabled = !fogEnabled;
        if (fogEnabled) glEnable(GL_FOG);
        else glDisable(GL_FOG);
        std::cout << "Fog: " << (fogEnabled ? "ON" : "OFF") << std::endl;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 700);
    glutCreateWindow("梦幻教堂 - 体积光与彩窗");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    std::cout << "操作说明：\n";
    std::cout << "方向键：移动阳光\n";
    std::cout << "空格键：切换材质\n";
    std::cout << "F 键：开关雾效\n";

    glutMainLoop();
    return 0;
}