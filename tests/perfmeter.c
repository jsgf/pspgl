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
	GLCHK(glPushAttrib(GL_ENABLE_BIT |
			   GL_COLOR_BUFFER_BIT |
			   GL_CURRENT_BIT |
			   GL_TRANSFORM_BIT));

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
#if GL_PSP_vertex_blend
	GLCHK(glDisable(GL_VERTEX_BLEND_PSP));
#endif

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

	GLCHK(glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT));

	GLCHK(glEnableClientState(GL_VERTEX_ARRAY));
	GLCHK(glDisableClientState(GL_NORMAL_ARRAY));
	GLCHK(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
	GLCHK(glDisableClientState(GL_COLOR_ARRAY));
#if GL_PSP_vertex_blend
	GLCHK(glDisableClientState(GL_WEIGHT_ARRAY_PSP));
#endif

	static GLuint bufferid = 0;
#define NTICKS	100
	if (bufferid == 0) {
		int i;
		struct vertex {
			float x,y,z;
		} ticks[NTICKS * 2];

		GLCHK(glGenBuffersARB(1, &bufferid));

		for(i = 0; i < NTICKS; i++) {
			ticks[i*2+0].x = i / 1000.f;
			ticks[i*2+0].y = 0.f;
			ticks[i*2+0].z = 0.f;
			ticks[i*2+1].x = i / 1000.f;
			ticks[i*2+1].y = i % 10 ? ((i % 5) ? .25 : .5) : .75;
			ticks[i*2+1].z = 0.f;
		}

		GLCHK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, bufferid));
		GLCHK(glBufferDataARB(GL_ARRAY_BUFFER_ARB, 
				      sizeof(ticks), ticks,
				      GL_STATIC_DRAW_ARB));
	} else
		GLCHK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, bufferid));
	
	GLCHK(glVertexPointer(3, GL_FLOAT, 0, NULL+0));
	GLCHK(glColor4f(.5,.5,0,.5));
	GLCHK(glDrawArrays(GL_LINES, 0, NTICKS*2));

	GLCHK(glPopClientAttrib());

	glBegin(GL_LINES);
		glColor3f(0,1,0);
		glVertex2f(1./60, 0);
		glVertex2f(1./60, 1);

		glColor3f(1,1,0);
		glVertex2f(1./30, 0);
		glVertex2f(1./30, 1);

		glColor3f(1,0,0);
		glVertex2f(1./15, 0);
		glVertex2f(1./15, 1);
	GLCHK(glEnd());

	GLCHK(glPopAttrib());
	
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
