#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include"simulator.h"



class FluidScene {
    friend class FluidSimulator;

public:
    const float dx;
    const int nw;
    const int nh;

    const float width;
    const float height;

    FluidSimulator* simulator;

public:
    FluidScene(int nw, int nh, float dx = 1.0f);
    ~FluidScene() = default;

    // Initialize background
    void setCircleAt(glm::vec2 center, float radius);
    void setBackground() {
        setCircleAt({ nw / 2, nh / 2 }, std::min(nw, nh) / 6.0f);
    }

    // Some functions that might be helpful
    unsigned int idxFromCoord(int iw, int ih) { return ih * nw + iw; }
    float getDensity(int iw, int ih) { return simulator->get_density(iw, ih); };
    glm::vec2 getVelocity(int iw, int ih) { return simulator->get_velocity(iw, ih); }
    
    void swap(float* &a, float* &b) {
		float* t = a;
		a = b;
		b = t;
	}
    void applyImpulsiveVelocity(glm::vec2 pos, glm::vec2 delta_vel);
    void step() {
        simulator->InletJetflow();
        swap(simulator->u, simulator->u_);
        simulator->diffuse_u();
        simulator->project_u();
        swap(simulator->u, simulator->u_);
        simulator->advect_u();
        simulator->project_u();
        swap(simulator->u, simulator->u_);
        simulator->advect_d();
        // simulator->diffuse_d();
        swap(simulator->d, simulator->d_);
    }

private:
    // Consider your data structure to save fluid properties...
};