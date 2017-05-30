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

#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"


struct vertex {
    int p = 0; // point
    int t = 0; // uv
    int n = 0; // normal
};

struct triangle {
    vertex v[3]; //requires 3 verticies
};

struct material {
    cgra::vec3 diffuse;
    cgra::vec4 ambience;
    cgra::vec3 specular;
    cgra::vec3 color;
    float shine;
};

class Geometry {
private:
    
    // Feilds for storing raw obj information
    std::string m_filename;
    std::string m_texturename;
    std::vector<cgra::vec3> m_points;	// Point list
    std::vector<cgra::vec2> m_uvs;		// Texture Coordinate list
    std::vector<cgra::vec3> m_normals;	// Normal list
    std::vector<triangle> m_triangles;	// Triangle/Face list
    cgra::vec3 m_position;
    bool m_wireFrameOn = false;
    
    // IDs for the display list to render
    GLuint m_displayListPoly = 0; // DisplayList for Polygon
    
    void readOBJ(std::string);
    void readTexture();
    void createNormals();
    void createDisplayListPoly();
    
public:
    GLuint m_texture;
    material m_material;
    Geometry(std::string, std::string, material, cgra::vec3);
    ~Geometry();
    void renderGeometry();
    void toggleWireFrame();
    
};
