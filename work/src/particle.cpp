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

vec3 initial_position;

Particle::Particle(vec3 position, vec3 velocity) {
    m_position = position;
    initial_position = position;
    m_velocity = velocity;
}

Particle::~Particle() { }

void Particle::render(){
    glPushMatrix();
    glTranslatef(m_position.x, m_position.y, m_position.z);

    glBegin(GL_QUADS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glNormal3f(0.0, 0.0, 1.0);
//    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.01, -0.1, 0.0);
//    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.01, 0.1, 0.0);
//    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.01, 0.1, 0.0);
//    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.01, -0.1, 0.0);
    glEnd();
    glFlush();
    glDisable(GL_BLEND);
    glPopMatrix();
}

bool Particle::update() {
    m_position += m_velocity;
    return (initial_position.y - m_position.y) < 30;
}
