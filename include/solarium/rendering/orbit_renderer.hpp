#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/shader.hpp"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace solarium::rendering {

class OrbitRenderer {
public:
    explicit OrbitRenderer(std::size_t segments = 192);
    ~OrbitRenderer();

    OrbitRenderer(const OrbitRenderer&) = delete;
    OrbitRenderer& operator=(const OrbitRenderer&) = delete;

    void render(
        const celestial::CelestialBody& body,
        const Camera& camera,
        const math::Vec3& parentPosition = {},
        bool visible = true
    );

    void clear();

private:
    struct OrbitBuffer {
        unsigned int vao = 0;
        unsigned int vbo = 0;
        std::size_t count = 0;
    };

    std::size_t segments_;
    std::unordered_map<std::string, OrbitBuffer> buffers_;
    Shader shader_;

    OrbitBuffer& bufferFor(const celestial::CelestialBody& body);
    void buildBuffer(const celestial::CelestialBody& body, OrbitBuffer& buffer);
};

} // namespace solarium::rendering
