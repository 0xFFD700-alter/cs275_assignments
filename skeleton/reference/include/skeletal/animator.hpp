/**
 * A design hint for you, modify anything you want
 * You can also choose to design in another way
 * */

#pragma once

#include <set>

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <glm/gtx/quaternion.hpp>

#include "skeletal/skeleton.hpp"

class SkeletalAnimator
{
    ////////////
    // You might need a class to save keyframes 
    // generate animations by interpolation
    ////////////'
public:
    typedef struct KeyframeState {
        float timestamp;
        glm::quat rotationData = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    } KeyframeState;

    std::vector<std::vector<KeyframeState>> keyframe_states;
    // std::vector<float> times;

public:
    /** 
     * Some reference code to load data with tinygltf
     * Check the gltf 2.0 specification if you feel confused
     * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html 
     * */
    bool loadFromTinyGLTF(
        const tinygltf::Model& mdl,
        const Skeleton& skel,
        std::string& warn,
        std::string& err
    );
};