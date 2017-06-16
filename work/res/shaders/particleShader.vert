#version 120

// Constant across both shaders
uniform sampler2D texture0;
uniform float fog_density;
uniform vec3 sky_color;

// Values to pass to the fragment shader
varying vec3 vNormal;
varying vec4 vPosition;
varying vec2 vTextureCoord0;
varying float visibility;

const float gradient = 1.0;

void main() {
    mat4 matrix = gl_ModelViewMatrix;
    
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[0][2] = 0;
    
    // Column 1:
    matrix[1][0] = 0;
    matrix[1][1] = 1;
    matrix[1][2] = 0;
    
    // Column 2:
    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 1;
    
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    vPosition = matrix * gl_Vertex;
    vTextureCoord0 = gl_MultiTexCoord0.xy;
    
    // fog
    vec4 worldPosition = vPosition;
    vec4 positionRelativeToCamera = gl_ModelViewMatrix * worldPosition;
    float distance = length(positionRelativeToCamera.xyz);
    visibility = exp(-pow((distance * fog_density), gradient));
    visibility = clamp(visibility,0.0,1.0);
    
    // IMPORTANT tell OpenGL where the vertex is
    gl_Position = gl_ProjectionMatrix * vPosition;
}
