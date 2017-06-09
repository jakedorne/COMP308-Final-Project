#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"

class Particle {
    
private:
    cgra::vec3 m_position;
    cgra:vec3 m_velocity;
    
public:
    Particle(cgra::vec3, cgra::vec3);
    ~Particle();
    void renderParticle();
    
};
