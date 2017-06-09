#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"

using namespace cgra;

class ParticleSystem {
    
private:
    cgra::vec3 m_position;
    float m_width;
    float m_length;
    long m_ppr;
    vec3 m_velocity;
    
public:
    ParticleSystem(vec3 position, float width, float length, long ppr, vec3 velocity);
    ~ParticleSystem();
    void renderParticles();
    void generateParticles();
    void updateParticles();
    
};
