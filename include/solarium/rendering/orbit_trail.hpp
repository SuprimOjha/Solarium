#pragma once

#include "solarium/math/vec3.hpp"

#include <cstddef>
#include <vector>

namespace solarium::rendering {

class OrbitTrail {
public:...
    explicit OrbitTrail(
        std::size_t maxPoints = 2'000
    );

    ~OrbitTrail();

    OrbitTrail(const OrbitTrail&) = delete;
    OrbitTrail& operator=(const OrbitTrail&) = delete;

    void addPoint(
        const math::Vec3& point
    );

    void clear();

    void upload();

    void draw() const;

    [[nodiscard]]
    std::size_t size() const noexcept;

private:
    std::vector<math::Vec3> points_;

    std::size_t maxPoints_;

    unsigned int vao_;
    unsigned int vbo_;

    bool dirty_;

    void initializeOpenGL();
};

}