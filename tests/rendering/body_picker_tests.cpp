#include "solarium/rendering/body_picker.hpp"

#include "solarium/math/vec3.hpp"

#include <cassert>
#include <iostream>
#include <vector>

int main() {
    using namespace solarium;

    std::vector<celestial::CelestialBody> bodies;
    bodies.emplace_back("Sun", 1.0, 1.0, math::Vec3{}, math::Vec3{});

    rendering::Camera camera;
    const std::size_t selected = rendering::BodyPicker::pick(
        bodies,
        camera,
        640.0,
        360.0,
        1280,
        720
    );

    assert(selected == 0);
    std::cout << "BodyPickerTests passed.\n";
    return 0;
}
