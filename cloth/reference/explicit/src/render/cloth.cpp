#include "render/cloth.hpp"

#include <iostream>
#include <numbers>

RectClothRenderer::
RectClothRenderer(
    Shader* shader,
    FirstPersonCamera* camera,
    RectCloth* cloth
) {
    this->shader = shader;
    this->camera = camera;
    this->cloth = cloth;

    this->initVertices();
    this->initIndices();
    this->updateNormals();

    this->glo.initData(); // set pointer here
}

void RectClothRenderer::
draw() {
    this->updatePositions();
    this->updateNormals();

    // Update Data
    glBindBuffer(GL_ARRAY_BUFFER, glo.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * this->glo.vertices.size(), this->glo.vertices.data(), GL_STREAM_DRAW);

    GLint previous[2];
    glGetIntegerv(GL_POLYGON_MODE, previous);

    this->shader->use();
    this->shader->setMat4("Projection", this->camera->getProjection());
    this->shader->setMat4("View", this->camera->getView());
    this->shader->setVec3("CameraPos", this->camera->getCameraPos());

    glBindVertexArray(glo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, glo.VBO);

    this->shader->setBool("DrawLine", false);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FLAT); // We want flat mode
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(glo.indices.size()), GL_UNSIGNED_INT, 0);

    // this->shader->setBool("DrawLine", true);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // We want line mode
    // glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(glo.indices.size()), GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, *previous); // restore previous mode
}

void RectClothRenderer::
initVertices() {
    const int nh = this->cloth->nh;
    const int nw = this->cloth->nw;
    const size_t total = nh * nw;

    this->glo.vertices.clear();
    this->glo.vertices.resize(total);

    for (size_t i = 0; i < total; ++i) {
        this->glo.vertices[i].position = this->cloth->getPosition(i);
    }
}


void RectClothRenderer::
initIndices() {
    this->glo.indices.clear();

    const int nh = this->cloth->nh;
    const int nw = this->cloth->nw;

    this->glo.indices.reserve((nh - 1) * (nw - 1) * 6);
    for (int ih = 0; ih < nh - 1; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            size_t leftDownIdx = this->cloth->idxFromCoord(iw, ih);
            size_t rightDownIdx = this->cloth->idxFromCoord(iw + 1, ih);
            size_t leftUpIdx = this->cloth->idxFromCoord(iw, ih + 1);
            size_t rightUpIdx = this->cloth->idxFromCoord(iw + 1, ih + 1);

            this->glo.indices.push_back(leftDownIdx);
            this->glo.indices.push_back(rightDownIdx);
            this->glo.indices.push_back(rightUpIdx);

            this->glo.indices.push_back(leftDownIdx);
            this->glo.indices.push_back(rightUpIdx);
            this->glo.indices.push_back(leftUpIdx);
        }
    }
}

void RectClothRenderer::
updatePositions() {
    const int nh = this->cloth->nh;
    const int nw = this->cloth->nw;
    const size_t total = nh * nw;

    for (size_t i = 0; i < total; ++i) {
        this->glo.vertices[i].position = this->cloth->getPosition(i);
    }
}

void RectClothRenderer::
updateNormals() {
    const int nh = this->cloth->nh;
    const int nw = this->cloth->nw;
    const size_t total = nh * nw;

    for (size_t i = 0; i < total; ++i) {
        this->glo.vertices[i].normal = glm::vec3(0);
    }

    for (int ih = 0; ih < nh - 1; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            size_t leftDownIdx = this->cloth->idxFromCoord(iw, ih);
            size_t rightDownIdx = this->cloth->idxFromCoord(iw + 1, ih);
            size_t leftUpIdx = this->cloth->idxFromCoord(iw, ih + 1);
            size_t rightUpIdx = this->cloth->idxFromCoord(iw + 1, ih + 1);

            size_t lower[3] = {leftDownIdx, rightDownIdx, rightUpIdx};
            size_t upper[3] = {leftDownIdx, rightUpIdx, leftUpIdx};
            glm::vec3 lowerNormal = calcNormal(
                this->glo.vertices[leftDownIdx].position,
                this->glo.vertices[rightDownIdx].position,
                this->glo.vertices[rightUpIdx].position
            );
            glm::vec3 upperNormal = calcNormal(
                this->glo.vertices[leftDownIdx].position,
                this->glo.vertices[rightUpIdx].position,
                this->glo.vertices[leftUpIdx].position
            );
            for (int i = 0; i < 3; ++i) {
                this->glo.vertices[lower[i]].normal += lowerNormal;
                this->glo.vertices[upper[i]].normal += upperNormal;
            }
        }
    }

    for (size_t i = 0; i < total; ++i) {
        this->glo.vertices[i].normal = glm::normalize(this->glo.vertices[i].normal);
    }
}