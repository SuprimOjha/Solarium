#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/shader.hpp"

namespace solarium::rendering {

class RingRenderer {
public:
    RingRenderer();
    ~RingRenderer();

    RingRenderer(const RingRenderer&) = delete;
    RingRenderer& operator=(const RingRenderer&) = delete;

    void render(
        const celestial::CelestialBody& body,
        const Camera& camera
    );

private:
    unsigned int vao_;
    unsigned int vbo_;
    unsigned int vertexCount_;
    Shader shader_;

    void createRings();
};

} // namespace solarium::rendering
