#include "solarium/rendering/orbit_trail.hpp"

#include <stdexcept>

namespace solarium::rendering {

OrbitTrail::OrbitTrail(std::size_t maxPoints)
    : maxPoints_(maxPoints) {

    if (maxPoints_ == 0) {
        throw std::invalid_argument(
            "OrbitTrail maxPoints must be greater than zero"
        );
    }

    points_.reserve(maxPoints_);
}

void OrbitTrail::addPoint(const math::Vec3& point) {

    if (points_.size() >= maxPoints_) {
        points_.erase(points_.begin());
    }

    points_.push_back(point);
}

void OrbitTrail::clear() {
    points_.clear();
}

const std::vector<math::Vec3>& OrbitTrail::points() const noexcept {
    return points_;
}

std::size_t OrbitTrail::size() const noexcept {
    return points_.size();
}

std::size_t OrbitTrail::maxPoints() const noexcept {
    return maxPoints_;
}

}