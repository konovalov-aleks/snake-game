// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from snake.djinni

#pragma once

#include "color_model.hpp"
#include "vector_model.hpp"
#include <utility>
#include <vector>

struct SnakeModel final {
    std::vector<VectorModel> points;
    VectorModel left_eye;
    VectorModel right_eye;
    ColorModel color;

    SnakeModel(std::vector<VectorModel> points_,
               VectorModel left_eye_,
               VectorModel right_eye_,
               ColorModel color_)
    : points(std::move(points_))
    , left_eye(std::move(left_eye_))
    , right_eye(std::move(right_eye_))
    , color(std::move(color_))
    {}
};
