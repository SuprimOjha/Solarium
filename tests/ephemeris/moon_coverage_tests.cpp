#include "solarium/celestial/body_registry.hpp"
#include "solarium/ephemeris/jpl_horizons_provider.hpp"
#include "solarium/ephemeris/spice_provider.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <string>

int main() {
    using namespace solarium;
    const std::array<ephemeris::BodyId, 20> moons = {
        ephemeris::BodyId::Moon,
        ephemeris::BodyId::Phobos, ephemeris::BodyId::Deimos,
        ephemeris::BodyId::Io, ephemeris::BodyId::Europa,
        ephemeris::BodyId::Ganymede, ephemeris::BodyId::Callisto,
        ephemeris::BodyId::Mimas, ephemeris::BodyId::Enceladus,
        ephemeris::BodyId::Tethys, ephemeris::BodyId::Dione,
        ephemeris::BodyId::Rhea, ephemeris::BodyId::Titan,
        ephemeris::BodyId::Iapetus, ephemeris::BodyId::Miranda,
        ephemeris::BodyId::Ariel, ephemeris::BodyId::Umbriel,
        ephemeris::BodyId::Titania, ephemeris::BodyId::Oberon,
        ephemeris::BodyId::Triton
    };

    celestial::BodyRegistry registry;
    registry.initializeSolarSystem();
    for (const auto moon : moons) {
        const auto* body = registry.find(std::string(ephemeris::bodyName(moon)));
        assert(body != nullptr);
        assert(!body->parentName().empty());
        assert(!ephemeris::horizonsBodyId(moon).empty());
        assert(ephemeris::spiceBodyId(moon) != 0);
    }

    std::cout << "MoonCoverageTests passed: 20 mapped bodies; runtime coverage remains kernel/provider-dependent.\n";
    return 0;
}
