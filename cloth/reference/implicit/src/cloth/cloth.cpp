#include "cloth/cloth.hpp"

RectCloth::RectCloth(
    unsigned int nw,
    unsigned int nh,
    float dx,
    float mass,
    float timestep,
    glm::mat4 transform
) : nw(nw), nh(nh), dx(dx), width(nw * dx), height(nh * dx),
    m(mass / (nw * nh)), h(timestep),
    q(3 * nw * nh), fext(3 * nw * nh), q0(3 * nw * nh)
{
    fext = Eigen::VectorXf::Zero(3 * nw * nh);
    for (int ih = 0; ih < nh; ++ih) {
        for (int iw = 0; iw < nw; ++iw) {
            glm::vec3 init(
                transform * 
                glm::vec4(iw * dx - width / 2.0f, ih * dx - height / 2.0f, 0.0f, 1.0f)
            );
            q[3 * idxFromCoord(iw, ih)] = init.x;
            q[3 * idxFromCoord(iw, ih) + 1] = init.y;
            q[3 * idxFromCoord(iw, ih) + 2] = init.z;
            fext[3 * idxFromCoord(iw, ih) + 1] = -m * g;
        }
    }
    q0 = q;

    std::vector<std::pair<unsigned int, unsigned int>> fixcoords({
        std::pair<unsigned int, unsigned int>(0, nh - 1),
        std::pair<unsigned int, unsigned int>(nw - 1, nh - 1)
    });
    unsigned int fixn;
    for (const auto &fixc : fixcoords) {
        fixn = idxFromCoord(fixc.first, fixc.second);
        fixpoints[fixn] = Eigen::Vector3f(q[3 * fixn], q[3 * fixn + 1], q[3 * fixn + 2]);
    }

    initSprings();
    std::vector<Eigen::Triplet<float>> MTris, LTris, JTris;

    // compute M
    M.resize(3 * nw * nh, 3 * nw * nh);
    for (unsigned int i = 0; i < nw * nh; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == 1)
            MTris.push_back(
                Eigen::Triplet<float>(3 * i + j, 3 * i + j, m)
            );
        }
    }
    M.setFromTriplets(MTris.begin(), MTris.end());
    for (const auto &fixp : fixpoints) {
        M.coeffRef(3 * fixp.first, 3 * fixp.first) = 10000.0f;
        M.coeffRef(3 * fixp.first + 1, 3 * fixp.first + 1) = 10000.0f;
        M.coeffRef(3 * fixp.first + 2, 3 * fixp.first + 2) = 10000.0f;
    }

    // compute L
    L.resize(3 * nw * nh, 3 * nw * nh);
    for (const auto &i : springs) {
        for (int j = 0; j < 3; ++j) {
            LTris.push_back(
                Eigen::Triplet<float>(3 * i.first + j, 3 * i.first + j, k)
            );
            LTris.push_back(
                Eigen::Triplet<float>(3 * i.second + j, 3 * i.second + j, k)
            );
            LTris.push_back(
                Eigen::Triplet<float>(3 * i.first + j, 3 * i.second + j, -k)
            );
            LTris.push_back(
                Eigen::Triplet<float>(3 * i.second + j, 3 * i.first + j, -k)
            );
        }
    }
    L.setFromTriplets(LTris.begin(), LTris.end());

    // compute J
    unsigned int n = 0;
    J.resize(3 * nw * nh, 3 * springs.size());
    for (const auto &i : springs) {
        for (int j = 0; j < 3; ++j) {
            JTris.push_back(
                Eigen::Triplet<float>(3 * i.first + j, 3 * n + j, k)
            );
            JTris.push_back(
                Eigen::Triplet<float>(3 * i.second + j, 3 * n + j, -k)
            );
        }
        ++n;
    }
    J.setFromTriplets(JTris.begin(), JTris.end());

    d.resize(3 * springs.size());
    sysmat.compute(M + h * h * L);
}

void RectCloth::
step()
{
    Eigen::VectorXf My = M * ((1 + a) * q - a * q0);
    q0 = q;

    for (int iter = 0; iter < 1; iter++) {
        // compute d
        unsigned int n = 0;
        for (const auto &i : springs) {
            Eigen::Vector3f p(
                q[3 * i.first] - q[3 * i.second],
                q[3 * i.first + 1] - q[3 * i.second + 1],
                q[3 * i.first + 2] - q[3 * i.second + 2]
            );
            p.normalize();
            d[3 * n] = r[n] * p[0];
            d[3 * n + 1] = r[n] * p[1];
            d[3 * n + 2] = r[n] * p[2];
            ++n;
        }

        // update q
        q = sysmat.solve(My + h * h * (J * d + fext));
    }

    // for (const auto &fixp : fixpoints) {
    //     q[3 * fixp.first] = fixp.second[0];
    //     q[3 * fixp.first + 1] = fixp.second[1];
    //     q[3 * fixp.first + 2] = fixp.second[2];
    // }
}

void RectCloth::
initSprings()
{
    for (int ih = 0; ih < nh; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            springs.push_back(
                std::pair<unsigned int, unsigned int>(idxFromCoord(iw, ih), idxFromCoord(iw + 1, ih))
            );
            r.push_back(dx);
        }
    }
    for (int iw = 0; iw < nw; ++iw) {
        for (int ih = 0; ih < nh - 1; ++ih) {
            springs.push_back(
                std::pair<unsigned int, unsigned int>(idxFromCoord(iw, ih), idxFromCoord(iw, ih + 1))
            );
            r.push_back(dx);
        }
    }

    for (int ih = 0; ih < nh; ++ih) {
        for (int iw = 0; iw < nw - 2; ++iw) {
            springs.push_back(
                std::pair<unsigned int, unsigned int>(idxFromCoord(iw, ih), idxFromCoord(iw + 2, ih))
            );
            r.push_back(2.0f * dx);
        }
    }
    for (int iw = 0; iw < nw; ++iw) {
        for (int ih = 0; ih < nh - 2; ++ih) {
            springs.push_back(
                std::pair<unsigned int, unsigned int>(idxFromCoord(iw, ih), idxFromCoord(iw, ih + 2))
            );
            r.push_back(2.0f * dx);
        }
    }

    for (int ih = 0; ih < nh - 1; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            springs.push_back(
                std::pair<unsigned int, unsigned int>(idxFromCoord(iw, ih), idxFromCoord(iw + 1, ih + 1))
            );
            r.push_back(std::sqrt(2.0f) * dx);
        }
    }
    for (int ih = 1; ih < nh; ++ih) {
        for (int iw = 0; iw < nw - 1; ++iw) {
            springs.push_back(
                std::pair<unsigned int, unsigned int>(idxFromCoord(iw, ih), idxFromCoord(iw + 1, ih - 1))
            );
            r.push_back(std::sqrt(2.0f) * dx);
        }
    }
}
