// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from snake.djinni

#pragma once

#include "point.hpp"
#include <utility>
#include <vector>

struct SnakeModel final {
    std::vector<Point> points;

    SnakeModel(std::vector<Point> points_)
    : points(std::move(points_))
    {}
};
