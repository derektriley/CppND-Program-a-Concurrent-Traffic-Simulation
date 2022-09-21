#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"
#include "TrafficLight.h"

// forward declarations to avoid include cycle
class Vehicle;

enum TrafficLightPhase {
    red,
    green
};

template <class T>
class MessageQueue
{
public:

    void send(T && phase);

    T receive();

private:
    std::deque<TrafficLightPhase> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
};

class TrafficLight : public TrafficObject
{
public:
    // constructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();

    void simulate();


private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
    MessageQueue<TrafficLightPhase> _queue;
};

#endif