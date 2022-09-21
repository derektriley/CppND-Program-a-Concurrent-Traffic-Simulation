#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> ul(_mutex);
    _cond.wait(ul, [this]{return !_queue.empty();});
    T message = std::move(_queue.back());
    _queue.pop_back();
    return std::move(message);
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lg(_mutex);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */
 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase phase = _queue.receive();
        if (TrafficLightPhase::green == phase) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
   threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::random_device randDevice;
    std::mt19937 engine(randDevice());
    std::uniform_real_distribution<> distribution(4000, 6000);

    double cycleDuration = distribution(engine);

    std::chrono::time_point<std::chrono::system_clock> cycleTime = std::chrono::system_clock::now();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        int64_t lastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - cycleTime).count();
        if (static_cast<double>(lastUpdate) >= cycleDuration) {
            if (_currentPhase == TrafficLightPhase::red) {
                _currentPhase = TrafficLightPhase::green;
            } else {
                _currentPhase = TrafficLightPhase::red;
            }
            // send update to message queue using move semantics
            _queue.send(std::move(_currentPhase));
            cycleTime = std::chrono::system_clock::now();
        }
    }
}