//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
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

#version 120

// Constant across both shaders
uniform sampler2D texture0;

// Values to pass to the fragment shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;
varying float visibility;

const float fog_density = 0.01;
const float gradient = 1.0;

void main() {
    // Transform and pass on the normal/position/texture to fragment shader
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    vTextureCoord0 = gl_MultiTexCoord0.xy;
  
    vec4 worldPosition =   vec4(vPosition,1.0);
    vec4 positionRelativeToCamera = gl_ModelViewMatrix * worldPosition;
    float distance = length(positionRelativeToCamera.xyz);
    visibility = exp(-pow((distance * fog_density), gradient));
    visibility = clamp(visibility,0.0,1.0);


	// IMPORTANT tell OpenGL where the vertex is
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
