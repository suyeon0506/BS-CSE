#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
GLuint h_ShaderProgram_simple, h_ShaderProgram_PS, h_ShaderProgram_GS; // handles to shader programs

#define PHONG_SHADER 0
#define GOURAUD_SHADER 1
int shader_mode = PHONG_SHADER; //기본적으로 퐁쉐이더로 설정

// for simple shaders
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phone Shading shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
///////////////////효과추가///////////////////////////////////
GLfloat loc_screen_effect, loc_screen_width;
GLfloat loc_blind_effect, loc_blind_ratio;
GLboolean loc_fog_effect;
GLint loc_cartoon_effect, loc_cartoon_levels;
int flag_draw_screen, flag_screen_effect, flag_blind_effect, flag_fog_effect, flag_cartoon_effect;
float screen_width, blind_ratio = 0.0f, cartoon_levels;
/////////////////////////////////////////////////////////////////
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;

///camera 변수 설정
#define NUMBER_OF_CAMERAS 7
#define CAM_TRANSLATION_SPEED 0.025f
#define CAM_ROTATION_SPEED 0.1f

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_POSITION 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

void prepare_shader_program(void);
void initialize_lights_and_material(void);
void set_up_scene_lights(void);///이 세 함수는 다른 함수에서 미리 쓰여서 선언 필요

typedef struct _CAMERA {
	glm::vec3 pos, uaxis, vaxis, naxis;
	float fov_y, aspect_ratio, near_clip, far_clip;
	int move_status;
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
//int camera_selected;

typedef struct _VIEWPORT {
	int x, y, w, h;
} VIEWPORT;
VIEWPORT viewport[NUMBER_OF_CAMERAS];

typedef enum {
	VIEW_WORLD, VIEW_SQUARE, VIEW_TIGER, VIEW_COW
} VIEW_MODE;

#include "Object_Definitions.h"

void renew_cam_position_along_axis(int cam_index, float del, glm::vec3 trans_axis) {
	camera[cam_index].pos += CAM_TRANSLATION_SPEED*del*trans_axis;
}

void renew_cam_orientation_rotation_around_axis(int cam_index, float angle, glm::vec3 rot_axis) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED*TO_RADIAN*angle, rot_axis));

	camera[cam_index].uaxis = RotationMatrix * camera[cam_index].uaxis;
	camera[cam_index].vaxis = RotationMatrix * camera[cam_index].vaxis;
	camera[cam_index].naxis = RotationMatrix * camera[cam_index].naxis;
}

/////변수 설정 끝////////////////////////////////////////////////////////////////////////////////////////////

// Start of callback function definitions
typedef struct _CALLBACK_CONTEXT {
	int left_button_status, rotation_mode_cow, timestamp_cow;
	int prevx, prevy;
	float rotation_angle_cow;
} CALLBACK_CONTEXT;
CALLBACK_CONTEXT cc;

void display(void) {
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (i = 0; i < NUMBER_OF_CAMERAS; i++)
	{
		glUseProgram(h_ShaderProgram_simple);
		glLineWidth(2.0f);
		draw_axes(i);
		glLineWidth(1.0f);
		glUseProgram(0);
	}

	set_up_scene_lights();
	if (shader_mode == PHONG_SHADER)
		glUseProgram(h_ShaderProgram_PS);
	else if(shader_mode == GOURAUD_SHADER)
		glUseProgram(h_ShaderProgram_GS);

	draw_static_object(&(static_objects[OBJ_BUILDING]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_TABLE]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_TABLE]), 1, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 1, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 2, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 3, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 4, 1200, 800);
	draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, 1200, 800);;
	draw_static_object(&(static_objects[OBJ_TEAPOT2]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_BIKE]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_IM]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_GODZ]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_BUS]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_FRAME]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_COW]), 0, 1200, 800);
	draw_animated_tiger(1200, 800);

	glUseProgram(0);

	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float zoom = 0;
float cam_u, cam_v, cam_n;
float eye_u, eye_v, eye_n;
float rotate_X, rotate_Y, rotate_Z;
float cam_u_0 = 600, cam_v_0 = 600, cam_n_0 = 200;//첫 화면의 카메라 좌표
float eye_u_0 = 125, eye_v_0 = 80, eye_n_0 = 25;//첫 화면의 바라보는 좌표
float up_u_0 = 0, up_v_0 = 0, up_n_0 = 1;//첫 화면의 up벡터
float now_cam_u = 600, now_cam_v = 600, now_cam_n = 200;//현재 화면의 카메라 좌표(처음엔 첫화면)
float now_eye_u = 125, now_eye_v = 80, now_eye_n = 25;//현재 화면의 바라보는 좌표(처음엔 첫화면)
float now_up_u = 0, now_up_v = 0, now_up_n = 1;//현재 화면의 up벡터(처음엔 첫화면)
int not_move_flag = 0; //정적인 cctv일때 움직이지 않기 위해 세우는 플래그
void init_vars()
{
	zoom = 0;
	cam_u = cam_v = cam_n = 0;
	eye_u = eye_v = eye_n = 0;
	rotate_X = rotate_Y = rotate_Z = 0;
	not_move_flag = 0;
}//카메라 변환시에 변수들 초기화
void initialize_camera(void);
///////////////////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;
	glm::vec4 tmp;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	////////////////////////기능 추가 작성 시작////////////////////////////////////////////////////////////////////////////
	case '0'://첫 화면으로 돌아가기(정면도)
		init_vars();
		ViewMatrix[0] = glm::lookAt(glm::vec3(cam_u_0, cam_v_0, cam_n_0), glm::vec3(eye_u_0, eye_v_0, eye_n_0),
			glm::vec3(up_u_0, up_v_0, up_n_0));
		ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
		now_cam_u = cam_u_0; now_cam_v = cam_v_0; now_cam_n = cam_n_0;
		now_eye_u = eye_u_0; now_eye_v = eye_v_0; now_eye_n = eye_n_0;
		now_up_u = up_u_0; now_up_v = up_v_0; now_up_n = up_n_0;
		glutPostRedisplay();
		break;
	case '1'://첫 화면을 위에서 보기(상면도)
		init_vars();
		ViewMatrix[0] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f));
		ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
		now_cam_u = 120.0f; now_cam_v = 90.0f; now_cam_n = 1000.0f;
		now_eye_u = 120.0f; now_eye_v = 90.0f; now_eye_n = 0.0f;
		now_up_u = -10.0f; now_up_v = 0.0f; now_up_n = 0.0f;
		glutPostRedisplay();
		break;
	case '2'://첫 화면을 옆에서 보기(측면도)
		init_vars();
		ViewMatrix[0] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
		now_cam_u = 800.0f; now_cam_v = 90.0f; now_cam_n = 25.0f;
		now_eye_u = 0.0f; now_eye_v = 90.0f; now_eye_n = 25.0f;
		now_up_u = 0.0f; now_up_v = 0.0f; now_up_n = 1.0f;
		glutPostRedisplay();
		break;
	case '3':// 현재 화면에서 카메라를 U축 양의 방향으로 이동
		if (not_move_flag != 1 && cam_u + 1 <= 600)
		{
			cam_u += 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(1.0f, 0.0f, 0.0f));
			now_cam_u += cam_u;
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '4':// 현재 화면에서 카메라를 U축 음의 방향으로 이동
		if (not_move_flag != 1 && cam_u - 1 > -600)
		{
			cam_u -= 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(-1.0f, 0.0f, 0.0f));
			now_cam_u += cam_u;
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '5':// 현재 화면에서 카메라를 V축 양의 방향으로 이동
		if (not_move_flag != 1 && cam_v + 1 <= 600)
		{
			cam_v += 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 1.0f, 0.0f));
			now_cam_v += cam_v;
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '6':// 현재 화면에서 카메라를 V축 음의 방향으로 이동
		if (not_move_flag != 1 && cam_v - 1 >= -600)
		{
			cam_v -= 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, -1.0f, 0.0f));
			now_cam_v += cam_v;
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '7':// 현재 화면에서 카메라를 N축 양의 방향으로 이동
		if (not_move_flag != 1 && cam_n + 1 <= 200)
		{
			cam_n += 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			now_cam_n += cam_n;
			glutPostRedisplay();
		}
		break;
	case '8':// 현재 화면에서 카메라를 N축 음의 방향으로 이동
		if (not_move_flag != 1 && cam_n - 1 >= -200)
		{
			cam_n -= 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 0.0f, -1.0f));
			now_cam_n += cam_n;
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '+': // Zoom in
		if (not_move_flag != 1 && zoom + 0.01 <= 1)
		{
			zoom += 0.01;
			ViewMatrix[0] = glm::lookAt(glm::vec3(now_cam_u, now_cam_v, now_cam_n), glm::vec3(now_eye_u, now_eye_v, now_eye_n),
				glm::vec3(now_up_u, now_up_v, now_up_n));
			ViewMatrix[0] = glm::scale(ViewMatrix[0], glm::vec3(1 + zoom, 1 + zoom, 1 + zoom));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '-': // Zoom out
		if (not_move_flag != 1 && zoom - 0.01 >= -1)
		{
			zoom -= 0.01;
			ViewMatrix[0] = glm::lookAt(glm::vec3(now_cam_u, now_cam_v, now_cam_n), glm::vec3(now_eye_u, now_eye_v, now_eye_n),
				glm::vec3(now_up_u, now_up_v, now_up_n));
			ViewMatrix[0] = glm::scale(ViewMatrix[0], glm::vec3(1 + zoom, 1 + zoom, 1 + zoom));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'q': //x축 시계 반대방향 회전 ;; 회전:키보드 맨 위 맨 왼쪽 키 6개(qwerty)
		if (not_move_flag != 1)
		{
			rotate_X++;
			if (rotate_X == 360)
				rotate_X = 0;
			ViewMatrix[0] = glm::rotate(ViewMatrix[0], TO_RADIAN * 1, glm::vec3(1.0, 0.0, 0.0));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'w': //x축 시계 방향 회전
		if (not_move_flag != 1)
		{
			rotate_X--;
			if (rotate_X < 0)
				rotate_X = 359;
			ViewMatrix[0] = glm::rotate(ViewMatrix[0], TO_RADIAN * (-1), glm::vec3(1.0, 0.0, 0.0));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'e': //y축 시계 반대방향 회전
		if (not_move_flag != 1)
		{
			rotate_Y++;
			if (rotate_Y == 360)
				rotate_Y = 0;
			ViewMatrix[0] = glm::rotate(ViewMatrix[0], TO_RADIAN * 1, glm::vec3(0.0, 1.0, 0.0));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'r': //y축 시계 방향 회전
		if (not_move_flag != 1)
		{
			rotate_Y--;
			if (rotate_Y < 0)
				rotate_Y = 359;
			ViewMatrix[0] = glm::rotate(ViewMatrix[0], TO_RADIAN * (-1), glm::vec3(0.0, 1.0, 0.0));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 't': //z축 시계 반대방향 회전
		if (not_move_flag != 1)
		{
			rotate_Z++;
			if (rotate_Z == 360)
				rotate_Z = 0;
			ViewMatrix[0] = glm::rotate(ViewMatrix[0], TO_RADIAN * 1, glm::vec3(0.0, 0.0, 1.0));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'y': //z축 시계 방향 회전
		if (not_move_flag != 1)
		{
			rotate_Z--;
			if (rotate_Z < 0)
				rotate_Z = 359;
			ViewMatrix[0] = glm::rotate(ViewMatrix[0], TO_RADIAN * (-1), glm::vec3(0.0, 0.0, 1.0));
			ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'j': //동적 cctv v 음의 방향으로 시선 전환
		if (not_move_flag != 1)
		{
			eye_v -= 10;
			if (eye_v < 0) eye_v = 350;
			tmp = glm::vec4(-20.0f, -15.0f, -15.0f, 1.0f);
			tmp = glm::rotate(glm::mat4(1.0f), eye_u*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f)) * tmp;
			tmp = glm::rotate(glm::mat4(1.0f), eye_v*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f)) * tmp;
			tmp = glm::rotate(glm::mat4(1.0f), eye_n*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f)) * tmp;
			tmp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 15.0f, 15.0f)) * tmp;
			ViewMatrix[5] = glm::lookAt(glm::vec3(20.0f, 15.0f, 15.0f), glm::vec3(tmp.x, tmp.y, tmp.z),
				glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewMatrix = ProjectionMatrix[5] * ViewMatrix[5];
			glutPostRedisplay();
		}
		break;
	case 'i': //동적 cctv v 양의 방향으로 시선 전환
		if (not_move_flag != 1)
		{
			eye_v += 10;
			if (eye_v == 360) eye_v = 0;
			tmp = glm::vec4(-20.0f, -15.0f, -15.0f, 1.0f);
			tmp = glm::rotate(glm::mat4(1.0f), eye_u*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f)) * tmp;
			tmp = glm::rotate(glm::mat4(1.0f), eye_v*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f)) * tmp;
			tmp = glm::rotate(glm::mat4(1.0f), eye_n*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f)) * tmp;
			tmp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 15.0f, 15.0f)) * tmp;
			ViewMatrix[5] = glm::lookAt(glm::vec3(20.0f, 15.0f, 15.0f), glm::vec3(tmp.x, tmp.y, tmp.z),
				glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewMatrix = ProjectionMatrix[5] * ViewMatrix[5];
			glutPostRedisplay();
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}

void reshape(int width, int height) {
	camera[0].aspect_ratio = (float)width / height;

	ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
	ModelViewMatrix = ProjectionMatrix[0] * ViewMatrix[0];

	camera[1].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[1] = glm::perspective(camera[1].fov_y*TO_RADIAN, camera[1].aspect_ratio, camera[1].near_clip, camera[1].far_clip);
	ModelViewMatrix = ProjectionMatrix[1] * ViewMatrix[1];

	camera[2].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[2] = glm::perspective(camera[2].fov_y*TO_RADIAN, camera[2].aspect_ratio, camera[2].near_clip, camera[2].far_clip);
	ModelViewMatrix = ProjectionMatrix[2] * ViewMatrix[2];

	camera[3].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[3] = glm::perspective(camera[3].fov_y*TO_RADIAN, camera[3].aspect_ratio, camera[3].near_clip, camera[3].far_clip);
	ModelViewMatrix = ProjectionMatrix[3] * ViewMatrix[3];

	camera[4].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[4] = glm::perspective(camera[4].fov_y*TO_RADIAN, camera[4].aspect_ratio, camera[4].near_clip, camera[4].far_clip);
	ModelViewMatrix = ProjectionMatrix[4] * ViewMatrix[4];
	glutPostRedisplay(); ///정적인 cctv ;; 주전자와 소를 바라보고 있으며 이 상태에서는 카메라 이동,회전,줌인아웃 불가

	camera[5].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[5] = glm::perspective(camera[5].fov_y*TO_RADIAN, camera[5].aspect_ratio, camera[5].near_clip, camera[5].far_clip);
	ModelViewMatrix = ProjectionMatrix[5] * ViewMatrix[5];
	glutPostRedisplay(); ///동적인 cctv ;; 내가 생성한 객체를 보고있으며 카메라 이동,회전,줌인아웃,클릭시 타방향 뷰 가능
}

void timer_scene(int timestamp_scene) {
	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	tiger_data.go -= 1;
	if (tiger_data.stop != 1)
	{
		glutPostRedisplay();
		glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
	}
}

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < n_static_objects; i++)
	{
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++)
	{
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);
}

void mouse(int button, int state, int x, int y) {  // mouse callback.
	if (button == GLUT_RIGHT_BUTTON) {
		int mod = glutGetModifiers();
		if (state == GLUT_DOWN && mod == GLUT_ACTIVE_SHIFT)
		{
			shader_mode = GOURAUD_SHADER;
			prepare_shader_program();
			initialize_lights_and_material();
			set_up_scene_lights();
			glutPostRedisplay();
		}
		else
		{
			shader_mode = PHONG_SHADER;
			prepare_shader_program();
			initialize_lights_and_material();
			set_up_scene_lights();
			glutPostRedisplay();
		}
	}
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
//	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_PS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");
	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");

//	loc_blind_ratio = glGetUniformLocation(h_ShaderProgram_PS, "u_ratio");
//	loc_screen_width = glGetUniformLocation(h_ShaderProgram_PS, "u_screen");
//	loc_fog_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_flag_fog");

	if (shader_mode == PHONG_SHADER)
	{
		for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++)
		{
			sprintf(string, "u_light[%d].light_on", i);
			loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].position", i);
			loc_light[i].position = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].ambient_color", i);
			loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].diffuse_color", i);
			loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].specular_color", i);
			loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].spot_direction", i);
			loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].spot_exponent", i);
			loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].spot_cutoff_angle", i);
			loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_PS, string);
			sprintf(string, "u_light[%d].light_attenuation_factors", i);
			loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_PS, string);
		}

		loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
		loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
		loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
		loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
		loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");
	}
	else if (shader_mode == GOURAUD_SHADER)
	{
		for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++)
		{
			sprintf(string, "u_light[%d].light_on", i);
			loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].position", i);
			loc_light[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].ambient_color", i);
			loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].diffuse_color", i);
			loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].specular_color", i);
			loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].spot_direction", i);
			loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].spot_exponent", i);
			loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].spot_cutoff_angle", i);
			loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
			sprintf(string, "u_light[%d].light_attenuation_factors", i);
			loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
		}

		loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
		loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
		loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
		loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
		loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");
	}
}

//////////////////////추가 구현사항///////////////////////////////////
void initialize_screen(void) {
	flag_screen_effect = 0;
	screen_width = 0.125f;
}

void initialize_blind(void) {
	flag_blind_effect = 0;
}

void initialize_fog(void) {
	flag_fog_effect = 0;
}

void initialize_cartoon(void) {
	flag_cartoon_effect = 0; cartoon_levels = 3.0f;
}
////////////////////////////////////////////////////////////////

void initialize_lights_and_material(void) { //새로 추가된 함수!!!// follow OpenGL conventions for initialization
	int i;
	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	if(shader_mode == PHONG_SHADER)
		glUseProgram(h_ShaderProgram_PS);
	else if(shader_mode == GOURAUD_SHADER)
		glUseProgram(h_ShaderProgram_GS);

	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 1); // turn on all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

//	glUniform1f(loc_blind_ratio, 0);
//	glUniform1i(loc_fog_effect, 0);

	glUseProgram(0);
}

void initialize_OpenGL(void) {

	glEnable(GL_DEPTH_TEST); // Default state
	glEnable(GL_MULTISAMPLE); //추가
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//Line에서 Fill로 바꿈
	glCullFace(GL_BACK);//추가
	glEnable(GL_CULL_FACE);//추가
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);

	ViewMatrix[0] = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	ViewMatrix[1] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
		glm::vec3(-10.0f, 0.0f, 0.0f));
	ViewMatrix[2] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	ViewMatrix[3] = glm::lookAt(glm::vec3(100.0f, 600.0f, 25.0f), glm::vec3(130.0f, 90.0f, 25.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	ViewMatrix[4] = glm::lookAt(glm::vec3(100.0f, 116.0f, 30.0f), glm::vec3(193.0f, 120.0f, 11.0f),
		glm::vec3(0.0f, 0.0f, 3.0f));
	ViewMatrix[5] = glm::lookAt(glm::vec3(80.0f, 130.0f, 40.0f), glm::vec3(105.0f, 140.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -3.0f));

	initialize_camera();
	initialize_lights_and_material();
}

//////////////////////////////////추가된함수//////////////////////////////////
void set_up_scene_lights(void) {

	int i;
	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	if (shader_mode == PHONG_SHADER)
		glUseProgram(h_ShaderProgram_PS);
	else if (shader_mode == GOURAUD_SHADER)
		glUseProgram(h_ShaderProgram_GS);

	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);

	glUniform1f(loc_blind_ratio, blind_ratio);
	glUniform1f(loc_screen_width, screen_width);

/*	for (i = 0; i < NUMBER_OF_CAMERAS; i++)
	{
		// point_light_EC: use light 0
		light[0].light_on = 1;
		light[0].position[0] = 0.0f; light[0].position[1] = 10.0f; 	// point light position in EC
		light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

		light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
		light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

		light[0].diffuse_color[0] = 0.7f; light[0].diffuse_color[1] = 0.7f;
		light[0].diffuse_color[2] = 0.7f; light[0].diffuse_color[3] = 1.0f;

		light[0].specular_color[0] = 0.9f; light[0].specular_color[1] = 0.9f;
		light[0].specular_color[2] = 0.9f; light[0].specular_color[3] = 1.0f;

		// spot_light_WC: use light 1
		light[1].light_on = 1;
		light[1].position[0] = -200.0f; light[1].position[1] = 500.0f; // spot light position in WC
		light[1].position[2] = -200.0f; light[1].position[3] = 1.0f;

		light[1].ambient_color[0] = 0.2f; light[1].ambient_color[1] = 0.2f;
		light[1].ambient_color[2] = 0.2f; light[1].ambient_color[3] = 1.0f;

		light[1].diffuse_color[0] = 0.82f; light[1].diffuse_color[1] = 0.82f;
		light[1].diffuse_color[2] = 0.82f; light[1].diffuse_color[3] = 1.0f;

		light[1].specular_color[0] = 0.82f; light[1].specular_color[1] = 0.82f;
		light[1].specular_color[2] = 0.82f; light[1].specular_color[3] = 1.0f;

		light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
		light[1].spot_direction[2] = 0.0f;
		light[1].spot_cutoff_angle = 20.0f;
		light[1].spot_exponent = 27.0f;

	//	glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_light[0].light_on, light[0].light_on);
		glUniform4fv(loc_light[0].position, 1, light[0].position);
		glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
		glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
		glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

		glUniform1i(loc_light[1].light_on, light[1].light_on);
		// need to supply position in EC for shading
		glm::vec4 position_EC = ViewMatrix[i] * glm::vec4(light[1].position[0], light[1].position[1], light[1].position[2], light[1].position[3]);
		glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
		glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
		glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
		glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
		// need to supply direction in EC for shading in this example shader
		// note that the viewing transform is a rigid body transform
		// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
		glm::vec3 direction_EC = glm::mat3(ViewMatrix[i]) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1], light[1].spot_direction[2]);
		glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
		glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
		glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
		glUseProgram(0);
	}*/

	light[0].position[0] = 0.0f; light[0].position[1] = 0.0f; light[0].position[2] = -10.0f; light[0].position[3] = 1.0f;	// EC
	light[0].ambient_color[0] = 0.0f; light[0].ambient_color[1] = 0.0f; light[0].ambient_color[2] = 0.0f; light[0].ambient_color[3] = 1.0f;
	light[0].diffuse_color[0] = 0.3f; light[0].diffuse_color[1] = 0.3f; light[0].diffuse_color[2] = 0.3f; light[0].diffuse_color[3] = 1.0f;
	light[0].specular_color[0] = 0.3f; light[0].specular_color[1] = 0.3f; light[0].specular_color[2] = 0.3f; light[0].specular_color[3] = 1.0f;
	light[0].spot_direction[0] = 0.0f; light[0].spot_direction[1] = 0.0f; light[0].spot_direction[2] = -1.0f;
	light[0].spot_exponent = 27.0f;
	light[0].spot_cutoff_angle = 20.0f;
	light[0].light_attenuation_factors[0] = 1.0f; light[0].light_attenuation_factors[1] = 0.0f; light[0].light_attenuation_factors[2] = 0.0f; light[0].light_attenuation_factors[3] = 1.0f;

	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);
	glUniform3fv(loc_light[0].spot_direction, 1, light[0].spot_direction);
	glUniform1f(loc_light[0].spot_cutoff_angle, light[0].spot_cutoff_angle);
	glUniform1f(loc_light[0].spot_exponent, light[0].spot_exponent);
	glUniform4fv(loc_light[0].light_attenuation_factors, 1, light[0].light_attenuation_factors);

	for (int i = 1; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		position_EC = ViewMatrix[i] * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
		direction_EC = glm::mat3(ViewMatrix[i]) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);

		glUniform1i(loc_light[i].light_on, light[i].light_on);
		glUniform4fv(loc_light[i].position, 1, &position_EC[0]);
		glUniform4fv(loc_light[i].ambient_color, 1, light[i].ambient_color);
		glUniform4fv(loc_light[i].diffuse_color, 1, light[i].diffuse_color);
		glUniform4fv(loc_light[i].specular_color, 1, light[i].specular_color);
		glUniform3fv(loc_light[i].spot_direction, 1, &direction_EC[0]);
		glUniform1f(loc_light[i].spot_cutoff_angle, light[i].spot_cutoff_angle);
		glUniform1f(loc_light[i].spot_exponent, light[i].spot_exponent);
		glUniform4fv(loc_light[i].light_attenuation_factors, 1, light[i].light_attenuation_factors);
	}

	glUseProgram(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////

void prepare_scene(void) {
	define_axes();
	define_static_objects();
	define_animated_tiger();
//	define_viewingvolume();
	set_up_scene_lights();
	prepare_floor();
}

void initialize_camera(void) {
	// called only once when the OpenGL system is initialized!!!
	// only ViewMatrix[*] are set up in this function.
	// ProjectionMatrix will be initialized in the reshape callback function when the window pops up.

	// Camera 0
	camera[0].pos = glm::vec3(25.0f, 0.5f, 0.0f);
	camera[0].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[0].vaxis = glm::vec3(0.0f, 1.0f, 0.0f);
	camera[0].naxis = glm::vec3(1.0f, 0.0f, 0.0f);

	camera[0].move_status = 0;
//	camera[0].fov_y = 30.0f;//이걸로 하면 더 작게보임(더 멀리 보임)
	camera[0].fov_y = 15.0f;
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[0].near_clip = 0.01f;
	camera[0].far_clip = 1000.0f;

	ViewMatrix[0] = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	//Camera 1
	camera[1].pos = glm::vec3(50.0f, 0.0f, 0.0f);
	camera[1].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[1].vaxis = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera[1].naxis = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[1].move_status = 0;
	camera[1].fov_y = 15.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[1].near_clip = 0.01f;
	camera[1].far_clip = 4000.0f;

	ViewMatrix[1] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
		glm::vec3(-10.0f, 0.0f, 0.0f));

	camera[2].pos = glm::vec3(50.0f, 0.0f, 0.0f);
	camera[2].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[2].vaxis = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera[2].naxis = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[2].move_status = 0;
	camera[2].fov_y = 15.0f;
	camera[2].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[2].near_clip = 0.01f;
	camera[2].far_clip = 4000.0f;

	ViewMatrix[2] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	camera[3].pos = glm::vec3(50.0f, 0.0f, 0.0f);
	camera[3].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[3].vaxis = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera[3].naxis = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[3].move_status = 0;
	camera[3].fov_y = 15.0f;
	camera[3].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[3].near_clip = 0.01f;
	camera[3].far_clip = 4000.0f;

	ViewMatrix[3] = glm::lookAt(glm::vec3(100.0f, 600.0f, 25.0f), glm::vec3(130.0f, 90.0f, 25.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	camera[4].pos = glm::vec3(50.0f, 0.0f, 0.0f);
	camera[4].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[4].vaxis = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera[4].naxis = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[4].move_status = 0;
	camera[4].fov_y = 15.0f;
	camera[4].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[4].near_clip = 0.01f;
	camera[4].far_clip = 4000.0f;
	ViewMatrix[4] = glm::lookAt(glm::vec3(100.0f, 116.0f, 30.0f), glm::vec3(193.0f, 120.0f, 11.0f),
		glm::vec3(0.0f, 0.0f, 3.0f));

	camera[5].pos = glm::vec3(50.0f, 0.0f, 0.0f);
	camera[5].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[5].vaxis = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera[5].naxis = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[5].move_status = 0;
	camera[5].fov_y = 15.0f;
	camera[5].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[5].near_clip = 0.01f;
	camera[5].far_clip = 4000.0f;
	ViewMatrix[5] = glm::lookAt(glm::vec3(80.0f, 130.0f, 40.0f), glm::vec3(105.0f, 140.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -3.0f));
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170 Our_House_GLSL_V_0.5";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'c', 'f', 'd', 'ESC'" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
