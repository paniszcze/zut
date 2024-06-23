#include <algorithm>
#include <array>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <type_traits>

template<std::size_t N, std::size_t M, typename T,
         typename = std::enable_if_t<N != 0 && M != 0 && !std::is_same_v<T, bool>>>
struct Matrix {
    Matrix(T t = 0) { for (auto& row : m) row.fill(t); }
    Matrix(const Matrix& mtx) : m(mtx.m) {}
    template<std::size_t N1, std::size_t M1, typename T1>
    Matrix(const Matrix<N1, M1, T1>& mtx) {
        static_assert(std::is_convertible_v<T1, T>,
                "type has to be convertible into destination matrix type");
        static_assert(N1 <= N && M1 <= M, "cannot exceed dimensions of destination matrix");
        for (std::size_t row = 0; row < N; ++row) {
            for (std::size_t col = 0; col < M; ++col) {
                m[row][col] = (row < N1 && col < M1) ? mtx[row][col] : 0;
            }
        }
    }
    Matrix(Matrix&& mtx) : m(mtx.m) { mtx.m = {}; }
    Matrix(std::initializer_list<std::initializer_list<T>> ls) {
        std::size_t row = 0;
        for (auto& l : ls) {
            std::size_t col = 0;
            for (auto& el : l) {
                m[row][col] = el;
                ++col;
            }
            ++row;
        }
    }
    
    auto& operator[](std::size_t idx) { return m[idx]; }
    const auto& operator[](std::size_t idx) const { return m[idx]; }
    auto begin() { return m.begin(); }
    auto end() { return m.end(); }

    Matrix& operator=(const Matrix& mtx) {
        if (this == &mtx) {
            return *this;
        }
        m = mtx.m;
        return *this;
    }

    Matrix& operator+=(const Matrix& mtx) {
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < M; ++j) {
                m[i][j] += mtx[i][j];
            }
        }
        return *this;
    }
    template<typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
    Matrix& operator+=(const U& scalar) {
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < M; ++j) {
                m[i][j] += scalar;
            }
        }
        return *this;
    }
    Matrix operator+(const Matrix& mtx) {
        Matrix<N, M, T> tmp = Matrix<N, M, T>(*this);
        tmp += mtx;
        return tmp;
    }
    template<typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
    Matrix operator+(const U& scalar) {
        Matrix<N, M, T> tmp = Matrix<N, M, T>(*this);
        tmp += scalar;
        return tmp;
    }

    Matrix& operator*=(const Matrix& mtx) {
        static_assert(N == M, "matrix has to be square in order to reassign back to it"
                "the result of its multiplication");
        auto tmp = Matrix<N, M, T>();
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < M; ++j) {
                for (std::size_t k = 0; k < M; ++k) {
                    tmp[i][j] += m[i][k] * mtx[k][j];
                }
            }
        }
        return (*this = tmp);
    }
    template<typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
    Matrix& operator*=(const U& scalar) {
        for (auto& row : m) {
            for (auto& el : row) {
                el *= scalar;
            }
        }
        return *this;
    }
    template<std::size_t N1, std::size_t M1, typename T1>
    Matrix<N, M1, T> operator*(const Matrix<N1, M1, T1>& mtx) {
        Matrix<N, M1, T> tmp;
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < M1; ++j) {
                for (std::size_t k = 0; k < M; ++k) {
                    tmp[i][j] += m[i][k] * mtx[k][j];
                }
            }
        }
        return tmp;
    }
    template<typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
    Matrix operator*(const U& scalar) {
        std::cout << "scalar * operator\n";
        Matrix<N, M, T> tmp = Matrix<N, M, T>(*this);
        tmp *= scalar;
        return tmp;
    }

    void print(std::ostream& os = std::cout) const {
        // enable column alignment for integers only (excluding chars)
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, char>) {
            auto widths = getWidths();
            for (const auto& row : m) {
                std::size_t i = 0;
                for (const auto& el : row) {
                    os << std::setw(widths[i]) << el << " ";
                    ++i;
                }
                os << "\n";
            }
        } else {
            for (const auto& row : m) {
                for (const auto& el : row) {
                    os << el << " ";
                }
                os << "\n";
            }
        }
    }

    template<std::size_t P, std::size_t R, typename U>
    friend std::ostream& operator<<(std::ostream& os, const Matrix<P, R, U>& m);

private:
    std::array<std::array<T, M>, N> m = {};

    auto getWidth(T num) const {
        int width = 1;
        if (num < 0) {
            num *= -1;
            width++;
        }
        while (num > 10) {
            width++;
            num /= 10;
        }
        return width;
    }
    auto getWidths() const {
        std::array<int, M> tmp;
        T max, min, curr;
        for (std::size_t col = 0; col < M; ++col) {
            max = min = m[0][col];
            for (std::size_t row = 0; row < N; ++row) {
                curr = m[row][col];
                if (curr > max) max = curr;
                if (curr < min) min = curr;
            }
            tmp[col] = std::max(getWidth(max), getWidth(min));
        }
        return tmp;
    }
};

template<std::size_t N, std::size_t M, typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<N, M, T>& m) {
    m.print(os);
    return os;
}

template<std::size_t N, std::size_t M, typename T = int>
auto getMatrix(T t = 0) {
    return Matrix<N, M, T>{ t };
}

template<std::size_t N, std::size_t M, typename T = int>
auto getMatrix(std::initializer_list<std::initializer_list<T>> ls) {
    return Matrix<N, M, T>{ ls };
}

int main() {
    auto A = Matrix<3, 4, int>();
    A[0][2] = 2;
    A[1][3] = 1;
    std::cout << "A\n";
    A.print();

    auto B = getMatrix<3, 4>(1);
    B[0][1] = 0;
    std::cout << "B\n" << B << "\n";
    
    B += A;
    std::cout << "B+=A\n" << B
              << "A+B+A\n" << A + B + A << "\n";
    std::cout << "A+3\n" << A + 3
              << "A*4\n" << A * 4 << "\n";
    B = A;
    A[0][2] = 7;
    std::cout << "B=A\n" << B
              << "A\n" << A
              << "B\n" << B << "\n";

    auto C = getMatrix<4, 3>({{ 9, 1, 2 }, { 1 }, { 3, -5, 2 }, { 1, 1, 7 }});
    auto D = Matrix<3, 2, int>{{ 0, 1 } , { -3, 5 }, { 2, 8 }};
    std::cout << "C\n" << C
              << "D\n" << D
              << "C*D\n" << C * D << "\n";

    auto E = getMatrix<2, 2>({{ 1, 2 }, { 3, 4 }});
    auto F = getMatrix<2, 2>({{ 5, 6 }, { 7, 8 }});
    std::cout << "E\n" << E << "F\n" << F;
    E *= F;
    std::cout << "E*=F\n" << E << "\n";

    auto G = getMatrix<2, 2>({{ 1.1, 3.2 }, { 4.1, 2.6 }});
    auto H = getMatrix<2, 5>({{ 0.1, 0.2, 0.3, 0.4, 0.5 },
                              { 1.1, 1.2, 1.3, 1.4, 1.5 }});
    std::cout << "G\n" << G << "H\n" << H;
    H = G;
    std::cout << "H=G\n" << H;

    // disabled due to dimensions restriction:
    // G = H;
    // disabled due to type conversion restriction:
    // auto P = getMatrix<1, 1, std::string>({{ "what's the point?" }});
    // G = P;

    // disabled for empty matrices and bools:
    // auto Q = getMatrix<0, 0>();
    // auto R = getMatrix<2, 3, bool>(true);
}

