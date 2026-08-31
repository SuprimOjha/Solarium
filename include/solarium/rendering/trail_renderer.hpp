#pragma once

#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/orbit_trail.hpp"
#include "solarium/rendering/shader.hpp"

namespace solarium::rendering {

class TrailRenderer {
public:
    TrailRenderer();

    void render(
        const OrbitTrail& trail,
        const Camera& camera
    );

private:
    Shader shader_;
};

}