#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <fstream>


#define PI 3.14159265

static int X = 0, Y = 0, Z = 0;
static float zoom = 0.0;
static unsigned int base; // Display lists base index.
static float R = 0.3; // Radius of hemisphere.
static int p = 50; // Number of longitudinal slices.
static int q = 10; // Number of latitudinal slices.
static GLUquadricObj *qobj; // Pointer to GLU quadric object.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.
static float increment = 0.01;
static int animation = 1;
static float pos = 0.0;
static float show = 0.0;
static float star_dobra = 0.0;
static int direction[3] = {0, 0, 0};
static bool texture_on = true;

static unsigned int texture[2]; // Array of texture indices.

struct BitMapFile {
    int sizeX;
    int sizeY;
    unsigned char *data;
};

static float controlPointsPropellerBlade[4][4][3] = {
    {{-0.20, 0.20, -0.05}, {-0.1, 0.0, -0.02}, {0.10, 0.00, 0.03}, {0.20, 0.20, 0.05}},
    {{-0.20, 0.40, -0.10}, {-0.1, 0.4, -0.05}, {0.10, 0.40, 0.05}, {0.20, 0.40, 0.10}},
    {{-0.20, 0.60, -0.20}, {-0.1, 0.6, -0.10}, {0.10, 0.60, 0.10}, {0.20, 0.60, 0.20}},
    {{-0.20, 0.80, -0.20}, {-0.1, 1.0, -0.10}, {0.10, 1.00, 0.10}, {0.20, 0.80, 0.20}}
};

void efeito(int j) {
    int t, i;
    for(i = 0; i < 40; i++) {
        glPushMatrix();
        glTranslatef(0.5 * cos(t), 0.5 * sin(t), star_dobra + (j / 2));
        if((star_dobra + (j / 2)) >= 5.0) {
            star_dobra = 0.0;
        }
        t += 2 * PI / i;
        glScaled(0.1, 0.1, 30.0);
        glutSolidSphere(show, 50.0, 50.0);
        glPopMatrix();
    }
}

void dobra() {
    zoom -= increment;
    star_dobra += 0.6;
    if (increment > 0.2) {
        increment += increment;
    } else if (increment > 0.1 && Y >= 0 && pos >= 0.0) {
        show = 0.01;
    }
    increment += 0.01;
    if (zoom <= -35.0) {
        isAnimate = 0;
        show = 0.0;
    }
}

void animate(int value) {
    if (isAnimate) {
        if (animation == 2) {
            if(pos < 0.0) {
                pos += 0.4;
            }
            if(Y < 0) {
                Y += 7;
            }
        }
        dobra();
    }
    glutTimerFunc(animationPeriod, animate, 1);
    glutPostRedisplay();
}

static void torus(int numc, int numt, float size) {
    int i, j, k;
    double s, t, x, y, z, twopi;

    glNormal3f(1.0f, 1.0f, 1.0f);
    twopi = 2 * PI;
    for (i = 0; i < numc; i++) {
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= numt / 2; j++) {
            for (k = 1; k >= 0; k--) {
                s = (i + k) % numc + 0.5;
                t = j % numt;

                x = (size + 0.05 * cos(s * twopi / numc)) * cos(t * twopi / numt);
                y = (size + 0.05 * cos(s * twopi / numc)) * sin(t * twopi / numt);
                z = 0.05 * sin(s * twopi / numc);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}

void drawScene(void) {
    int i, j;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GLU_FILL);

    glColor3f(0.0, 0.0, 0.0);
    if(texture_on == true) {
        glColor3f(1.0, 1.0, 1.0);
    }
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-30.0, -15.0, -15.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(30.0, -15.0, -15.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(30.0, 15.0, -15.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-30.0, 15.0, -15.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glColor3f(0.3, 0.3, 0.3);

    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
        glRotatef(X, 1.0, 0.0, 0.0);
        glRotatef(Y, 0.0, 1.0, 0.0);
        glRotatef(Z, 0.0, 0.0, 1.0);
        for(i = 1; i < 10; i++) {
            efeito(i);
        }
    glPopMatrix();
    glColor3f(0.3, 0.3, 0.3);

    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glEnable(GL_TEXTURE_2D);
        gluQuadricTexture(qobj, GL_TRUE);
        glTranslatef(pos, 0.0, 0.0);
        glTranslatef(0.0, -0.8, 0.0);
        glTranslatef(0.0, 0.0, zoom);
        glRotatef(X, 1.0, 0.0, 0.0);
        glRotatef(Y, 0.0, 1.0, 0.0);
        glRotatef(Z, 0.0, 0.0, 1.0);

        glPushMatrix();
            glTranslatef(0.0, 0.0, -1.4);
            glRotatef(90, 1.0, 0.0, 0.0);
            glScaled(1.0, 0.4, 1.0);
            for(j = 0; j < q; j++) {
                glBegin(GL_TRIANGLE_STRIP);
                for(i = 0; i <= p; i++) {
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(R * cos((float)(j + 1) / q * PI / 2.0) * cos(2.0 * (float)i / p * PI),
                        R * sin((float)(j + 1) / q * PI / 2.0),
                        R * cos((float)(j + 1) / q * PI / 2.0) * sin(2.0 * (float)i / p * PI));
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(R * cos((float) j / q * PI / 2.0) * cos(2.0 * (float) i / p * PI),
                        R * sin((float) j / q * PI / 2.0),
                        R * cos((float) j / q * PI / 2.0) * sin(2.0 * (float) i / p * PI));
                }
                glEnd();
            }
            glRotatef(90, 1.0, 0.0, 0.0);
            gluCylinder(qobj, 0.001, 0.001, 0.5, 10, 10);
        glPopMatrix();

        // sphere right turbine
        glPushMatrix();
            glTranslatef(1.0, 1.6, 0.8);
            gluSphere(qobj, 0.15, 50.0, 50.0);
        glPopMatrix();


        glPushMatrix();
            glTranslatef(1.1, 1.6, 2.5);
            glRotatef(90, 0.0, 1.0, 0.0);
            glRotatef(40, 1.0, 0.0, 0.0);
            glScaled(1.0, 0.5, 1.0);
            torus(30, 30, 0.2);
        glPopMatrix();

        // right turbine
        glPushMatrix();
            glTranslatef(1.0, 1.6, 0.8);
            gluCylinder(qobj, 0.15, 0.15, 2.2, 50, 50);
        glPopMatrix();

        // tap right turbine
        glPushMatrix();
            glTranslatef(1.0, 1.6, 3.0);
            gluDisk(qobj, 0.0, 0.15, 50.0, 1.0);
        glPopMatrix();

        // sphere left turbine
        glPushMatrix();
            glTranslatef(-1.0, 1.6, 0.8);
            gluSphere(qobj, 0.15, 50.0, 50.0);
        glPopMatrix();


        glPushMatrix();
            glTranslatef(-1.1, 1.6, 2.5);
            glRotatef(90, 0.0, 1.0, 0.0);
            glRotatef(-40, 1.0, 0.0, 0.0);
            glScaled(1.0, 0.5, 1.0);
            torus(30, 30, 0.2);
        glPopMatrix();

        // left turbine
        glPushMatrix();
            glTranslatef(-1.0, 1.6, 0.8);
            gluCylinder(qobj, 0.15, 0.15, 2.2, 50, 50);
        glPopMatrix();

        // tap left turbine
        glPushMatrix();
            glTranslatef(-1.0, 1.6, 3.0);
            gluDisk(qobj, 0.0, 0.15, 50.0, 1.0);
        glPopMatrix();

        // tap engine
        glPushMatrix();
            glTranslatef(0.0, 0.14, 2.0);
            glRotatef(-2.5, 1.0, 0.0, 0.0);
            glScaled(1.0, 1.0, 0.0);
            gluSphere(qobj, 0.25, 50.0, 50.0);
        glPopMatrix();

        // center connection
        glPushMatrix();
            glTranslatef(0.0, 0.0, -0.7);
            glTranslatef(0.0, 1.0, 0.0);
            glRotatef(90, 1.0, 0.0, 0.0);
            glRotatef(-20, 1.0, 0.0, 0.0);
            glScaled(0.3, 1.0, 1.0);
            gluCylinder(qobj, 0.30, 0.30, 0.8, 50, 50);
        glPopMatrix();

        // engine
        glPushMatrix();
            glTranslatef(0.0, 0.0, -1.3);
            gluCylinder(qobj, 0.40, 0.43, 0.3, 30, 30);
            glTranslatef(0.0, 0.0, -0.05);
            gluCylinder(qobj, 0.30, 0.30, 0.3, 30, 30);
            glTranslatef(0.0, 0.0, 0.15);
            glRotatef(-2.5, 1.0, 0.0, 0.0);
            gluCylinder(qobj, 0.45, 0.25, 3.2, 50, 50);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, 1.0, -1.5);
            gluSphere(qobj, 0.4, 50.0, 50.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, 1.0, -1.5);
            glRotatef(90, 1.0, 0.0, 0.0);
            glScaled(1.0, 1.0, 0.2);
            gluSphere(qobj, 1.2, 50.0, 50.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, 0.9, -1.5);
            glRotatef(90, 1.0, 0.0, 0.0);
            glScaled(1.0, 1.0, 0.2);
            gluSphere(qobj, 1.2, 50.0, 50.0);
        glPopMatrix();

        // nave
        glPushMatrix();
            glRotatef(90, 1.0, 0.0, 0.0);
            glTranslatef(0.0, -1.5, -1.1);
            gluCylinder(qobj, 1.5, 1.4, 0.3, 40, 40);
            glScaled(1.0, 1.0, 0.0);
            gluSphere(qobj, 1.5, 50.0, 50.0);
            glTranslatef(0.0, 0.0, -0.8);
            gluSphere(qobj, 1.3, 50.0, 50.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.9, 1.6, 2.5);
            glRotatef(90, 0.0, 1.0, 0.0);
            glRotatef(40, 1.0, 0.0, 0.0);
            glScaled(1.0, 0.5, 1.0);
            torus(30, 30, 0.2);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.9, 1.6, 2.5);
            glRotatef(90, 0.0, 1.0, 0.0);
            glRotatef(-40, 1.0, 0.0, 0.0);
            glScaled(1.0, 0.5, 1.0);
            torus(30, 30, 0.2);
        glPopMatrix();

        // right connection
        glPushMatrix();
            glTranslatef(0.0, 0.0, 1.0);
            glTranslatef(0.0, 0.3, 0.0);
            glTranslatef(0.1, 0.0, 0.0);
            glRotatef(90, 1.0, 0.0, 0.0);
            glRotatef(145, 0.0, 1.0, 0.0);
            glRotatef(-20, 1.0, 0.0, 0.0);
            glScaled(0.3, 1.0, 1.0);
            gluCylinder(qobj, 0.15, 0.15, 1.7, 50, 50);
        glPopMatrix();

        // left connection
        glPushMatrix();
            glTranslatef(0.0, 0.0, 1.0);
            glTranslatef(0.0, 0.3, 0.0);
            glTranslatef(-0.1, 0.0, 0.0);
            glRotatef(90, 1.0, 0.0, 0.0);
            glRotatef(20, 1.0, 0.0, 0.0);
            glRotatef(-145, 0.0, 1.0, 0.0);
            glScaled(0.3, 1.0, 1.0);
            gluCylinder(qobj, 0.15, 0.15, 1.7, 50, 50);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

BitMapFile *getBMPData(char* namefile) {
    BitMapFile *bmp = new BitMapFile;
    unsigned int size, offset, headerSize;

    // Read input file name.
    std::ifstream infile (namefile, std::ios::binary);

    // Get the starting point of the image data.
    infile.seekg(10);
    infile.read((char *) &offset, 4);

    // Get the header size of the bitmap.
    infile.read((char *) &headerSize, 4);

    // Get width and height values in the bitmap header.
    infile.seekg(18);
    infile.read((char *) &bmp->sizeX, 4);
    infile.read((char *) &bmp->sizeY, 4);

    // Allocate buffer for the image.
    size = bmp->sizeX * bmp->sizeY * 24;
    bmp->data = new unsigned char[size];

    // // Read bitmap data.
    infile.seekg(offset);
    infile.read((char *) bmp->data , size);

    // // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < size; i += 3) {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i+2];
        bmp->data[i+2] = temp;
    }

    return bmp;
}

void LoadTexture() {
    // Local storage for bmp image data.
    BitMapFile *background;
    BitMapFile *nave_tex;

    // Load the texture.
    char back_name[] = "stars.bmp";
    char tex_name[] = "tex.bmp";
    background = getBMPData(back_name);
    nave_tex = getBMPData(tex_name);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Specify an image as the texture to be bound with the currently active texture index.
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, background->sizeX, background->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, background->data);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nave_tex->sizeX, nave_tex->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, nave_tex->data);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void setup(void) {
    GLfloat luzAmbiente[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat luzDifusa[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat luzEspecular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat posicaoLuz[] = {0.0, 1.0, 2.0, 0.0};
    GLfloat especularidade[] = {1.0, 1.0, 1.0, 1.0};
    GLint especMaterial = 50.0;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH);
    // Define a refletância do material
    glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
    // Define a concentração do brilho
    glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

    // Habilita a definição da cor do material a partir da cor corrente
    glEnable(GL_COLOR_MATERIAL);
    //Habilita o uso de iluminação
    glEnable(GL_LIGHTING);
    // Habilita a luz de número 0
    glEnable(GL_LIGHT0);
    // Habilita o depth-buffering
    glEnable(GL_DEPTH_TEST);

    // Ativa o uso da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    glShadeModel(GL_SMOOTH);

    if(texture_on == true) {
        glGenTextures(2, texture);
        LoadTexture();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    qobj = gluNewQuadric();
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

void cleanup() {
    gluDeleteQuadric(qobj);
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (0.0, 0.0, 4.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyInput(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
        case ' ':
            if (isAnimate) {
                isAnimate = 0;
            } else {
                if (zoom <= -35.0) {
                    zoom = 0.0;
                    increment = 0.01;
                }
                isAnimate = 1;
            }
            break;
        case 'x':
            X = (X + 5) % 360;
            break;
        case 'X':
            X = (X - 5) % 360;
            break;
        case 'y':
            Y = (Y + 5) % 360;
            break;
        case 'Y':
            Y = (Y - 5) % 360;
            break;
        case 'z':
            Z = (Z + 5) % 360;
            break;
        case 'Z':
            Z = (Z - 5) % 360;
            break;
        case 'w':
            zoom = (zoom - 0.5);
            break;
        case 'W':
            zoom = (zoom + 0.5);
            break;
        case '1':
            animation = 1;
            isAnimate = 0;
            pos = 0.0;
            Y += -Y;
            X += -X;
            Z += -Z;
            zoom = 0;
            increment = 0.01;
            break;
        case '2':
            animation = 2;
            isAnimate = 0;
            pos = -3.0;
            Y += -Y - 90;
            zoom = 0;
            increment = 0.01;
            break;
        default:
            break;
    }
    direction[0] = X;
    direction[1] = Y;
    direction[2] = Z;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Enterprise");
    setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyInput);
    glutTimerFunc(5, animate, 1);
    glutMainLoop();
    return 0;
}
