#pragma once

#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/shader.hpp"

#include <cstddef>
#include <vector>

namespace solarium::rendering {

class StarFieldRenderer {
public:
    explicit StarFieldRenderer(std::size_t starCount = 2500);
    ~StarFieldRenderer();

    StarFieldRenderer(const StarFieldRenderer&) = delete;
    StarFieldRenderer& operator=(const StarFieldRenderer&) = delete;

    void render(const Camera& camera);

private:
    struct Star {
        float x;
        float y;
        float z;
        float brightness;
        float temperature;
    };

    unsigned int vao_;
    unsigned int vbo_;
    std::size_t count_;
    Shader shader_;

    void createStars(std::size_t count);
};

} // namespace solarium::rendering
