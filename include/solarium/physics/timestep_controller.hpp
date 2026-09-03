#pragma once

namespace solarium::physics {

struct TimestepControllerConfig {
    double absoluteTolerance = 1e-10;
    double relativeTolerance = 1e-8;

    double minimumTimestep = 1e-6;
    double maximumTimestep = 10.0;

    double safetyFactor = 0.9;

    double minimumScale = 0.2;
    double maximumScale = 5.0;
};

class TimestepController {
public:
    explicit TimestepController(
        const TimestepControllerConfig& config = {}
    );

    [[nodiscard]]
    bool acceptStep(
        double error,
        double scale
    ) const noexcept;

    [[nodiscard]]
    double nextTimestep(
        double currentTimestep,
        double error
    ) const noexcept;

    [[nodiscard]]
    const TimestepControllerConfig&
    config() const noexcept;

private:
    TimestepControllerConfig config_;
};

} // namespace solarium::physics