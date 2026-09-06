#include "solarium/rendering/camera.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace solarium;

    rendering::Camera camera;
    assert(camera.mode() == rendering::CameraMode::Orbit);
    assert(camera.zoomSpeed() > 0.0f);

    const math::Vec3 initial = camera.position();
    camera.setMode(rendering::CameraMode::Follow);
    camera.setMovementSpeed(0.2f);
    camera.focus({1.0, 0.0, 0.0});
    camera.update(0.25);

    const math::Vec3 moved = camera.position();
    assert(std::isfinite(moved.x));
    assert(std::isfinite(moved.y));
    assert(std::isfinite(moved.z));
    assert(moved.x != initial.x || moved.y != initial.y || moved.z != initial.z);

    camera.reset();
    assert(camera.mode() == rendering::CameraMode::Orbit);
    std::cout << "CameraTests passed.\n";
    return 0;
}
