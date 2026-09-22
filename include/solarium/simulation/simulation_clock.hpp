#pragma once

#include "solarium/time/astronomical_time.hpp"

namespace solarium::simulation {

class SimulationClock {
public:
    SimulationClock();

    void update(double realDeltaTime);

    void pause();
    void resume();
    void togglePause();

    void step(double simulationSeconds);

    void increaseSpeed();
    void decreaseSpeed();

    void reset();

    [[nodiscard]]
    double simulationTime() const noexcept;

    [[nodiscard]]
    double deltaTime() const noexcept;

    [[nodiscard]]
    double timeScale() const noexcept;

    [[nodiscard]]
    bool paused() const noexcept;

    [[nodiscard]]
    const time::AstronomicalTime& currentEpoch() const noexcept;

    [[nodiscard]]
    time::TimeScale astronomicalTimeScale() const noexcept;

private:
    double simulationTime_;
    double deltaTime_;
    double speed_;

    time::AstronomicalTime currentEpoch_;

    bool paused_;

    static constexpr double MinimumTimeScale = 1.0;
    static constexpr double MaximumTimeScale = 31'536'000.0;
};

}