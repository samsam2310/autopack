#include <emscripten.h>
extern "C" {
EMSCRIPTEN_KEEPALIVE
int a() {
	return a();
}
}
