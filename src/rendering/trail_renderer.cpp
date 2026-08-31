#include "solarium/rendering/trail_renderer.hpp"

namespace solarium::rendering {

TrailRenderer::TrailRenderer()
    : shader_(
        "assets/shaders/trail.vert",
        "assets/shaders/trail.frag"
    ) {
}

void TrailRenderer::render(
    const OrbitTrail& trail,
    const Camera& camera
) {

    if (trail.size() < 2) {
        return;
    }

    shader_.bind();

    shader_.setMat4(
        "uView",
        camera.viewMatrix()
    );

    shader_.setMat4(
        "uProjection",
        camera.projectionMatrix()
    );

    shader_.setVec3(
        "uColor",
        0.15f,
        0.35f,
        1.0f
    );

    trail.draw();

    shader_.unbind();
}

}