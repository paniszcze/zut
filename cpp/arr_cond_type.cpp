#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <type_traits>

using namespace std;

template <size_t N, typename T, bool isHeap>
struct Container {
    T& operator[](size_t idx) {
        if constexpr (isHeap) {
            if (idx >= data.size()) {       // allow an out-of-bounds index to be
                data.resize(idx + 1);       // used for resizing the vector
            }
        }
        return data[idx];
    }

    decltype(auto) begin() { return data.begin(); }
    decltype(auto) end()   { return data.end(); }
    decltype(auto) min()   { return min_element(begin(), end()); }
    decltype(auto) max()   { return max_element(begin(), end()); }

    void print(bool addSpaces = true, bool addNewline = true) {
        for (auto el : data) {
            cout << el << (addSpaces ? " " : "");
        }
        if (addNewline) cout << endl;
    }

private:
    conditional_t<isHeap, vector<T>, array<T, N>> data = {};
};

int main(void) {
    Container<10, int, false> arr;
    Container<10, int, true>  vec;

    arr[1] =  3;
    arr[4] = -1;
    
    vec[2] =  9;
    vec[7] = -2;
    vec[4] =  2;

    arr.print();
    vec.print();

    cout << "min of arr: " << *arr.min() << endl;
    cout << "min of vec: " << *vec.min() << endl;
}
