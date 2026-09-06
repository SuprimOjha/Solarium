#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/shader.hpp"

#include <vector>

namespace solarium::rendering {

enum class VisualizationMode {
    Realistic,
    Presentation,
    Exploration
};

class Renderer {
public:
    Renderer(
        int width,
        int height
    );

    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void beginFrame();

    void renderBody(
        const celestial::CelestialBody& body,
        const Camera& camera,
        bool selected = false
    );

    void endFrame();

    void setVisualizationMode(VisualizationMode mode) noexcept;

    [[nodiscard]] VisualizationMode visualizationMode() const noexcept;

private:
    unsigned int vao_;
    unsigned int vbo_;
    unsigned int ebo_;

    unsigned int indexCount_;
    VisualizationMode visualizationMode_;

    Shader shader_;

    void createSphere();
};

}