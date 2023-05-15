#pragma once
#include <glm/vec2.hpp>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>

class FluidSimulator {
public:
	unsigned nw_;
	unsigned nh_;
	float visc = 0.0f;	// viscosity
	unsigned y1 = 20;	// lower boundary of jetflow
	unsigned y2 = 30;	// upper boundary of jetflow
	float jetv = 5.0f;	// jetflow velocity

	float* u_;			// velocity (current timestamp)
	float* uh;			// velocity (1/2 next timestamp)
	float* u;			// velocity (next timestamp)

	float* d_;			// density (current timestamp)
	float* d;			// density (next timestamp)


public:
	FluidSimulator(int nw, int nh, float dx);
	unsigned int idx(int x, int y) { return y * nw_ + x; }				// return density idx
	unsigned int idxx(int x, int y) { return 2 * (y * nw_ + x); }		// return even idx
	unsigned int idxy(int x, int y) { return 2 * (y * nw_ + x) + 1; }	// return odd  idx

	void advect_u();
	void advect_uh();
	void advect_d();
	void project_u();
	void project_uh();
	void diffuse_u();
	void diffuse_d();
	void InletJetflow();
	void ApplyDeltaVelocity(int x, int y, glm::vec2 delta_velocity) {
		u_[idxx(x, y)] += delta_velocity.x;
		u_[idxy(x, y)] += delta_velocity.y;
	//	std::cout << u_[idxx(x, y)] << " " << u_[idxy(x, y)] << std::endl;
	}

	glm::vec2 get_velocity(int x, int y) {
		return glm::vec2(u_[idxx(x, y)], u_[idxy(x, y)]);
	}
	float get_density(int x, int y) {
		return d_[idx(x, y)];
	}

private:
	typedef Eigen::VectorXf VectorXf;
	typedef Eigen::SparseMatrix<float> SparseMatrix;
	typedef Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> SimplicialLDLT;
	// typedef Eigen::ConjugateGradient<Eigen::SparseMatrix<float>, Eigen::Lower | Eigen::Upper> ConjugateGradient;
	typedef Eigen::Map<Eigen::VectorXf> Map;
	typedef Eigen::Triplet<float> Triplet;
	
	SparseMatrix proj;
	SparseMatrix div;
	SparseMatrix grad;
	SparseMatrix diff_u;
	SparseMatrix diff_d;
	SimplicialLDLT projSolver;
	SimplicialLDLT diffSolver_u;
	SimplicialLDLT diffSolver_d;
	// ConjugateGradient projSolver;
	// ConjugateGradient diffSolver_u;
	// ConjugateGradient diffSolver_d;
};
