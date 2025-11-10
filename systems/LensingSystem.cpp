#include "LensingSystem.h"
#include <cmath>

void LensingSystem::geodesicRHS(const GeodesicState& state, float rhs[4], float rs) {
    float r = state.r;
    float dr = state.dr;
    float dphi = state.dphi;
    float E = state.E;
    float f = 1.0f - rs/r;
    float dt_dl = E / f;
    
    // dr/dl = dr
    rhs[0] = dr;
    
    // dφ/dl = dphi
    rhs[1] = dphi;
    
    // d²r/dl² from Schwarzschild null geodesic
    rhs[2] = -(rs/(2*r*r)) * f * (dt_dl*dt_dl)
             + (rs/(2*r*r*f)) * (dr*dr)
             + (r - rs) * (dphi*dphi);
             
    // d²φ/dl² = -2*(dr * dphi) / r
    rhs[3] = -2.0f * dr * dphi / r;
}

void LensingSystem::addState(const float a[4], const float b[4], float factor, float out[4]) {
    for (int i = 0; i < 4; i++) {
        out[i] = a[i] + b[i] * factor;
    }
}

void LensingSystem::rk4Step(GeodesicState& state, float dl, float rs) {
    float y0[4] = { state.r, state.phi, state.dr, state.dphi };
    float k1[4], k2[4], k3[4], k4[4], temp[4];
    
    geodesicRHS(state, k1, rs);
    
    // k2 calculation
    addState(y0, k1, dl/2.0f, temp);
    GeodesicState s2 = state;
    s2.r = temp[0]; s2.phi = temp[1]; s2.dr = temp[2]; s2.dphi = temp[3];
    geodesicRHS(s2, k2, rs);
    
    // k3 calculation
    addState(y0, k2, dl/2.0f, temp);
    GeodesicState s3 = state;
    s3.r = temp[0]; s3.phi = temp[1]; s3.dr = temp[2]; s3.dphi = temp[3];
    geodesicRHS(s3, k3, rs);
    
    // k4 calculation
    addState(y0, k3, dl, temp);
    GeodesicState s4 = state;
    s4.r = temp[0]; s4.phi = temp[1]; s4.dr = temp[2]; s4.dphi = temp[3];
    geodesicRHS(s4, k4, rs);
    
    // Final update using RK4 formula
    state.r    += (dl/6.0f)*(k1[0] + 2*k2[0] + 2*k3[0] + k4[0]);
    state.phi  += (dl/6.0f)*(k1[1] + 2*k2[1] + 2*k3[1] + k4[1]);
    state.dr   += (dl/6.0f)*(k1[2] + 2*k2[2] + 2*k3[2] + k4[2]);
    state.dphi += (dl/6.0f)*(k1[3] + 2*k2[3] + 2*k3[3] + k4[3]);
}

void LensingSystem::updatePosition(Transform2D& pos, Velocity2D& vel, GeodesicState& state) {
    // Convert polar coordinates back to Cartesian
    pos.position.x = state.r * cos(state.phi);
    pos.position.y = state.r * sin(state.phi);
    
    // Update velocity components
    vel.velocity.x = state.dr * cos(state.phi) - state.r * state.dphi * sin(state.phi);
    vel.velocity.y = state.dr * sin(state.phi) + state.r * state.dphi * cos(state.phi);
}

glm::vec4 LensingSystem::rhs(glm::vec4 const& r_theta_dr_dtheta, float const& r_s) {
    float r  = r_theta_dr_dtheta[0];
    //float th = r_theta_dr_dtheta[1];
    float dr = r_theta_dr_dtheta[2];
    float dth= r_theta_dr_dtheta[3];

    // Guard r against the horizon to avoid division by zero
    const float rmin = r_s * 1.001f;
    r = std::max(r, rmin);

    const float f  = 1.0f - r_s / r;
    const float df = r_s / (r*r);

    // Avoid divide-by-zero in 1/f by flooring f
    const float fmin = 1e-6f;
    const float invf = 1.0f / std::max(f, fmin);

    const float rp   = dr;
    const float thp  = dth;

    const float rpp  = -0.5f * c2 * f * df
                     + 1.5f * df * dr * dr * invf
                     + r * f * dth * dth;

    const float thpp = dth * dr * (df * invf - 2.0f / r);

    return glm::vec4(rp, thp, rpp, thpp);
}


void LensingSystem::update(float dt) {


    // Integration sub-steps. Tune for stability/perf.
    const int substeps = 8;
    const float h = dt / float(substeps);

    Transform2D blackholePos{};
    GravityWell blackholeData{};
    for (Entity e : listOfEntities) {
        if (coordinator.hasComponent<GravityWell>(e)) {
            blackholePos = coordinator.getComponent<Transform2D>(e);
            blackholeData = coordinator.getComponent<GravityWell>(e);
            break;
        }
    }

    for (Entity entity : listOfEntities) {
        if (coordinator.hasComponent<GravityWell>(entity)) continue;

        auto &rayPosition = coordinator.getComponent<Transform2D>(entity);
        auto &rayVelocity = coordinator.getComponent<Velocity2D>(entity);
        auto &trail = coordinator.getComponent<Trail>(entity);

        for (int s = 0; s < substeps; ++s) {
            glm::vec2 relPos = rayPosition.position - blackholePos.position;

            GeodesicState state{};
            state.r   = glm::length(relPos);
            state.phi = std::atan2(relPos.y, relPos.x);

            // Keep a small safety margin from r_s
            const float eps = 1e-3f * blackholeData.r_s;
            if (state.r <= blackholeData.r_s + eps) break;

            const float v = glm::length(rayVelocity.velocity);
            const float velAngle = std::atan2(rayVelocity.velocity.y, rayVelocity.velocity.x);
            state.dr   = v * std::cos(velAngle - state.phi);
            state.dphi = v * std::sin(velAngle - state.phi) / std::max(state.r, eps);

            // Integrate one small step in "time". Using h here helps a lot.
            rk4Step(state, h, blackholeData.r_s);

            // Reject NaNs / infs
            if (!std::isfinite(state.r) || !std::isfinite(state.phi) ||
                !std::isfinite(state.dr) || !std::isfinite(state.dphi))
                break;

            // Update Cartesian position/velocity
            updatePosition(rayPosition, rayVelocity, state);
        }

        // Update trail
        glm::vec3 poss = glm::vec3(rayPosition.position, 0.0f);
        trail.trail.push_back(poss);
        if (trail.trail.size() > 200) trail.trail.erase(trail.trail.begin(), trail.trail.begin() + (trail.trail.size() - 200));
    }
}
