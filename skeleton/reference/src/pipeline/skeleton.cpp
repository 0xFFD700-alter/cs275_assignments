#include "pipeline/skeleton.hpp"
#include <iostream>

WireframeSkeletonPipeline::
WireframeSkeletonPipeline(
    Shader* _shader,
    FirstPersonCamera* _camera,
    Skeleton* _skel,
    SkeletalAnimator* _anim,
    BoneWeightedMesh* _mesh,
    Shader* _mesh_shader
) {

    this->shader = _shader;
    this->camera = _camera;
    this->skeleton = _skel;
    this->animator = _anim;
    this->mesh = _mesh;
    this->mesh_shader = _mesh_shader;
    
    glGenVertexArrays(1, &this->glo.VAO); // Allocate a Vertex Array Object to manage data
    glGenBuffers(1, &this->glo.VBO); // Allocate a Vertex Buffer Object to save vertex data
    glGenBuffers(1, &this->glo.EBO); // Allocate an Element Buffer Object to save indices data

    glBindVertexArray(this->glo.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glo.EBO); // Bound to VAO
    
    glBindBuffer(GL_ARRAY_BUFFER, this->glo.VBO); // WARN: A global binding instead of binding to VAO

    // Define how to send data into layout 0
    // Check "res/shader/skeleton.vs"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    
    glBindVertexArray(0); // unbind VAO


    //setup mesh render properties;
    glGenVertexArrays(1, &this->mesh_glo.VAO); //allocate a Vertex Array Object to mesh data
    glGenBuffers(1, &this->mesh_glo.VBO); // allocate a Vertex Buffer Object to save mesh vertex data;
    glGenBuffers(1, &this->mesh_glo.EBO); //allocate a Element Buffer Object to save mash indices data;

    //Bind Mesh VAO
    glBindVertexArray(this->mesh_glo.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mesh_glo.EBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->mesh_glo.VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mesh->indices.size() * sizeof(unsigned int), this->mesh->indices.data(), GL_STATIC_DRAW);
  
    glBindVertexArray(0); // unbind VAO
    mesh_vertices.resize(mesh->positions.size());
    animQuats.resize(skeleton->joints.size());
    jointMats.resize(skeleton->joints.size()); 
    this->setupIndices();
    this->computeJointInvBindMats();

    auto &joints = skeleton->joints;
    for (size_t i = 0; i < animQuats.size(); ++i) {
        animQuats[i] = joints[i].nodeRotate;
    }
}

void WireframeSkeletonPipeline::
setupIndices() {
    // Load indices to render a skeleton
    const auto& joints = this->skeleton->getJoints();

    this->indices.clear();
    this->vertices.resize(joints.size());

    // GL_LINES mode, two indice as a line
    for (int i = 0; i < joints.size(); ++i) {
        for (int j = 0; j < joints[i].children_joints.size(); j++) {
            this->indices.push_back(i);
            this->indices.push_back(joints[i].children_joints[j]);

        }
    }

    // Copy the indice to EBO since they are fixed
    glBindVertexArray(this->glo.VAO); // EBO is bound to VAO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0); // unbind VAO
}

void WireframeSkeletonPipeline::
updateVertices() {
    // !compute vertices.position (global position) for each joint
    // !binary search for linear interpolation
    const auto &joints = skeleton->getJoints();
    for (size_t i = 0; i < joints.size(); ++i) {
        vertices[i].position.x = joints[i].globalTrans[3][0];
        vertices[i].position.y = joints[i].globalTrans[3][1];
        vertices[i].position.z = joints[i].globalTrans[3][2];
    }
}

void WireframeSkeletonPipeline::
drawSkeleton() {

    GLint previous[2];
    glGetIntegerv(GL_POLYGON_MODE, previous); // save previous drawing mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // we want wire-frame mode

    glBindVertexArray(this->glo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->glo.VBO);

    // update and copy the new vertex data into VBO
    updateVertices();

    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexData), this->vertices.data(), GL_STREAM_DRAW);
    
    this->shader->use();
    this->camera->applyToShader(this->shader);

    // Draw using indices 
    glDrawElements(GL_LINES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Hint: when drawing mesh, you might want to use a draw mode of GL_TRIANGLES
    // For other drawing modes, check
    // https://www.evl.uic.edu/julian/cs488/2005-09-01/fig2-6.gif

    glPolygonMode(GL_FRONT_AND_BACK, *previous); // restore previous drawing mode
}

void WireframeSkeletonPipeline::
updateMeshes() {
    // !perform linear blend skinning
    // !copy data from mesh->positions to this->mesh_vertices.position
    const auto &joints = skeleton->getJoints();
    const auto &influences = mesh->influences;
    const auto &weights = mesh->weights;
    glm::mat4 skinMat;
    glm::vec4 worldPos;

    for (size_t i = 0; i < joints.size(); ++i) {
        jointMats[i] = joints[i].globalTrans * joints[i].invBindMat;
    }

    for (size_t i = 0; i < mesh->positions.size(); ++i) {
        skinMat = 
            weights[i].y * jointMats[influences[i].y] + 
            weights[i].z * jointMats[influences[i].z] + 
            weights[i].x * jointMats[influences[i].x] + 
            weights[i].w * jointMats[influences[i].w];
        worldPos = skinMat * glm::vec4(mesh->positions[i], 1.0f);
        mesh_vertices[i].position = glm::vec3(worldPos.x, worldPos.y, worldPos.z);
    }
}

void WireframeSkeletonPipeline::
drawMesh() {

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(this->mesh_glo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->mesh_glo.VBO);

    // update and copy the new vertex data into VBO
    updateMeshes();

    glBufferData(GL_ARRAY_BUFFER, this->mesh_vertices.size() * sizeof(VertexData), this->mesh_vertices.data(), GL_STREAM_DRAW);
    this->mesh_shader->use();
    this->camera->applyToShader(this->mesh_shader);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(this->mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void WireframeSkeletonPipeline::
computeJointInvBindMats() {
    auto &joints = skeleton->joints;
    int cur_id = -1;
    int child_id = skeleton->root_id;
    glm::mat4 invTrans = glm::mat4(1.0f);
    glm::mat4 invScale = glm::mat4(1.0f);
    glm::quat invRotate;
    invTrans = glm::translate(invTrans, -joints[child_id].nodeTransl);
    invScale = glm::scale(invScale, glm::vec3(1.0f) / joints[child_id].nodeScale);
    invRotate = glm::quat(glm::conjugate(joints[child_id].nodeRotate));
    joints[child_id].invBindMat = invScale * glm::mat4_cast(invRotate) * invTrans;
    
    std::queue<int> q;
    q.push(skeleton->root_id);
    while (!q.empty()) {
        cur_id = q.front();
        for (size_t i = 0; i < joints[cur_id].children_joints.size(); ++i) {
            child_id = joints[cur_id].children_joints[i];
            invTrans = glm::mat4(1.0f);
            invScale = glm::mat4(1.0f);
            invTrans = glm::translate(invTrans, -joints[child_id].nodeTransl);
            invScale = glm::scale(invScale, glm::vec3(1.0f) / joints[child_id].nodeScale);
            invRotate = glm::quat(glm::conjugate(joints[child_id].nodeRotate));
            joints[child_id].invBindMat = invScale * glm::mat4_cast(invRotate) * invTrans * joints[cur_id].invBindMat;
            q.push(child_id);
        }
        q.pop();
    }
}

void WireframeSkeletonPipeline::
computeJointPositions(float time) {
    // !update globalTrans for each joint
    auto &joints = skeleton->joints;
    const auto &keyframes = animator->keyframe_states;
    float intrplT;
    for (size_t i = 0; i < animQuats.size(); ++i) {
        for (size_t j = 0; j < keyframes[i].size(); ++j) {
            if (time < keyframes[i][j].timestamp) {
                if (j == 0) {
                    intrplT = time / keyframes[i][j].timestamp;
                    animQuats[i] = joints[i].nodeRotate * (1 - intrplT) + keyframes[i][j].rotationData * intrplT;
                } else {
                    intrplT = (time - keyframes[i][j - 1].timestamp) / (keyframes[i][j].timestamp - keyframes[i][j - 1].timestamp);
                    animQuats[i] = keyframes[i][j - 1].rotationData * (1 - intrplT) + keyframes[i][j].rotationData * intrplT;
                }
                break;
            }
        }
    }

    int cur_id = -1;
    int child_id = skeleton->root_id;
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    trans = glm::translate(trans, joints[child_id].nodeTransl);
    scale = glm::scale(scale, joints[child_id].nodeScale);
    joints[child_id].globalTrans = trans * glm::mat4_cast(animQuats[child_id]) * scale;
    
    std::queue<int> q;
    q.push(skeleton->root_id);
    while (!q.empty()) {
        cur_id = q.front();
        for (size_t i = 0; i < joints[cur_id].children_joints.size(); ++i) {
            child_id = joints[cur_id].children_joints[i];
            trans = glm::mat4(1.0f);
            scale = glm::mat4(1.0f);
            trans = glm::translate(trans, joints[child_id].nodeTransl);
            scale = glm::scale(scale, joints[child_id].nodeScale);
            joints[child_id].globalTrans = joints[cur_id].globalTrans * trans * glm::mat4_cast(animQuats[child_id]) * scale;
            q.push(child_id);
        }
        q.pop();
    }
}
