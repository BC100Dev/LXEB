#ifndef LXEB_PROVIDERINTEGRATIONENTRY_H
#define LXEB_PROVIDERINTEGRATIONENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

int lxeb_module_start(const char* data);

void lxeb_module_stop(const char* reason);

#ifdef __cplusplus
}
#endif

#endif //LXEB_PROVIDERINTEGRATIONENTRY_H
