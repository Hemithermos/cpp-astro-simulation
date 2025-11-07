#include "LensingSystem.h"

void LensingSystem::update(float dt)
{
    Transform2D blackholePos;
    GravityWell blackholeData;
    
    // get the components of the blackhole
    for(Entity entity : listOfEntities)
    {
        if(coordinator.hasComponent<GravityWell>(entity))
        {
            blackholePos = coordinator.getComponent<Transform2D>(entity);
            blackholeData = coordinator.getComponent<GravityWell>(entity);
            break; // we found it
        }
    }

    // for each entity
    for(Entity entity : listOfEntities)
    {
        // if they dont have the component GravityWell, we assume entities are rays
        if(!coordinator.hasComponent<GravityWell>(entity))
        {
            // get the ray components
            Transform2D& rayPosition = coordinator.getComponent<Transform2D>(entity);
            Velocity2D& rayVelocity = coordinator.getComponent<Velocity2D>(entity);
            Projectile& rayImpactParam = coordinator.getComponent<Projectile>(entity);

            // update where the photon should be after dt
            calcDeviation(rayPosition, rayVelocity, rayImpactParam, blackholePos, blackholeData, dt);

        }

    }

}

void LensingSystem::calcDeviation(Transform2D& pos, Velocity2D& vel, Projectile const& b, Transform2D const& bhpos, GravityWell const& gw, float dt)
{

    // switch to polar
    glm::vec2 relPos = pos.position - bhpos.position; 
    const float r = glm::length(relPos);
    const float r2 = r*r;
    const float theta = atan2(relPos[1], relPos[0]);

    const float f = 1 - gw.r_s/r;
    const float impact = b.impactParameter;
    const float df = gw.r_s/r2;

    const float dr = (vel.velocity[0] * relPos[0] + vel.velocity[1] * relPos[1])/r;
    const float dtheta = (relPos[0] * vel.velocity[1] - relPos[1] * vel.velocity[0]) / r2;

    // rk4 steps to get a better next r, th, dr, dth
    glm::vec4 y_0 = glm::vec4(r, theta, dr, dtheta);
    glm::vec4 k1 = rhs(y_0, gw.r_s);
    glm::vec4 k2 = rhs(y_0 + 0.5f * dt * k1 , gw.r_s);
    glm::vec4 k3 = rhs(y_0 + 0.5f * dt * k2, gw.r_s);
    glm::vec4 k4 = rhs(y_0 + dt * k3, gw.r_s);

    glm::vec4 y = y_0 + (dt / 6.0f) * (k1 + 2.0f * k2 + 2.0f * k3 + k4); // format r theta rdot thetadot



    // mutate the components of the ray


    // (x,y) = (bhx, bhy) + (r cos (theta), r sin(theta))
    pos.position = bhpos.position + glm::vec2(y[0]*cos(y[1]), y[0] * sin(y[1]));
    // (dx, dy) = (dr cos(theta) - r dtheta sin(theta), dr sin(theta) + r * dtheta * cos(theta))
    vel.velocity = glm::vec2(y[2] * cos(y[1]) - y[0] * y[3] * sin(y[1]),
                             y[2] * sin(y[1]) + y[0] * y[3] * cos(y[1]));

}

glm::vec4 LensingSystem::rhs(glm::vec4 const& r_theta_dr_dtheta, float const& r_s)
{
    // fetch the data
    float r, th, dr, dth, f, df;
    r = r_theta_dr_dtheta[0];
    th = r_theta_dr_dtheta[1];
    dr = r_theta_dr_dtheta[2];
    dth = r_theta_dr_dtheta[3];

    f = 1.0f - r_s/r;
    df = r_s/(r*r);

    // rp, thp, rpp, thpp = f(r, th, rp, thp)
    float rp = dr;
    float thp = dth;
    float rpp = -(c2 * f * df)/2.0f + (3.0f * df * dr * dr)/(2.0f*f) + r * f * dth * dth;
    float thpp = dth * dr * (df/f - 2.0f/r); 

    return glm::vec4(rp, thp, rpp, thpp);
}
