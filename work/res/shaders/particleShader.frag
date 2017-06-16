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
uniform vec3 sky_color;


// Values passed in from the vertex shader
varying vec3 vNormal;
varying vec4 vPosition;
varying vec2 vTextureCoord0;
varying float visibility;

void main() {
    
    vec4 color;
    color = texture2D(texture0, vTextureCoord0).rgba;
    
    // IMPORTANT tell OpenGL what the final color of the fragment is (vec4)
    gl_FragColor = mix(vec4(sky_color,1), color, visibility);
}
