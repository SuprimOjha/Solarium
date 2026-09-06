#include "solarium/celestial/body_registry.hpp"

#include "solarium/math/constants.hpp"

#include <algorithm>
#include <cmath>
#include <string>

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

    bodies_.back().setMetadata(
        definition.type,
        definition.parentName == nullptr
            ? std::string{}
            : definition.parentName,
        definition.orbitalParameters,
        definition.rotation,
        definition.visualProperties
    );
}

CelestialBody*
BodyRegistry::find(
    const std::string& name
) noexcept {
    const auto iterator = std::find_if(
        bodies_.begin(),
        bodies_.end(),
        [&name](const CelestialBody& body) {
            return body.name() == name;
        }
    );

    return iterator == bodies_.end() ? nullptr : &*iterator;
}

const CelestialBody*
BodyRegistry::find(
    const std::string& name
) const noexcept {
    const auto iterator = std::find_if(
        bodies_.begin(),
        bodies_.end(),
        [&name](const CelestialBody& body) {
            return body.name() == name;
        }
    );

    return iterator == bodies_.end() ? nullptr : &*iterator;
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

    const auto setVisuals = [this](
        const char* name,
        BodyType type,
        const Vec3& color,
        double emissive,
        double radiusMultiplier,
        bool atmosphere,
        bool rings,
        double rotationPeriod,
        double axialTilt
    ) {
        auto* body = find(name);
        if (body == nullptr) {
            return;
        }

        body->setMetadata(
            type,
            body->parentName(),
            body->orbitalParameters(),
            RotationParameters{
                rotationPeriod,
                axialTilt,
                0.0
            },
            VisualProperties{
                color,
                atmosphere ? color : Vec3{},
                emissive,
                radiusMultiplier,
                atmosphere,
                rings
            }
        );
    };

    setVisuals("Sun", BodyType::Star, {1.0, 0.63, 0.12}, 1.0, 6.0, false, false, 2.2e6, 0.0);
    setVisuals("Mercury", BodyType::Planet, {0.45, 0.43, 0.40}, 0.0, 1'800.0, false, false, 5.064e6, 0.034);
    setVisuals("Venus", BodyType::Planet, {0.82, 0.68, 0.42}, 0.0, 1'200.0, true, false, -2.096e7, 3.096);
    setVisuals("Earth", BodyType::Planet, {0.12, 0.32, 0.72}, 0.0, 700.0, true, false, 86'164.1, 0.4091);
    setVisuals("Mars", BodyType::Planet, {0.67, 0.20, 0.10}, 0.0, 1'800.0, true, false, 88'642.7, 0.4397);
    setVisuals("Jupiter", BodyType::Planet, {0.72, 0.54, 0.36}, 0.0, 120.0, true, false, 35'730.0, 0.4014);
    setVisuals("Saturn", BodyType::Planet, {0.76, 0.67, 0.45}, 0.0, 140.0, true, true, 38'018.0, 0.4665);
    setVisuals("Uranus", BodyType::Planet, {0.35, 0.75, 0.78}, 0.0, 250.0, true, false, -62'064.0, 1.706);
    setVisuals("Neptune", BodyType::Planet, {0.12, 0.25, 0.68}, 0.0, 250.0, true, false, 57'996.0, 0.4948);

    const auto setOrbit = [this](
        const char* name,
        double semiMajorAxis,
        double eccentricity,
        double period
    ) {
        auto* body = find(name);
        if (body == nullptr) {
            return;
        }

        body->setMetadata(
            body->type(),
            body->parentName(),
            OrbitalParameters{
                semiMajorAxis,
                eccentricity,
                0.0,
                0.0,
                0.0,
                0.0,
                period
            },
            body->rotation(),
            body->visualProperties()
        );
    };

    setOrbit("Mercury", 0.387098 * AstronomicalUnit, 0.2056, 87.969 * 86'400.0);
    setOrbit("Venus", 0.723332 * AstronomicalUnit, 0.0068, 224.701 * 86'400.0);
    setOrbit("Earth", AstronomicalUnit, 0.0167, 365.256 * 86'400.0);
    setOrbit("Mars", 1.523679 * AstronomicalUnit, 0.0934, 686.98 * 86'400.0);
    setOrbit("Jupiter", 5.2044 * AstronomicalUnit, 0.0489, 4332.59 * 86'400.0);
    setOrbit("Saturn", 9.5826 * AstronomicalUnit, 0.0565, 10'759.22 * 86'400.0);
    setOrbit("Uranus", 19.2184 * AstronomicalUnit, 0.0463, 30'688.5 * 86'400.0);
    setOrbit("Neptune", 30.11 * AstronomicalUnit, 0.0095, 60'182.0 * 86'400.0);

    const auto addMoon = [this](
        const char* name,
        const char* parentName,
        double mass,
        double radius,
        double orbitalRadius,
        double orbitalPeriod,
        const Vec3& color,
        double radiusMultiplier
    ) {
        const auto* parent = find(parentName);
        if (parent == nullptr || orbitalPeriod <= 0.0) {
            return;
        }

        const double orbitalSpeed =
            2.0 * 3.14159265358979323846 * orbitalRadius /
            orbitalPeriod;

        add({
            name,
            mass,
            radius,
            parent->position() + Vec3{orbitalRadius, 0.0, 0.0},
            parent->velocity() + Vec3{0.0, orbitalSpeed, 0.0},
            BodyType::Moon,
            parentName,
            OrbitalParameters{
                orbitalRadius,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                orbitalPeriod
            },
            RotationParameters{
                orbitalPeriod,
                0.0,
                0.0
            },
            VisualProperties{
                color,
                Vec3{},
                0.0,
                radiusMultiplier,
                false,
                false
            }
        });
    };

    constexpr double day = 86'400.0;
    addMoon("Moon", "Earth", 7.342e22, 1.7374e6, 384.4e6, 27.3217 * day, {0.58, 0.59, 0.60}, 2'000.0);
    addMoon("Phobos", "Mars", 1.0659e16, 11.27e3, 9.377e6, 0.31891 * day, {0.32, 0.29, 0.25}, 12'000.0);
    addMoon("Deimos", "Mars", 1.4762e15, 6.2e3, 23.463e6, 1.26244 * day, {0.40, 0.35, 0.29}, 14'000.0);
    addMoon("Io", "Jupiter", 8.9319e22, 1.8216e6, 421.7e6, 1.769 * day, {0.86, 0.68, 0.36}, 1'300.0);
    addMoon("Europa", "Jupiter", 4.7998e22, 1.5608e6, 671.1e6, 3.551 * day, {0.72, 0.66, 0.52}, 1'500.0);
    addMoon("Ganymede", "Jupiter", 1.4819e23, 2.6341e6, 1.0704e9, 7.155 * day, {0.48, 0.43, 0.37}, 950.0);
    addMoon("Callisto", "Jupiter", 1.0759e23, 2.4103e6, 1.8827e9, 16.689 * day, {0.34, 0.31, 0.28}, 1'050.0);
    addMoon("Titan", "Saturn", 1.3452e23, 2.5747e6, 1.2219e9, 15.945 * day, {0.75, 0.58, 0.33}, 1'000.0);
    addMoon("Rhea", "Saturn", 2.3065e21, 7.638e5, 5.271e8, 4.518 * day, {0.62, 0.61, 0.58}, 2'400.0);
    addMoon("Enceladus", "Saturn", 1.0802e20, 2.521e5, 2.3795e8, 1.370 * day, {0.78, 0.82, 0.86}, 6'000.0);
    addMoon("Iapetus", "Saturn", 1.8056e21, 7.345e5, 3.5608e9, 79.3215 * day, {0.42, 0.40, 0.36}, 2'500.0);
    addMoon("Dione", "Saturn", 1.0955e21, 5.614e5, 3.774e8, 2.737 * day, {0.66, 0.67, 0.66}, 3'200.0);
    addMoon("Tethys", "Saturn", 6.1745e20, 5.311e5, 2.9467e8, 1.888 * day, {0.70, 0.72, 0.72}, 3'400.0);
    addMoon("Mimas", "Saturn", 3.7493e19, 1.982e5, 1.8554e8, 0.942 * day, {0.58, 0.59, 0.60}, 8'000.0);
    addMoon("Titania", "Uranus", 3.527e21, 7.889e5, 4.3584e8, 8.706 * day, {0.53, 0.54, 0.55}, 2'500.0);
    addMoon("Oberon", "Uranus", 3.014e21, 7.614e5, 5.8352e8, 13.463 * day, {0.45, 0.46, 0.47}, 2'600.0);
    addMoon("Ariel", "Uranus", 1.353e21, 5.789e5, 1.9102e8, 2.520 * day, {0.62, 0.64, 0.65}, 3'500.0);
    addMoon("Umbriel", "Uranus", 1.172e21, 5.847e5, 2.66e8, 4.144 * day, {0.34, 0.35, 0.37}, 3'500.0);
    addMoon("Miranda", "Uranus", 6.59e19, 2.358e5, 1.299e8, 1.413 * day, {0.52, 0.55, 0.58}, 7'000.0);
    addMoon("Triton", "Neptune", 2.14e22, 1.3534e6, 3.5476e8, 5.877 * day, {0.62, 0.66, 0.72}, 1'800.0);
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