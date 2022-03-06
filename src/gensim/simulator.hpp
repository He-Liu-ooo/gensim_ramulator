#ifndef GENSIM_INCLUDE_SIMULATOR_SIMULATOR_HPP__
#define GENSIM_INCLUDE_SIMULATOR_SIMULATOR_HPP__

#include <vector>
#include <functional>
#include <memory>

#include "simulator/simobject.hpp"

class Simulator
{

public:
    // Constructor
    // for lvalue, &name is copied
    // for rvalue, name is moved
    explicit Simulator(std::string name) : _name{std::move(name)} {}   // ME: this function cannot be called implicitly

    /*** member functions ***/
    // raw pointers and smart pointers are both supported
    // users do not need to manage dynamic memory: "delete ptr"
    void regObj(SimObject* obj) { _objs.emplace_back(obj); }
    void regObj(std::shared_ptr<SimObject> obj) { _objs.push_back(std::move(obj)); }

    // ME: init all elements in objs
    void initObj() {
      std::for_each(_objs.begin(), _objs.end(), [](auto& _obj){ _obj->init();}); 
    }
  
    void simulate() {
      while (!_eventq.empty()) {
        // evetn = _eventq.top(); create a copy of the eventq.top()
        // if use auto& event = _eventq.top(); then eventq.pop() should be called
        // after event.process()
        auto& ev = _eventq.top();   // ME: ev has the highest priority among all events of the eventq

        // ME: curTick() is _eventq.curTick()
        if (ev.tick() >= curTick()) {    // #TODO ME: what does a event's tick represent?
          _eventq.setTick(ev.tick());
          ev.process();
          _eventq.pop();
        } 
        else {
          throw std::runtime_error("Simulator::simulate(): "
                                  "event tick is smaller than current tick");
        }
      }
    }
    // tick management
    Tick curTick() const {return _eventq.curTick();}
    // object management
    bool containObj(const SimObject& obj) {
      return std::find_if(_objs.begin(), _objs.end(), 
          [&](auto& _obj){return _obj->name() == obj.name();}) != _objs.end(); 
    }
    bool containObj(const std::string& name) {
      return std::find_if(_objs.begin(), _objs.end(), 
          [&](auto& _obj){return _obj->name() == name;}) != _objs.end(); 
    }

    EventQueue& eventq() { return _eventq; }

private:
    std::string _name;
    EventQueue _eventq {};
    // TODO: using unordered_set
    // std::vector<std::reference_wrapper<SimObject>> _objs;
    // Using std::unique_ptr ensures that the objects are deleted
    // when the simulator is destroyed.
    // This means that "Simulator owns the SimObjects"
    // std::vector<std::unique_ptr<SimObject>> _objs;
    // if we are using vector<SimObject>, then vector tries to copy the
    // SimObject, then further modification of the original object will
    // not take effect in the simulator.
    std::vector<std::shared_ptr<SimObject>> _objs;
};

#endif // GENSIM_INCLUDE_SIMULATOR_SIMULATOR_HPP__
