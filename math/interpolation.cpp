//
// Created by thomasgandy on 16/02/2022.
//

#include "interpolation.h"

float MyMath::lerp(float l, float r, float t) {
    return (1 - t) * l + t * r;
}
