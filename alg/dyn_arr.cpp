#include <iostream>

class Vector {
    private:
        int count = 0;
        int size = 10;
    public:
        int *tabela;

        Vector() {
            tabela = new int[size];
        }

        ~Vector() {
            if (tabela != nullptr) {
                delete tabela;
                tabela = nullptr;
            }
        }

        void push_back(int val) {
            if (count >= size) {
                size += 10;
                int *tempTabela = new int[size];

                for (int i = 0; i < count; i++) {
                    tempTabela[i] = tabela[i];
                }

                if (tabela != nullptr) {
                    delete tabela;
                    tabela = nullptr;
                }
                tabela = tempTabela;
            }

            tabela[count] = val;
            count++;
        }

        void print_info(void) {
            std::cout << "size: "  << size  << std::endl
                      << "count: " << count << std::endl;
        }

        void print_values(void) {
            if (count == 0) {
                std::cout << "Empty array";
            } else {
                for (int i = 0; i < count; i++) {
                    std::cout << tabela[i] << " ";
                }
            }
            std::cout << std::endl << "--------------" << std::endl;
        }
};

int main(void) {
    Vector w;

    w.print_info();
    w.print_values();
    
    w.push_back(3);
    w.push_back(7);
    w.push_back(8);
    w.push_back(0);
    w.push_back(9);
    w.push_back(1);
    w.push_back(2);
    w.push_back(4);
    w.push_back(3);
    w.push_back(7); // 10th element

    w.print_info();
    w.print_values();

    w.push_back(9);
    w.push_back(0);
    w.push_back(9);
    w.push_back(9);
    w.push_back(5);
    w.push_back(4);
    w.push_back(2);
    w.push_back(1);
    w.push_back(2);
    w.push_back(3); // 20th element

    w.print_info();
    w.print_values();

    w.push_back(2);

    w.print_info();
    w.print_values();

    return 0;
}

