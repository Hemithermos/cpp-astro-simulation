#ifndef SYSTEMS_LENSING_SYSTEM_H
#define SYSTEMS_LENSING_SYSTEM_H

#include "glm/glm.hpp"


#include "../core/System.h"
#include "../core/Coordinator.h"
#include "../components/GravityWell.h"
#include "../components/Projectile.h"
#include "../components/Transform2D.h"
#include "../components/Velocity2D.h"
class Coordinator;
extern Coordinator coordinator;
#define c2 299792458*299792458.0f



class LensingSystem : public System
{
public:
    void update(float);

private:
    void calcDeviation(Transform2D&, Velocity2D&, Projectile const&, Transform2D const&, GravityWell const&, float); // return the value of r, theta, dr, dtheta after dt
    glm::vec4 rhs(glm::vec4 const&, float const&);



};

#endif