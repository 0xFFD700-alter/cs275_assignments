#include "skeletal/skeleton.hpp"

#include <array>
#include <iostream>
#include "gltf/tinygltf_helper.h"

bool Skeleton::
loadFromTinyGLTF(
    const tinygltf::Model& mdl, 
    std::string& warn, 
    std::string& err
) {
    // if (mdl.scenes[0].nodes.size() == 0) {
    //     err = "No root node in file";
    //     return false;
    // }

    // auto rootNode = mdl.nodes[mdl.scenes[0].nodes[0]];

    // if (rootNode.rotation.size() == 0) {
    //     originRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    // } else if (rootNode.rotation.size() >= 4){
    //     float x = static_cast<float>(rootNode.rotation[0]);
    //     float y = static_cast<float>(rootNode.rotation[1]);
    //     float z = static_cast<float>(rootNode.rotation[2]);
    //     float w = static_cast<float>(rootNode.rotation[3]);

    //     // Save into your own structure
    //     originRotate = glm::quat(w, x, y, z);
    //     /////////
    //     std::cout << "Origin Quaternion: " << x << " " << y << " " << z << " " << w << std::endl;
    // }
    
    if (mdl.skins.size() == 0) {
        err = "No skeleton data in file.";
        return false;
    }
    
    const tinygltf::Skin& skin = mdl.skins[0];

    if (skin.joints.size() == 0) {
        err = "No joints specified in skeleton.";
        return false;
    }

    // Find our root node.
    root_id = skin.skeleton;

    std::cout << "root id is: " << skin.skeleton << std::endl;

    if (root_id == -1) {
        // Some gltf files might not explicitly specify a root.
        // To make FK work, we need to make an educated guess.
        warn += "\nFile does not specify a root skeleton node. " \
                "Assuming first node index in `skin.joints` as root.";
        root_id = skin.joints[0];
    }


    this->joints.clear();
    this->joints.resize(skin.joints.size());
    for (size_t i = 0; i < skin.joints.size(); ++i) {
        //The current joint...
        int j_id = skin.joints[i];
        auto curNode = mdl.nodes[j_id];
        // glm::mat4 trans = glm::mat4(1.0f);
        // glm::mat4 scale = glm::mat4(1.0f);
       
        std::cout << "Skeleton Joints " << j_id << " Loaded: " << curNode.name << std::endl;
        if (curNode.translation.size() == 0) {
            joints[j_id].nodeTransl = glm::vec3(1.0f);
        } else if (curNode.translation.size() >= 3) {
            float x = static_cast<float>(curNode.translation[0]);
            float y = static_cast<float>(curNode.translation[1]);
            float z = static_cast<float>(curNode.translation[2]);
            
            /////////
            // Save into your own structure
            joints[j_id].nodeTransl = glm::vec3(x, y, z);
            /////////
            std::cout << "Position: " << x << " " << y << " " << z << std::endl;
        }

        if (curNode.rotation.size() == 0) {
            joints[j_id].nodeRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        } else if (curNode.rotation.size() >= 4) {
            // WARN: gltf: xyzw, eigen/glm: wxyz

            float x = static_cast<float>(curNode.rotation[0]);
            float y = static_cast<float>(curNode.rotation[1]);
            float z = static_cast<float>(curNode.rotation[2]);
            float w = static_cast<float>(curNode.rotation[3]);

            /////////
            // Save into your own structure
            joints[j_id].nodeRotate = glm::quat(w, x, y, z);
            /////////
            std::cout << "Quaternion: " << x << " " << y << " " << z << " " << w << std::endl;
        }

        if (curNode.scale.size() == 0) {
            joints[j_id].nodeScale = glm::vec3(1.0f);
        } else if (curNode.scale.size() >= 3) {
            float x = static_cast<float>(curNode.scale[0]);
            float y = static_cast<float>(curNode.scale[1]);
            float z = static_cast<float>(curNode.scale[2]);

            /////////
            // Save into your own structure
            joints[j_id].nodeScale = glm::vec3(x, y, z);
            /////////
            std::cout << "Scaling: " << x << " " << y << " " << z << std::endl;
        }

        if (curNode.children.size() == 0) {
            // vector children_joints is empty
        } else {
            joints[j_id].children_joints = curNode.children;
            for (size_t j = 0; j < curNode.children.size(); ++j) {
                joints[curNode.children[j]].parent = j_id;
            }

            std::cout << "Number of children: " << curNode.children.size() << std::endl;
        }

        // For some glTF files, the translations and rotations are saved using a single transform matrix curNode.matrix
        // however we ignore the format since it requires a matrix decomposition to get quaternion and translation seperatedly
        // if (curNode.matrix.size() == 16) {}
            
        // For some glTF files, there are inverseBindMatrix saved with each joints
        // we ignore it since we are always computing the inverseBindMatrix ourselves with initially defined joints
        /**
         * auto invBindGetter = tinygltf_buildDataGetter(mdl, skin.inverseBindMatrices);
         * if (invBindGetter.elementSize != 16 * sizeof(float)) {}
         * */

    }
    return true;
}


