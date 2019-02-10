#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 200.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 200.0f;
	//axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	//axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f; 
	//axes[2][1] = -win_height / 2.25f;
	//axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 1 , 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

#define AIRPLANE_ROTATION_RADIUS 200.0f
int airplane_clock = 0;
int airplane_clock_time = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2)+sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//shirt
/*#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 }, { -8.0, -9.0 }, { -11.0, 8.0 }, { -6.0, 10.0 }, { -3.0, 7.0 }, { 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 }, { 0.0, 9.0 }, { 3.0, 7.0 }, { 6.0, 10.0 }, { 11.0, 8.0 }, { 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 }, { -3.0, 7.0 }, { -6.0, 10.0 }, { -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 }, { 4.0, 11.0 }, { 6.0, 10.0 }, { 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 }, { 4.0, 1.0 }, { 4.0, 3.0 }, { 7.0, 3.0 }, { 7.0, 1.0 }, { 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 }, { 1.0, 6.0 }, { 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 }, { 1.0, 3.0 }, { 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 }, { 1.0, -3.0 }, { 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2)+sizeof(button3)+sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2)+sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}*/

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 }, { 0.0, 12.0 }, { 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 }, { -12.0, 0.0 }, { 12.0, 0.0 }, { 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 }, { 6.0, 14.0 }, { 10.0, 14.0 }, { 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 }, { -8.0, -8.0 }, { -4.0, -8.0 }, { -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 }, { 4.0, -2.0 }, { 8.0, -2.0 }, { 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof)+sizeof(house_body)+sizeof(chimney)+sizeof(door)
		+sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body)+sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body)+sizeof(chimney)+sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 }, { -16.0, 0.0 }, { 16.0, 0.0 }, { 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 }, { -10.0, 10.0 }, { 10.0, 10.0 }, { 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 }, { -8.0, 8.0 }, { 8.0, 8.0 }, { 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 }, { -10.0, -5.0 }, { -9.0, -4.0 }, { -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 }, { 8.0, -5.0 }, { 9.0, -4.0 }, { 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 }, { -10.0, -8.0 }, { -6.0, -8.0 }, { -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 }, { 6.0, -8.0 }, { 10.0, -8.0 }, { 10.0, -12.0 } };

GLfloat car_color0[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLfloat car_color1[7][3] = {
	{ 200 / 255.0f, 120 / 255.0f, 70 / 255.0f },
	{ 200 / 255.0f, 120 / 255.0f, 70 / 255.0f },
	{ 10/ 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 120/ 255.0f, 79 / 255.0f, 220/ 255.0f },
	{ 120 / 255.0f, 79 / 255.0f, 220 / 255.0f },
	{ 180 / 255.0f, 220 / 255.0f, 69 / 255.0f },
	{ 180 / 255.0f, 220 / 255.0f, 69 / 255.0f }
};

GLfloat car_color2[7][3] = {
	{ 99 / 255.0f, 100 / 255.0f, 101 / 255.0f },
	{ 99 / 255.0f, 100 / 255.0f, 101 / 255.0f },
	{ 200 / 255.0f, 120/ 255.0f, 180 / 255.0f },
	{ 46 / 255.0f, 97 / 255.0f, 104 / 255.0f },
	{ 46 / 255.0f, 97 / 255.0f, 104 / 255.0f },
	{ 131 / 255.0f, 230 / 255.0f, 40 / 255.0f },
	{ 131 / 255.0f, 230 / 255.0f, 40 / 255.0f }
};

GLfloat car_color3[7][3] = {
	{ 234 / 255.0f, 189 / 255.0f, 55 / 255.0f },
	{ 234 / 255.0f, 189 / 255.0f, 55 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 49 / 255.0f, 24 / 255.0f, 171 / 255.0f },
	{ 49 / 255.0f, 24 / 255.0f, 171 / 255.0f },
	{ 24 / 255.0f, 222 / 255.0f, 26 / 255.0f },
	{ 24 / 255.0f, 222 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
#define CAR_ROTATION_RADIUS 200.0f
int car_clock = 0;
float car_s_factor = 1.0f;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light)+sizeof(car_left_wheel)+sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light)+sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	if (car_clock % 4 == 0)
	{
		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_FRAME]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_WINDOW]);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_LEFT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_RIGHT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_LEFT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

		glUniform3fv(loc_primitive_color, 1, car_color0[CAR_RIGHT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	}
	else if (car_clock % 4 == 1)
	{
		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_FRAME]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_WINDOW]);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_LEFT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_RIGHT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_LEFT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

		glUniform3fv(loc_primitive_color, 1, car_color1[CAR_RIGHT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	}
	else if (car_clock % 4 == 2)
	{
		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_FRAME]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_WINDOW]);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_LEFT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_RIGHT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_LEFT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

		glUniform3fv(loc_primitive_color, 1, car_color2[CAR_RIGHT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	}
	else if (car_clock % 4 == 3)
	{
		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_FRAME]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_WINDOW]);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_LEFT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_RIGHT_LIGHT]);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_LEFT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

		glUniform3fv(loc_primitive_color, 1, car_color3[CAR_RIGHT_WHEEL]);
		glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	}
	glBindVertexArray(0);
}

//draw cocktail
/*#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 }, { -6.0, -11.0 }, { -1.0, 0.0 }, { 1.0, 0.0 }, { 6.0, -11.0 }, { 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 }, { -9.0, 4.0 }, { -12.0, 7.0 }, { 12.0, 7.0 }, { 9.0, 4.0 }, { 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 }, { -12.0, 10.0 }, { 12.0, 10.0 }, { 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 }, { 12.0, 12.0 }, { 14.0, 12.0 }, { 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 }, { 10.0, 14.0 }, { 10.0, 16.0 }, { 12.0, 18.0 }, { 14.0, 18.0 }, { 16.0, 16.0 }, { 16.0, 14.0 }, { 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck)+sizeof(liquid)+sizeof(remain)+sizeof(straw)
		+sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck)+sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck)+sizeof(liquid)+sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck)+sizeof(liquid)+sizeof(remain)+sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}*/

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 }, { -18.0, 0.0 }, { -13.0, 0.0 }, { -10.0, 8.0 }, { 10.0, 8.0 }, { 13.0, 0.0 }, { 18.0, 0.0 }, { 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 }, { -8.0, 6.0 }, { -2.0, 6.0 }, { -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 }, { 0.0, 6.0 }, { 8.0, 6.0 }, { 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 }, { -13.0, -8.0 }, { -13.0, -7.0 }, { -11.0, -4.0 }, { -7.0, -4.0 }, { -5.0, -7.0 }, { -5.0, -8.0 }, { -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 }, { 5.0, -8.0 }, { 5.0, -7.0 }, { 7.0, -4.0 }, { 11.0, -4.0 }, { 13.0, -7.0 }, { 13.0, -8.0 }, { 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 }, { -17.0, -2.0 }, { -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 }, { -17.0, -5.0 }, { -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel)+sizeof(car2_light1)+sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel)+sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

float car2_clock = 0;
float car2_speed = 0.3;
int return_car2_flag = 0;
float car2_movX, car2_movY;
#define CAR2_ROTATION_RADIUS 200.0f
void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

//draw my FIRE
#define FIRE_BODY 0
#define FIRE_RED1 1
#define FIRE_RED2 2
#define FIRE_RED3 3
#define FIRE_RED4 4
#define FIRE_ORG1 5
#define FIRE_ORG2 6
#define FIRE_ORG3 7
#define FIRE_ORG4 8
#define FIRE_CENTER 9

GLfloat fire_body[4][2] = { { -4.0, -10.0 },{ -4.0, -4.0 },{ 4.0, -4.0 },{ 4.0, -10.0 }};
GLfloat fire_red1[3][2] = { { -4.0, -4.0 },{ 0.0, -2.0 },{ 4.0, -4.0 }};
GLfloat fire_red2[3][2] = { { 4.0, -4.0 },{ 6.0, -7.0 },{ 4.0, -10.0 }};
GLfloat fire_red3[3][2] = { { 4.0, -10.0 },{ 0.0, -12.0 },{ -4.0, -10.0 }};
GLfloat fire_red4[3][2] = { { -4.0, -10.0 },{ -6.0, -7.0 },{ -4.0, -4.0 }};
GLfloat fire_org1[3][2] = { { 0.0, -4.0 },{ 6.0, -1.0 },{ 4.0, -7.0 } };
GLfloat fire_org2[3][2] = { { 4.0, -7.0 },{ 6.0, -13.0 },{ 0.0, -10.0 } };
GLfloat fire_org3[3][2] = { { 0.0, -10.0 },{ -6.0, -13.0 },{ -4.0, -7.0 } };
GLfloat fire_org4[3][2] = { { -4.0, -7.0 },{ -6.0, -1.0 },{ 0.0, -4.0 } };
GLfloat fire_center[1][2] = { { 0.0, -7.0 }};


GLfloat fire_color_before[10][3] = {
	{ 230 / 255.0f, 200 / 255.0f, 200 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 190 / 255.0f, 150 / 255.0f, 0 / 255.0f },
	{ 190 / 255.0f, 150 / 255.0f, 0 / 255.0f },
	{ 190 / 255.0f, 150 / 255.0f, 0 / 255.0f },
	{ 190 / 255.0f, 150 / 255.0f, 0 / 255.0f },
	{ 230 / 255.0f, 200 / 255.0f, 200 / 255.0f }
};

GLfloat fire_color_after[10][3] = {
	{ 255 / 255.0f, 100 / 255.0f, 100 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 250 / 255.0f, 50 / 255.0f, 30 / 255.0f },
	{ 190 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 190 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 190 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 190 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f }
};

GLuint VBO_fire, VAO_fire;
void prepare_fire() {
	GLsizeiptr buffer_size = sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3)
		+ sizeof(fire_red4) + sizeof(fire_org1) + sizeof(fire_org2) + sizeof(fire_org3) + sizeof(fire_org4) + sizeof(fire_center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_fire);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_fire);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fire_body), fire_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body), sizeof(fire_red1), fire_red1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1), sizeof(fire_red2), fire_red2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2), sizeof(fire_red3), fire_red3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3),
		sizeof(fire_red4), fire_red4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3)
		+ sizeof(fire_red4), sizeof(fire_org1), fire_org1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3)
		+ sizeof(fire_red4) + sizeof(fire_org1), sizeof(fire_org2), fire_org2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3)
		+ sizeof(fire_red4) + sizeof(fire_org1) + sizeof(fire_org2), sizeof(fire_org3), fire_org3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3)
		+ sizeof(fire_red4) + sizeof(fire_org1) + sizeof(fire_org2) + sizeof(fire_org3), sizeof(fire_org4), fire_org4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_body) + sizeof(fire_red1) + sizeof(fire_red2) + sizeof(fire_red3)
		+ sizeof(fire_red4) + sizeof(fire_org1) + sizeof(fire_org2) + sizeof(fire_org3) + sizeof(fire_org4), sizeof(fire_center), fire_center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_fire);
	glBindVertexArray(VAO_fire);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_fire);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int fire_start = 0;
int fire_clock = 0;
#define FIRE_ROTATION_RADIUS 150.0f
void draw_fire() {
	glBindVertexArray(VAO_fire);

	if (fire_start == 0)
	{
		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_RED1]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_RED2]);
		glDrawArrays(GL_TRIANGLE_FAN, 7, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_RED3]);
		glDrawArrays(GL_TRIANGLE_FAN, 10, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_RED4]);
		glDrawArrays(GL_TRIANGLE_FAN, 13, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_ORG1]);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_ORG2]);
		glDrawArrays(GL_TRIANGLE_FAN, 19, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_ORG3]);
		glDrawArrays(GL_TRIANGLE_FAN, 22, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_ORG4]);
		glDrawArrays(GL_TRIANGLE_FAN, 25, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_before[FIRE_CENTER]);
		glDrawArrays(GL_TRIANGLE_FAN, 28, 1);
	}
	else if (fire_start == 1)
	{
		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_RED1]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_RED2]);
		glDrawArrays(GL_TRIANGLE_FAN, 7, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_RED3]);
		glDrawArrays(GL_TRIANGLE_FAN, 10, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_RED4]);
		glDrawArrays(GL_TRIANGLE_FAN, 13, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_ORG1]);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_ORG2]);
		glDrawArrays(GL_TRIANGLE_FAN, 19, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_ORG3]);
		glDrawArrays(GL_TRIANGLE_FAN, 22, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_ORG4]);
		glDrawArrays(GL_TRIANGLE_FAN, 25, 3);

		glUniform3fv(loc_primitive_color, 1, fire_color_after[FIRE_CENTER]);
		glDrawArrays(GL_TRIANGLE_FAN, 25, 3);
	}
	glBindVertexArray(0);
}

//draw my DOG
#define DOG_FACE 0
#define DOG_EAR1 1
#define DOG_EAR2 2
#define DOG_EYE1 3
#define DOG_EYE2 4
#define DOG_NOSE 5
#define DOG_MOUTH1 6
#define DOG_MOUTH2 7
#define DOG_MOUTH3 8

GLfloat dog_face[4][2] = { { 30.0, 10.0 },{ 30.0, 40.0 },{ 50.0, 40.0 },{ 50.0, 10.0 } };
GLfloat dog_ear1[3][2] = { { 22.0, 28.0 },{ 22.0, 40.0 },{ 30.0, 40.0 } };
GLfloat dog_ear2[3][2] = { { 50.0, 40.0 },{ 58.0, 40.0 },{ 58.0, 28.0 } };
GLfloat dog_eye1[4][2] = { { 34.0, 32.0 },{ 34.0, 36.0 },{ 36.0, 36.0 },{ 36.0, 32.0 } };
GLfloat dog_eye2[4][2] = { { 44.0, 32.0 },{ 44.0, 36.0 },{ 46.0, 36.0 },{ 46.0, 32.0 } };
GLfloat dog_nose[3][2] = { { 40.0, 25.0 },{ 37.0, 28.0 },{ 43.0, 28.0 } };
GLfloat dog_mouth1[2][2] = { { 33.0, 22.0 },{ 33.0, 26.0 }};
GLfloat dog_mouth2[2][2] = { { 33.0, 22.0 },{ 41.0, 22.0 }};
GLfloat dog_mouth3[2][2] = { { 41.0, 22.0 },{ 47.0, 26.0 }};

GLfloat dog_color[9][3] = {
	{ 200 / 255.0f, 100 / 255.0f, 0 / 255.0f },
	{ 150 / 255.0f, 150 / 255.0f, 0 / 255.0f },
	{ 150 / 255.0f, 150 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_dog, VAO_dog;
void prepare_dog() {
	GLsizeiptr buffer_size = sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2) + sizeof(dog_eye1)
		+ sizeof(dog_eye2) + sizeof(dog_nose) + sizeof(dog_mouth1) + sizeof(dog_mouth2) + sizeof(dog_mouth3);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_dog);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_dog);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(dog_face), dog_face);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face), sizeof(dog_ear1), dog_ear1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1), sizeof(dog_ear2), dog_ear2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2), sizeof(dog_eye1), dog_eye1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2) + sizeof(dog_eye1),
		sizeof(dog_eye2), dog_eye2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2) + sizeof(dog_eye1)
		+ sizeof(dog_eye2), sizeof(dog_nose), dog_nose);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2) + sizeof(dog_eye1)
		+ sizeof(dog_eye2) + sizeof(dog_nose), sizeof(dog_mouth1), dog_mouth1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2) + sizeof(dog_eye1)
		+ sizeof(dog_eye2) + sizeof(dog_nose) + sizeof(dog_mouth1), sizeof(dog_mouth2), dog_mouth2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(dog_face) + sizeof(dog_ear1) + sizeof(dog_ear2) + sizeof(dog_eye1)
		+ sizeof(dog_eye2) + sizeof(dog_nose) + sizeof(dog_mouth1) + sizeof(dog_mouth2), sizeof(dog_mouth3), dog_mouth3);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_dog);
	glBindVertexArray(VAO_dog);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_dog);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int dog_clock = 0;
void draw_dog() {
	glBindVertexArray(VAO_dog);
	
	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_FACE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_EAR1]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 3);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_EAR2]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 3);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_EYE1]);
	glDrawArrays(GL_TRIANGLE_FAN, 10, 4);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_EYE2]);
	glDrawArrays(GL_TRIANGLE_FAN, 14, 4);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_NOSE]);
	glDrawArrays(GL_TRIANGLE_FAN, 18, 3);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_MOUTH1]);
	glDrawArrays(GL_TRIANGLE_FAN, 21, 2);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_MOUTH2]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 2);

	glUniform3fv(loc_primitive_color, 1, dog_color[DOG_MOUTH3]);
	glDrawArrays(GL_TRIANGLE_FAN, 25, 2);

	glBindVertexArray(0);
}

void display(void) {
	int i;
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);
	
	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

/*	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_shirt();*/

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, 100.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(10.0f, 10.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_house();
	
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, -800.0f + dog_clock, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(19.0f, 19.0f, 19.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_dog();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(800.0f-car_clock, -100.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(17.0f, 17.0f, 17.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	draw_car();
	
/*	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	draw_cocktail();*/
	
	if (return_car2_flag == 1)
	{
		car2_movX = 2.73*car2_clock;
		car2_movY = car2_clock;
	}
	else if (return_car2_flag == 0)
	{
		car2_movX = 500.0f - (2.73*car2_clock);
		car2_movY = 210.0f - car2_clock;
	}
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car2_movX, car2_movY, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 20.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 20.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, 100.0f, 100.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(6.0f, 6.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_fire();

	if (airplane_clock <= 360)
	{
		airplane_clock_time = 1;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));
	}
	else
	{
		airplane_clock_time = 3;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -(airplane_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));

		if (airplane_clock <= 540)
			airplane_s_factor = (airplane_clock - 360.0f) / 180.0f + 1.0f;
		else
		{
			airplane_s_factor = (airplane_clock - 540.0f) / 180.0f + 2.0f;
			airplane_clock_time = 0;
			fire_start = 1;
			glClearColor(250 / 255.0f, 150 / 255.0f, 150 / 255.0f, 1.0f);
			for (int i = 0; i < 100; i++)
			{
				if (i % 6 == 1)
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-FIRE_ROTATION_RADIUS, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, fire_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(FIRE_ROTATION_RADIUS, 0.0f, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
				}
				else if (i % 6 == 2)
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(FIRE_ROTATION_RADIUS, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, fire_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-FIRE_ROTATION_RADIUS, 0.0f, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
				}
				else if (i % 6 == 3)
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(FIRE_ROTATION_RADIUS-50, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, fire_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-FIRE_ROTATION_RADIUS+50, 0.0f, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
				}
				else if (i % 6 == 4)
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-FIRE_ROTATION_RADIUS + 50, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, fire_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(FIRE_ROTATION_RADIUS - 50, 0.0f, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
				}
				else if (i % 6 == 5)
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(FIRE_ROTATION_RADIUS - 100, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, fire_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-FIRE_ROTATION_RADIUS + 100, 0.0f, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
				}
				else
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-FIRE_ROTATION_RADIUS + 100, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, fire_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(+FIRE_ROTATION_RADIUS - 100, 0.0f, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
				}
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_fire();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(fire_center[0][0], fire_center[0][1], 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, rotate_angle, glm::vec3(0.0f, 0.0f, 1.0f));


				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_fire();
			}
		}
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_s_factor, airplane_s_factor, 1.0f));
	}

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, rotate_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	glFlush();	
}   

void special(int key, int x, int y)
{
#define SENSITIVITY 2.0
	switch (key)
	{
	case GLUT_KEY_LEFT:
		centerx -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		centerx += SENSITIVITY;

		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		centery -= SENSITIVITY;

		glutPostRedisplay();

		break;
	case GLUT_KEY_UP:
		centery += SENSITIVITY;

		glutPostRedisplay();
		break;
	case GLUT_KEY_F1:
		return_car2_flag = 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F2:
		car2_speed += 0.05;
		glutPostRedisplay();
		break;

	}
}

int leftbuttonpressed = 0;

void mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

void motion(int x, int y)
{
	static int delay = 0;
	static float tmpx = 0.0, tmpy = 0.0;
	float dx, dy;
	if (leftbuttonpressed) {
		centerx = x - win_width / 2.0f, centery = (win_height - y) - win_height / 2.0f;
		if (delay == 8) {
			dx = centerx - tmpx;
			dy = centery - tmpy;

			if (dx > 0.0) {
				rotate_angle = atan(dy / dx) + 90.0f*TO_RADIAN;
			}
			else if (dx < 0.0) {
				rotate_angle = atan(dy / dx) - 90.0f*TO_RADIAN;
			}
			else if (dx == 0.0) {
				if (dy > 0.0) rotate_angle = 180.0f*TO_RADIAN;
				else  rotate_angle = 0.0f;
			}
			tmpx = centerx, tmpy = centery;
			delay = 0;
		}
		glutPostRedisplay();
		delay++;
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void timer(int value)
{
	airplane_clock = (airplane_clock + airplane_clock_time) % 720;
	car_clock = car_clock + 1;
	dog_clock = dog_clock + 1;
	car2_clock = car2_clock + car2_speed;
	fire_clock = (fire_clock + 1) % 270;
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
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
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(250 / 255.0f, 200 / 255.0f, 200 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
//	prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_dog();
//	prepare_cocktail();
	prepare_car2();
	prepare_fire();
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

#define N_MESSAGE_LINES 4
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.1";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
		"    - Keys used: 'F1'  "
		"    - Keys used: 'F2'  "
		"    - Mouse used: L-click and move"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop ();
}


