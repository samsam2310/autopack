#include "mylib.cpp"

#include <cstdio>
#include <emscripten.h>
#include <iostream>
using namespace std;

extern "C" {
EMSCRIPTEN_KEEPALIVE
int add_i32(int*a1) {
	return add(*a1);
}
EMSCRIPTEN_KEEPALIVE
float add_f32(float*a1) {
	return add(*a1);
}
EMSCRIPTEN_KEEPALIVE
double add_f64(double*a1) {
	return add(*a1);
}
}

int main() {}