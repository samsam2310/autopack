#include <cstdio>
#include <emscripten.h>
#include <stdarg.h>


int add(int&a) {
	return a += 1;
}

float add(float &a) {
	return a += 1;
}

double add(double &a) {
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
int printf_i32(const char* fmt, ...) {
	va_list arg;
	int done;

	va_start(arg, fmt);
	done = printf(fmt, arg);
	va_end(arg);

	return done;
}
EMSCRIPTEN_KEEPALIVE
void debug() {
	printf("debug!!!!!\n");
}
}

int main() {
	int a;
	double b;
	add(a);
	add(b);
}