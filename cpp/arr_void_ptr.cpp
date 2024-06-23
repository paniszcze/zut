#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

template<size_t N, typename T, bool IsHeap>
class Array {
public:
    Array() {
        if constexpr (IsHeap) {
            ptr = new vector<T>();
        } else {
            ptr = new array<T, N>();
        }
    };

    ~Array() {
        if constexpr (IsHeap) {
            delete (vector<T>*)ptr;
        } else {
            delete (array<T, N>*)ptr;
        }
    }

    T& operator[](size_t idx) {
        if constexpr (IsHeap) {
            vector<T>* tmp = (vector<T>*)ptr;
            if (idx >= tmp->size()) {           // use an out-of-bounds index to
                tmp->resize(idx + 1);           // resize the vector
            }
            return tmp->at(idx);
        } else {
            return (*(array<T, N>*)ptr)[idx];
        }
    }

    T* begin() {
        if constexpr (IsHeap) {
            return &(*(*(vector<T>*)ptr).begin());
        } else {
            return &(*(*(array<T, N>*)ptr).begin());
        }
    }

    T* end() {
        if constexpr (IsHeap) {
            return &(*(*(vector<T>*)ptr).end());
        } else {
            return &(*(*(array<T, N>*)ptr).end());
        }
    }

    T* min() {
        return min_element(begin(), end());
    }

    void print() {
        if constexpr (IsHeap) {
            for (auto e : *(vector<T>*)ptr) {
                cout << e << " ";
            }
        } else {
            for (auto e : *(array<T, N>*)ptr) {
                cout << e << " ";
            }
        }
        cout << endl;
    }
private:
    void* ptr;
};


int main(void) {
    Array<10, int, false> arr;
    Array<10, int, true>  vec;

    arr[1] = 3;
    arr[4] = -1;
    arr.print();

    vec[2] = 9;
    vec[4] = 2;
    vec[8] = -2;
    vec.print();

    for (auto el : vec) {
        cout << el << " ";
    }
    cout << endl;

    cout << *(arr.min()) << endl;
    cout << *(vec.min()) << endl;
}

