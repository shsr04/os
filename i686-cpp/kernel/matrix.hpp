#pragma once
#include "core.hpp"

namespace matrix {

template <int N, int M> using matrix = array<array<double, M>, N>;

template <int N> class LU_decomposition {
    matrix<N, N> lu;
    array<int, N> perm;

  public:
    /// Decompose a parameter matrix A into the lower and upper triangular
    /// A*x=b => L*U*x=b => L*y=b and then U*x=y
    /// where y_0=b_0/l_00 and y_i=1/l_ii(b_i-sum:j=0..i-1[a_ij*y_j])
    /// and x_N-1=y_N-1/u_N-1,N-1 and x_i=1/u_ii(y_i-sum:j=i+1..N-1[u_ij*x_j])
    constexpr LU_decomposition(matrix<N, N> p) : lu(move(p)) {
        array<double, N> scaling;
        int parity = 1;
        // set scale factor of each row
        for (auto i : range(0, N)) {
            double big = 0.0;
            for (auto j : range(0, N)) {
                if (abs(lu[i][j]) > big)
                    big = abs(lu[i][j]);
            }
            scaling[i] = 1. / big;
        }
        int imax = -1;
        for (auto k : range(0, N)) {
            // Search the largest element to pivot
            double big = 0.0;
            for (auto i : range(k, N)) {
                double tmp = scaling[i] * abs(lu[i][k]);
                if (tmp > big) {
                    big = tmp;
                    imax = i;
                }
            }
            // Swap rows to maximize row k
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
                // Divide by the pivot element
                lu[i][k] /= lu[k][k];
                double tmp = lu[i][k];
                for (auto j : range(k + 1, N)) {
                    lu[i][j] -= tmp * lu[k][j];
                }
            }
        }
    }

    constexpr auto solve(array<double, N> b) const {
        array<double, N> r = b.clone();
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

constexpr auto lu = LU_decomposition<4>(
    {array<double, 4>{1, 0, 0, 0}, array<double, 4>{0, 2, 0, 0},
     array<double, 4>{0, 0, 3, 0}, array<double, 4>{2, 0, 0, 4}});
static_assert(lu.solve({5, 2, 9, 2}) == array<double, 4>{5, 1, 3, -2});

} // namespace matrix
