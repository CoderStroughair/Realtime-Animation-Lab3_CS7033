
#include <windows.h>
#include "Antons_maths_funcs.h"
#include "teapot.h" // teapot mesh
#include "Utilities.h"
#include "Particle.h"
#include "Collision.h"
#include "Skeleton.h"
using namespace std;

const float width = 900, height = 900;
/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/
vec3 mouseLocation = vec3(0.0f, 0.5f, 0.0f);
vec3 closestPoint = vec3(0.5f, -0.5f, 0.0f);

vec3 triangle[] =
{
	vec3(0.0f, 0.5f, 0.0f),
	vec3(0.5f, -0.5f, 0.0f),
	vec3(-0.5f, -0.5f, 0.0f)
};

Mesh cubeID, cubeMapID;

/*----------------------------------------------------------------------------
						CAMERA VARIABLES
----------------------------------------------------------------------------*/

vec3 startingPos = { 0.0f, 0.0f, 10.0f };
GLfloat pitCam = 0, yawCam = 0, rolCam = 0, frontCam = 0, sideCam = 0, speed = 1;
float rotateY = 50.0f, rotateLight = 0.0f;
EulerCamera cam(startingPos, vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), 270.0f, 0.0f, 0.0f);

/*----------------------------------------------------------------------------
						SHADER VARIABLES
----------------------------------------------------------------------------*/
GLuint simpleShaderID, noTextureShaderID, cubeMapShaderID;
Shader shaderFactory;
/*----------------------------------------------------------------------------
						OTHER VARIABLES
----------------------------------------------------------------------------*/

const char* atlas_image = "../freemono.png";
const char* atlas_meta = "../freemono.meta";

float fontSize = 25.0f;
int textID = -1;
Skeleton skeleton;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

void drawloop(mat4 view, mat4 proj, GLuint framebuffer);
void rotateTriangle(float degrees);

/*--------------------------------------------------------------------------*/

void init()
{
	if (!init_text_rendering(atlas_image, atlas_meta, width, height)) 
	{
		fprintf(stderr, "ERROR init text rendering\n");
		exit(1);
	}
	simpleShaderID = shaderFactory.CompileShader(SIMPLE_VERT, SIMPLE_FRAG);
	noTextureShaderID = shaderFactory.CompileShader(NOTEXTURE_VERT, NOTEXTURE_FRAG);
	cubeMapShaderID = shaderFactory.CompileShader(SKY_VERT, SKY_FRAG);
	skeleton = Skeleton(cubeID);
}

void display() 
{
	mat4 proj = perspective(87.0, width/height, 1, 1000.0);
	mat4 view = look_at(cam.getPosition(), cam.getPosition() + cam.getFront(), cam.getUp());
	glViewport(0, 0, width, height);
	drawloop(view, proj, 0);
	draw_texts();
	glutSwapBuffers();
}

void updateScene() {
	static DWORD  last_frame;	//time when last frame was drawn
	static DWORD last_timer = 0;	//time when timer was updated
	DWORD  curr_time = timeGetTime();//for frame Rate Stuff.
	static bool first = true;
	if (first)
	{
		last_frame = curr_time;
		first = false;
	}
	float  delta = (curr_time - last_frame) * 0.001f;
	if (delta >= 0.03f) 
	{
		last_frame = curr_time;
		glutPostRedisplay();
		rotateTriangle(0.5);

		closestPoint = getClosestPointTriangle(triangle, mouseLocation);

		string text;
		text += "P1 = [" + to_string(triangle[0].v[0]) + "," + to_string(triangle[0].v[1]) + "," + to_string(triangle[0].v[2]) + "]\n";
		text += "P2 = [" + to_string(triangle[1].v[0]) + "," + to_string(triangle[1].v[1]) + "," + to_string(triangle[1].v[2]) + "]\n";
		text += "P3 = [" + to_string(triangle[2].v[0]) + "," + to_string(triangle[2].v[1]) + "," + to_string(triangle[2].v[2]) + "]\n";
		text += "Distance = " + to_string(getDistance(mouseLocation, closestPoint)) + "\n";
		update_text(textID, text.c_str());
	}
	
}

#pragma region INPUT FUNCTIONS

void keypress(unsigned char key, int x, int y) 
{
	if (key == (char)27)	//Pressing Escape Ends the game
	{
		exit(0);
	}
}

void keypressUp(unsigned char key, int x, int y)
{
}

void specialKeypress(int key, int x, int y) 
{
}

void specialKeypressUp(int key, int x, int y) 
{
}

void (mouse)(int x, int y)
{
	mouseLocation.v[0] = ((2 / width) * x) - 1;
	mouseLocation.v[1] = 1 - (((2 / height) * y));
}

#pragma endregion INPUT FUNCTIONS

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("GameApp");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);


	// Tell glut where the display function is
	glutWarpPointer(width / 2, height / 2);
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);

	// Input Function Initialisers //
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouse);
	glutSpecialFunc(specialKeypress);
	glutSpecialUpFunc(specialKeypressUp);
	glutKeyboardUpFunc(keypressUp);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	init();
	textID = add_text("hi",
		-0.95, 0.9, fontSize, 1.0f, 1.0f, 1.0f, 1.0f);

	glutMainLoop();
	return 0;
}

void drawloop(mat4 view, mat4 proj, GLuint framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);								// enable depth-testing
	glDepthFunc(GL_LESS);									// depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear the color and buffer bits to make a clean slate
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);					//Create a background	

															// light properties
	vec3 Ls = vec3(0.0001f, 0.0001f, 0.0001f);	//Specular Reflected Light
	vec3 Ld = vec3(0.5f, 0.5f, 0.5f);	//Diffuse Surface Reflectance
	vec3 La = vec3(1.0f, 1.0f, 1.0f);	//Ambient Reflected Light
	vec3 light = vec3(5.0f, 10, -5.0f);//light source location
	vec3 coneDirection = light + vec3(0.0f, -1.0f, 0.0f);
	float coneAngle = 10.0f;
	// object colour
	vec3 Ks = vec3(0.1f, 0.1f, 0.1f); // specular reflectance
	vec3 Kd = BLUE;
	vec3 Ka = BLUE; // ambient reflectance
	float specular_exponent = 0.000000005f; //specular exponent - size of the specular elements

	mat4 model = identity_mat4();

	drawCubeMap(cubeMapShaderID, cubeMapID.tex, view, proj, model, vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), cam, cubeMapID, GL_TRIANGLES);
}

void rotateTriangle(float degrees)
{
	mat4 rotate = rotate_z_deg(identity_mat4(), degrees);
	for (int i = 0; i < 3; i++)
	{
		triangle[i] = multiply(rotate, triangle[i]);
	}
}