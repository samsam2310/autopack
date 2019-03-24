#include <algorithm>
#include <cmath>
#include <iostream>

int add(int &a) {
  // cout << "Hi : " << a << endl;
  return a += 1;
}

float add(float &a) {
  // cout << "Hi : " << a << endl;
  return a += 1;
}

double add(double &a) {
  // cout << "Hi : " << a << endl;
  return a += 1;
}

void debug(int x) { std::cout << "Hi : " << x << std::endl; }

struct Point {
  int x, y;
  Point() : x(0), y(0) {}
  Point(int _x, int _y) : x(_x), y(_y) {}
  double disToOrigin() {
    std::cout << "C disToOrigin : " << ' ' << x << ' ' << y << std::endl;
    return sqrt(x * x + y * y);
  }
};

double distance(Point a, Point b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

/*
class myVector {
private:
  size_t m_size, alloc_size;
  int *buf;

  void _realloc(size_t n_size) {
    if (n_size <= alloc_size)
      return;
    if (alloc_size == 0) {
      buf = new int[1];
      alloc_size = 1;
      return;
    }
    int n_alloc_size = std::max(alloc_size << 1, n_size);
    int *new_buf = new int[n_alloc_size];
    std::copy(buf, buf + alloc_size, new_buf);
    delete[] buf;
    buf = new_buf;
    alloc_size = n_alloc_size;
  }

public:
  myVector() : m_size(0), alloc_size(0), buf(NULL) {}
  myVector(size_t _m_size, int val = 0) : m_size(_m_size), alloc_size(_m_size) {
    if (val == 0) {
      buf = new int[m_size]();
    } else {
      buf = new int[m_size];
      for (int i = 0; i < m_size; ++i) {
        buf[i] = val;
      }
    }
  }

  ~myVector() { delete[] buf; }

  size_t size() { return m_size; }

  int &get(int pos) { return buf[pos]; }

  void set(int pos, int val) { buf[pos] = val; }

  void push_back(int val) {
    _realloc(m_size + 1);
    buf[m_size++] = val;
  }

  void pop_back() { --m_size; }

  void resize(size_t n_size, int val = 0) {
    _realloc(n_size);
    if (m_size > n_size) {
      m_size = n_size;
      return;
    }
    while (m_size < n_size) {
      buf[m_size++] = val;
    }
  }

  void reserve(size_t n_alloc_size) { _realloc(n_alloc_size); }
};
*/