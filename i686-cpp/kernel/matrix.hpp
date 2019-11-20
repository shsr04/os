#pragma once
#include "core.hpp"

namespace matrix {

template <int N, int M> using matrix = array<array<double, M>, N>;

template <int N> class LU_decomposition {
    matrix<N, N> lu;
    array<int, N> perm;

  public:
    LU_decomposition(matrix<N, N> p) : lu(move(p)) {
        array<double, N> scaling;
        int parity = 1;
        for (auto i : range(0,N)) {
            double big = 0.0;
            for (auto j : range(0,N)) {
                if (abs(lu[i][j]) > big)
                    big = abs(lu[i][j]);
            }
            scaling[i] = 1. / big;
        }
        int imax = -1;
        for (auto k : range(0, N)) {
            double big = 0.0;
            for (auto i : range(k, N)) {
                double tmp = scaling[i] * abs(lu[i][k]);
                if (tmp > big) {
                    big = tmp;
                    imax = i;
                }
            }
            if (k != imax) {
                for (auto j : range(0, N)) {
                    double tmp = lu[imax][j];
                    lu[imax][j] = lu[k][j];
                    lu[k][j] = tmp;
                }
                parity *= -1;
                scaling[imax] = scaling[k];
            }
            perm[k] = imax;
            if (lu[k][k] == 0.0)
                lu[k][k] = 1e-7;
            for (auto i : range(k + 1, N)) {
                lu[i][k] /= lu[k][k];
                double tmp = lu[i][k];
                for (auto j : range(k + 1, N)) {
                    lu[i][j] -= tmp * lu[k][j];
                }
            }
        }
    }

    auto solve(array<double, N> b) {
        array<double, N> r(b);
        double sum = 0;
        int ii = 0;
        for (auto i : range(0, N)) {
            auto ip = perm[i];
            sum = r[ip];
            r[ip] = r[i];
            if (ii > 0) {
                for (auto j : range(ii - 1, i))
                    sum -= lu[i][j] * r[j];
            } else if (sum != 0.0) {
                ii = i + 1;
            }
            r[i] = sum;
        }
        for (auto i = N - 1; i >= 0; i--) {
            sum = r[i];
            for (auto j : range(i + 1, N))
                sum -= lu[i][j] * r[j];
            r[i] = sum / lu[i][i];
        }
        return r;
    }
};

} // namespace matrix
