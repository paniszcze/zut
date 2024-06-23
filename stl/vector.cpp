#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template<class T>
void printVec(const vector<T>& vec,
              bool addSpace = true,
              bool addNewLine = true) {
    if (vec.empty()) {
        cout << "<empty>" << endl;
        return;
    }
    
    for (const auto& el : vec) {
        cout << el << (addSpace ? " " : "");
    }
    if (addNewLine) cout << endl;
}

template<class T>
void printVec2D(const vector<T>& vec,
                bool addSpace = true,
                bool addNewLine = true) {
    if (vec.empty()) {
        cout << "<empty>" << endl;
        return;
    }
    
    for (const auto& row : vec) {
        for (const auto& el : row) {
            cout << el << (addSpace ? " " : "");
        }
        cout << endl;
    }
    if (addNewLine) cout << endl;
}

void printVecAt(const vector<int>& vec, const size_t n) {
    if (n >= vec.size()) {
        cout << "n ouside vector" << endl;
        return;
    }

    for (size_t i = n; i < vec.size(); i++) {
        cout << vec.at(i) << " ";
    }
    cout << endl;
}

void populateVec(vector<int>& vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        vec[i] = (int)i + 1;
    }
}

void populateVec2D(vector<vector<int>>& vec) {
    size_t input;
    cin >> input;

    for (size_t i = 0; i < input; i++) {
        vec.push_back(vector<int>(i + 1));
        for (size_t j = 0; j <= i; j++) {
            vec.at(i).at(j) = i + (j + 1);
        }
    }
}

void eraseHashed(vector<char>& vec) {
    const char hash = '#';
    auto start = find(vec.begin(), vec.end(), hash);

    while (start != vec.end()) {
        auto end = find(start + 1, vec.end(), hash);

        if (end == vec.end()) {
            vec.erase(start, vec.end());
            return;
        } else {
            vec.erase(start, end + 1);
        }

        start = find(vec.begin(), vec.end(), hash);
    }
}

vector<int> copyVec(vector<int> vec, size_t start, size_t end) {
    if (end < start || end >= vec.size()) {
        return vector<int>();
    }
    
    return vector<int>(vec.begin() + start, vec.begin() + end);
}

int main(void) {

    cout << "ZADANIE 1" << endl;

    vector<int> vec1(10);
    auto itrB = vec1.begin();
    auto itrE = vec1.end() - 1;

    *itrB = 3;
    *itrE = 3;
    printVec(vec1);

    *(itrB + 3) = 5;
    printVec(vec1);
    
    vec1.at(3) = 9;
    printVec(vec1);

    cout << endl << "ZADANIE 2" << endl;

    vector<int> vec2{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    printVec(vec2);
    printVecAt(vec2, 7);
    printVecAt(vec2, vec2.size());          // no output
    printVecAt(vec2, vec2.size() - 1);      // last element

    cout << endl << "ZADANIE 3" << endl;

    vector<int> vec3;
    populateVec(vec3);
    printVec(vec3);

    vec3.resize(10);
    populateVec(vec3);
    printVec(vec3);

    vec3.resize(20);
    populateVec(vec3);
    printVec(vec3);

    vec3.resize(5);
    populateVec(vec3);
    printVec(vec3);

    cout << endl << "ZADANIE 4" << endl;

    vector<vector<char>> strings{
        {'a','b','c','#','e','f','#'},
        {'a','b','c','#','e','f','g'},
        {'a','b','c','#','e','#','g'},
        {'#','b','c','d','e','f','g'},
        {'#','b','c','d','e','#','g'},
        {'a','#','c','#','e','#','g','#','i'},
        {'a','#','c','#','e','f','#','h','i'},
        {'#','#','c','d','e','f'},
        {'a','b','#','#','e','f'},
        {'a','b','c','d','#','#'},
        {'#','b','c','d','e','#'},
        {'a','b','c','d','e','f'}
    };

    for (auto s : strings) {
        printVec(s, false, false);
        eraseHashed(s);
        cout << " -> ";
        printVec(s, false);
    }

    cout << endl << "ZADANIE 5" << endl;

    vector<int> vec4 = {1,2,3,4,5};
    vector<int> vec4cpy = copyVec(vec4, 2, 4);
    printVec(vec4cpy);

    cout << endl << "ZADANIE 6" << endl;

    vector<vector<int>> vec5;
    populateVec2D(vec5);
    printVec2D(vec5);

}

