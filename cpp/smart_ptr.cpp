#include <atomic>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

template<typename T>
class UniquePointer {
public:
    UniquePointer() : ptr(nullptr) {
        std::cout << "[U] default unique ptr\n";
    }
    UniquePointer(nullptr_t) : ptr(nullptr) {
        std::cout << "[U] nullptr unique ptr\n";
    }
    UniquePointer(T* p) : ptr(p) {
        std::cout << "[U] unique ptr from raw ptr\n";
    };
    UniquePointer(UniquePointer&& u) : ptr(u.release()) {
        std::cout << "[U] unique ptr move\n";
    }
    ~UniquePointer() {
        reset();
        std::cout << "[U] unique ptr destructed\n";
    }

    UniquePointer& operator=(UniquePointer&& u) {
        std::cout << "[U] move assignment\n";
        reset(u.release());
        return *this;
    }
    UniquePointer& operator=(nullptr_t) {
        std::cout << "[U] nullptr assignment\n";
        reset();
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }

    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }
    void reset(T* p = nullptr) {
        T* tmp = ptr;
        ptr = p;
        if (tmp) {
            delete tmp;
        }
    }
    void swap(UniquePointer& u) { std::swap(ptr, u.ptr); }

private:
    T* ptr;
};

// TODO: make operators work with nullptr_t
template<typename T>
inline void swap(UniquePointer<T>& x, UniquePointer<T>& y) {
    return x.swap(y);
}
template<typename T1, typename T2>
inline bool operator==(const UniquePointer<T1>& x, const UniquePointer<T2>& y) {
    return x.get() == y.get();
}
template<typename T1, typename T2>
inline bool operator!=(const UniquePointer<T1>& x, const UniquePointer<T2>& y) {
    return !(x == y);
}
template<typename T1, typename T2>
inline bool operator<(const UniquePointer<T1>& x, const UniquePointer<T2>& y) {
    return std::less<std::common_type_t<T1*, T2*>>()(x.get(), y.get());
}
template<typename T1, typename T2>
inline bool operator>(const UniquePointer<T1>& x, const UniquePointer<T2>& y) {
    return y < x;
}
template<typename T1, typename T2>
inline bool operator<=(const UniquePointer<T1>& x, const UniquePointer<T2>& y) {
    return !(y < x);
}
template<typename T1, typename T2>
inline bool operator>=(const UniquePointer<T1>& x, const UniquePointer<T2>& y) {
    return !(x < y);
}

// TODO: unique pointer for arrays of type T
// template<typename T>
// class UniquePointer<T[]> {
// public:
//     UniquePointer() : ptr(nullptr) {}
//     UniquePointer(nullptr_t) : ptr(nullptr) {}
//     ...
//     T& operator[](std::size_t i) const { return ptr[i]; }
//     ...
// private:
//     T* ptr;
// };

template<typename T>
class SharedPointer {
public:
    SharedPointer() : ptr(nullptr), cntrl(nullptr) {
        std::cout << "[S] default shared ptr\n";
    }
    SharedPointer(nullptr_t) : ptr(nullptr), cntrl(nullptr) {
        std::cout << "[S] nullptr shared ptr\n";
    }
    SharedPointer(T* p) : ptr(p) {
        std::cout << "[S] shared ptr from raw ptr\n";
        cntrl = new Cntrl(p);
    }
    SharedPointer(const SharedPointer& p) : ptr(p.ptr), cntrl(p.cntrl) {
        std::cout << "[S] shared ptr copy\n";
        if (cntrl) cntrl->add();
    }
    SharedPointer(SharedPointer&& p) : ptr(p.ptr), cntrl(p.cntrl) {
        std::cout << "[S] shared ptr move\n";
        p.ptr = nullptr;
        p.cntrl = nullptr;
    }
    ~SharedPointer() {
        if (cntrl) {
            cntrl->release();
            if (cntrl->use_count() == 0) {
                delete cntrl;
            }
        }
        std::cout << "[S] shared ptr destructed\n";
    }

    SharedPointer& operator=(const SharedPointer& p) {
        std::cout << "[S] copy assignment\n";
        SharedPointer(p).swap(*this);
        return *this;
    }
    SharedPointer& operator=(SharedPointer&& p) {
        std::cout << "[S] move assignment\n";
        SharedPointer(std::move(p)).swap(*this);
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
    explicit operator bool() const { return get() != nullptr; }

    long use_count() const { return cntrl ? cntrl->use_count() : 0; }
    bool unique() const { return use_count() == 1; }
    void swap(SharedPointer& p) {
        std::swap(ptr, p.ptr);
        std::swap(cntrl, p.cntrl);
    }
    void reset() { SharedPointer().swap(*this); }
    void reset(T* p) { SharedPointer(p).swap(*this); }

private:
    class Cntrl; // control block

    T* ptr;
    Cntrl* cntrl;
};

template<typename T>
class SharedPointer<T>::Cntrl {
public:
    Cntrl(T* p, long refs = 0) : data(p), count(refs) {
        std::cout << "[C] cntrl block constructed (" << use_count() << ")\n";
    };
    ~Cntrl() { std::cout << "[C] cntrl block destructed\n"; };

    void add() {
        std::atomic_fetch_add_explicit(&count, 1, std::memory_order_relaxed);
        std::cout << "[C] cntrl block incremented (" << use_count() << ")\n";
    }
    bool release() {
        std::atomic_fetch_add_explicit(&count, -1, std::memory_order_acq_rel);
        std::cout << "[C] cntrl block decremented (" << use_count() << ")\n";
        if (use_count() == 0) {
            on_zero();
            return true;
        }
        return false;
    }
    long use_count() const {
        return std::atomic_load_explicit(&count, std::memory_order_relaxed) + 1;
    }

private:
    T* data;
    std::atomic<long> count;
    
    void on_zero() { delete data; };
};

// TODO: make operators work with nullptr_t
// TODO: create Pointer class and make UniquePointer and SharedPointer
//       inherit the operators and common methods to avoid duplicated code
template<typename T>
inline void swap(SharedPointer<T>& x, SharedPointer<T>& y) {
    return x.swap(y);
}
template<typename T1, typename T2>
inline bool operator==(const SharedPointer<T1>& x, const SharedPointer<T2>& y) {
    return x.get() == y.get();
}
template<typename T1, typename T2>
inline bool operator!=(const SharedPointer<T1>& x, const SharedPointer<T2>& y) {
    return !(x == y);
}
template<typename T1, typename T2>
inline bool operator<(const SharedPointer<T1>& x, const SharedPointer<T2>& y) {
    return std::less<std::common_type_t<T1*, T2*>>()(x.get(), y.get());
}
template<typename T1, typename T2>
inline bool operator>(const SharedPointer<T1>& x, const SharedPointer<T2>& y) {
    return y < x;
}
template<typename T1, typename T2>
inline bool operator<=(const SharedPointer<T1>& x, const SharedPointer<T2>& y) {
    return !(y < x);
}
template<typename T1, typename T2>
inline bool operator>=(const SharedPointer<T1>& x, const SharedPointer<T2>& y) {
    return !(x < y);
}

// TODO: make SharedPointer work with array types

void separator(std::string header = "", char c = '-', size_t w = 30) {
    std::cout << std::string(w, c) + '\n';
    if (!header.empty()) {
        auto padding = std::string(header.length() >= w ? 0
                                       : (w - header.length()) / 2, ' ');
        std::cout << padding + header + '\n';
    }
}

struct A {
    A(std::string n = "unnamed") : name(n) {
        std::cout << "[A] " << name << " constructed\n";
    }
    ~A() { std::cout << "[A] "<< name << " destructed\n"; }

    std::string name;
};

int main() {
    separator("UNIQUE POINTER", '=');
    separator("CONSTRUCTORS, MOVE ASSIGNMENT");
    {
        auto hi = new A("hi");
        auto hello = new A("hello");

        auto u1 = UniquePointer(hi);            // cons. from raw ptr
        std::cout << u1.get()->name << "\n";
        u1 = UniquePointer(hello);              // move assignment
        std::cout << u1.get()->name << "\n";
        auto u2 = UniquePointer<A>();           // default constructor
        u2.swap(u1);
        std::cout << u2.get()->name << "\n";
        swap(u1, u2);
        std::cout << u1.get()->name << "\n";

        // NOTE: copy constructor and copy assignment disabled by design;
        // each of the following would produce a compilation error:
        //   auto u3 = UniquePointer(u1);
        //   u2 = u1;
    }
    separator("MOVE CONSTRUCTOR, BOOL");
    {
        auto empty = new A();
        auto u1 = UniquePointer(empty);
        auto u2 = std::move(u1);                // move constructor
        std::cout << (u1 ? u1.get()->name : "nullptr") << "\n"
                  << (u2 ? u2.get()->name : "nullptr") << "\n";
    }
    separator("NULLPTR, OPERATORS");
    {
        auto daft = new A("daft");
        auto punk = new A("punk");
        UniquePointer<A> u1(nullptr), u2;
        u2 = nullptr;
        u1 = daft;                  // temporary created implicitly, then moved
        u2 = punk;                  // (same)
        std::cout << u1->name
                  << (u1 == u2 ? " == " : " != ")
                  << (*u2).name << "\n";
        std::cout << "u1 at " << u1.get()
                  << (u1 > u2 ? " > " : " <= ")
                  << "u2 at " << u2.get() << "\n";
    }
    separator();
    separator("SHARED POINTER", '=');
    separator("CONSTRUCTORS, ACCESS");
    {
        auto dog = new A("dog");
        auto s1 = SharedPointer(dog);           // cnstr. from raw ptr
        auto s2 = s1;                           // copy constructor
        std::cout << s2->name << "\n";
        auto s3 = SharedPointer<A>();           // default constructor
        s3 = s2;                                // copy assignment
        s3->name = "cat";
        auto s4 = SharedPointer<A>(nullptr);    // nullptr cnstr.
        s4 = s3;                                // copy assignment
        std::cout << (*s1).name << "-"
                  << s2.get()->name << "-"
                  << s3->name << "-"
                  << (*(s4.get())).name << "\n";

        // NOTE: constructing a shared pointer from a raw pointer already
        // owned by another shared pointer leads to undefined behaviour,
        // thus the following is incorrect:
        //   auto s5 = SharedPointer(dog);
    }
    separator("MOVE, BOOL");
    {
        auto ricky = new A("ricky");
        auto morty = [](){ return new A("morty"); };

        auto s1 = SharedPointer<A>();
        s1 = morty();                               // move assignment
        std::cout << s1->name << "\n";
        auto s2 = SharedPointer(std::move(s1));     // move constructor
        std::cout << (s1 ? s1->name : "who?") << "\n"
                  << s2->name << "\n";
        s1 = ricky;                                 // move assignment
        std::cout << s1->name << " & " << s2->name << "\n";
    }
    separator("OPERATORS");
    {
        auto s1 = SharedPointer(new A());
        auto s2 = s1;
        auto s3 = SharedPointer(new A("named"));

        std::cout << s1->name
                  << (s1 == s2 ? " (s1) == " : " != ")
                  << s2->name << " (s2)\n";
        std::cout << s1->name
                  << (s1 == s3 ? " == " : " != ")
                  << s3->name << "\n";
        std::cout << "s1 at " << s1.get()
                  << (s1 > s3 ? " > " : " <= ")
                  << "s3 at " << s3.get() << "\n";
    }
    separator();
}

