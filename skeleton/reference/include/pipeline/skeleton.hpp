/**
 * A partial implmentation to show how to draw with opengl
 * this pipeline is intended to draw a skeleton with lines
 * but not finished since it is combined with interface of 
 * the class Skeleton
 * 
 * You can modify what ever you want or even delete and take
 * another brand new design on your own
 * */

#pragma once

#include <queue>
#include <algorithm>

#include <tiny_gltf.h>
#include <eigen3/Eigen/Geometry>

#include "camera/fpc.hpp"
#include "shader/shader.hpp"
#include "skeletal/skeleton.hpp"
#include "skeletal/animator.hpp"
#include "skeletal/mesh.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp> 

class WireframeSkeletonPipeline
{
private:
    typedef struct VertexData { // skeleton.vs
        glm::vec3 position = glm::vec3(0.0f); // global position of each joint
    } VertexData;

    struct GLO {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    } glo, mesh_glo;

    /* data structure for rendering */
    std::vector<unsigned int> indices;
    std::vector<VertexData> vertices;
    std::vector<VertexData> mesh_vertices;
    std::vector<glm::quat> animQuats;
    std::vector<glm::mat4> jointMats;

    Shader* shader;
    Shader* mesh_shader;
    FirstPersonCamera* camera;
    Skeleton* skeleton;
    SkeletalAnimator* animator;
    BoneWeightedMesh* mesh;

    

public:
    WireframeSkeletonPipeline(
        Shader* _shader,
        FirstPersonCamera* _camera,
        Skeleton* _skel,
        SkeletalAnimator* _anim,
        BoneWeightedMesh* _mesh,
        Shader* _mesh_shader
    );

    /* draw updated skeleton and mesh real time */
    void drawSkeleton();
    void drawMesh();

    /* interpolate quaternion and compute globalTrans for each joint */
    void computeJointPositions(float time);


private:
    /* set up skeleton render properties */
    void setupIndices();
    /* compute invBindMat for each joint */
    void computeJointInvBindMats();

    /* update Mesh and joints */
    /* you could choose to update skeleton and mesh here or others ways just you like */
    void updateVertices();
    void updateMeshes();

};