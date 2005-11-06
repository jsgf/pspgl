#define GL_GLEXT_PROTOTYPES

#include <sys/time.h>
#include <GL/gl.h>

#include "glchk.h"
#include "perfmeter.h"

static unsigned long long start, end, prev;

static long long now(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);

	return t.tv_sec * 1000000ll + t.tv_usec;
}

static void showstats(float drawtime, float frametime, float queuewait)
{
	GLCHK(glMatrixMode(GL_MODELVIEW));
	GLCHK(glPushMatrix());
	GLCHK(glLoadIdentity());

	GLCHK(glMatrixMode(GL_PROJECTION));
	GLCHK(glPushMatrix());
	GLCHK(glLoadIdentity());
	GLCHK(glOrtho(0, 1, 0, 10, -1, 1));

#if GL_PSP_view_matrix
	GLCHK(glMatrixMode(GL_VIEW_PSP));
	GLCHK(glPushMatrix());
	GLCHK(glLoadIdentity());
#endif

	GLCHK(glDisable(GL_DEPTH_TEST));
	GLCHK(glDisable(GL_LIGHTING));
	GLCHK(glDisable(GL_CULL_FACE));
	GLCHK(glDisable(GL_TEXTURE_2D));
	GLCHK(glEnable(GL_BLEND));
	GLCHK(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

	/* draw graph background */
	glColor4f(.25,.25,.25,.5);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,0);
	glVertex2f(1,0);
	glVertex2f(1,1);
	glVertex2f(0,1);
	glEnd();

	/* scale so that full scale = 1/10th sec */
	GLCHK(glMatrixMode(GL_MODELVIEW));
	GLCHK(glScalef(10, 1, 1));

	GLCHK(glShadeModel(GL_FLAT));

	GLCHK(glColor4f(.5,.5,.5,.5));

	glBegin(GL_TRIANGLE_STRIP);
	  glVertex2f(0,0);
	  glVertex2f(0,.75);
	  glVertex2f(drawtime,0);
	  glVertex2f(drawtime,.75);

	  glColor4f(.8,.2,0,.5);
	  glVertex2f(frametime,0);
	  glVertex2f(frametime,.75);
	GLCHK(glEnd());

	if (queuewait) {
		glColor4f(0,1,1,.5);
		glBegin(GL_TRIANGLES);
		  glVertex2f(queuewait-.0005, 1);
		  glVertex2f(queuewait, .75);
		  glVertex2f(queuewait+.0005, 1);
		glEnd();
	}

	glBegin(GL_LINES);
	{
		int i;

		glColor4f(.5,.5,0,.5);
		for(i = 0; i < 100; i++) {
			glVertex2f(i / 1000., 0);
			glVertex2f(i / 1000., i % 10 ? ((i % 5) ? .25 : .5) : .75);
		}

		glColor3f(0,1,0);
		glVertex2f(1./60, 0);
		glVertex2f(1./60, 1);

		glColor3f(1,1,0);
		glVertex2f(1./30, 0);
		glVertex2f(1./30, 1);

		glColor3f(1,0,0);
		glVertex2f(1./15, 0);
		glVertex2f(1./15, 1);
	}
	GLCHK(glEnd());
	
#if GL_PSP_view_matrix
	GLCHK(glMatrixMode(GL_VIEW_PSP));
	GLCHK(glPopMatrix());
#endif

	GLCHK(glMatrixMode(GL_PROJECTION));
	GLCHK(glPopMatrix());

	GLCHK(glMatrixMode(GL_MODELVIEW));
	GLCHK(glPopMatrix());
}


void pm_framestart()
{
#if GL_PSP_statistics
	GLCHK(glEnableStatsPSP(GL_STATS_TIMING_PSP));
#endif

	start = now();
}

void pm_frameend()
{
	float drawtime;
	float frametime;
	float queuewait;

	glFinish();
	end = now();

#if GL_PSP_statistics
	{
		GLuint t;

		glGetStatisticsuivPSP(GL_STATS_APPTIME_PSP, &t);
		drawtime = t;
		glGetStatisticsuivPSP(GL_STATS_FRAMETIME_PSP, &t);
		frametime = t;
		glGetStatisticsuivPSP(GL_STATS_QUEUEWAITTIME_PSP, &t);
		queuewait = t;

		glResetStatsPSP(GL_STATS_QUEUEWAITTIME_PSP);
	}
#else
	drawtime = (end - start);
	frametime = (end - prev);
	queuewait = 0;
#endif

	drawtime /= 1000000.;
	frametime /= 1000000.;
	queuewait /= 1000000.;

	showstats(drawtime, frametime, queuewait);

	prev = now();

}
