//
// Created by xer on 2021-07-15.
//

#ifndef MU_FIXINGS_H
#define MU_FIXINGS_H

/* Fixes for Visual Studio. */
#if defined(_MSC_VER)
	/* Prevent any assembly references from being used. */
	#define __asm__(what)

	/* GCC's __builtin_popcount */
	#include <intrin.h>
	#define __builtin_popcount __popcnt
#endif

#endif // MU_FIXINGS_H
