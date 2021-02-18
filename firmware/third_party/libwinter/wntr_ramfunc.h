#pragma once

#ifdef __arm__
#define RAMFUNC __attribute__((section(".ramfunc")))
#else
#define RAMFUNC
#endif
