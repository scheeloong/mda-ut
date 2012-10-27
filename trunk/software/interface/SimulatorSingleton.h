#ifndef SIMULATOR_SINGLETON_H
#define SIMULATOR_SINGLETON_H

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "physical_model.h"

class SimulatorSingleton {
  public:
    SimulatorSingleton() : instances(0), created(false) {}
    static SimulatorSingleton& get_instance()
    {
      static SimulatorSingleton instance;
      return instance;
    }

    void increment_instances() {instances++;}
    void decrement_instances()
    {
      instances--;
      if (instances == 0) {
        destroy();
      }
    }
    void create();

    physical_model attitude();
    void add_position(world_vector);
    void add_orientation(orientation);
    FILE *write_fp() {return write;}
    FILE *read_fp() {return read;}
  private:
    SimulatorSingleton(SimulatorSingleton const&); // Don't implement
    void operator=(SimulatorSingleton const&);     // Don't implement

    void destroy();

    int instances;
    bool created;
    pthread_t sim_thread;
    FILE *read, *write;
    int rh, wh;
};

#endif
