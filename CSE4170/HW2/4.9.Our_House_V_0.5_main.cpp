#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

//#include "Object_Definitions.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUMBER_OF_CAMERAS 7
#define CAM_TRANSLATION_SPEED 0.025f
#define CAM_ROTATION_SPEED 0.1f

typedef struct _CAMERA {
	glm::vec3 pos, uaxis, vaxis, naxis;
	float fov_y, aspect_ratio, near_clip, far_clip;
	int move_status;
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int camera_selected;

typedef struct _VIEWPORT {
	int x, y, w, h;
} VIEWPORT;
VIEWPORT viewport[NUMBER_OF_CAMERAS];

typedef enum {
	VIEW_WORLD, VIEW_SQUARE, VIEW_TIGER, VIEW_COW
} VIEW_MODE;

glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ModelViewMatrix[NUMBER_OF_CAMERAS], ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

// Start of callback function definitions

typedef struct _CALLBACK_CONTEXT {
	int left_button_status, rotation_mode_cow, timestamp_cow;
	int prevx, prevy;
	float rotation_angle_cow;
} CALLBACK_CONTEXT;
CALLBACK_CONTEXT cc;
////////////////////////////////////////////////////////////////////////////////////////////////////////

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	glLineWidth(2.0f);
//	draw_axes();
	glLineWidth(1.0f);
 
    draw_static_object(&(static_objects[OBJ_BUILDING]), 0, 1200,800);

	draw_static_object(&(static_objects[OBJ_TABLE]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_TABLE]), 1, 1200, 800);

	draw_static_object(&(static_objects[OBJ_LIGHT]), 0, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 1, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 2, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 3, 1200, 800);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 4, 1200, 800);

	draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, 1200, 800);
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
		ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		now_cam_u = cam_u_0; now_cam_v = cam_v_0; now_cam_n = cam_n_0;
		now_eye_u = eye_u_0; now_eye_v = eye_v_0; now_eye_n = eye_n_0;
		now_up_u = up_u_0; now_up_v = up_v_0; now_up_n = up_n_0;
		glutPostRedisplay();
		break;
	case '1'://첫 화면을 위에서 보기(상면도)
		init_vars();
		ViewMatrix[0] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f));
		ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		now_cam_u = 120.0f; now_cam_v = 90.0f; now_cam_n = 1000.0f;
		now_eye_u = 120.0f; now_eye_v = 90.0f; now_eye_n = 0.0f;
		now_up_u = -10.0f; now_up_v = 0.0f; now_up_n = 0.0f;
		glutPostRedisplay();
		break;
	case '2'://첫 화면을 옆에서 보기(측면도)
		init_vars();
		ViewMatrix[0] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '4':// 현재 화면에서 카메라를 U축 음의 방향으로 이동
		if (not_move_flag != 1 && cam_u - 1 > -600)
		{
			cam_u -= 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(-1.0f, 0.0f, 0.0f));
			now_cam_u += cam_u;
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '5':// 현재 화면에서 카메라를 V축 양의 방향으로 이동
		if (not_move_flag != 1 && cam_v + 1 <= 600)
		{
			cam_v += 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 1.0f, 0.0f));
			now_cam_v += cam_v;
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '6':// 현재 화면에서 카메라를 V축 음의 방향으로 이동
		if (not_move_flag != 1 && cam_v - 1 >= -600)
		{
			cam_v -= 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, -1.0f, 0.0f));
			now_cam_v += cam_v;
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case '7':// 현재 화면에서 카메라를 N축 양의 방향으로 이동
		if (not_move_flag != 1 && cam_n + 1 <= 200)
		{
			cam_n += 1;
			ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ModelViewMatrix[5] = ProjectionMatrix[5] * ViewMatrix[5];
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
			ModelViewMatrix[5] = ProjectionMatrix[5] * ViewMatrix[5];
			glutPostRedisplay();
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}

void reshape(int width, int height) {
	camera[0].aspect_ratio = (float)width / height;

	ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
	ModelViewMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];

	camera[1].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[1] = glm::perspective(camera[1].fov_y*TO_RADIAN, camera[1].aspect_ratio, camera[1].near_clip, camera[1].far_clip);
	ModelViewMatrix[1] = ProjectionMatrix[1] * ViewMatrix[1];

	camera[2].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[2] = glm::perspective(camera[2].fov_y*TO_RADIAN, camera[2].aspect_ratio, camera[2].near_clip, camera[2].far_clip);
	ModelViewMatrix[2] = ProjectionMatrix[2] * ViewMatrix[2];

	camera[3].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[3] = glm::perspective(camera[3].fov_y*TO_RADIAN, camera[3].aspect_ratio, camera[3].near_clip, camera[3].far_clip);
	ModelViewMatrix[3] = ProjectionMatrix[3] * ViewMatrix[3];

	camera[4].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[4] = glm::perspective(camera[4].fov_y*TO_RADIAN, camera[4].aspect_ratio, camera[4].near_clip, camera[4].far_clip);
	ModelViewMatrix[4] = ProjectionMatrix[4] * ViewMatrix[4];
	glutPostRedisplay(); ///정적인 cctv ;; 주전자와 소를 바라보고 있으며 이 상태에서는 카메라 이동,회전,줌인아웃 불가

	camera[5].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	ProjectionMatrix[5] = glm::perspective(camera[5].fov_y*TO_RADIAN, camera[5].aspect_ratio, camera[5].near_clip, camera[5].far_clip);
	ModelViewMatrix[5] = ProjectionMatrix[5] * ViewMatrix[5];
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

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	initialize_camera();

	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
}

void prepare_scene(void) {
	define_axes();
	define_static_objects();
	define_animated_tiger();
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
//	initialize_camera();
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
