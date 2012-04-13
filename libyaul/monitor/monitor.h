#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdbool.h>

#include "vt100.h"

void monitor_init(void);
void monitor(int, struct cha *);

#endif /* !_MONITOR_H_ */
