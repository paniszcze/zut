/*  iterator mechanics based on Boost's circular buffer module:
 *  https://github.com/boostorg/circular_buffer/
 */

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <type_traits>

template<typename T, size_t N,
         typename = std::enable_if<N != 0>>
struct CircBuf {
    CircBuf() : sz(0) {
        s = first = last = buf.data();
        e = s + N;
    }

    T& operator[](std::size_t idx) { return *add(first, idx); }
    const T& operator[](std::size_t idx) const { return *add(first, idx); }
    T& at(std::size_t idx) { return (*this)[idx]; }
    const T& at(std::size_t idx) const { return (*this)[idx]; }

    T& front() { return *first; }
    const T& front() const { return *first; }
    T& back() { return *((last == s ? e : last) - 1); }
    const T& back() const { return *((last == s ? e : last) - 1); }

    std::size_t size() const { return sz; }
    std::size_t capacity() const { return N; }
    bool empty() const { return size() == 0; }
    bool full() const { return size() == N; }

    void push_back(T el) {
        *last = el;
        increment(last);
        if (full()) {
            first = last;
        } else {
            ++sz;
        }
    }
    void push_front(T el) {
        decrement(first);
        *first = el;
        if (full()) {
            last = first;
        } else {
            ++sz;
        }
    }
    void pop_back() {
        if (!empty()) {
            decrement(last);
            --sz;
        }
    }
    void pop_front() {
        if (!empty()) {
            increment(first);
            --sz;
        }
    }

    auto begin() { return iterator(this, empty() ? 0 : first); }
    const auto begin() const { return iterator(this, empty() ? 0 : first); }
    auto end() { return iterator(this, 0); }
    const auto end() const { return iterator(this, 0); }
    const auto data() const { return buf.data(); }

    void increment(T*& p) const {
        if (++p == e) {
            p = s;
        }
    }
    void decrement(T*& p) const {
        if (p == s) {
            p = e;
        }
        --p;
    }
    T* add(T* p, std::size_t n) const {
        return p + (n < (e - p) ? n : n - N);
    }
    T* sub(T* p, std::size_t n) const {
        return p - (n > (p - s) ? n - N : n);
    }

private:
    std::array<T, N> buf = {};
    T* s; // start
    T* e; // end
    T* first;
    T* last;
    std::size_t sz;

    struct iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;

        const CircBuf* cbuf;
        pointer ptr;

        iterator() : cbuf(0), ptr(0) {}
        iterator(const CircBuf* cb, const pointer p) : cbuf(cb), ptr(p) {}
        iterator(const iterator& it) : cbuf(it.cbuf), ptr(it.ptr) {}

        reference operator*() const { return *ptr; }
        pointer operator->() const { return &(operator*()); }
        reference operator[](difference_type n) const { return *(*this + n); }

        iterator& operator++() {
            cbuf->increment(ptr);
            if (ptr == cbuf->last) {
                ptr = 0;
            }
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        iterator& operator--() {
            if (ptr == 0) {
                ptr = cbuf->last;
            }
            cbuf->decrement(ptr);
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
        iterator& operator+=(difference_type n) {
            if (n > 0) {
                ptr = cbuf->add(ptr, n);
                if (ptr == cbuf->last) {
                    ptr = 0;
                }
            } else if (n < 0) {
                *this -= -n;
            }
            return *this;
        }
        iterator operator+(difference_type n) const {
            return iterator(*this) += n;
        }
        iterator& operator-=(difference_type n) {
            if (n > 0) {
                ptr = cbuf->sub(ptr == 0 ? cbuf->last : ptr, n);
            } else if (n < 0) {
                *this += -n;
            }
            return *this;
        }
        iterator operator-(difference_type n) const {
            return iterator(*this) -= n;
        }
        difference_type operator-(const iterator& it) const {
            return linearize(*this) - linearize(it);
        }

        bool operator==(const iterator& it) const { return ptr == it.ptr; }
        bool operator!=(const iterator& it) const { return ptr != it.ptr; }
        bool operator<(const iterator& it) const {
            return linearize(*this) < linearize(it);
        }
        bool operator>(const iterator& it) const { return it < *this; }
        bool operator<=(const iterator& it) const { return !(it < *this); }
        bool operator>=(const iterator& it) const { return !(*this < it); }

        // pointer comparison helper
        pointer linearize(const iterator& it) const {
            return it.ptr == 0 ? cbuf->s + cbuf->size() :
                (it.ptr < cbuf->first ? it.ptr + (cbuf->e - cbuf->first)
                    : cbuf->s + (it.ptr - cbuf->first));
        }
    };
};

template<typename T, std::size_t N>
void print(const CircBuf<T, N>& cb) {
    for (const auto& el : cb) {
        std::cout << el << " ";
    }
    std::cout << "\n";
}

template<typename T, std::size_t N>
void data_snapshot(const CircBuf<T, N>& cb) {
    const T* start = cb.data();
    for (std::ptrdiff_t i = 0; i < N; ++i) {
        std::cout << *(start + i) << " ";
    }
    std::cout << "\n";
}

template<typename T, std::size_t N>
void log(const CircBuf<T, N>& cb, std::string info = "") {
    if (!info.empty()) {
        std::cout << info << ":\n";
    }
    std::cout << "B: ";
    print(cb);
    std::cout << "D: ";
    data_snapshot(cb);
    std::cout << std::string(20, '-') << "\n";
}

int main() {
    CircBuf<int, 5> cbuf;

    std::cout << "   CircBuf<int,5>\n"
              << std::string(20, '=') << "\n"
              << "B - buffer\n"
              << "D - underlying array\n"
              << std::string(20, '-') << "\n";

    std::cout << "empty(): " << (cbuf.empty() ? "true" : "false") << "\n"
              << "full(): " << (cbuf.full() ? "true" : "false") << "\n";
    log(cbuf);
    for (int i = 1; i <= 3; ++i) {
        cbuf.push_back(i);
    }
    log(cbuf, "push back 3 items");
    cbuf.pop_front();
    log(cbuf, "pop front once");
    for (int i = 9; i > 5; --i) {
        cbuf.push_back(i);
    }
    log(cbuf, "push back 4 items");
    std::cout << "empty(): " << (cbuf.empty() ? "true" : "false") << "\n"
              << "full(): " << (cbuf.full() ? "true" : "false") << "\n";
    log(cbuf);
    std::sort(cbuf.begin(), cbuf.end());
    log(cbuf, "std::sort");
    cbuf.pop_back();
    log(cbuf, "pop back once");
    for (int i = 0; i < 3; ++i) {
        cbuf.push_front(i);
    }
    log(cbuf, "push front 3 items");
}

