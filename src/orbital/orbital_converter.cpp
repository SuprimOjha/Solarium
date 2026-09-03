#include "solarium/orbital/orbital_converter.hpp"
#include <numbers>
#include <cmath>
#include <stdexcept>

namespace solarium::orbital {

OrbitalElements OrbitalConverter::fromState(
    const math::Vec3& position,
    const math::Vec3& velocity,
    double mu
) {
    constexpr double epsilon = 1e-12;

    const double r = position.magnitude();
    const double v = velocity.magnitude();

    if (r <= epsilon) {
        throw std::invalid_argument(
            "Position magnitude is too small."
        );
    }

    if (mu <= 0.0) {
        throw std::invalid_argument(
            "Gravitational parameter must be positive."
        );
    }

    const math::Vec3 hVector =
        position.cross(velocity);

    const double h =
        hVector.magnitude();

    if (h <= epsilon) {
        throw std::invalid_argument(
            "Angular momentum is too small."
        );
    }

    /*
     * Specific orbital energy
     *
     * epsilon = v²/2 - mu/r
     */

    const double energy =
        0.5 * v * v -
        mu / r;

    /*
     * Eccentricity vector
     *
     * e = ((v × h) / mu) - r_hat
     */

    const math::Vec3 eccentricityVector =
        velocity.cross(hVector) / mu -
        position / r;

    const double eccentricity =
        eccentricityVector.magnitude();

    /*
     * Semi-major axis
     *
     * a = -mu / (2 epsilon)
     */

    if (std::abs(energy) <= epsilon) {
        throw std::invalid_argument(
            "Parabolic orbit is not supported."
        );
    }

    const double semiMajorAxis =
        -mu / (2.0 * energy);

    /*
     * Inclination
     */

    double inclination =
        std::acos(
            hVector.z / h
        );

    /*
     * Node vector
     *
     * n = k × h
     */

    const math::Vec3 k(
        0.0,
        0.0,
        1.0
    );

    const math::Vec3 nodeVector =
        k.cross(hVector);

    const double nodeMagnitude =
        nodeVector.magnitude();

    double longitudeAscendingNode = 0.0;

    if (nodeMagnitude > epsilon) {

        longitudeAscendingNode =
            std::atan2(
                nodeVector.y,
                nodeVector.x
            );

        if (longitudeAscendingNode < 0.0) {
            longitudeAscendingNode +=
                2.0 * std::numbers::pi;
        }
    }

    /*
     * Argument of periapsis
     */

    double argumentOfPeriapsis = 0.0;

    if (
        nodeMagnitude > epsilon &&
        eccentricity > epsilon
    ) {

        argumentOfPeriapsis =
            std::acos(
                nodeVector.dot(eccentricityVector) /
                (nodeMagnitude * eccentricity)
            );

        if (eccentricityVector.z < 0.0) {
            argumentOfPeriapsis =
                2.0 * std::numbers::pi -
                argumentOfPeriapsis;
        }
    }

    /*
     * True anomaly
     */

    double trueAnomaly = 0.0;

    if (eccentricity > epsilon) {

        trueAnomaly =
            std::acos(
                eccentricityVector.dot(position) /
                (eccentricity * r)
            );

        if (position.dot(velocity) < 0.0) {
            trueAnomaly =
                2.0 * std::numbers::pi -
                trueAnomaly;
        }
    }

    /*
     * Eccentric anomaly
     *
     * Only valid for elliptical orbits.
     */

    double eccentricAnomaly = 0.0;
    double meanAnomaly = 0.0;

    if (eccentricity < 1.0 - epsilon) {

        eccentricAnomaly =
            2.0 *
            std::atan2(
                std::sqrt(1.0 - eccentricity) *
                    std::sin(trueAnomaly / 2.0),

                std::sqrt(1.0 + eccentricity) *
                    std::cos(trueAnomaly / 2.0)
            );

        if (eccentricAnomaly < 0.0) {
            eccentricAnomaly +=
                2.0 * std::numbers::pi;
        }

        /*
         * Kepler's equation
         *
         * M = E - e sin(E)
         */

        meanAnomaly =
            eccentricAnomaly -
            eccentricity *
                std::sin(eccentricAnomaly);

        if (meanAnomaly < 0.0) {
            meanAnomaly +=
                2.0 * std::numbers::pi;
        }
    }

    /*
     * Orbital period
     *
     * T = 2π sqrt(a³ / mu)
     */

    double period = 0.0;

    if (semiMajorAxis > 0.0) {

        period =
            2.0 * std::numbers::pi *
            std::sqrt(
                (semiMajorAxis *
                 semiMajorAxis *
                 semiMajorAxis) /
                mu
            );
    }

    OrbitalElements elements;

    elements.semiMajorAxis =
        semiMajorAxis;

    elements.eccentricity =
        eccentricity;

    elements.inclination =
        inclination;

    elements.longitudeAscendingNode =
        longitudeAscendingNode;

    elements.argumentOfPeriapsis =
        argumentOfPeriapsis;

    elements.trueAnomaly =
        trueAnomaly;

    elements.eccentricAnomaly =
        eccentricAnomaly;

    elements.meanAnomaly =
        meanAnomaly;

    elements.specificAngularMomentum =
        h;

    elements.orbitalEnergy =
        energy;

    elements.period =
        period;

    return elements;
}

} // namespace solarium::orbital