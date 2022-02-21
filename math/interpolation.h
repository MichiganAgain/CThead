//
// Created by thomasgandy on 16/02/2022.
//

#ifndef CTHEAD_INTERPOLATION_H
#define CTHEAD_INTERPOLATION_H

namespace MyMath {
    /**
     * Carry out linear interpolation on two numbers, given a distance between them
     * @param l The 'left'/first value to interpolate from
     * @param r The 'right'/last value to interpolate from
     * @param t The distance in-between the two values
     * @return The calculated interpolated value between l and r based off t
     */
     template<typename T>
    float lerp(T l, T r, float t);
}

template <typename T>
float MyMath::lerp(T l, T r, float t) {
    return (1 - t) * l + t * r;
}

#endif //CTHEAD_INTERPOLATION_H
