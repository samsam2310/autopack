#include <emscripten.h>
extern "C" {
EMSCRIPTEN_KEEPALIVE
double test_i32(int *a0, float *a1) {
	return test(*a0, *a1);
}
EMSCRIPTEN_KEEPALIVE
int test_1_i32(int *a0) {
	return test_1(*a0);
}
EMSCRIPTEN_KEEPALIVE
double test_1() {
	return test_1();
}
EMSCRIPTEN_KEEPALIVE
short test_2_i32(int *a0) {
	return test_2(*a0);
}
}
