#include "Ray.h"

void Ray::draw()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);

    glVertex2f(position[0], position[1]);

    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);


    size_t N = trail.size();
    if (N >= 2) {

        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < N; ++i) {
            // older points (i=0) get alpha≈0, newer get alpha≈1
            float alpha = float(i) / float(N - 1);
            glColor4f(1.0f, 1.0f, 1.0f, std::max(alpha, 0.05f));
            glVertex2f(trail[i][0], trail[i][1]);
        }
    

    }
    glEnd();
    glDisable(GL_BLEND);


}

void Ray::step(float dt)
{   

    // basic euler, change to RK4
    position[0] += speed[0] * dt;
    position[1] += speed[1] * dt;

    if(trail.size()>=30)
        trail.erase(trail.begin());

    trail.push_back(position);

}
