#include "solarium/simulation/simulation_clock.hpp"

#include <algorithm>
#include <cmath>

namespace solarium::simulation {

SimulationClock::SimulationClock()
    : simulationTime_(0.0),
      deltaTime_(0.0),
            speed_(86'400.0),
            currentEpoch_(),
      paused_(false) {
}

void SimulationClock::update(
    double realDeltaTime
) {
    if (paused_) {
        deltaTime_ = 0.0;
        return;
    }

    deltaTime_ = realDeltaTime * speed_;

    simulationTime_ += deltaTime_;
    currentEpoch_.advanceSeconds(deltaTime_);
}

void SimulationClock::pause() {
    paused_ = true;
}

void SimulationClock::resume() {
    paused_ = false;
}

void SimulationClock::togglePause() {
    paused_ = !paused_;
}

void SimulationClock::step(double simulationSeconds) {
    if (!std::isfinite(simulationSeconds) || simulationSeconds <= 0.0) {
        return;
    }
    deltaTime_ = simulationSeconds;
    simulationTime_ += simulationSeconds;
    currentEpoch_.advanceSeconds(simulationSeconds);
}

void SimulationClock::increaseSpeed() {

    speed_ *= 2.0;

    speed_ =
        std::min(
            speed_,
            MaximumTimeScale
        );
}

void SimulationClock::decreaseSpeed() {

    speed_ *= 0.5;

    speed_ =
        std::max(
            speed_,
            MinimumTimeScale
        );
}

void SimulationClock::reset() {

    simulationTime_ = 0.0;
    deltaTime_ = 0.0;
    speed_ = 86'400.0;
    currentEpoch_ = time::AstronomicalTime();
    paused_ = false;
}

double SimulationClock::simulationTime()
    const noexcept {

    return simulationTime_;
}

double SimulationClock::deltaTime()
    const noexcept {

    return deltaTime_;
}

double SimulationClock::timeScale()
    const noexcept {

    return speed_;
}

bool SimulationClock::paused()
    const noexcept {

    return paused_;
}

const time::AstronomicalTime& SimulationClock::currentEpoch() const noexcept {
    return currentEpoch_;
}

time::TimeScale SimulationClock::astronomicalTimeScale() const noexcept {
    return currentEpoch_.scale();
}

}