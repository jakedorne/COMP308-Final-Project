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

using namespace std;
using namespace cgra;

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
float g_pitch = 0;
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
    
    if(key==GLFW_KEY_A && action == 1) {
        humidity -= 0.1;
        if(humidity < 0) {humidity = 0;}
        cout << "Humidity: " << humidity << endl;
    }
    if(key==GLFW_KEY_D && action == 1) {
        humidity += 0.1;
        if(humidity > 1.0) {humidity = 1.0;}
        cout << "Humidity: " << humidity << endl;
    }
    if(key==GLFW_KEY_LEFT && action == 1) {
        temperature -= 0.1;
        if(temperature < 0) {temperature = 0;}
        cout << "Temperature: " << temperature << endl;
    }
    if(key==GLFW_KEY_RIGHT && action == 1) {
        temperature += 0.1;
        if(temperature > 1.0) {temperature = 1.0;}
        cout << "Temperature: " << temperature << endl;
    }
    
}


// Character callback
// Called for every character input event on since the last glfwPollEvents
//
void charCallback(GLFWwindow *win, unsigned int c) {
	// cout << "Char Callback :: c=" << char(c) << endl;
	// Not needed for this assignment, but useful to have later on
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
//    particle_shader = makeShaderProgram({GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, {"./work/res/shaders/particleShader.vert", "./work/res/shaders/particleShader.frag"});
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

void calculateWeather(){
    
    // sky color
    float r = 0.7;
    float g = 0.7 + (0.2 * temperature);
    float b = 0.7 + (0.3 * temperature);
    sky_color = vec3(r, g, b);
    
    // determine if raining, snowing or clear
    if(humidity >= 0.8) {
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
    glUniform1f(glGetUniformLocation(g_shader, "fog_density"), 0.01 * humidity);

    // heatwave memes
    glUniform1i(glGetUniformLocation(g_shader, "heatwave"), (temperature >= 0.9) ? 1 : 0);

}


// Draw function
//
void render(int width, int height) {

    
	// Grey/Blueish background
	glClearColor(sky_color.x, sky_color.y, sky_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	setupCamera(width, height);

//    initLight();
    
    if(g_rotating) {
        g_angle++;
        if(g_angle > 360) {
            g_angle = 0;
            g_rotating = false;
        }
        glRotatef(g_angle, 0, 1, 0);
    }
    
    glUseProgram(g_shader);
    calculateWeather();
    glUniform1i(glGetUniformLocation(g_shader, "texture0"), 0);
    glUniform1f(glGetUniformLocation(g_shader, "time"), glfwGetTime());
    glUniform3f(glGetUniformLocation(g_shader, "sky_color"), sky_color.x, sky_color.y, sky_color.z);
    
    for(Geometry obj: objects) {
        obj.renderGeometry();
    }
    
//        glUseProgram(particle_shader);
//        glUniform1i(glGetUniformLocation(particle_shader, "texture0"), 0);
    
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

    // diffuse, ambient, specular, color, shine
    material bronze{vec3(0.7,0.4,0.18), vec4(0.2,0.1,0,1), vec3(0.4,0.3,0.2), vec3(0.6,0.4,0.3), 0.2f};
    material plastic{vec3(0.5,0,0), vec4(0,0,1,1), vec3(0.7,0.6,0.6), vec3(1,0,0), 0.25f};
    material metal{vec3(1,0.8,0.8), vec4(0.25,0.2,0.2,1), vec3(0.3,0.3,0.3), vec3(0.5,0.7,1), 0.09f};
    material bone{vec3(1,0.8,0.8), vec4(0.25,0.2,0.2,1), vec3(0.3,0.3,0.3), vec3(1.0,1.0,0.95), 0.09f};
    material wood{vec3(0,0,0), vec4(1,0.5,0,1), vec3(1.0,0.8,0), vec3(1,1,1), 0.2f};
    material brick{vec3(0,0,0), vec4(0,0,1,0), vec3(0,0,0), vec3(1,1,1), 1.0f};
    
    objects.push_back(Geometry("./work/res/assets/torus.obj","", plastic, vec3(6,1,6)));
    objects.push_back(Geometry("./work/res/assets/teapot.obj","cube", metal, vec3(-6,0.5,-6)));
    objects.push_back(Geometry("./work/res/assets/sphere.obj","", bronze, vec3(-6,2,4)));
    objects.push_back(Geometry("./work/res/assets/bunny.obj","", bone, vec3(1,0.5,1)));
    objects.push_back(Geometry("./work/res/assets/box.obj","./work/res/textures/brick.jpg", brick, vec3(6,2.5,-6)));
    objects.push_back(Geometry("./work/res/assets/table.obj","./work/res/textures/wood.jpg",wood, vec3(1,0,1)));

    // rain and possibly snow particle systems.
    ParticleSystem rain_system = ParticleSystem("./work/res/textures/rain.png", vec3(0,30,0), 300, 300, 200, vec3(0,-1,0));
    ParticleSystem snow_system = ParticleSystem("./work/res/textures/snow2.png", vec3(0,30,0), 300, 300, 50, vec3(0,-0.1,0));
    
    particle_systems.push_back(rain_system);
    particle_systems.push_back(snow_system);
    
	initShader();

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
