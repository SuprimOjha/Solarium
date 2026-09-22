#include "solarium/ephemeris/jpl_horizons_provider.hpp"

#include <cassert>
#include <iostream>

int main() {
    using namespace solarium;

    ephemeris::JplHorizonsProvider provider;
    const auto state = provider.getState(ephemeris::EphemerisRequest{
        ephemeris::BodyId::Earth,
        time::JulianDate(2'451'545.0),
        ephemeris::BodyId::Sun,
        reference::ReferenceFrame::Heliocentric,
        ephemeris::TimeScale::TDB
    });

    assert(state.body() == ephemeris::BodyId::Earth);
    assert(state.source().provider == "JPL Horizons");
    std::cout << "JplHorizonsLiveTests passed.\n";
    return 0;
}
