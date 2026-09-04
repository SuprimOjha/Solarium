#include "solarium/reference/frame_transform.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    std::cout << "Running Reference Frame tests...\n";

    using namespace solarium;

    math::Vec3 objectPosition(10.0, 20.0, 30.0);
    math::Vec3 originPosition(2.0, 5.0, 8.0);

    auto heliocentric =
        reference::FrameTransform::position(
            objectPosition,
            originPosition,
            reference::ReferenceFrame::Barycentric,
            reference::ReferenceFrame::Heliocentric
        );

    assert(std::abs(heliocentric.x - 8.0) < 1e-12);
    assert(std::abs(heliocentric.y - 15.0) < 1e-12);
    assert(std::abs(heliocentric.z - 22.0) < 1e-12);

    auto barycentric =
        reference::FrameTransform::position(
            heliocentric,
            originPosition,
            reference::ReferenceFrame::Heliocentric,
            reference::ReferenceFrame::Barycentric
        );

    assert(std::abs(barycentric.x - 10.0) < 1e-12);
    assert(std::abs(barycentric.y - 20.0) < 1e-12);
    assert(std::abs(barycentric.z - 30.0) < 1e-12);

    std::cout << "Reference Frame tests passed.\n";

    return 0;
}