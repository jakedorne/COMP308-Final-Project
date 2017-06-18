//---------------------------------------------------------------------------
//
// Copyright (c) 2016 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "simple_image.hpp"
#include "simple_shader.hpp"
#include "opengl.hpp"
#include "geometry.hpp"
#include "particle_system.hpp"
#include "tree.hpp"

using namespace std;
using namespace cgra;


bool showTrees = true; //Sets displaying of trees or table/bunny objects
bool animate = true;
bool windy = false;

// Window
//
GLFWwindow* g_window;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 1000.0;


// Mouse controlled Camera values
//
bool g_leftMouseDown = false;
vec2 g_mousePosition;
float g_pitch = 1.0;
float g_yaw = 0;
float g_zoom = 1.0;


bool g_rotating = false;
float g_angle = 0;

// Values and fields to showcase the use of shaders
// Remove when modifying main.cpp for Assignment 3
//
bool g_useShader = true;
GLuint g_texture = 0;
GLuint g_shader = 0;
GLuint particle_shader = 0;

vector<Geometry> objects;
vector<ParticleSystem> particle_systems;
vector<Tree> trees;
Tree tree = Tree();

// weather variables
vec3 sky_color = vec3(0.7, 0.9, 1);
bool raining = false;
bool snowing = false;
float humidity = 0.7;
float temperature = 0.5;

// Mouse Button callback
// Called for mouse movement event on since the last glfwPollEvents
//
void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	// cout << "Mouse Movement Callback :: xpos=" << xpos << "ypos=" << ypos << endl;
	if (g_leftMouseDown) {
		g_yaw -= g_mousePosition.x - xpos;
		g_pitch -= g_mousePosition.y - ypos;
	}
	g_mousePosition = vec2(xpos, ypos);
}


// Mouse Button callback
// Called for mouse button event on since the last glfwPollEvents
//
void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	// cout << "Mouse Button Callback :: button=" << button << "action=" << action << "mods=" << mods << endl;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		g_leftMouseDown = (action == GLFW_PRESS);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (g_useShader) {
			g_useShader = false;
			cout << "Using the default OpenGL pipeline" << endl;
		}
		else {
			g_useShader = true;
			cout << "Using a shader" << endl;
		}
	}
}


// Scroll callback
// Called for scroll event on since the last glfwPollEvents
//
void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	// cout << "Scroll Callback :: xoffset=" << xoffset << "yoffset=" << yoffset << endl;
	g_zoom -= yoffset * g_zoom * 0.2;
}


// Keyboard callback
// Called for every key event on since the last glfwPollEvents
//
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
//	 cout << "Key Callback :: key=" << key << "scancode=" << scancode << "action=" << action << "mods=" << mods << endl;
    
    if(key==GLFW_KEY_UP && action == 2) {
        humidity += 0.02;
        if(humidity > 1) {humidity = 1;}
        cout << "Humidity: " << humidity << endl;
    }
    if(key==GLFW_KEY_DOWN && action == 2) {
        humidity -= 0.02;
        if(humidity < 0) {humidity = 0;}
        cout << "Humidity: " << humidity << endl;
    }
    if(key==GLFW_KEY_LEFT && action == 2) {
        temperature -= 0.02;
        if(temperature < 0) {temperature = 0;}
        cout << "Temperature: " << temperature << endl;
    }
    if(key==GLFW_KEY_RIGHT && action == 2) {
        temperature += 0.02;
        if(temperature > 1.0) {temperature = 1.0;}
        cout << "Temperature: " << temperature << endl;
    }
	else if (key == GLFW_KEY_1 && action == 0) {
		if (animate) {
			animate = false;
		}
		else {
			animate = true;
		}
	}
	else if (key == GLFW_KEY_2 && action == 0) {
		if (windy == true) {
			windy = false;
		}
		else {
			windy = true;
		}
	}
	else if (key == GLFW_KEY_3 && action == 0) {
//        for(Tree tree: trees) {
            tree.setAngle(1);
            cout << "Angle: " << tree.angle << endl;
//        }
	}
	else if (key == GLFW_KEY_4 && action == 0) {
//        for(Tree tree: trees) {
            tree.setAngle(-1);
            cout << "Angle: " << tree.angle << endl;
//        }
	}
	else if (key == GLFW_KEY_5 && action == 0) {
//        for(Tree tree: trees) {
            if (tree.grow) {
                tree.grow = false;
            }
            else {
                tree.grow = true;
            }
//        }
	}
	/*
		TREE CONTROLS
		
		1 = Toggle Animate
		2 = Toggle Wind
		3 = Increase angle
		4 = Decrease Angle
		5 = Toggle grow/shrink
	*/
}


// Character callback
// Called for every character input event on since the last glfwPollEvents
//
void charCallback(GLFWwindow *win, unsigned int c) {
	// cout << "Char Callback :: c=" << char(c) << endl;
	// Not needed for this assignment, but useful to have later on
}

void initTexture(){
    glGenTextures(1, &g_texture);
    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    Image tex("./work/res/textures/grass.jpg");
    glBindTexture(GL_TEXTURE_2D, g_texture); // Bind it as a 2D texture
    
    // Setup sampling strategies
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Finnaly, actually fill the data into our texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE, tex.dataPointer());
}

// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
    
    // weak direction light
    float diffintensity[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
    
    // weak ambient light
    float ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

// An example of how to load a shader from a hardcoded location
//
void initShader() {
	g_shader = makeShaderProgramFromFile({GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/shaderDemo.vert", "./work/res/shaders/shaderDemo.frag" });

    particle_shader = makeShaderProgramFromFile({GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, {"./work/res/shaders/particleShader.vert", "./work/res/shaders/particleShader.frag"});
    
    glUniform1i(glGetUniformLocation(g_shader, "texture0"), 0);
    glUniform1i(glGetUniformLocation(particle_shader, "texture0"), 0);
}


// Sets up where the camera is in the scene
// 
void setupCamera(int width, int height) {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, width / float(height), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -50 * g_zoom);
	glRotatef(g_pitch, 1, 0, 0);
	glRotatef(g_yaw, 0, 1, 0);
}

void renderFloor(){
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texture);
    
    glPushMatrix();
    glBegin(GL_QUADS);
    
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-20.0, 0, -20.0);
    glTexCoord2f(0.0, 10.0);
    glVertex3f(20.0, 0, -20.0);
    glTexCoord2f(10.0, 10.0);
    glVertex3f(20, 0, 20);
    glTexCoord2f(10.0, 0.0);
    glVertex3f(-20, 0, 20);
    glEnd();
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void calculateWeather(){
    
    // sky color
    float r = 0.7;
    float g = 0.7 + (0.2 * temperature);
    float b = 0.7 + (0.3 * temperature);
    sky_color = vec3(r, g, b);
    
    // determine if raining, snowing or clear
    if(humidity >= 0.8 && temperature < 0.9) {
        if (temperature <= 0.1) {
            particle_systems[0].disable();
            particle_systems[1].enable();
            
        } else {
            particle_systems[0].enable();
            particle_systems[1].disable();
        }
    } else {
        particle_systems[0].disable();
        particle_systems[1].disable();
    }
    
    // determine amount of fog
    glUniform1f(glGetUniformLocation(g_shader, "fog_density"), 0.02 * humidity);
    glUniform3f(glGetUniformLocation(g_shader, "sky_color"), sky_color.x, sky_color.y, sky_color.z);
    glUniform1f(glGetUniformLocation(particle_shader, "fog_density"), 0.02 * humidity);
    glUniform3f(glGetUniformLocation(particle_shader, "sky_color"), sky_color.x, sky_color.y, sky_color.z);
    // heatwave memes
    glUniform1i(glGetUniformLocation(g_shader, "heatwave"), (temperature >= 0.9) ? 1 : 0);

}


// Draw function
//
void render(int width, int height) {
    
    // use default shader
    glUseProgram(g_shader);
    
	// Grey/Blueish background
	glClearColor(sky_color.x, sky_color.y, sky_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	setupCamera(width, height);

    if(g_rotating) {
        g_angle++;
        if(g_angle > 360) {
            g_angle = 0;
            g_rotating = false;
        }
        glRotatef(g_angle, 0, 1, 0);
    }
    

    calculateWeather();

    glUniform1f(glGetUniformLocation(g_shader, "time"), glfwGetTime());

    renderFloor();
    
    if (showTrees) {
//        for(Tree tree: trees) {
            tree.drawTree();
            if (animate) {
                tree.animate();
            }
            if (windy) {
                tree.applyWind();
            }
//        }
    }
    
    glUseProgram(particle_shader);
    
    for(ParticleSystem ps: particle_systems) {
        ps.generateParticles();
        ps.updateParticles();
        ps.renderParticles();
    }
    
    // Unbind our shader
    glUseProgram(0);
    
	// Disable flags for cleanup (optional)
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
}


// Forward decleration for cleanliness (Ignore)
void APIENTRY debugCallbackARB(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);


//Main program
// 
int main(int argc, char **argv) {
    
    // Initialize the GLFW library
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW" << endl;
		abort(); // Unrecoverable error
	}

	// Get the version for GLFW for later
	int glfwMajor, glfwMinor, glfwRevision;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

	// Create a windowed mode window and its OpenGL context
	g_window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!g_window) {
		cerr << "Error: Could not create GLFW window" << endl;
		abort(); // Unrecoverable error
	}

	// Make the g_window's context is current.
	// If we have multiple windows we will need to switch contexts
	glfwMakeContextCurrent(g_window);



	// Initialize GLEW
	// must be done after making a GL context current (glfwMakeContextCurrent in this case)
	glewExperimental = GL_TRUE; // required for full GLEW functionality for OpenGL 3.0+
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}



	// Print out our OpenGL verisions
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using GLFW " << glfwMajor << "." << glfwMinor << "." << glfwRevision << endl;



	// Attach input callbacks to g_window
	glfwSetCursorPosCallback(g_window, cursorPosCallback);
	glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
	glfwSetScrollCallback(g_window, scrollCallback);
	glfwSetKeyCallback(g_window, keyCallback);
	glfwSetCharCallback(g_window, charCallback);



	// Enable GL_ARB_debug_output if available. Not nessesary, just helpful
	if (glfwExtensionSupported("GL_ARB_debug_output")) {
		// This allows the error location to be determined from a stacktrace
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// Set the up callback
		glDebugMessageCallbackARB(debugCallbackARB, nullptr);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
		cout << "GL_ARB_debug_output callback installed" << endl;
	}
	else {
		cout << "GL_ARB_debug_output not available. No worries." << endl;
	}

    // initialising particle system
    ParticleSystem rain_system = ParticleSystem("./work/res/textures/rain.png", vec3(0,60,0), 100, 100, 200, vec3(0,-1,0));
    ParticleSystem snow_system = ParticleSystem("./work/res/textures/snow2.png", vec3(0,60,0), 100, 100, 20, vec3(0,-0.05,0));
    
    particle_systems.push_back(rain_system);
    particle_systems.push_back(snow_system);
    
	double num = (double)rand() / (double)RAND_MAX;
    
    trees.push_back(Tree());
    
//    for (Tree tree: trees) {
        for (int i = 0; i < tree.TREEDEPTH+1; i++) {
            tree.expandTree(0,1); //Rule set 0 is first set off rules
        }
        
        for (int i = 0; i < tree.trees->size(); i++) {
            tree.compressTree(tree.trees->at(i));
        }
        tree.trees = tree.expandedTrees;
        
        cout << tree.trees->at(1) << endl;
        
        cout << tree.trees->size() << endl;
//    }
    
	initShader();
    initTexture();

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(g_window)) {

		// Make sure we draw to the WHOLE window
		int width, height;
		glfwGetFramebufferSize(g_window, &width, &height);

		// Main Render
		render(width, height);

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}






//-------------------------------------------------------------
// Fancy debug stuff
//-------------------------------------------------------------

// function to translate source to string
string getStringForSource(GLenum source) {

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		return("API");
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return("Window System");
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return("Shader Compiler");
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return("Third Party");
	case GL_DEBUG_SOURCE_APPLICATION:
		return("Application");
	case GL_DEBUG_SOURCE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// function to translate severity to string
string getStringForSeverity(GLenum severity) {

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		return("HIGH!");
	case GL_DEBUG_SEVERITY_MEDIUM:
		return("Medium");
	case GL_DEBUG_SEVERITY_LOW:
		return("Low");
	default:
		return("n/a");
	}
}

// function to translate type to string
string getStringForType(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		return("Error");
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return("Deprecated Behaviour");
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return("Undefined Behaviour");
	case GL_DEBUG_TYPE_PORTABILITY:
		return("Portability Issue");
	case GL_DEBUG_TYPE_PERFORMANCE:
		return("Performance Issue");
	case GL_DEBUG_TYPE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// actually define the function
void APIENTRY debugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, GLvoid*) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) return;

	cerr << endl; // extra space

	cerr << "Type: " <<
		getStringForType(type) << "; Source: " <<
		getStringForSource(source) << "; ID: " << id << "; Severity: " <<
		getStringForSeverity(severity) << endl;

	cerr << message << endl;

	if (type == GL_DEBUG_TYPE_ERROR_ARB) throw runtime_error("");
}
