#include "cloth/cloth.hpp"

RectCloth::RectCloth(
    unsigned int nw,
    unsigned int nh,
    float dx,
    float totalm,
    glm::mat4 transform
) : nw(nw), nh(nh), dx(dx), width(nw * dx), height(nh * dx), m(totalm / (nw * nh))
{
    for (int ih = 0; ih < nh; ++ih) {
        for (int iw = 0; iw < nw; ++iw) {
            glm::vec3 init {iw * dx - width / 2.0f, ih * dx - height / 2.0f, 0};
            positions.push_back(
                transform * glm::vec4(init, 1.0f)
            );
        }
    }
    velocities.resize(nw * nh, glm::vec3(0.0f));
    netforces.resize(nw * nh, glm::vec3(0.0f));
}

void RectCloth::
step(float timestep, float g, float k, float c)
{
    // fix two particles
    const glm::vec3 fixPos1 = getPosition(0, nh - 1);
    const glm::vec3 fixPos2 = getPosition(nw - 1, nh - 1);

    // init gravity and damping
    for (int i = 0; i < nw * nh; ++i) {
        netforces[i] = glm::vec3(0.0f, -m * g, 0.0f);
        netforces[i] -= c * velocities[i];
    }
    
    // add spring forces
    auto f = [&k](const glm::vec3 &p, const glm::vec3 &q, float l0) {
        return k * (l0 - glm::length(p - q)) * glm::normalize(p - q);
    };

    for (int ih = 0; ih < nh; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            const glm::vec3 &sf = f(getPosition(iw, ih), getPosition(iw + 1, ih), dx);
            netforces[idxFromCoord(iw, ih)] += sf;
            netforces[idxFromCoord(iw + 1, ih)] -= sf;
        }
    }
    for (int iw = 0; iw < nw; ++iw) {
        for (int ih = 0; ih < nh - 1; ++ih) {
            const glm::vec3 &sf = f(getPosition(iw, ih), getPosition(iw, ih + 1), dx);
            netforces[idxFromCoord(iw, ih)] += sf;
            netforces[idxFromCoord(iw, ih + 1)] -= sf;
        }
    }

    for (int ih = 0; ih < nh; ++ih) {
        for (int iw = 0; iw < nw - 2; ++iw) {
            const glm::vec3 &sf = f(getPosition(iw, ih), getPosition(iw + 2, ih), 2.0f * dx);
            netforces[idxFromCoord(iw, ih)] += sf;
            netforces[idxFromCoord(iw + 2, ih)] -= sf;
        }
    }
    for (int iw = 0; iw < nw; ++iw) {
        for (int ih = 0; ih < nh - 2; ++ih) {
            const glm::vec3 &sf = f(getPosition(iw, ih), getPosition(iw, ih + 2), 2.0f * dx);
            netforces[idxFromCoord(iw, ih)] += sf;
            netforces[idxFromCoord(iw, ih + 2)] -= sf;
        }
    }

    for (int ih = 0; ih < nh - 1; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            const glm::vec3 &sf = f(getPosition(iw, ih), getPosition(iw + 1, ih + 1), 1.4f * dx);
            netforces[idxFromCoord(iw, ih)] += sf;
            netforces[idxFromCoord(iw + 1, ih + 1)] -= sf;
        }
    }
    for (int ih = 1; ih < nh; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            const glm::vec3 &sf = f(getPosition(iw, ih), getPosition(iw + 1, ih - 1), 1.4f * dx);
            netforces[idxFromCoord(iw, ih)] += sf;
            netforces[idxFromCoord(iw + 1, ih - 1)] -= sf;
        }
    }

    // update velocities and positions
    for (int i = 0; i < nw * nh; ++i) {
        velocities[i] += timestep / m * netforces[i];
        positions[i] += timestep * velocities[i];
    }
    
    positions[idxFromCoord(0, nh - 1)] = fixPos1;
    positions[idxFromCoord(nw - 1, nh - 1)] = fixPos2;
}