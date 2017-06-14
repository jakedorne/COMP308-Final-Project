#version 120

// Constant across both shaders
uniform sampler2D texture0;

// Values to pass to the fragment shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;

void main() {
    // Transform and pass on the normal/position/texture to fragment shader
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    vTextureCoord0 = gl_MultiTexCoord0.xy;
    
	// IMPORTANT tell OpenGL where the vertex is
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
