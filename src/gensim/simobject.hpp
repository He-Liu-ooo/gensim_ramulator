#ifndef GENSIM_INCLUDE_SIMULATOR_SIMOBJECT_HPP__
#define GENSIM_INCLUDE_SIMULATOR_SIMOBJECT_HPP__

// #include <functional>
// #include <iostream>
// #include <memory>

#include "simulator/eventq.hpp"

class EventManager { 
public:
  EventManager(EventQueue& eventq) : _eventq{eventq} {}
  virtual ~EventManager() = default;

  // trigger an event, insert to the event queue
  // bass by value would copy Event& (lv), and move Event&& (rv)
  void emitEvent(Event e) { _eventq.push(std::move(e)); };       // ME: after we use move func, e is empty
  template <class... Args>         // ME: Args can match multiple types
  void emitEvent(Args&&... args) {
    _eventq.emplace(std::forward<Args>(args)...);
  }
  // deSchedule maybe used in speculative execution
  // void deSchedule(Event *e);

  // get the next event from the event queue
  // const Event& getNextEvent() { return _eventq.top(); }
  // get the event queue
  const EventQueue& eventq() { return _eventq; }

  // current system tick
  Tick curTick() const { return _eventq.curTick(); }

private:
  // EventQueue& _eventq;
  EventQueue& _eventq;
  // std::shared_ptr<EventQueue> _eventq;
 };

class SimObject : public EventManager {
public:
  // constructor
  SimObject(std::string name, EventQueue& eventq)
      : EventManager{eventq}, _name{std::move(name)} {}
  // deconsructor
  virtual ~SimObject() = default;

  // for object comparison
  friend bool operator==(const SimObject& lhs, const SimObject& rhs) {
    return lhs._name == rhs._name;
  }

  // util functions
  virtual std::function<void(const Event&)> eventHandlerWrapper() {
    // or use lambda functions
    return std::bind(&SimObject::handleEvent, this, std::placeholders::_1);    // ME: bind *this* to create member function 
                                                                               // ME: return this->handleEvent(future param)
  }
  /*** virtual functions ***/
  // to be implemented by simulation modules
  // init() is mainly for modules to emit initial events
  virtual void init() = 0;
  virtual void handleEvent(const Event& e) = 0;

  // getters
  const std::string& name() const { return _name; }

private:
  std::string _name; 
  // dependency injection: using reference for _sim to save Simulator
  // This is because simulator must have a longer lifetime than all SimObjects.
  // Note: this makes SimObject not copyable and not assignable.
  // Simulator& _sim;
  // std::reference_wrapper<Simulator> _sim;
  // std::weak_ptr<Simulator> _sim;
};

#endif // GENSIM_INCLUDE_SIMULATOR_SIMOBJECT_HPP__
