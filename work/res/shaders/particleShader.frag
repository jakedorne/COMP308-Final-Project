#version 120

// Constant across both shaders
uniform sampler2D texture0;

// Values passed in from the vertex shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;

void main() {

	// Can do all sorts of cool stuff here
    vec4 color;
    color = texture2D(texture0, vTextureCoord0).rgba;

	// IMPORTANT tell OpenGL what the final color of the fragment is (vec4)
    gl_FragColor = color;
}
