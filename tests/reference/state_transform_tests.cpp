#include "solarium/reference/state_transform.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    std::cout << "Running State Transform tests...\n";

    using namespace solarium;

    reference::CoordinateState object;

    object.position =
        math::Vec3(100.0, 200.0, 300.0);

    object.velocity =
        math::Vec3(10.0, 20.0, 30.0);

    object.frame =
        reference::ReferenceFrame::Barycentric;

    reference::CoordinateState sun;

    sun.position =
        math::Vec3(10.0, 20.0, 30.0);

    sun.velocity =
        math::Vec3(1.0, 2.0, 3.0);

    sun.frame =
        reference::ReferenceFrame::Barycentric;

    const auto heliocentric =
        reference::StateTransform::transform(
            object,
            sun,
            reference::ReferenceFrame::Heliocentric
        );

    assert(
        std::abs(
            heliocentric.position.x - 90.0
        ) < 1e-12
    );

    assert(
        std::abs(
            heliocentric.position.y - 180.0
        ) < 1e-12
    );

    assert(
        std::abs(
            heliocentric.position.z - 270.0
        ) < 1e-12
    );

    assert(
        std::abs(
            heliocentric.velocity.x - 9.0
        ) < 1e-12
    );

    assert(
        std::abs(
            heliocentric.velocity.y - 18.0
        ) < 1e-12
    );

    assert(
        std::abs(
            heliocentric.velocity.z - 27.0
        ) < 1e-12
    );

    assert(
        heliocentric.frame ==
        reference::ReferenceFrame::Heliocentric
    );

    std::cout << "State Transform tests passed.\n";

    return 0;
}