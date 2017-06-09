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

ParticleSystem::ParticleSystem(vec3 position, float width, float length, long ppr, vec3 velocity) {
    m_position = position;
    m_velocity = velocity;
    m_width = width;
    m_length = length;
    m_ppr = ppr;
}


ParticleSystem::~ParticleSystem() { }

void ParticleSystem::renderParticles(){
    for(Particle p: m_particles) {
        p.render();
    }
}

void ParticleSystem::generateParticles(){
    for(int i = 0; i < m_ppr; i++) {
        float x = (float) rand() / RAND_MAX;
        float z = (float) rand() / RAND_MAX;

        x = x * m_width - m_width/2;
        z = z * m_length - m_length/2;
        m_particles.push_back(Particle(vec3(x, m_position.y, z), m_velocity));
    }
}

void ParticleSystem::updateParticles(){
    for(int i = 0; i < m_particles.size(); i++) {
        if(!m_particles[i].update()){
            m_particles.erase(m_particles.begin());
        }
    }
}
