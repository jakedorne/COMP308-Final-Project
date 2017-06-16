#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>


#include "cgra_math.hpp"
#include "particle.hpp"
#include "opengl.hpp"
#include "simple_image.hpp"
#include "particle_system.hpp"

using namespace std;
using namespace cgra;

vector<Particle> m_particles;

ParticleSystem::ParticleSystem(string texture_name, vec3 position, float width, float length, long ppr, vec3 velocity) {
    m_position = position;
    m_velocity = velocity;
    m_width = width;
    m_length = length;
    m_ppr = ppr;
    
    initTexture(texture_name);
}


ParticleSystem::~ParticleSystem() { }

void ParticleSystem::initTexture(string texture_name) {
    glGenTextures(1, &m_texture);
    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    Image tex(texture_name);
    glBindTexture(GL_TEXTURE_2D, m_texture); // Bind it as a 2D texture
    
    // Setup sampling strategies
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Finnaly, actually fill the data into our texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE, tex.dataPointer());
}

void ParticleSystem::renderParticles(){
    glEnable(GL_TEXTURE_2D);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    for(Particle p: m_particles) {
        p.render();
    }
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void ParticleSystem::generateParticles(){
    if (enabled) {
        for(int i = 0; i < m_ppr; i++) {
            float x = (float) rand() / RAND_MAX;
            float z = (float) rand() / RAND_MAX;

            x = x * m_width - m_width/2;
            z = z * m_length - m_length/2;
            m_particles.push_back(Particle(vec3(x, m_position.y, z), m_velocity, m_texture));
        }
    }
}

void ParticleSystem::updateParticles(){
    for(int i = 0; i < m_particles.size(); i++) {
        if(!m_particles[i].update()){
            m_particles.erase(m_particles.begin());
        }
//        m_particles[i].update();
    }
}


void ParticleSystem::enable() {
    enabled = true;
}

void ParticleSystem::disable() {
    enabled = false;
}
