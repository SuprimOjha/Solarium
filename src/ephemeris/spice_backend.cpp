#include "solarium/ephemeris/spice_backend.hpp"

#include "solarium/ephemeris/ephemeris_error.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#if defined(SOLARIUM_HAS_SPICE)
#include <SpiceUsr.h>
#endif

namespace solarium::ephemeris {
namespace {

[[nodiscard]] const char* timeScaleName(TimeScale timeScale) {
    switch (timeScale) {
    case TimeScale::UTC: return "UTC";
    case TimeScale::TT: return "TT";
    case TimeScale::TDB: return "TDB";
    }
    throw EphemerisError(EphemerisErrorCode::UnsupportedTimeScale, "unsupported SPICE time scale");
}

#if defined(SOLARIUM_HAS_SPICE)
void throwSpiceFailure(const char* operation) {
    SpiceChar message[1841]{};
    getmsg_c("LONG", static_cast<SpiceInt>(sizeof(message)), message);
    reset_c();
    throw EphemerisError(EphemerisErrorCode::ProviderFailure, std::string("SPICE ") + operation + " failed: " + message);
}
#endif

} // namespace

double CspiceStateBackend::toEphemerisTime(
    time::JulianDate epoch,
    TimeScale timeScale
) const {
#if defined(SOLARIUM_HAS_SPICE)
    std::ostringstream input;
    input << "JD " << std::fixed << std::setprecision(9) << epoch.value()
          << ' ' << timeScaleName(timeScale);
    SpiceDouble ephemerisTime = 0.0;
    str2et_c(input.str().c_str(), &ephemerisTime);
    if (failed_c()) {
        throwSpiceFailure("time conversion");
    }
    return ephemerisTime;
#else
    (void)epoch;
    (void)timeScale;
    throw EphemerisError(
        EphemerisErrorCode::MissingKernel,
        "SPICE support is disabled; configure with SOLARIUM_ENABLE_SPICE=ON"
    );
#endif
}

SpiceState CspiceStateBackend::queryState(
    int targetNaifId,
    int centerNaifId,
    double ephemerisTime,
    std::string_view frameName
) const {
#if defined(SOLARIUM_HAS_SPICE)
    SpiceDouble state[6]{};
    SpiceDouble lightTime = 0.0;
    const std::string target = std::to_string(targetNaifId);
    const std::string center = std::to_string(centerNaifId);
    const std::string frame(frameName);
    spkezr_c(
        target.c_str(), ephemerisTime, frame.c_str(), "NONE", center.c_str(),
        state, &lightTime
    );
    if (failed_c()) {
        throwSpiceFailure("state query");
    }
    return SpiceState{
        {state[0], state[1], state[2]},
        {state[3], state[4], state[5]},
        frame,
        centerNaifId,
        ephemerisTime
    };
#else
    (void)targetNaifId;
    (void)centerNaifId;
    (void)ephemerisTime;
    (void)frameName;
    throw EphemerisError(
        EphemerisErrorCode::MissingKernel,
        "SPICE support is disabled; configure with SOLARIUM_ENABLE_SPICE=ON"
    );
#endif
}

} // namespace solarium::ephemeris
