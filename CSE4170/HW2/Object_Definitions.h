
// The object modelling tasks performed by this file are usually done 
// by reading a scene configuration file or through a help of graphics user interface!!!

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
//////////////////////////////////////////////////////////////////
#define INDEX_VERTEX_POSITION 0

#define NUMBER_OF_POINTS 1
GLuint points_VBO, points_VAO;
GLfloat points_vertices[NUMBER_OF_POINTS][3] = { { 0.0f, 0.0f, 0.0f } };
////////////////////////////////////////////////////////////////////
void prepare_points(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &points_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, points_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_vertices), &points_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &points_VAO);
	glBindVertexArray(points_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, points_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_points(float r, float g, float b) {
	glBindVertexArray(points_VAO);
	glUniform3f(loc_primitive_color, r, g, b);
	glDrawArrays(GL_POINTS, 0, NUMBER_OF_POINTS);
	glBindVertexArray(0);
}////////////////////////////////////////////////////////////////////

typedef struct _material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
} Material;

#define N_MAX_GEOM_COPIES 5
typedef struct _Object {
	char filename[512];

	GLenum front_face_mode; // clockwise or counter-clockwise
	int n_triangles;

	int n_fields; // 3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	GLfloat *vertices; // pointer to vertex array data
	GLfloat xmin, xmax, ymin, ymax, zmin, zmax; // bounding box <- compute this yourself

	GLuint VBO, VAO; // Handles to vertex buffer object and vertex array object

	int n_geom_instances;
	glm::mat4 ModelMatrix[N_MAX_GEOM_COPIES];
	Material material[N_MAX_GEOM_COPIES];
} Object;

#define N_MAX_STATIC_OBJECTS		15
Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
int n_static_objects = 0;

#define OBJ_BUILDING		0
#define OBJ_TABLE			1
#define OBJ_LIGHT			2
#define OBJ_TEAPOT			3
#define OBJ_NEW_CHAIR		4
#define OBJ_FRAME			5
#define OBJ_NEW_PICTURE		6
#define OBJ_COW				7
#define OBJ_TEAPOT2			8
#define OBJ_BIKE			9
#define OBJ_IM				10
#define OBJ_GODZ			11
#define OBJ_BUS				12


int read_geometry(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(*object, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void compute_AABB(Object *obj_ptr)
{
	// Do it yourself.
}
	 
void prepare_geom_of_static_object(Object *obj_ptr) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = obj_ptr->n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	obj_ptr->n_triangles = read_geometry(&(obj_ptr->vertices), n_bytes_per_triangle, obj_ptr->filename);

	// Initialize vertex buffer object.
	glGenBuffers(1, &(obj_ptr->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glBufferData(GL_ARRAY_BUFFER, obj_ptr->n_triangles*n_bytes_per_triangle, obj_ptr->vertices, GL_STATIC_DRAW);

	compute_AABB(obj_ptr);
	free(obj_ptr->vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &(obj_ptr->VAO));
	glBindVertexArray(obj_ptr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void define_static_objects(void) {
	// building
	strcpy(static_objects[OBJ_BUILDING].filename, "Data/Building1_vnt.geom");
	static_objects[OBJ_BUILDING].n_fields = 8;

	static_objects[OBJ_BUILDING].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUILDING]));

	static_objects[OBJ_BUILDING].n_geom_instances = 1;

    static_objects[OBJ_BUILDING].ModelMatrix[0] = glm::mat4(1.0f);
	
	static_objects[OBJ_BUILDING].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].exponent = 128.0f*0.1f;

	// table
	strcpy(static_objects[OBJ_TABLE].filename, "Data/Table_vn.geom");
	static_objects[OBJ_TABLE].n_fields = 6;

	static_objects[OBJ_TABLE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TABLE]));

	static_objects[OBJ_TABLE].n_geom_instances = 2;

	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[0], 
		glm::vec3(0.5f, 0.5f, 0.5f));

	static_objects[OBJ_TABLE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[0].ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[0].diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	static_objects[OBJ_TABLE].material[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[0].exponent = 15.0f;

	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[1],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[1].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[1].diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	static_objects[OBJ_TABLE].material[1].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[1].exponent = 128.0f*0.078125f;

	// Light
	strcpy(static_objects[OBJ_LIGHT].filename, "Data/Light_vn.geom");
	static_objects[OBJ_LIGHT].n_fields = 6;

	static_objects[OBJ_LIGHT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(static_objects + OBJ_LIGHT);

	static_objects[OBJ_LIGHT].n_geom_instances = 5;

	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[3],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[4],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[4].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].exponent = 128.0f*0.4f;

	// teapot
	strcpy(static_objects[OBJ_TEAPOT].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT].n_fields = 6;

	static_objects[OBJ_TEAPOT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT]));

	static_objects[OBJ_TEAPOT].n_geom_instances = 1;

	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[0],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].exponent = 128.0f*0.6;

	////////////////////////////의자 위의 주전자/////////////////////////////////////
	strcpy(static_objects[OBJ_TEAPOT2].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT2].n_fields = 6;

	static_objects[OBJ_TEAPOT2].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT2]));

	static_objects[OBJ_TEAPOT2].n_geom_instances = 1;

	static_objects[OBJ_TEAPOT2].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 105.0f, 7.0f));
	static_objects[OBJ_TEAPOT2].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT2].ModelMatrix[0],
		glm::vec3(1.0f, 1.0f, 1.0f));

	static_objects[OBJ_TEAPOT2].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	static_objects[OBJ_TEAPOT2].material[0].ambient = glm::vec4(0.10745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT2].material[0].diffuse = glm::vec4(0.21424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT2].material[0].specular = glm::vec4(0.127811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT2].material[0].exponent = 208.0f*0.6;
	///////////////////////////////////////////////////////////
	
	///////////////////////////////자전거////////////////////////
	strcpy(static_objects[OBJ_BIKE].filename, "Data/Bike.geom");
	static_objects[OBJ_BIKE].n_fields = 8;

	static_objects[OBJ_BIKE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BIKE]));

	static_objects[OBJ_BIKE].n_geom_instances = 1;

	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 100.0f, 0.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::scale(static_objects[OBJ_BIKE].ModelMatrix[0],
		glm::vec3(7.0f, 7.0f, 7.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BIKE].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_BIKE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BIKE].material[0].ambient = glm::vec4(0.02f, 0.02f, 0.0f, 1.0f);
	static_objects[OBJ_BIKE].material[0].diffuse = glm::vec4(0.25164f, 0.60648f, 0.42648f, 1.0f);
	static_objects[OBJ_BIKE].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_BIKE].material[0].exponent = 278.0f*0.078125f;
	////////////////////////////////////////////////////////////////////

	///////////////////////////////아이언맨;호랑이 길막////////////////////////
	strcpy(static_objects[OBJ_IM].filename, "Data/IronMan.geom");
	static_objects[OBJ_IM].n_fields = 8;

	static_objects[OBJ_IM].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_IM]));

	static_objects[OBJ_IM].n_geom_instances = 1;

	static_objects[OBJ_IM].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 96.0f, 0.0f));
	static_objects[OBJ_IM].ModelMatrix[0] = glm::scale(static_objects[OBJ_IM].ModelMatrix[0],
		glm::vec3(10.0f, 10.0f, 10.0f));
	static_objects[OBJ_IM].ModelMatrix[0] = glm::rotate(static_objects[OBJ_IM].ModelMatrix[0],
		-25.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_IM].ModelMatrix[0] = glm::rotate(static_objects[OBJ_IM].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));


	static_objects[OBJ_IM].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_IM].material[0].ambient = glm::vec4(0.02f, 0.02f, 0.0f, 1.0f);
	static_objects[OBJ_IM].material[0].diffuse = glm::vec4(0.85164f, 0.30648f, 0.02648f, 1.0f);
	static_objects[OBJ_IM].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_IM].material[0].exponent = 222.0f*0.078125f;
	////////////////////////////////////////////////////////////////////

	///////////////////////////////고질라;집밖////////////////////////
	strcpy(static_objects[OBJ_GODZ].filename, "Data/Godzilla.geom");
	static_objects[OBJ_GODZ].n_fields = 8;

	static_objects[OBJ_GODZ].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_GODZ]));

	static_objects[OBJ_GODZ].n_geom_instances = 1;

	static_objects[OBJ_GODZ].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(160.0f, -30.0f, 0.0f));
	static_objects[OBJ_GODZ].ModelMatrix[0] = glm::scale(static_objects[OBJ_GODZ].ModelMatrix[0],
		glm::vec3(0.5f, 0.5f, 0.5f));
	static_objects[OBJ_GODZ].ModelMatrix[0] = glm::rotate(static_objects[OBJ_GODZ].ModelMatrix[0],
		-90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_GODZ].ModelMatrix[0] = glm::rotate(static_objects[OBJ_GODZ].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));


	static_objects[OBJ_GODZ].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_GODZ].material[0].ambient = glm::vec4(0.02f, 0.02f, 0.0f, 1.0f);
	static_objects[OBJ_GODZ].material[0].diffuse = glm::vec4(0.85164f, 0.70648f, 0.75648f, 1.0f);
	static_objects[OBJ_GODZ].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_GODZ].material[0].exponent = 222.0f*0.078125f;
	////////////////////////////////////////////////////////////////////

	///////////////////////////////버스;내부주차////////////////////////
	strcpy(static_objects[OBJ_BUS].filename, "Data/Bus.geom");
	static_objects[OBJ_BUS].n_fields = 8;

	static_objects[OBJ_BUS].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUS]));

	static_objects[OBJ_BUS].n_geom_instances = 1;

	static_objects[OBJ_BUS].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(105.0f, 140.0f, 0.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::scale(static_objects[OBJ_BUS].ModelMatrix[0],
		glm::vec3(2.5f, 2.5f, 2.5f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BUS].ModelMatrix[0],
		-90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BUS].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));


	static_objects[OBJ_BUS].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUS].material[0].ambient = glm::vec4(0.02f, 0.02f, 0.0f, 1.0f);
	static_objects[OBJ_BUS].material[0].diffuse = glm::vec4(0.65164f, 0.50648f, 0.95648f, 1.0f);
	static_objects[OBJ_BUS].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_BUS].material[0].exponent = 222.0f*0.078125f;
	////////////////////////////////////////////////////////////////////

	// new_chair
	strcpy(static_objects[OBJ_NEW_CHAIR].filename, "Data/new_chair_vnt.geom");
	static_objects[OBJ_NEW_CHAIR].n_fields = 8;

	static_objects[OBJ_NEW_CHAIR].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_CHAIR]));

	static_objects[OBJ_NEW_CHAIR].n_geom_instances = 1;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].exponent = 128.0f*0.078125f;

	// frame
	strcpy(static_objects[OBJ_FRAME].filename, "Data/Frame_vn.geom");
	static_objects[OBJ_FRAME].n_fields = 6;

	static_objects[OBJ_FRAME].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_FRAME]));

	static_objects[OBJ_FRAME].n_geom_instances = 1;

	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[0],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[0].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[0].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[0].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[0].exponent = 128.0f*0.1f;


	// new_picture
	strcpy(static_objects[OBJ_NEW_PICTURE].filename, "Data/new_picture_vnt.geom");
	static_objects[OBJ_NEW_PICTURE].n_fields = 8;

	static_objects[OBJ_NEW_PICTURE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_PICTURE]));

	static_objects[OBJ_NEW_PICTURE].n_geom_instances = 1;

	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].exponent = 128.0f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_COW].filename, "Data/cow_vn.geom");
	static_objects[OBJ_COW].n_fields = 6;

	static_objects[OBJ_COW].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_COW]));

	static_objects[OBJ_COW].n_geom_instances = 1;

	static_objects[OBJ_COW].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::scale(static_objects[OBJ_COW].ModelMatrix[0],
		glm::vec3(30.0f, 30.0f, 30.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
 
	static_objects[OBJ_COW].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_COW].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_COW].material[0].exponent = 0.21794872f*0.6f;

	n_static_objects = 8;
}

void draw_static_object(Object *obj_ptr, int instance_ID,int width, int height) {
	glFrontFace(obj_ptr->front_face_mode);

	glViewport(0, 0, 0.70f*width, 0.60f*height);
	ModelViewMatrix[0] = ViewMatrix[0] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[0] * ModelViewMatrix[0];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);

	glViewport(0.70f*width, 0, 0.30f*width, 0.30*height);
	ModelViewMatrix[1] = ViewMatrix[1] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[1] * ModelViewMatrix[1];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);

	glViewport(0.70f*width, 0.30f*height, 0.30f*width, 0.30*height);
	ModelViewMatrix[2] = ViewMatrix[2] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[2] * ModelViewMatrix[2];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);

	glViewport(0.70f*width, 0.60f*height, 0.30f*width, 0.30*height);
	ModelViewMatrix[3] = ViewMatrix[3] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[3] * ModelViewMatrix[3];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);

	glViewport(0, 0.60f*height, 0.30f*width, 0.30*height);
	ModelViewMatrix[4] = ViewMatrix[4] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[4] * ModelViewMatrix[4];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);

	glViewport(0.40f*width, 0.60f*height, 0.30f*width, 0.30f*height);
	ModelViewMatrix[5] = ViewMatrix[5] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[5] * ModelViewMatrix[5];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);
}

GLuint VBO_axes, VAO_axes;
GLfloat vertices_axes[6][3] = {
	{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

void define_axes(void) {  
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define WC_AXIS_LENGTH		60.0f
void draw_axes(int num) {
	ModelViewMatrix[num] = glm::scale(ViewMatrix[num], glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	ModelViewProjectionMatrix = ProjectionMatrix[num] * ModelViewMatrix[num];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

#define N_TIGER_FRAMES 12
Object tiger[N_TIGER_FRAMES];
struct {
	int cur_frame = 0;
	float rotation_angle = 0.0f;
	int go = -20;
	int stop = 0;
} tiger_data;

void define_animated_tiger(void) {
	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(tiger[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);

		tiger[i].n_fields = 8;
		tiger[i].front_face_mode = GL_CW;
		prepare_geom_of_static_object(&(tiger[i]));

		tiger[i].n_geom_instances = 1;

		tiger[i].ModelMatrix[0] = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		tiger[i].ModelMatrix[1] = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		tiger[i].ModelMatrix[2] = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		tiger[i].ModelMatrix[3] = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

		tiger[i].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		tiger[i].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		tiger[i].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		tiger[i].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		tiger[i].material[0].exponent = 128.0f*0.21794872f;
	}
}

void draw_animated_tiger(int width, int height) {
	
	glViewport(0, 0, 0.70f*width, 0.60f*height);	
	tiger_data.stop = 0;

	if (tiger_data.go <= -150)
	{
		tiger_data.stop = 1;
		ModelViewMatrix[0] = glm::rotate(ViewMatrix[0], -180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[0] = glm::translate(ModelViewMatrix[0], glm::vec3(-105.0f, 145.0f, 0.0f));

	}
	else if (tiger_data.go <= -100 && tiger_data.go > -150)
	{
		ModelViewMatrix[0] = glm::rotate(ViewMatrix[0], -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[0] = glm::translate(ModelViewMatrix[0], glm::vec3(-100.0f, 200.0f, 0.0f));
	}
	else if (tiger_data.go <= -60 && tiger_data.go > -100)
	{
		ModelViewMatrix[0] = glm::rotate(ViewMatrix[0], 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[0] = glm::translate(ModelViewMatrix[0], glm::vec3(0.0f, 150.0f, 0.0f));
	}
	else
		ModelViewMatrix[0] = glm::rotate(ViewMatrix[0], 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[0] = glm::translate(ModelViewMatrix[0], glm::vec3(0.0f, tiger_data.go, 0.0f));
	ModelViewMatrix[0] = glm::translate(ModelViewMatrix[0], glm::vec3(80.0f, -20.0f, 0.0f));
	ModelViewMatrix[0] *= tiger[tiger_data.cur_frame].ModelMatrix[0];

	ModelViewProjectionMatrix = ProjectionMatrix[0] * ModelViewMatrix[0];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(0);
	////////////////////////////////
	glViewport(0.70f*width, 0, 0.30f*width, 0.30*height);
	tiger_data.stop = 0;

	if (tiger_data.go <= -150)
	{
		tiger_data.stop = 1;
		ModelViewMatrix[1] = glm::rotate(ViewMatrix[1], -180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[1] = glm::translate(ModelViewMatrix[1], glm::vec3(-105.0f, 145.0f, 0.0f));

	}
	else if (tiger_data.go <= -100 && tiger_data.go > -150)
	{
		ModelViewMatrix[1] = glm::rotate(ViewMatrix[1], -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[1] = glm::translate(ModelViewMatrix[1], glm::vec3(-100.0f, 200.0f, 0.0f));
	}
	else if (tiger_data.go <= -60 && tiger_data.go > -100)
	{
		ModelViewMatrix[1] = glm::rotate(ViewMatrix[1], 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[1] = glm::translate(ModelViewMatrix[1], glm::vec3(0.0f, 150.0f, 0.0f));
	}
	else
		ModelViewMatrix[1] = glm::rotate(ViewMatrix[1], 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[1] = glm::translate(ModelViewMatrix[1], glm::vec3(0.0f, tiger_data.go, 0.0f));
	ModelViewMatrix[1] = glm::translate(ModelViewMatrix[1], glm::vec3(80.0f, -20.0f, 0.0f));
	ModelViewMatrix[1] *= tiger[tiger_data.cur_frame].ModelMatrix[1];

	ModelViewProjectionMatrix = ProjectionMatrix[1] * ModelViewMatrix[1];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(1);
	///////////////////////////////////////////////////////////
	glViewport(0.70f*width, 0.30f*height, 0.30f*width, 0.30*height);
	tiger_data.stop = 0;

	if (tiger_data.go <= -150)
	{
		tiger_data.stop = 1;
		ModelViewMatrix[2] = glm::rotate(ViewMatrix[2], -180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[2] = glm::translate(ModelViewMatrix[2], glm::vec3(-105.0f, 145.0f, 0.0f));

	}
	else if (tiger_data.go <= -100 && tiger_data.go > -150)
	{
		ModelViewMatrix[2] = glm::rotate(ViewMatrix[2], -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[2] = glm::translate(ModelViewMatrix[2], glm::vec3(-100.0f, 200.0f, 0.0f));
	}
	else if (tiger_data.go <= -60 && tiger_data.go > -100)
	{
		ModelViewMatrix[2] = glm::rotate(ViewMatrix[2], 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[2] = glm::translate(ModelViewMatrix[2], glm::vec3(0.0f, 150.0f, 0.0f));
	}
	else
		ModelViewMatrix[2] = glm::rotate(ViewMatrix[2], 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[2] = glm::translate(ModelViewMatrix[2], glm::vec3(0.0f, tiger_data.go, 0.0f));
	ModelViewMatrix[2] = glm::translate(ModelViewMatrix[2], glm::vec3(80.0f, -20.0f, 0.0f));
	ModelViewMatrix[2] *= tiger[tiger_data.cur_frame].ModelMatrix[2];

	ModelViewProjectionMatrix = ProjectionMatrix[2] * ModelViewMatrix[2];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(2);
////////////////////////////////////////////////////////////////////////
	glViewport(0.70f*width, 0.60f*height, 0.30f*width, 0.30*height);
	tiger_data.stop = 0;

	if (tiger_data.go <= -150)
	{
		tiger_data.stop = 1;
		ModelViewMatrix[3] = glm::rotate(ViewMatrix[3], -180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[3] = glm::translate(ModelViewMatrix[3], glm::vec3(-105.0f, 145.0f, 0.0f));

	}
	else if (tiger_data.go <= -100 && tiger_data.go > -150)
	{
		ModelViewMatrix[3] = glm::rotate(ViewMatrix[3], -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[3] = glm::translate(ModelViewMatrix[3], glm::vec3(-100.0f, 200.0f, 0.0f));
	}
	else if (tiger_data.go <= -60 && tiger_data.go > -100)
	{
		ModelViewMatrix[3] = glm::rotate(ViewMatrix[3], 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[3] = glm::translate(ModelViewMatrix[3], glm::vec3(0.0f, 150.0f, 0.0f));
	}
	else
		ModelViewMatrix[3] = glm::rotate(ViewMatrix[3], 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[3] = glm::translate(ModelViewMatrix[3], glm::vec3(0.0f, tiger_data.go, 0.0f));
	ModelViewMatrix[3] = glm::translate(ModelViewMatrix[3], glm::vec3(80.0f, -20.0f, 0.0f));
	ModelViewMatrix[3] *= tiger[tiger_data.cur_frame].ModelMatrix[3];

	ModelViewProjectionMatrix = ProjectionMatrix[3] * ModelViewMatrix[3];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(3);
	///////////////////////////////////////////////////////////////////////////
/*	glViewport(0, 0.60f*height, 0.30f*width, 0.30f*height);
	tiger_data.stop = 0;

	if (tiger_data.go <= -150)
	{
		tiger_data.stop = 1;
		ModelViewMatrix[4] = glm::rotate(ViewMatrix[4], -180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[4] = glm::translate(ModelViewMatrix[4], glm::vec3(-105.0f, 145.0f, 0.0f));

	}
	else if (tiger_data.go <= -100 && tiger_data.go > -150)
	{
		ModelViewMatrix[4] = glm::rotate(ViewMatrix[4], -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[4] = glm::translate(ModelViewMatrix[4], glm::vec3(-100.0f, 200.0f, 0.0f));
	}
	else if (tiger_data.go <= -60 && tiger_data.go > -100)
	{
		ModelViewMatrix[4] = glm::rotate(ViewMatrix[4], 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[4] = glm::translate(ModelViewMatrix[4], glm::vec3(0.0f, 150.0f, 0.0f));
	}
	else
		ModelViewMatrix[4] = glm::rotate(ViewMatrix[4], 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[4] = glm::translate(ModelViewMatrix[4], glm::vec3(0.0f, tiger_data.go, 0.0f));
	ModelViewMatrix[4] = glm::translate(ModelViewMatrix[4], glm::vec3(80.0f, -20.0f, 0.0f));
	ModelViewMatrix[4] *= tiger[tiger_data.cur_frame].ModelMatrix[4];

	ModelViewProjectionMatrix = ProjectionMatrix[4] * ModelViewMatrix[4];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(4);*////4번은 정적 cctv라서 호랑이 움직임 필요X

	glViewport(0.40f*width, 0.60f*height, 0.30f*width, 0.30f*height);
	tiger_data.stop = 0;

	if (tiger_data.go <= -150)
	{
		tiger_data.stop = 1;
		ModelViewMatrix[5] = glm::rotate(ViewMatrix[5], -180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[5] = glm::translate(ModelViewMatrix[5], glm::vec3(-105.0f, 145.0f, 0.0f));

	}
	else if (tiger_data.go <= -100 && tiger_data.go > -150)
	{
		ModelViewMatrix[5] = glm::rotate(ViewMatrix[5], -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[5] = glm::translate(ModelViewMatrix[5], glm::vec3(-100.0f, 200.0f, 0.0f));
	}
	else if (tiger_data.go <= -60 && tiger_data.go > -100)
	{
		ModelViewMatrix[5] = glm::rotate(ViewMatrix[5], 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix[5] = glm::translate(ModelViewMatrix[5], glm::vec3(0.0f, 150.0f, 0.0f));
	}
	else
		ModelViewMatrix[5] = glm::rotate(ViewMatrix[5], 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[5] = glm::translate(ModelViewMatrix[5], glm::vec3(0.0f, tiger_data.go, 0.0f));
	ModelViewMatrix[5] = glm::translate(ModelViewMatrix[5], glm::vec3(80.0f, -20.0f, 0.0f));
	ModelViewMatrix[5] *= tiger[tiger_data.cur_frame].ModelMatrix[5];

	ModelViewProjectionMatrix = ProjectionMatrix[5] * ModelViewMatrix[5];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(5);
}

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < n_static_objects; i++) {
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);
}