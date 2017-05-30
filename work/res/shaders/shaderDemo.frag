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

// fog stuff
const vec4 fog_color = vec4(0.7,0.7,0.7,1);

// Values passed in from the vertex shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;
varying float visibility;

void main() {

	// Can do all sorts of cool stuff here
    
    vec3 color;
    color = texture2D(texture0, vTextureCoord0).rgb;
    

	// IMPORTANT tell OpenGL what the final color of the fragment is (vec4)
 	gl_FragColor = mix(fog_color, vec4(color,1), visibility);
}
