#pragma once

#include "solarium/validation/validation_engine.hpp"

#include <string>

namespace solarium::validation {

[[nodiscard]] std::string toCsv(const ValidationReport& report);
[[nodiscard]] std::string toJson(const ValidationReport& report);
void writeCsv(const ValidationReport& report, const std::string& path);
void writeJson(const ValidationReport& report, const std::string& path);

} // namespace solarium::validation
