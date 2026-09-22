#include "solarium/validation/validation_export.hpp"

#include "solarium/ephemeris/body_id.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace solarium::validation {
namespace {

[[nodiscard]] const char* timeScaleName(ephemeris::TimeScale scale) noexcept {
    switch (scale) {
    case ephemeris::TimeScale::UTC: return "UTC";
    case ephemeris::TimeScale::TAI: return "TAI";
    case ephemeris::TimeScale::TT: return "TT";
    case ephemeris::TimeScale::TDB: return "TDB";
    }
    return "UNKNOWN";
}

[[nodiscard]] const char* frameName(reference::ReferenceFrame frame) noexcept {
    switch (frame) {
    case reference::ReferenceFrame::Barycentric: return "BARYCENTRIC";
    case reference::ReferenceFrame::Heliocentric: return "HELIOCENTRIC";
    case reference::ReferenceFrame::Geocentric: return "GEOCENTRIC";
    case reference::ReferenceFrame::PlanetCentered: return "PLANET_CENTERED";
    case reference::ReferenceFrame::MoonCentered: return "MOON_CENTERED";
    }
    return "UNKNOWN";
}

void writeVec(std::ostringstream& output, const math::Vec3& vector) {
    output << std::setprecision(17) << vector.x << ',' << vector.y << ',' << vector.z;
}

} // namespace

std::string toCsv(const ValidationReport& report) {
    std::ostringstream output;
    output << "body,epoch,provider,dataset,frame,frame_name,center,time_scale,"
              "reference_px,reference_py,reference_pz,solarium_px,solarium_py,solarium_pz,"
              "position_error_x,position_error_y,position_error_z,position_error_magnitude,"
              "relative_position_error,reference_vx,reference_vy,reference_vz,solarium_vx,"
              "solarium_vy,solarium_vz,velocity_error_x,velocity_error_y,velocity_error_z,"
              "velocity_error_magnitude,relative_velocity_error,radial_error,along_track_error,cross_track_error\n";
    for (const auto& sample : report.samples) {
        output << ephemeris::bodyName(sample.body) << ','
               << std::setprecision(17) << sample.epoch.value() << ','
               << sample.provider << ',' << sample.dataset << ','
               << frameName(sample.frame) << ',' << sample.frameName << ','
               << (sample.center.has_value() ? ephemeris::bodyName(*sample.center) : "") << ','
               << timeScaleName(sample.timeScale) << ',';
        writeVec(output, sample.referencePosition); output << ',';
        writeVec(output, sample.solariumPosition); output << ',';
        writeVec(output, sample.metrics.positionError); output << ','
               << sample.metrics.positionMagnitude << ',';
        if (sample.metrics.relativePositionError.has_value()) output << *sample.metrics.relativePositionError;
        output << ',';
        writeVec(output, sample.referenceVelocity); output << ',';
        writeVec(output, sample.solariumVelocity); output << ',';
        writeVec(output, sample.metrics.velocityError); output << ','
               << sample.metrics.velocityMagnitude << ',';
        if (sample.metrics.relativeVelocityError.has_value()) output << *sample.metrics.relativeVelocityError;
        output << ',';
        if (sample.metrics.radialError.has_value()) output << *sample.metrics.radialError;
        output << ',';
        if (sample.metrics.alongTrackError.has_value()) output << *sample.metrics.alongTrackError;
        output << ',';
        if (sample.metrics.crossTrackError.has_value()) output << *sample.metrics.crossTrackError;
        output << '\n';
    }
    return output.str();
}

std::string toJson(const ValidationReport& report) {
    std::ostringstream output;
    output << "{\"body\":\"" << ephemeris::bodyName(report.body)
           << "\",\"provider\":\"" << report.provider
           << "\",\"dataset\":\"" << report.dataset
           << "\",\"frame\":\"" << frameName(report.frame)
           << "\",\"frameName\":\"" << report.frameName
           << "\",\"timeScale\":\"" << timeScaleName(report.timeScale)
           << "\",\"statistics\":{\"sampleCount\":" << report.statistics.sampleCount
           << ",\"minimumPositionError\":" << report.statistics.minimumPositionError
           << ",\"maximumPositionError\":" << report.statistics.maximumPositionError
           << ",\"meanPositionError\":" << report.statistics.meanPositionError
           << ",\"rmsPositionError\":" << report.statistics.rmsPositionError
           << ",\"minimumVelocityError\":" << report.statistics.minimumVelocityError
           << ",\"maximumVelocityError\":" << report.statistics.maximumVelocityError
           << ",\"meanVelocityError\":" << report.statistics.meanVelocityError
           << ",\"rmsVelocityError\":" << report.statistics.rmsVelocityError << "},\"samples\":[";
    for (std::size_t index = 0; index < report.samples.size(); ++index) {
        const auto& sample = report.samples[index];
        if (index != 0) output << ',';
        output << "{\"epoch\":" << std::setprecision(17) << sample.epoch.value()
               << ",\"positionErrorMagnitude\":" << sample.metrics.positionMagnitude
               << ",\"velocityErrorMagnitude\":" << sample.metrics.velocityMagnitude << '}';
    }
    output << "]}";
    return output.str();
}

void writeCsv(const ValidationReport& report, const std::string& path) {
    std::ofstream output(path);
    if (!output) throw std::runtime_error("unable to write validation CSV: " + path);
    output << toCsv(report);
}

void writeJson(const ValidationReport& report, const std::string& path) {
    std::ofstream output(path);
    if (!output) throw std::runtime_error("unable to write validation JSON: " + path);
    output << toJson(report);
}

} // namespace solarium::validation
