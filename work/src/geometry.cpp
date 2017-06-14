#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>


#include "cgra_math.hpp"
#include "geometry.hpp"
#include "opengl.hpp"
#include "simple_image.hpp"

using namespace std;
using namespace cgra;


Geometry::Geometry(string filename, string texturename, material material, vec3 position) {
    m_filename = filename;
    m_texturename = texturename;
    m_material = material;
    m_position = position;
    
    readOBJ(filename);
    if(texturename != "") {
        readTexture();
    } else {
        m_texture = -1;
    }

    if (m_triangles.size() > 0) {
        createDisplayListPoly();
    }
}


Geometry::~Geometry() { }

void Geometry::readTexture() {

    glGenTextures(1, &m_texture);
    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    if(m_texturename != "cube") {
        Image tex(m_texturename);

        glBindTexture(GL_TEXTURE_2D, m_texture); // Bind it as a 2D texture
        
        // Setup sampling strategies
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // Finnaly, actually fill the data into our texture
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE, tex.dataPointer());
    } else {
        vector<Image> images;
        images.push_back(Image("./work/res/textures/right.png"));
        images.push_back(Image("./work/res/textures/left.png"));
        images.push_back(Image("./work/res/textures/top.png"));
        images.push_back(Image("./work/res/textures/bottom.png"));
        images.push_back(Image("./work/res/textures/back.png"));
        images.push_back(Image("./work/res/textures/front.png"));
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
        for(int i = 0; i < 6; i++) {
            Image image = images[i];
            glTexImage2D(
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_RGB, image.w, image.h, 0, GL_RGB, GL_UNSIGNED_BYTE, image.dataPointer()
                         );
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    }
    
}


void Geometry::readOBJ(string filename) {
    
    // Make sure our geometry information is cleared
    m_points.clear();
    m_uvs.clear();
    m_normals.clear();
    m_triangles.clear();
    
    // Load dummy points because OBJ indexing starts at 1 not 0
    m_points.push_back(vec3(0,0,0));
    m_uvs.push_back(vec2(0,0));
    m_normals.push_back(vec3(0,0,1));
    
    
    ifstream objFile(filename);
    
    if(!objFile.is_open()) {
        cerr << "Error reading " << filename << endl;
        throw runtime_error("Error :: could not open file.");
    }
    
    cout << "Reading file " << filename << endl;
    
    // good() means that failbit, badbit and eofbit are all not set
    while(objFile.good()) {
        
        // Pull out line from file
        string line;
        std::getline(objFile, line);
        istringstream objLine(line);
        
        // Pull out mode from line
        string mode;
        objLine >> mode;
        
        // Reading like this means whitespace at the start of the line is fine
        // attempting to read from an empty string/line will set the failbit
        if (!objLine.fail()) {
            
            if (mode == "v") {
                vec3 v;
                objLine >> v.x >> v.y >> v.z;
                m_points.push_back(v);
                
            } else if(mode == "vn") {
                vec3 vn;
                objLine >> vn.x >> vn.y >> vn.z;
                m_normals.push_back(vn);
                
            } else if(mode == "vt") {
                vec2 vt;
                objLine >> vt.x >> vt.y;
                m_uvs.push_back(vt);
                
            } else if(mode == "f") {
                
                vector<vertex> verts;
                while (objLine.good()){
                    vertex v;
                    objLine >> v.p;		// Scan in position index

                    
                    if (m_uvs.size()<2) {
                        // x//x
                        objLine.ignore(2);
                        objLine >> v.n;
                    }
                    if (m_normals.size() < 2) {
                        // x
                    }
                    
                    if (m_uvs.size() > 2 && m_normals.size() > 2) {
                        // x/x/x
                        objLine.ignore(1);
                        objLine >> v.t;
                        objLine.ignore(1);
                        objLine >> v.n;
                    }

                    verts.push_back(v);
                }
                
                // If we have 3 verticies, construct a triangle
                if(verts.size() >= 3){
                    triangle tri;
                    tri.v[0] = verts[0];
                    tri.v[1] = verts[1];
                    tri.v[2] = verts[2];
                    m_triangles.push_back(tri);
                    
                }
            }
        }
    }
    
    cout << "Reading OBJ file is DONE." << endl;
    cout << m_points.size()-1 << " points" << endl;
    cout << m_uvs.size()-1 << " uv coords" << endl;
    cout << m_normals.size()-1 << " normals" << endl;
    cout << m_triangles.size() << " faces" << endl;
    
    
    // If we didn't have any normals, create them
    if (m_normals.size() <= 1) createNormals();
    
    cout << m_normals.size()-1 << " normals now" << endl;
}

void Geometry::createNormals() {
    
    // face normals
    
    for (int i = 0; i < m_triangles.size(); i++) {
        triangle t = m_triangles[i];
        
        vec3 edge1 = m_points[t.v[1].p] - m_points[t.v[0].p];
        vec3 edge2 = m_points[t.v[2].p] - m_points[t.v[0].p];
        vec3 normal = normalize(cross(edge1, edge2));
        
        m_normals.push_back(normal);
        m_triangles[i].v[0].n = i+1;
        m_triangles[i].v[1].n = i+1;
        m_triangles[i].v[2].n = i+1;
    }
    
}

void Geometry::createDisplayListPoly() {
    // Delete old list if there is one
    if (m_displayListPoly) glDeleteLists(m_displayListPoly, 1);
    
    // Create a new list
    cout << "Creating Poly Geometry" << endl;
    m_displayListPoly = glGenLists(1);
    
    glNewList(m_displayListPoly, GL_COMPILE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < m_triangles.size(); i++) {
        for(int j = 0; j < 3; j++) {
            vertex v = m_triangles[i].v[j];
            vec3 normal = m_normals[v.n];
            vec2 uv = m_uvs[v.t];
            
            vec3 vertex = m_points[v.p];
            glNormal3f(normal.x, normal.y, normal.z);
            
            glTexCoord2d(uv.x * 5, uv.y * 5);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    
    glEnd();
    glEndList();
    cout << "Finished creating Poly Geometry" << endl;
}

void Geometry::renderGeometry() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    
    glPushMatrix();
    glTranslatef(m_position.x, m_position.y, m_position.z);
    glCallList(m_displayListPoly);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);

}
