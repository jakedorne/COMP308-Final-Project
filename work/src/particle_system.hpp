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
    GLuint m_texture;
    cgra::vec3 m_position;
    float m_width;
    float m_length;
    long m_ppr;
    vec3 m_velocity;
    bool enabled;
    
public:
    ParticleSystem(std::string texture_name, vec3 position, float width, float length, long ppr, vec3 velocity);
    ~ParticleSystem();
    void initTexture(std::string);
    void renderParticles();
    void generateParticles();
    void updateParticles();
    void enable();
    void disable();
    
};
