#include "mylib.cpp"

#include <cstdio>
#include <emscripten.h>
#include <iostream>
using namespace std;

char buf[256];

extern "C" {
EMSCRIPTEN_KEEPALIVE
char* add_i32(int*a1) {
	auto r = add(*a1);
	memcpy(buf, &r, sizeof(r));
	return buf;
}
EMSCRIPTEN_KEEPALIVE
char* add_f32(float*a1) {
	auto r = add(*a1);
	memcpy(buf, &r, sizeof(r));
	return buf;
}
EMSCRIPTEN_KEEPALIVE
char* add_f64(double*a1) {
	auto r = add(*a1);
	memcpy(buf, &r, sizeof(r));
	return buf;
}
EMSCRIPTEN_KEEPALIVE
void debug_i32(int*a1) {
	debug(*a1);
}
EMSCRIPTEN_KEEPALIVE
void debug_f64(double*a1) {
	cout << (*a1) << endl;
}
EMSCRIPTEN_KEEPALIVE
void construct_Point(Point*obj) {
	new (obj) Point();
}
EMSCRIPTEN_KEEPALIVE
void construct_Point_i32_i32(Point*obj,int*a1,int*a2) {
	cout << "DB : " << *a1 << ' ' << *a2 << endl;
	new (obj) Point(*a1,*a2);
	cout << "DB2 : " << obj->x << ' ' << obj->y << endl;
}
EMSCRIPTEN_KEEPALIVE
char* Point_disToOrigin(Point*obj) {
	auto r = obj->disToOrigin();
	memcpy(buf, &r, sizeof(r));
	return buf;
}
EMSCRIPTEN_KEEPALIVE
char* distance_Point_Point(Point*a,Point*b) {
	auto r = distance(*a,*b);
	memcpy(buf, &r, sizeof(r));
	return buf;
}
}

int main() {
	// myVector vec(3, 5);
	// vec.push_back(6);
	// vec.push_back(7);
	// vec.push_back(8);
	// cout << "--- Vec ---" << endl;
	// for (int i = 0; i < vec.size(); ++i) {
	// 	cout << i << " : " << vec.get(i) << endl;
	// }
	// cout << "--- End ---" << endl;
}