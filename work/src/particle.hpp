#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"

using namespace cgra;

class Particle {
    
private:
    vec3 m_position;
    vec3 m_velocity;
    
public:
    Particle(vec3, vec3);
    ~Particle();
    void render();
    bool update();
    
};
