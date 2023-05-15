#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/Dense>

struct RectCloth {
public:
    const float dx;
    const int nw;
    const int nh;
    const float width;
    const float height;
    Eigen::VectorXf q;      // current state q_n

public:
    RectCloth(
        unsigned int nw,
        unsigned int nh,
        float dx,           // meter
        float mass,         // kg
        float timestep,
        glm::mat4 transform
    );
    ~RectCloth() = default;

    unsigned int idxFromCoord(int iw, int ih) { return ih * nw + iw; };
    const glm::vec3 getPosition(size_t idx) {
        return glm::vec3(q[3 * idx], q[3 * idx + 1], q[3 * idx + 2]);
    };

    void step();
    void initSprings();

// class member for the solver
private:
    const float m;              // mass of each particle    | unit: kg
    const float h;              // timestep                 | unit: s
    const float g = 9.8f;       // gravity                  | unit: m/s^2
    const float k = 30.0f;      // stiffness                | unit N/m
    const float a = 0.993f;     // damping factor           | unit: 1

    Eigen::SparseMatrix<float> M;
    Eigen::SparseMatrix<float> L;
    Eigen::SparseMatrix<float> J;
    Eigen::VectorXf d;
    Eigen::VectorXf fext;       // external force
    Eigen::VectorXf q0;         // previous state q_n-1
    Eigen::SimplicialLLT<Eigen::SparseMatrix<float>> sysmat;        // M + h^2 L

    std::vector<std::pair<unsigned int, unsigned int>> springs;     // indices of springs
    std::vector<float> r;       // rest length of each spring

    std::unordered_map<unsigned int, Eigen::Vector3f> fixpoints;    // fixed points
};