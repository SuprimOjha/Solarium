#include "solarium/reference/frame_transformer.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace solarium;
    using reference::CoordinateState;
    using reference::FrameDescriptor;
    using reference::FrameTransformContext;
    using reference::FrameTransformer;
    using reference::ReferenceFrame;

    const time::JulianDate epoch(2'451'545.0);
    CoordinateState object;
    object.position = {100.0, 200.0, 300.0};
    object.velocity = {10.0, 20.0, 30.0};
    object.frame = ReferenceFrame::Barycentric;
    object.epoch = epoch;
    object.descriptor = FrameDescriptor::barycentric();

    CoordinateState sun;
    sun.position = {10.0, 20.0, 30.0};
    sun.velocity = {1.0, 2.0, 3.0};
    sun.frame = ReferenceFrame::Barycentric;
    sun.epoch = epoch;
    sun.descriptor = FrameDescriptor::barycentric();

    const auto heliocentric = FrameTransformer::transform(
        object,
        FrameDescriptor::heliocentric(),
        FrameTransformContext{&sun}
    );
    assert(heliocentric.position.x == 90.0);
    assert(heliocentric.velocity.z == 27.0);
    assert(heliocentric.center == ephemeris::BodyId::Sun);

    const auto barycentric = FrameTransformer::transform(
        heliocentric,
        FrameDescriptor::barycentric(),
        FrameTransformContext{&sun}
    );
    assert(std::abs(barycentric.position.x - object.position.x) < 1.0e-12);
    assert(std::abs(barycentric.velocity.y - object.velocity.y) < 1.0e-12);

    CoordinateState earth;
    earth.position = {90.0, 180.0, 270.0};
    earth.velocity = {9.0, 18.0, 27.0};
    earth.frame = ReferenceFrame::Barycentric;
    earth.epoch = epoch;
    earth.descriptor = FrameDescriptor::barycentric();

    const auto geocentric = FrameTransformer::transform(
        object,
        FrameDescriptor::geocentric(),
        FrameTransformContext{&earth}
    );
    assert(geocentric.position.x == 10.0);
    assert(geocentric.velocity.y == 2.0);
    assert(geocentric.center == ephemeris::BodyId::Earth);

    bool rejected = false;
    try {
        FrameTransformer::transform(
            object,
            FrameDescriptor::planetCentered(ephemeris::BodyId::Mars),
            FrameTransformContext{}
        );
    } catch (const reference::FrameTransformError&) {
        rejected = true;
    }
    assert(rejected);

    std::cout << "FrameTransformerTests passed.\n";
    return 0;
}
