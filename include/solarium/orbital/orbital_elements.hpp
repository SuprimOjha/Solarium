#pragma once

namespace solarium::orbital {

struct OrbitalElements {

    // Classical orbital elements

    double semiMajorAxis = 0.0;
    double eccentricity = 0.0;

    // radians
    double inclination = 0.0;
    double longitudeAscendingNode = 0.0;
    double argumentOfPeriapsis = 0.0;
    double trueAnomaly = 0.0;

    // radians
    double eccentricAnomaly = 0.0;
    double meanAnomaly = 0.0;

    // orbital quantities

    double specificAngularMomentum = 0.0;
    double orbitalEnergy = 0.0;
    double period = 0.0;
};

} // namespace solarium::orbital