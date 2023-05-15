/**
 * A design hint for you, modify whatever you want
 * You can also choose to design in another way
 * */
#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <tiny_gltf.h>
#include <glm/gtx/quaternion.hpp>

#include "shader/shader.hpp"

#define MAX_INFLUENCE_BONE_NUM 4

struct Joint {
    ///////////
    // Some other properties to save..?
    glm::vec3 nodeTransl;
    glm::quat nodeRotate;
    glm::vec3 nodeScale;
    glm::mat4 globalTrans = glm::mat4(1.0f);
    glm::mat4 invBindMat = glm::mat4(1.0f);
    int parent;
    std::vector<int> children_joints;
    ///////////
};

class Skeleton {
public:
    // Joint* root;
    std::vector<Joint> joints;
    int root_id = -1;


public:
    size_t getBoneNum() const { return this->joints.size(); }
    const auto& getJoints() const { return joints; }

    /** 
     * Some reference code to load data with tinygltf
     * Check the gltf 2.0 specification if you feel confused
     * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html 
     * */
    bool loadFromTinyGLTF(const tinygltf::Model& mdl, std::string& warn, std::string& err);

};