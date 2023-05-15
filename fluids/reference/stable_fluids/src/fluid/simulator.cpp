#include "../include/fluid/simulator.h"

FluidSimulator::
FluidSimulator(int nw, int nh, float dx)
: umap(NULL, 0), dmap(NULL, 0)
{
    nw_ = nw;
    nh_ = nh;
    u_ = new float[2 * nw * nh];	// 2 * nw * nh: u1x, u1y, u2x, u2y,... 
    u = new float[2 * nw * nh];
    d_ = new float[nw * nh];		// nw * nh: d1, d2,...
    d = new float[nw * nh];

    for (int y = 0; y < nh_; y++) {
        for (int x = 0; x < nw_; x++) {
            u_[idxx(x, y)] = 0.f;			// ux -> even: 0, 2, 4..
            u_[idxy(x, y)] = 0.f;			// uy -> odd : 1, 3, 5..
            u[idxx(x, y)] = 0.f;			// ux -> even: 0, 2, 4...
            u[idxy(x, y)] = 0.f;			// uy -> odd : 1, 3, 5...
            d_[idx(x, y)] = 0.f;
            d[idx(x, y)] = 0.f;
        }
    }

    std::vector<Triplet> TripletList;
    // build (negative) div matrix
    for (int y = 0; y < nh_; y++) {
        for (int x = 0; x < nw_; x++) {
            if (x + 1 <= nw_ - 1)
                TripletList.push_back(Triplet(idx(x, y), idxx(x + 1, y), -0.5f));
            if (x - 1 >= 0)
                TripletList.push_back(Triplet(idx(x, y), idxx(x - 1, y), 0.5f));
            if (y + 1 <= nh_ - 1)
                TripletList.push_back(Triplet(idx(x, y), idxy(x, y + 1), -0.5f));
            if (y - 1 >= 0)
                TripletList.push_back(Triplet(idx(x, y), idxy(x, y - 1), 0.5f));
        }
    }
    div.resize(nw_ * nh_, 2 * nw_ * nh_);
    div.setFromTriplets(TripletList.begin(), TripletList.end());
    TripletList.clear();
    // build grad matrix
    for (int y = 0; y < nh_; y++) {
        for (int x = 0; x < nw_; x++) {
            if (x + 1 <= nw_ - 1)
                TripletList.push_back(Triplet(idxx(x, y), idx(x + 1, y), 0.5f));
            if (x - 1 >= 0)
                TripletList.push_back(Triplet(idxx(x, y), idx(x - 1, y), -0.5f));
            if (y + 1 <= nh_ - 1)
                TripletList.push_back(Triplet(idxy(x, y), idx(x, y + 1), 0.5f));
            if (y - 1 >= 0)
                TripletList.push_back(Triplet(idxy(x, y), idx(x, y - 1), -0.5f));
        }
    }
    grad.resize(2 * nw_ * nh_, nw_ * nh_);
    grad.setFromTriplets(TripletList.begin(), TripletList.end());
    TripletList.clear();
    // build proj matrix
    for (int y = 0; y < nh_; y++) {
        for (int x = 0; x < nw_; x++) {
            TripletList.push_back(Triplet(idx(x, y), idx(x, y), 4.0f));
            if (x + 1 <= nw_ - 1)
                TripletList.push_back(Triplet(idx(x, y), idx(x + 1, y), -1.0f));
            if (x - 1 >= 0)
                TripletList.push_back(Triplet(idx(x, y), idx(x - 1, y), -1.0f));
            if (y + 1 <= nh_ - 1)
                TripletList.push_back(Triplet(idx(x, y), idx(x, y + 1), -1.0f));
            if (y - 1 >= 0)
                TripletList.push_back(Triplet(idx(x, y), idx(x, y - 1), -1.0f));
        }
    }
    proj.resize(nw_ * nh_, nw_ * nh_);
    proj.setFromTriplets(TripletList.begin(), TripletList.end());
    TripletList.clear();
    // build diff_u matrix
    for (int y = 0; y < nh_; y++) {
        for (int x = 0; x < nw_; x++) {
            TripletList.push_back(Triplet(idxx(x, y), idxx(x, y), 1.0f + 4.0f * visc));
            TripletList.push_back(Triplet(idxy(x, y), idxy(x, y), 1.0f + 4.0f * visc));
            if (x + 1 <= nw_ - 1) {
                TripletList.push_back(Triplet(idxx(x, y), idxx(x + 1, y), -1.0f * visc));
                TripletList.push_back(Triplet(idxy(x, y), idxy(x + 1, y), -1.0f * visc));
            }
            if (x - 1 >= 0) {
                TripletList.push_back(Triplet(idxx(x, y), idxx(x - 1, y), -1.0f * visc));
                TripletList.push_back(Triplet(idxy(x, y), idxy(x - 1, y), -1.0f * visc));
            }
            if (y + 1 <= nh_ - 1) {
                TripletList.push_back(Triplet(idxx(x, y), idxx(x, y + 1), -1.0f * visc));
                TripletList.push_back(Triplet(idxy(x, y), idxy(x, y + 1), -1.0f * visc));
            }
            if (y - 1 >= 0) {
                TripletList.push_back(Triplet(idxx(x, y), idxx(x, y - 1), -1.0f * visc));
                TripletList.push_back(Triplet(idxy(x, y), idxy(x, y - 1), -1.0f * visc));
            }
        }
    }
    diff_u.resize(2 * nw_ * nh_, 2 * nw_ * nh_);
    diff_u.setFromTriplets(TripletList.begin(), TripletList.end());
    TripletList.clear();
    // build diff_d matrix
    for (int y = 0; y < nh_; y++) {
        for (int x = 0; x < nw_; x++) {
            TripletList.push_back(Triplet(idx(x, y), idx(x, y), 1.0f + 4.0f * visc));
            if (x + 1 <= nw_ - 1) {
                TripletList.push_back(Triplet(idx(x, y), idx(x + 1, y), -1.0f * visc));
            }
            if (x - 1 >= 0) {
                TripletList.push_back(Triplet(idx(x, y), idx(x - 1, y), -1.0f * visc));
            }
            if (y + 1 <= nh_ - 1) {
                TripletList.push_back(Triplet(idx(x, y), idx(x, y + 1), -1.0f * visc));
            }
            if (y - 1 >= 0) {
                TripletList.push_back(Triplet(idx(x, y), idx(x, y - 1), -1.0f * visc));
            }
        }
    }
    diff_d.resize(nw_ * nh_, nw_ * nh_);
    diff_d.setFromTriplets(TripletList.begin(), TripletList.end());
    TripletList.clear();

    projSolver.compute(proj);
    diffSolver_u.compute(diff_u);
    diffSolver_d.compute(diff_d);
    // projSolver.setMaxIterations(100);
    // diffSolver_u.setMaxIterations(50);
    // diffSolver_d.setMaxIterations(50);
}

void FluidSimulator::
advect_u() {
    float px, py;
    float iX, iY;

    for (int y = 0; y < nh_; ++y) {
        for (int x = 0; x < nw_; ++x) {
            px = (float)x - u_[idxx(x, y)];     // timestep = 1
            py = (float)y - u_[idxy(x, y)];     // timestep = 1
            if (px < 0 && py > y1 && py < y2) {
                u[idxx(x, y)] = jetv;
                u[idxy(x, y)] = 0.0f;
                continue;
            }
            if (px < 0 || px >= nw_ - 1 || py < 0 || py >= nh_ - 1) {
                u[idxx(x, y)] = 0.0f;
                u[idxy(x, y)] = 0.0f;
                continue;
            }
            iX = px - (int)px;
            iY = py - (int)py;

            // interpolate
            // 00------x------01
            // |       |       |
            // |       |       |
            // |       x       |
            // |       |       |
            // |       |       |
            // 10------x------11

            u[idxx(x, y)] = (1 - iY) * ((1 - iX) * u_[idxx((int)px, (int)py)] + iX * u_[idxx((int)px + 1, (int)py)]) + iY * ((1 - iX) * u_[idxx((int)px, (int)py + 1)] + iX * u_[idxx((int)px + 1, (int)py + 1)]);
            u[idxy(x, y)] = (1 - iY) * ((1 - iX) * u_[idxy((int)px, (int)py)] + iX * u_[idxy((int)px + 1, (int)py)]) + iY * ((1 - iX) * u_[idxy((int)px, (int)py + 1)] + iX * u_[idxy((int)px + 1, (int)py + 1)]);
        }
    }
}

void FluidSimulator::
advect_d() {
    float px, py;
    float iX, iY;

    for (int y = 0; y < nh_; ++y) {
        for (int x = 0; x < nw_; ++x) {
            px = (float)x - u_[idxx(x, y)];     // timestep = 1
            py = (float)y - u_[idxy(x, y)];     // timestep = 1
            if (px < 0 && py > y1 && py < y2) {
                d[idx(x, y)] = 1.0f;
                continue;
            }
            if (px < 0 || px >= nw_ - 1 || py < 0 || py >= nh_ - 1) {
                d[idx(x, y)] = 0.0f;
                continue;
            }
            iX = px - (int)px;
            iY = py - (int)py;

            // interpolate
            // 00------x------01
            // |       |       |
            // |       |       |
            // |       x       |
            // |       |       |
            // |       |       |
            // 10------x------11

            d[idx(x, y)] = (1 - iY) * ((1 - iX) * d_[idx((int)px, (int)py)] + iX * d_[idx((int)px + 1, (int)py)]) + iY * ((1 - iX) * d_[idx((int)px, (int)py + 1)] + iX * d_[idx((int)px + 1, (int)py + 1)]);
        }
    }
}

void FluidSimulator::
project_u() {
    new (&umap) Map(u, 2 * nw_ * nh_);
    umap -= grad * projSolver.solve(div * umap);
}

void FluidSimulator::
diffuse_u() {
    new (&umap) Map(u, 2 * nw_ * nh_);
    umap = diffSolver_u.solve(umap);
}

void FluidSimulator::
diffuse_d() {
    new (&dmap) Map(d, nw_ * nh_);
    dmap = diffSolver_d.solve(dmap);
}

void FluidSimulator::
InletJetflow() {
    for (int y = y1 + 1; y < y2; ++y) {
        u_[idxx(0, y)] = jetv;
        u_[idxy(0, y)] = 0.0f;
        d_[idx(0, y)] = 1.0f;
    }
}