#include "solarium/celestial/body_registry.hpp"

#include "solarium/math/constants.hpp"

namespace solarium::celestial {

BodyRegistry::BodyRegistry()
    : bodies_{} {
}

void BodyRegistry::clear() {
    bodies_.clear();
}

void BodyRegistry::add(
    const BodyDefinition& definition
) {

    bodies_.emplace_back(
        definition.name,
        definition.mass,
        definition.radius,
        definition.position,
        definition.velocity
    );
}

void BodyRegistry::initializeSolarSystem() {

    clear();

    using namespace math;

    // Sun
    add({
        "Sun",

        1.98847e30,
        6.9634e8,

        Vec3{
            0.0,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            0.0,
            0.0
        }
    });

    // Mercury
    add({
        "Mercury",

        3.3011e23,
        2.4397e6,

        Vec3{
            0.387098 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            47'360.0,
            0.0
        }
    });

    // Venus
    add({
        "Venus",

        4.8675e24,
        6.0518e6,

        Vec3{
            0.723332 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            35'020.0,
            0.0
        }
    });

    // Earth
    add({
        "Earth",

        5.9722e24,
        6.371e6,

        Vec3{
            AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            29'780.0,
            0.0
        }
    });

    // Mars
    add({
        "Mars",

        6.4171e23,
        3.3895e6,

        Vec3{
            1.523679 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            24'130.0,
            0.0
        }
    });

    // Jupiter
    add({
        "Jupiter",

        1.89813e27,
        6.9911e7,

        Vec3{
            5.2044 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            13'070.0,
            0.0
        }
    });

    // Saturn
    add({
        "Saturn",

        5.6834e26,
        5.8232e7,

        Vec3{
            9.5826 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            9'680.0,
            0.0
        }
    });

    // Uranus
    add({
        "Uranus",

        8.6810e25,
        2.5362e7,

        Vec3{
            19.2184 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            6'800.0,
            0.0
        }
    });

    // Neptune
    add({
        "Neptune",

        1.02413e26,
        2.4622e7,

        Vec3{
            30.11 * AstronomicalUnit,
            0.0,
            0.0
        },

        Vec3{
            0.0,
            5'430.0,
            0.0
        }
    });
}

std::vector<CelestialBody>&
BodyRegistry::bodies() noexcept {
    return bodies_;
}

const std::vector<CelestialBody>&
BodyRegistry::bodies() const noexcept {
    return bodies_;
}

std::size_t
BodyRegistry::size() const noexcept {
    return bodies_.size();
}

} // namespace solarium::celestial