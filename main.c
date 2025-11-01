#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include <sys/time.h>

#define GL_SILENCE_DEPRECATION 1
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/GL.h>
#endif

#define MATH_3D_IMPLEMENTATION 1
#include "math_3d.h"

#define f32 float

#include "mdl/lakehairvp.h"
#include "mdl/lakehairvc.h"
#include "mdl/lakeheadvp.h"
#include "mdl/lakeheadvc.h"
#include "mdl/lakeeyevp.h"    //all white so no col
#include "mdl/lakeeyebrow.h"  //combined
#include "mdl/lakebodyvc.h"
#include "mdl/lakebodyvp.h"
#include "mdl/stage.h"        //combined
#include "mdl/lakehairhalves.h"
#include "mdl/scissor/scissor1vp.h"
#include "mdl/scissor/scissor1vc.h"
#include "mdl/scissor/scissor2vp.h"
#include "mdl/scissor/scissor2vc.h"
#include "mdl/stringvp.h"     // no col

#include "mdl/mouth/eh.h"
#include "mdl/mouth/teeth.h" 
#include "mdl/mouth/close.h" //monochrome
#include "mdl/mouth/o.h" //monochrome
#include "mdl/mouth/m.h" //monochrome

#include "mdl/credit/text1vp.h"
#include "mdl/credit/text2vp.h"
#include "mdl/credit/text3vp.h"

// #include "anim/lakeHairRegularAnim.h"
#include "anim/lakeHeadAnim.h"
#include "anim/lakeEyeAnim.h"
#include "anim/lakePupilAnim.h"
#include "anim/mouthAnims.h"
#include "anim/lakeEyebrowAnim.h"
#include "anim/lakeTorsoAnim.h"
#include "anim/lakeHairHalvesAnims.h"
#include "anim/scissoranims.h"
#include "anim/stringAnim.h"
#include "anim/lakeHairClipAnim.h"

#include "anim/camAnim.h"
#include "anim/camFovAnim.h"

unsigned char * song = NULL;
unsigned long long songpos = 0;
unsigned long long songlength = 0;

int frame = 0;

int width = 0;
int height = 0;
float ratio = 1.333f;

char shouldquit = 0;

void events() {
	SDL_Event event;
	
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					shouldquit = 1;
					break;
				}
				break;
			case SDL_QUIT:
				shouldquit = 1;
				break;
		}
	}
}

void audioCallback(void* userdata, unsigned char * stream, int len) {
	for(int i = 0; i < len; i++) {
		if(songpos > songlength) { stream[i] = 0; shouldquit = 1; continue; }
		stream[i] = song[songpos];
		songpos++;
	}
}

void initDraw() {
	ratio = (float)width / (float)height;
	
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.33f, 0.576f, 0.835f, 1.0f);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void drawFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	unsigned int frame12 = frame * 12;
	unsigned int frame9 = frame * 9;
	mat4_t view = mat4(cameraAnim[frame12], cameraAnim[(frame12) + 1], cameraAnim[(frame12) + 2], cameraAnim[(frame12) + 3],
	                   cameraAnim[(frame12) + 4], cameraAnim[(frame12) + 5], cameraAnim[(frame12) + 6], cameraAnim[(frame12) + 7],
					   cameraAnim[(frame12) + 8], cameraAnim[(frame12) + 9], cameraAnim[(frame12) + 10], cameraAnim[(frame12) + 11],
					   0.0f, 0.0f, 0.0f, 1.0f);
	mat4_t model, modelview, model2, mrx, mry, mrz, persp;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(frame > 1073 && frame < 1203) {
		persp = m4_perspective(cameraFovAnim[frame] * 2.0f, ratio, 11.0f, 30.0f);
	} else {
		persp = m4_perspective(cameraFovAnim[frame] * 2.0f, ratio, 0.5f, 30.0f);
	}
	glLoadMatrixf(&persp);
	glMatrixMode(GL_MODELVIEW);
	
	//Stage
	model = m4_identity();
	model2 = m4_identity();
	mrx = m4_identity();
	mry = m4_identity();
	mrz = m4_identity();
	if(frame >= 1538) {
		model = m4_translation((vec3_t){0.0f, 2.005f, 0.640f});
		mrx = m4_rotation_x(-1.571f);
		mrz = m4_rotation_z(3.141f);
		model2 = m4_scaling((vec3_t){2.0f, 2.0f, 8.0f});
	} else {
		model = m4_translation((vec3_t){0.0f, -0.001f, -1.354f});
		model2 = m4_scaling((vec3_t){2.0f, 2.0f, 2.0f});
	}
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 18; i++) {
		int i3 = i * 3;
		glColor3f(stageVertCol[i3], stageVertCol[i3+1], stageVertCol[i3+2]);
		glVertex3f(stageVertPos[i3], stageVertPos[i3+1], stageVertPos[i3+2]);
	}
	glEnd();
	
	//Hair
	if(frame < 1203) {
	mrx = m4_rotation_x(lakeHeadAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeHeadAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeHeadAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeHeadAnim[frame9], lakeHeadAnim[frame9 + 1], lakeHeadAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeHeadAnim[frame9 + 6], lakeHeadAnim[frame9 + 7], lakeHeadAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 681; i++) {
		int i3 = i * 3;
		glColor3f(lakeHairVertCol[i3], lakeHairVertCol[i3+1], lakeHairVertCol[i3+2]);
		glVertex3f(lakehairVertPos[i3], lakehairVertPos[i3+1], lakehairVertPos[i3+2]);
	}
	glEnd();
	} else {
	//Hair left
	mrx = m4_rotation_x(lakeHairRipLeftAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeHairRipLeftAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeHairRipLeftAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeHairRipLeftAnim[frame9], lakeHairRipLeftAnim[frame9 + 1], lakeHairRipLeftAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeHairRipLeftAnim[frame9 + 6], lakeHairRipLeftAnim[frame9 + 7], lakeHairRipLeftAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 162; i++) {
		int i3 = i * 3;
		glColor3f(lakeHairRipLeftVertCol[i3], lakeHairRipLeftVertCol[i3+1], lakeHairRipLeftVertCol[i3+2]);
		glVertex3f(lakeHairRipLeftVertPos[i3], lakeHairRipLeftVertPos[i3+1], lakeHairRipLeftVertPos[i3+2]);
	}
	glEnd();
	//Hair right
	mrx = m4_rotation_x(lakeHairRipRightAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeHairRipRightAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeHairRipRightAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeHairRipRightAnim[frame9], lakeHairRipRightAnim[frame9 + 1], lakeHairRipRightAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeHairRipRightAnim[frame9 + 6], lakeHairRipRightAnim[frame9 + 7], lakeHairRipRightAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 579; i++) {
		int i3 = i * 3;
		glColor3f(lakeHairRipRightVertCol[i3], lakeHairRipRightVertCol[i3+1], lakeHairRipRightVertCol[i3+2]);
		glVertex3f(lakeHairRipRightVertPos[i3], lakeHairRipRightVertPos[i3+1], lakeHairRipRightVertPos[i3+2]);
	}
	glEnd();
	}
	//Head
	mrx = m4_rotation_x(lakeHeadAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeHeadAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeHeadAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeHeadAnim[frame9], lakeHeadAnim[frame9 + 1], lakeHeadAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeHeadAnim[frame9 + 6], lakeHeadAnim[frame9 + 7], lakeHeadAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 102; i++) {
		int i3 = i * 3;
		glColor3f(lakeHeadVertCol[i3], lakeHeadVertCol[i3+1], lakeHeadVertCol[i3+2]);
		glVertex3f(lakeHeadVertPos[i3], lakeHeadVertPos[i3+1], lakeHeadVertPos[i3+2]);
	}
	glEnd();
	//Eye
	mrx = m4_rotation_x(lakeEyeAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeEyeAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeEyeAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeEyeAnim[frame9], lakeEyeAnim[frame9 + 1], lakeEyeAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeEyeAnim[frame9 + 6], lakeEyeAnim[frame9 + 7], lakeEyeAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 180; i++) {
		int i3 = i * 3;
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(lakeEyeVertPos[i3], lakeEyeVertPos[i3+1], lakeEyeVertPos[i3+2]);
	}
	glEnd();
	//PupilL
	mrx = m4_rotation_x(lakePupilLAnim[frame9 + 3]);
	mry = m4_rotation_y(lakePupilLAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakePupilLAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakePupilLAnim[frame9], lakePupilLAnim[frame9 + 1], lakePupilLAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakePupilLAnim[frame9 + 6], lakePupilLAnim[frame9 + 7], lakePupilLAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-1.0, 1.0, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0, 1.0, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0, -1.0, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-1.0, -1.0, 0.0f);
	glEnd();
	//PupilR
	mrx = m4_rotation_x(lakePupilRAnim[frame9 + 3]);
	mry = m4_rotation_y(lakePupilRAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakePupilRAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakePupilRAnim[frame9], lakePupilRAnim[frame9 + 1], lakePupilRAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakePupilRAnim[frame9 + 6], lakePupilRAnim[frame9 + 7], lakePupilRAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-1.0, 1.0, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0, 1.0, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0, -1.0, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-1.0, -1.0, 0.0f);
	glEnd();
	//Eyebrow
	mrx = m4_rotation_x(lakeEyebrowAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeEyebrowAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeEyebrowAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeEyebrowAnim[frame9], lakeEyebrowAnim[frame9 + 1], lakeEyebrowAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeEyebrowAnim[frame9 + 6], lakeEyebrowAnim[frame9 + 7], lakeEyebrowAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 384; i++) {
		int i3 = i * 3;
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(lakeEyebrowVertPos[i3], lakeEyebrowVertPos[i3+1], lakeEyebrowVertPos[i3+2]);
	}
	glEnd();
	//Eh mouth
	mrx = m4_rotation_x(lakeMouthEhAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeMouthEhAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeMouthEhAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeMouthEhAnim[frame9], lakeMouthEhAnim[frame9 + 1], lakeMouthEhAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeMouthEhAnim[frame9 + 6], lakeMouthEhAnim[frame9 + 7], lakeMouthEhAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 474; i++) {
		int i3 = i * 3;
		glColor3f(lakeMouthEhVertCol[i3], lakeMouthEhVertCol[i3+1], lakeMouthEhVertCol[i3+2]);
		glVertex3f(lakeMouthEhVertPos[i3], lakeMouthEhVertPos[i3+1], lakeMouthEhVertPos[i3+2]);
	}
	glEnd();
	//Teeth mouth
	mrx = m4_rotation_x(lakeMouthClosejawShoteefAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeMouthClosejawShoteefAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeMouthClosejawShoteefAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeMouthClosejawShoteefAnim[frame9], lakeMouthClosejawShoteefAnim[frame9 + 1], lakeMouthClosejawShoteefAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeMouthClosejawShoteefAnim[frame9 + 6], lakeMouthClosejawShoteefAnim[frame9 + 7], lakeMouthClosejawShoteefAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 24; i++) {
		int i3 = i * 3;
		glColor3f(lakeMouthTeethVertCol[i3], lakeMouthTeethVertCol[i3+1], lakeMouthTeethVertCol[i3+2]);
		glVertex3f(lakeMouthTeethVertPos[i3], lakeMouthTeethVertPos[i3+1], lakeMouthTeethVertPos[i3+2]);
	}
	glEnd();
	//Closed mouth
	mrx = m4_rotation_x(lakeMouthCloseAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeMouthCloseAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeMouthCloseAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeMouthCloseAnim[frame9], lakeMouthCloseAnim[frame9 + 1], lakeMouthCloseAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeMouthCloseAnim[frame9 + 6], lakeMouthCloseAnim[frame9 + 7], lakeMouthCloseAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 12; i++) {
		int i3 = i * 3;
		glColor3f(0.027f, 0.027f, 0.027f);
		glVertex3f(lakeMouthCloseVertPos[i3], lakeMouthCloseVertPos[i3+1], lakeMouthCloseVertPos[i3+2]);
	}
	glEnd();
	//mmmmmmouth
	mrx = m4_rotation_x(lakeMouthMAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeMouthMAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeMouthMAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeMouthMAnim[frame9], lakeMouthMAnim[frame9 + 1], lakeMouthMAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeMouthMAnim[frame9 + 6], lakeMouthMAnim[frame9 + 7], lakeMouthMAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 24; i++) {
		int i3 = i * 3;
		glColor3f(0.027f, 0.027f, 0.027f);
		glVertex3f(lakeMouthMVertPos[i3], lakeMouthMVertPos[i3+1], lakeMouthMVertPos[i3+2]);
	}
	glEnd();
	//O mouth
	mrx = m4_rotation_x(lakeMouthOAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeMouthOAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeMouthOAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeMouthOAnim[frame9], lakeMouthOAnim[frame9 + 1], lakeMouthOAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeMouthOAnim[frame9 + 6], lakeMouthOAnim[frame9 + 7], lakeMouthOAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 36; i++) {
		int i3 = i * 3;
		glColor3f(0.027f, 0.027f, 0.027f);
		glVertex3f(lakeMouthOVertPos[i3], lakeMouthOVertPos[i3+1], lakeMouthOVertPos[i3+2]);
	}
	glEnd();
	//Body
	mrx = m4_rotation_x(lakeTorsoAnim[frame9 + 3]);
	mry = m4_rotation_y(lakeTorsoAnim[frame9 + 4]);
	mrz = m4_rotation_z(lakeTorsoAnim[frame9 + 5]);
	model = m4_translation((vec3_t){lakeTorsoAnim[frame9], lakeTorsoAnim[frame9 + 1], lakeTorsoAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){lakeTorsoAnim[frame9 + 6], lakeTorsoAnim[frame9 + 7], lakeTorsoAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 69; i++) {
		int i3 = i * 3;
		glColor3f(lakeBodyVertCol[i3], lakeBodyVertCol[i3+1], lakeBodyVertCol[i3+2]);
		glVertex3f(lakeBodyVertPos[i3], lakeBodyVertPos[i3+1], lakeBodyVertPos[i3+2]);
	}
	glEnd();
	if (frame > 165 && frame < 340) {
	//Scissor 1
	mrx = m4_rotation_x(scissor1Anim[frame9 + 3]);
	mry = m4_rotation_y(scissor1Anim[frame9 + 4]);
	mrz = m4_rotation_z(scissor1Anim[frame9 + 5]);
	model = m4_translation((vec3_t){scissor1Anim[frame9], scissor1Anim[frame9 + 1], scissor1Anim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){scissor1Anim[frame9 + 6], scissor1Anim[frame9 + 7], scissor1Anim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 2514; i++) {
		int i3 = i * 3;
		glColor3f(scissor1VertCol[i3], scissor1VertCol[i3+1], scissor1VertCol[i3+2]);
		glVertex3f(scissor1VertPos[i3], scissor1VertPos[i3+1], scissor1VertPos[i3+2]);
	}
	glEnd();
	//Scissor 2
	mrx = m4_rotation_x(scissor2Anim[frame9 + 3]);
	mry = m4_rotation_y(scissor2Anim[frame9 + 4]);
	mrz = m4_rotation_z(scissor2Anim[frame9 + 5]);
	model = m4_translation((vec3_t){scissor2Anim[frame9], scissor2Anim[frame9 + 1], scissor2Anim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){scissor2Anim[frame9 + 6], scissor2Anim[frame9 + 7], scissor2Anim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 1452; i++) {
		int i3 = i * 3;
		glColor3f(scissor2VertCol[i3], scissor2VertCol[i3+1], scissor2VertCol[i3+2]);
		glVertex3f(scissor2VertPos[i3], scissor2VertPos[i3+1], scissor2VertPos[i3+2]);
	}
	glEnd();
	}
	//String
	if (frame > 842 && frame < 1204) {
	mrx = m4_rotation_x(stringAnim[frame9 + 3]);
	mry = m4_rotation_y(stringAnim[frame9 + 4]);
	mrz = m4_rotation_z(stringAnim[frame9 + 5]);
	model = m4_translation((vec3_t){stringAnim[frame9], stringAnim[frame9 + 1], stringAnim[frame9 + 2]});
	model2 = m4_scaling((vec3_t){stringAnim[frame9 + 6], stringAnim[frame9 + 7], stringAnim[frame9 + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 600; i++) {
		int i3 = i * 3;
		glColor3f(0.67f, 0.67f, 0.67f);
		glVertex3f(stringVertPos[i3], stringVertPos[i3+1], stringVertPos[i3+2]);
	}
	glEnd();
	}
	if (frame > 171 && frame < 349) {
	//Hair clipping
	int bluh = (frame - 169) * 9;
	mrx = m4_rotation_x(lakeHairCutBitAnim[bluh + 3]);
	mry = m4_rotation_y(lakeHairCutBitAnim[bluh + 4]);
	mrz = m4_rotation_z(lakeHairCutBitAnim[bluh + 5]);
	model = m4_translation((vec3_t){lakeHairCutBitAnim[bluh], lakeHairCutBitAnim[bluh + 1], lakeHairCutBitAnim[bluh + 2]});
	model2 = m4_scaling((vec3_t){lakeHairCutBitAnim[bluh + 6], lakeHairCutBitAnim[bluh + 7], lakeHairCutBitAnim[bluh + 8]});
	
	mry = m4_mul(mrz, mry);
	mrx = m4_mul(mry, mrx);
	model2 = m4_mul(mrx, model2);
	model = m4_mul(model, model2);
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
		glColor3f(0.1270934f, 0.1270934f, 0.1270934f);
		glVertex3f(-0.099130f, -0.000000f, 0.067562f);
		glColor3f(0.1496859f, 0.1496859f, 0.1496859f);
		glVertex3f(0.016780f, 0.000000f, -0.148370f);
		glColor3f(0.1244663f, 0.1244663f, 0.1244663f);
		glVertex3f(0.082355f, -0.000000f, 0.080809f);
	glEnd();
	}
	if(frame > 1688 && frame < 1830) { //text3
	model = m4_identity();
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 3789; i++) {
		int i3 = i * 3;
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(text3VertPos[i3], text3VertPos[i3+1], text3VertPos[i3+2]);
	}
	glEnd();
	}
	if(frame > 1538 && frame < 1689) { //text1
	model = m4_identity();
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 5820; i++) {
		int i3 = i * 3;
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(text1VertPos[i3], text1VertPos[i3+1], text1VertPos[i3+2]);
	}
	glEnd();
	model = m4_identity(); //text2
	modelview = m4_mul(view, model);
	glLoadMatrixf(&modelview);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 822; i++) {
		int i3 = i * 3;
		glColor3f(0.8f, 0.8f, 0.8f);
		glVertex3f(text2VertPos[i3], text2VertPos[i3+1], text2VertPos[i3+2]);
	}
	glEnd();
	}
	
	
	SDL_GL_SwapBuffers();
}

int main(int argc, char* argv[]) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		printf("i couldn't initialize sdl...\n");
		return -1;
	}
	const SDL_VideoInfo * info = SDL_GetVideoInfo();
	if(!info) {
		printf("i couldn't get video info...\n");
		SDL_Quit();
		return -69;
	}
	width = info->current_w;
	height = info->current_h;
	//width = 640; height = 480;
	//if(SDL_SetVideoMode(width, height, 8, SDL_OPENGL) == 0) {
	if(SDL_SetVideoMode(width, height, 8, SDL_OPENGL | SDL_FULLSCREEN) == 0) {
		printf("i couldn't set the video mode...\n");
		SDL_Quit();
		return -2;
	}

	SDL_AudioSpec desired, obtained;
	desired.freq = 44100;
	desired.format = AUDIO_S16LSB;
	desired.channels = 2;
	desired.samples = 4096;
	desired.callback = audioCallback;
	desired.userdata = NULL;
	
	if(SDL_OpenAudio(&desired, &obtained) != 0) {
		printf("i couldn't open the audio device...\n");
		SDL_Quit();
		return -3;
	}
	FILE * songhandle = fopen("song.raw", "r");
	if(songhandle == NULL) { FILE * songhandle = fopen("../Resources/song.raw", "r"); }
	perror("song.raw:");
	if(songhandle == NULL) { printf("couldn't get song..\n"); return -1; }
	unsigned int songhash = 0;
	int cur;
	while(cur != EOF) {
		cur = fgetc(songhandle);
		//songhash += cur; //slow
		songlength++;
	}
	//if(songhash != 719558098) printf("song hash mismatch...\n");
	if(songlength != 5696961) printf("song length mismatch...\n");
	//printf("songhash is %d and songlength is %d\n", songhash, songlength);
	rewind(songhandle);
	song = malloc(songlength);
	for(long long i = 0; i < songlength; i++) {
		song[i] = fgetc(songhandle);
	}

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	initDraw();
	SDL_PauseAudio(0);
	SDL_ShowCursor(SDL_DISABLE);
	
	struct timeval tv;
    gettimeofday(&tv, NULL);
	
    unsigned long long start, end;
	
	while(1){
		gettimeofday(&tv, NULL);
		start = (unsigned long long)tv.tv_sec * 1000000ULL + tv.tv_usec;
		events();
		//if(frame > 1738) break;
		if(shouldquit) break;
		drawFrame();
		//printf("%d\n", frame);
		switch(frame) {
			case 1:
				printf("\n");
				break;
			case 188:
				printf("I don't remember a damn thing from last week\n");
				break;
			case 344:
				printf("I'm sorry I could never be the one you thought I was\n");	
				break;
			case 518:
				printf("I try my best but everybody's looking past me\n");
				break;
			case 689:
				printf("I blacked out on the pavement and I think it's because\n");
				break;
			case 856:
				printf("She told me I was a waste of time, and I hated life\n");
				break;
			case 1080:
				printf("I was stuck and I had nowhere to go\n");
				break;
			case 1200:
				printf("and I know you're not a friend of mine, I'm not on your mind\n");
				break;
			case 1404:
				printf("so I said fuck it, I guess I'll be alone\n");
				break;
			case 1598: printf("\n");break;
			case 1600: printf("                       #                       #\n");break;
			case 1602: printf("                      #                       #\n");break;
			case 1604: printf("   ###   ##  #     # ###   ##  ###   ##     ####  ##      ###  ##\n");break;
			case 1606: printf("  #  # #  #  # # #  #  # #### #  # ####     #   #  #    #  # #  #\n");break;
			case 1608: printf(" #  #  ##    # #   #  #  ### #     ###      ##  ##      ###  ##\n");break;
			case 1610: printf("******************************************************** # ***\n");break;
			case 1612: printf(" (feat. jane remover) by juno     animation by lliy   ##\n");break;
			case 1614: printf("\n");break;
		}
		frame++;
		gettimeofday(&tv, NULL);
		end = (unsigned long long)tv.tv_sec * 1000000ULL + tv.tv_usec;
		int sleeptime = 16683LL - (end - start);
		//printf("%d sleeptime ", sleeptime);
		sleeptime = (sleeptime > 0) ? sleeptime : 1;
		usleep(sleeptime);
	}

	free(song);
	SDL_Quit();
	return 0;
}
