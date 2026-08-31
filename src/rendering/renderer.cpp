#include "solarium/rendering/renderer.hpp"

#include <glad/gl.h>

#include <cmath>
#include <vector>

namespace {

struct Vertex {
    float x;
    float y;
    float z;
};

}

namespace solarium::rendering {

Renderer::Renderer(
    int,
    int
)
    : vao_(0),
      vbo_(0),
      ebo_(0),
      indexCount_(0),
      shader_(
          "assets/shaders/planet.vert",
          "assets/shaders/planet.frag"
      ) {

    createSphere();

    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {

    if (ebo_ != 0) {
        glDeleteBuffers(
            1,
            &ebo_
        );
    }

    if (vbo_ != 0) {
        glDeleteBuffers(
            1,
            &vbo_
        );
    }

    if (vao_ != 0) {
        glDeleteVertexArrays(
            1,
            &vao_
        );
    }
}

void Renderer::createSphere() {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    constexpr int latitudeBands = 32;
    constexpr int longitudeBands = 32;

    constexpr float pi =
        3.14159265359f;

    for (
        int lat = 0;
        lat <= latitudeBands;
        ++lat
    ) {

        const float theta =
            static_cast<float>(lat) /
            latitudeBands *
            pi;

        const float sinTheta =
            std::sin(theta);

        const float cosTheta =
            std::cos(theta);

        for (
            int lon = 0;
            lon <= longitudeBands;
            ++lon
        ) {

            const float phi =
                static_cast<float>(lon) /
                longitudeBands *
                2.0f *
                pi;

            const float sinPhi =
                std::sin(phi);

            const float cosPhi =
                std::cos(phi);

            vertices.push_back({
                sinTheta * cosPhi,
                cosTheta,
                sinTheta * sinPhi
            });
        }
    }

    for (
        int lat = 0;
        lat < latitudeBands;
        ++lat
    ) {

        for (
            int lon = 0;
            lon < longitudeBands;
            ++lon
        ) {

            const unsigned int first =
                static_cast<unsigned int>(
                    lat *
                    (longitudeBands + 1) +
                    lon
                );

            const unsigned int second =
                first +
                longitudeBands +
                1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    indexCount_ =
        static_cast<unsigned int>(
            indices.size()
        );

    glGenVertexArrays(
        1,
        &vao_
    );

    glGenBuffers(
        1,
        &vbo_
    );

    glGenBuffers(
        1,
        &ebo_
    );

    glBindVertexArray(vao_);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        vbo_
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<long long>(
            vertices.size() *
            sizeof(Vertex)
        ),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        ebo_
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<long long>(
            indices.size() *
            sizeof(unsigned int)
        ),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        nullptr
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Renderer::beginFrame() {

    glClearColor(
        0.005f,
        0.005f,
        0.015f,
        1.0f
    );

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );
}

void Renderer::renderBody(
    const celestial::CelestialBody& body,
    const Camera& camera
) {

    shader_.bind();

    shader_.setMat4(
        "uView",
        camera.viewMatrix()
    );

    shader_.setMat4(
        "uProjection",
        camera.projectionMatrix()
    );

    const auto& position =
        body.position();

    // Visualization scale:
    //
    // Real distance is ~1.5e11 meters.
    // We convert it to AU-scale units.
    constexpr float AU =
        1.495978707e11f;

    const float x =
        static_cast<float>(
            position.x / AU
        );

    const float y =
        static_cast<float>(
            position.y / AU
        );

    const float z =
        static_cast<float>(
            position.z / AU
        );

    shader_.setVec3(
        "uPosition",
        x,
        y,
        z
    );

    float scale = 0.1f;

    if (body.name() == "Sun") {
        scale = 0.35f;
    }
    else {
        scale = 0.08f;
    }

    shader_.setFloat(
        "uScale",
        scale
    );

    if (body.name() == "Sun") {

        shader_.setVec3(
            "uColor",
            1.0f,
            0.75f,
            0.15f
        );

    } else {

        shader_.setVec3(
            "uColor",
            0.15f,
            0.4f,
            1.0f
        );
    }

    glBindVertexArray(vao_);

    glDrawElements(
        GL_TRIANGLES,
        static_cast<int>(indexCount_),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);

    shader_.unbind();
}

void Renderer::endFrame() {
}

}