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

using namespace std;
using namespace cgra;


Particle::Particle(vec3 position, vec3 velocity) {
    m_position = position;
    m_velocity = velocity;
}


Particle::~Particle() { }

void renderParticle(){

}
