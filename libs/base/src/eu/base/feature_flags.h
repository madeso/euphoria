#pragma once

// will error if check is not defined or typo
// https://x.com/voxagonlabs/status/1821444613430763778

#define FF_IS_ENABLED +
#define FF_IS_DISABLED -
#define FF_HAS(x) (1 x 1)

// #define FEATURE_A FF_IS_ENABLED
//
// #if FF_HAS(FEATURE_A)
//    ...
// #else
//    ...
// #endif
