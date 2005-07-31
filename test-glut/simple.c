#include <stdlib.h>
#include <GL/glut.h>


extern void __psp_log (const char *fmt, ...);

/* disable verbose logging to "ms0:/pspgl.ge" */
#if 1
	#define psp_log(x...) __psp_log(x)
#else
	#define psp_log(x...) do {} while (0)
#endif

/* enable GLerror logging to "ms0:/log.txt" */
#if 1
	#define GLCHK(x)							\
	do {									\
		GLint errcode;							\
		x;								\
		errcode = glGetError();						\
		if (errcode != GL_NO_ERROR) {					\
			__psp_log("%s (%d): GL error 0x%04x\n",			\
				__FUNCTION__, __LINE__, (unsigned int) errcode);\
		}								\
	} while (0)
#else
	#define GLCHK(x) x
#endif


static
void reshape (int w, int h)
{
	GLCHK(glViewport(0, 0, w, h));
	GLCHK(glMatrixMode(GL_PROJECTION));
	GLCHK(glLoadIdentity());
	GLCHK(glOrtho(-2, 2, -2, 2, -2, 2));
	GLCHK(glMatrixMode(GL_MODELVIEW));
	GLCHK(glLoadIdentity());
}


static float delta = 1.0;

static
void display (void)
{
	static GLfloat angle;
	angle += delta;

	GLCHK(glMatrixMode(GL_MODELVIEW));
	GLCHK(glLoadIdentity());
	GLCHK(glTranslatef(0.0, 0.0, -2.5));
//	GLCHK(glRotatef(angle * 0.79, 1.0, 0.0, 0.0));
//	GLCHK(glRotatef(angle * 0.98, 0.0, 1.0, 0.0));
	GLCHK(glRotatef(angle * 1.32, 0.0, 0.0, 1.0));

	GLCHK(glShadeModel(GL_SMOOTH));

	GLCHK(glClear(GL_COLOR_BUFFER_BIT));
	GLCHK(glBegin(GL_TRIANGLES));
		GLCHK(glColor3f(0.0, 0.0, 1.0)); GLCHK(glVertex3f(1.0, 0.0, 0.0));
		GLCHK(glColor3f(0.0, 1.0, 0.0)); GLCHK(glVertex3f(0.0, 1.0, 0.0));
		GLCHK(glColor3f(1.0, 0.0, 0.0)); GLCHK(glVertex3f(0.0, 0.0, 1.0));
	GLCHK(glEnd());
	glutSwapBuffers();
	glutPostRedisplay();
}


static
void keydown (unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':			/* delta, triangle */
		break;
	case 'o':			/* round */
		delta = 0.0;
		break;
	case 'q':			/* square*/
		break;
	case 'x':			/* cross button */
		exit(0);
		break;
	default:
		;
	}
}


static
void keyup (unsigned char key, int x, int y)
{
	switch (key) {
	case 'o':
		delta = 1.0;
		break;
	default:
		;
	}
}


static
void joystick (unsigned int buttonMask, int x, int y, int z)
{
	GLCHK(glClearColor(x * 1.0/2000.0 + 0.5, y * 1.0/2000.0 + 0.5, 1.0, 1.0));
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutCreateWindow( __FILE__ );
	glutKeyboardFunc(keydown);
	glutKeyboardUpFunc(keyup);
	glutJoystickFunc(joystick, 0);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

