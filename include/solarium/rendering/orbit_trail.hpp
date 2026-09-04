#pragma once

#include "solarium/math/vec3.hpp"

#include <cstddef>
#include <vector>

namespace solarium::rendering {

class OrbitTrail {
public:
    explicit OrbitTrail(std::size_t maxPoints = 2000);

    void addPoint(const math::Vec3& point);

    void clear();

    [[nodiscard]]
    const std::vector<math::Vec3>& points() const noexcept;

    [[nodiscard]]
    std::size_t size() const noexcept;

    [[nodiscard]]
    std::size_t maxPoints() const noexcept;

private:
    std::vector<math::Vec3> points_;
    std::size_t maxPoints_;
};

}