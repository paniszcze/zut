#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

struct File {
    File(std::string n) : name(n) {}

    void appendLine(std::string line) {
        std::fstream f(name, std::ios::app);
        if (f.good()) {
            f << line << "\n";
            f.close();
        }
    }

    bool appendUniqueWord(std::string s) {
        std::fstream r(name, std::ios::in);
        if (r.good()) {
            std::string word;
            while (!r.eof()) {
                r >> word;
                if (s == word) {
                    r.close();
                    return false;
                }
            }
            r.close();
        }
        std::fstream w(name, std::ios::app);
        if (w.good()) {
            w << s << "\n";
            w.close();
        }
        return true;
    }

    void makeReversedCopy(File& dst) {
        std::fstream src(name, std::ios::in);
        if (src.good()) {
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(src, line)) {
                lines.push_back(line);
            }
            src.close();
            dst.reset();
            std::reverse(lines.begin(), lines.end());
            for (auto& l : lines) {
                std::reverse(l.begin(), l.end());
                dst.appendLine(l);
            }
        }
    }

    void read() {
        std::fstream r(name, std::ios::in);
        if (r.good()) {
            std::string line;
            while (!r.eof()) {
                std::getline(r, line);
                std::cout << line << "\n";
            }
            r.close();
        }
    }

    void reset() {
        std::fstream f(name, std::ios::out);
        if (f.good()) f.close();
    }

private:
    std::string name;
};

int main() {
    File f{"file.txt"};
    std::cout << "ZADANIE 1\n";
    f.appendLine("hello from the other side");
    f.appendLine("a new beggining");
    f.read();

    std::cout << "ZADANIE 2\n";
    f.appendUniqueWord("beggining");
    f.appendUniqueWord("cat");
    f.read();

    std::cout << "ZADANIE 3\n";
    File rev{"file_reversed.txt"};
    f.makeReversedCopy(rev);
    rev.read();

    f.reset();
    rev.reset();
}

