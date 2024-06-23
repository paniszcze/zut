#include <iostream>
#include <cstring>
#include <typeinfo>

class BaseData {
public:
    static const int SIZE = 100;
    int data[SIZE];
    int currentPointer;

    BaseData() {
        memset(data, 0, sizeof(data));
        currentPointer = 0;
    }
    virtual ~BaseData() {}
    virtual void putData(int data) {
        if (currentPointer < SIZE) {
            this->data[currentPointer] = data;
            currentPointer++;
        }
    }
    virtual int getCurrentPointer() {
        return currentPointer;
    }
    virtual int *getData(int &ptr) {
        ptr = currentPointer;
        return data;
    }
    virtual void printDataType() = 0;
};

class CircData : public BaseData {
public:
    void putData(int dataIn) override {
        if (getCurrentPointer() >= SIZE) {
            currentPointer = 0;
        }

        data[currentPointer] = dataIn;  // currentPointer used instead of a getter
                                        // to avoid writing into 100th index in CircData2
        currentPointer++;
    }
    void printDataType() override {
        std::cout << "Circular data" << std::endl;
    }
};

class CircData2 : public CircData {
public:
    int *getData(int &ptr) override {
        ptr = SIZE;
        return data;
    }
    int getCurrentPointer() override {
        return SIZE;
    }
};

class OneData : public BaseData {
public:
    void printDataType() override {
        std::cout <<"One time data" << std::endl;
    }
    void flushBuffer() {
        memset(data, 0, sizeof(data));
        currentPointer = 0;
    }
};

void printData(BaseData *ptr) {
    int size;
    int *data = ptr->getData(size);

    std::cout << "= ";
    if (size == 0) {
        std::cout << "(empty)" << std::endl;
    } else {
        for (int i = 0; i < size; i++) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    if (typeid(*ptr) == typeid(OneData)) {
        static_cast<OneData*>(ptr)->flushBuffer();
    }
}

int main(void) {
    BaseData *klasa = nullptr;

    // CircData
    klasa = new CircData;
    klasa->printDataType();
    printData(klasa);
    for (int i = 0; i < 20; i++) {      // insert 20 elements
        klasa->putData(i);
    }
    printData(klasa);
    for (int i = 0; i < 10; i++) {      // insert next 10 elements
        klasa->putData(i);
    }
    printData(klasa);
    for (int i = 0; i < 100; i++) {     // insert next 100 elements; current ptr resets
                                        // after reaching SIZE elements (circular buffer)
        klasa->putData(i);
    }
    printData(klasa);
    delete klasa;
    std::cout << std::endl;

    // CircData2
    klasa = new CircData2;
    klasa->printDataType();
    printData(klasa);                   // CircData2 has always SIZE elements
    for (int i = 0; i < 20; i++) {
        klasa->putData(i);              // elements get inserted into the same place
    }
    printData(klasa);
    delete klasa;
    std::cout << std::endl;

    // OneData
    klasa = new OneData;
    klasa->printDataType();
    printData(klasa);
    for (int i = 0; i < 20; i ++) {     // insert 20 elements
        klasa->putData(i);
    }
    for (int i = 0; i < 10; i++) {      // insert next 10 elements
        klasa->putData(i);
    }
    printData(klasa);
    printData(klasa);                   // empty since previous print flushed the data
    for (int i = 0; i < 120; i ++) {
        klasa->putData(i);              // only SIZE elements get inserted into data
    }
    printData(klasa);
    delete klasa;
}

