#include <log.h>
void slim::log::Init(uv_loop_t *loop) {
    log_loop = loop;
}
