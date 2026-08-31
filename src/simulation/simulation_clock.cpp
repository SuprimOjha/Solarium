#include "solarium/simulation/simulation_clock.hpp"

#include <algorithm>

namespace solarium::simulation {

SimulationClock::SimulationClock()
    : simulationTime_(0.0),
      deltaTime_(0.0),
      timeScale_(86'400.0),
      paused_(false) {
}

void SimulationClock::update(
    double realDeltaTime
) {
    if (paused_) {
        deltaTime_ = 0.0;
        return;
    }

    deltaTime_ =
        realDeltaTime * timeScale_;

    simulationTime_ += deltaTime_;
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

void SimulationClock::increaseSpeed() {

    timeScale_ *= 2.0;

    timeScale_ =
        std::min(
            timeScale_,
            MaximumTimeScale
        );
}

void SimulationClock::decreaseSpeed() {

    timeScale_ *= 0.5;

    timeScale_ =
        std::max(
            timeScale_,
            MinimumTimeScale
        );
}

void SimulationClock::reset() {

    simulationTime_ = 0.0;
    deltaTime_ = 0.0;
    timeScale_ = 86'400.0;
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

    return timeScale_;
}

bool SimulationClock::paused()
    const noexcept {

    return paused_;
}

}