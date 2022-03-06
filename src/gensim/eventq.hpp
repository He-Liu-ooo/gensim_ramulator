#ifndef GENSIM_INCLUDE_SIMULATOR_EVENTQ_HPP__
#define GENSIM_INCLUDE_SIMULATOR_EVENTQ_HPP__

#include <functional>
#include <queue> // std::priority_queue
// #include <memory> // std::shared_ptr
#include <iostream>
#include <string>
#include <vector>

#include "simulator/types.hpp"

class EventBase {
protected:
  static const EventFlag ScheduleFlag = 0x1;

public:
  static const Priority MinPriority = -128;
  static const Priority DefaultPriority = 0;
  static const Priority MaxPriority = 127;
};

/* This class is used to represent an event in the simulator.
 * 
 */
class Event : public EventBase {
public:
  Event(std::string name, 
        Tick tick,
        std::function<void(const Event&)> process = nullptr,
        EventFlag flag = 1,
        std::string desc = "",
        Priority priority = DefaultPriority)
      : _name{std::move(name)}, _tick{tick}, _process{process}, _flag{flag},
        _desc{std::move(desc)}, _priority{priority} {}
  // deconstructor
  virtual ~Event() = default;


  /*** member functions ***/

  // methods
  void process() const { if (_process) _process(*this); }  // #TODO dont understand what this function means
  // getters
  const std::string& name() const { return _name; }
  const Tick tick() const { return _tick; }
  const std::string& desc() const { return _desc; }
  const Priority priority() const { return _priority; }
  const EventFlag flag() const { return _flag; }
  // setters
  void set_tick(Tick tick) const { _tick = tick; }
  void set_flag(EventFlag flag) const { _flag = flag; }
  // properties
  bool scheduled() const { return !(_flag | ScheduleFlag); }

  /*** non-member functions ***/

  // comparators: tick and priority
  friend bool operator<(const Event& lhs, const Event& rhs) {
    return (lhs.tick() < rhs.tick()) ||
           (lhs.tick() == rhs.tick() && lhs.priority() > rhs.priority());
  }
  friend bool operator>(const Event& lhs, const Event& rhs) {
    return rhs < lhs;
  }

  // operator <<
  friend std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << "Event: " << event.name() << " @tick: " << event.tick() << "\t  " << event.priority() << " " << event.desc();
    return os;
  }

private:
  //HINT: other attributes: id, info, type, src, dst?
  // Events are stored in a priority queue
  // the reference becomes const when accessed from priority_queue
  // so we need to make it mutable
  std::string _name;
  mutable Tick _tick;   // ME: even in a const func, mutable variable can be changed
                        // ME: every even in event queue has its own _tick, though a event queue has a _tick as well
                        // #TODO ME: what does a tick stands for? 
  std::function<void(const Event&)> _process;
  mutable EventFlag _flag;
  std::string _desc;
  Priority _priority;
};

/*
 * EventQueue is a priority queue of events.
 * 
 * Events are sorted by tick, and then by priority.
 */
class EventQueue {
public:
  /*** member functions ***/

  // tick management
  Tick curTick() const { return _tick; }
  void setTick(Tick tick) { _tick = tick; }
  // element access
  const Event& top() const { 
    return _eventq.top(); }
  // modifiers
  void push(const Event& event) { _eventq.push(event); }
  // users can emplace events from its constructor
  // avoid create a temporary Event object and copy/move
  template <class... Args>
  void emplace(Args&&... args) { _eventq.emplace(std::forward<Args>(args)...); }

  void pop() { _eventq.pop(); }
  // capacity
  bool empty() const { return _eventq.empty(); }
  size_t size() const { return _eventq.size(); }


private:
  Tick _tick {0};
  // greater: ascending tick, then decending priority
  // using EventPtr = std::shared_ptr<Event>;
  // TODO: whether to use std::shared_ptr<Event> ?
  // std::priority_queue<EventPtr, std::vector<EventPtr>, std::greater<EventPtr>> _eventq;
  std::priority_queue<Event, std::vector<Event>, std::greater<Event>> _eventq;
};

#endif // GENSIM_INCLUDE_SIMULATOR_EVENTQ_HPP__
