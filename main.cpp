#include <GL/glut.h>
#include <cmath>
#include <iostream>

// 光源位置
GLfloat lightPos[4] = { 2.0f, 3.0f, 5.0f, 1.0f }; // 点光源 (w=1)

// 材质模式：0=塑料，1=金属
int materialMode = 0;

// 材质定义
// 设置塑料材质
void setMaterialPlastic() {
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; //环境光反射：低强度，表示物体在无直射光下的基础亮度。
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };//漫反射：中等强度，表示物体在直射光下的亮度，体现了物体的颜色。
	GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };//镜面反射（Specular）：较弱，高光不明显，表示物体表面的光泽度较低。
    GLfloat shininess = 10.0f;//光泽度（Shininess）：10，较低 → 高光区域较大、柔和（符合塑料特性）。
    //将上述参数应用到正面（front-facing）多边形。
    //glMaterialfv 设置向量型材质属性，glMaterialf 设置标量（如 shininess）。
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// 设置金属材质
void setMaterialMetal() {
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f }; 
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// 镜面反射为纯白（1.0），非常强。
    GLfloat shininess = 100.0f;// 光泽度高达 100 → 高光区域小而锐利（典型金属反光效果）
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// 初始化 OpenGL 设置
void init() {
	glEnable(GL_DEPTH_TEST); // 启用深度测试
	// 启用光照 和光源0
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	// 设置光源属性
	GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };// 环境光：低强度，提供基础照明，防止阴影过暗。
	GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };// 漫反射：高强度，提供主要照明，体现物体的颜色和形状。
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// 镜面反射：最高强度，产生亮点，增强物体的光泽感。

	// 将上述光源属性应用到光源0
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	// 设置光源位置
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// 启用局部观察者模型，增强高光效果，计算镜面反射时，使用相机实际位置而非无穷远方向。
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // 深蓝色背景
}

// 绘制场景
void display() {
	// 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 重置模型视图矩阵
    glLoadIdentity();

    // 设置相机
    gluLookAt(0.0, 2.0, 8.0,   // eye
        0.0, 0.0, 0.0,   // center
        0.0, 1.0, 0.0);  // up

    // 更新光源位置（固定在世界坐标）
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 地面
	glColor3f(0.3f, 0.5f, 0.3f); // 绿色地面
	glBegin(GL_QUADS);// 绘制四边形
    glNormal3f(0, 1, 0);
    glVertex3f(-10, -1, -10);
    glVertex3f(10, -1, -10);
    glVertex3f(10, -1, 10);
    glVertex3f(-10, -1, 10);
    glEnd();

    // 球体（在原点）
	if (materialMode == 0) setMaterialPlastic();// 设置材质
    else setMaterialMetal();
	glutSolidSphere(1.0, 32, 32); // 半径1，32段经纬线

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
    glutCreateWindow("OpenGL 光照演示 ");

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