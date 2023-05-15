#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct RectCloth {
public:
    const float dx;
    const int nw;
    const int nh;
    const float width;
    const float height;
    const float m;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> netforces;

public:
    RectCloth(
        unsigned int nw,
        unsigned int nh,
        float dx,       // meter
        float totalm,   // kg
        glm::mat4 transform
    );
    ~RectCloth() = default;

    size_t idxFromCoord(int iw, int ih) { return ih * nw + iw; };
    glm::vec3& getPosition(size_t idx) { return this->positions[idx]; };
    glm::vec3& getPosition(int iw, int ih) { return this->positions[idxFromCoord(iw, ih)]; };

    void step(float timestep, float g, float k, float c);
};