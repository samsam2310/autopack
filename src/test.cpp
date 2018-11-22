#include <cstdio>
#include <emscripten.h>
#include <stdarg.h>
#include <iostream>
using namespace std;


int add(int&a) {
	cout << "Hi : " << a << endl;
	return a += 1;
}

float add(float &a) {
	cout << "Hi : " << a << endl;
	return a += 1;
}

double add(double &a) {
	cout << "Hi : " << a << endl;
	return a += 1;
}

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
EMSCRIPTEN_KEEPALIVE
int printf_i32(const char** fmt, ...) {
	va_list arg;
	int done;

	const char* ff = *fmt;
	va_start(arg, fmt);
	// done = printf(ff, *arg);
	std::cout << arg << std::endl;
	va_end(arg);

	return done;
}
EMSCRIPTEN_KEEPALIVE
void debug() {
	printf("debug!!!!!\n");
}
}

int main() {
	// int n = 7122;
	// const char* fmt = "Init %d\n";
	// cout << &n << endl;
	// cout << &fmt << endl;
	// printf_i32(&fmt, &n, &n, &n);
	// int a;
	// double b;
	// add(a);
	// add(b);
}