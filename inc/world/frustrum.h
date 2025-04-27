#ifndef JA_FRUSTRUM_H
#define JA_FRUSTRUM_H

#include <utility/angle.h>

namespace ja {

struct frustrum {
    ja::angle<float> fov{degrees(90.0f)};
    float near{0.1f};
    float far{100.0f};
};

}

#endif

