#ifndef SIMULATOR_SINGLETON_H
#define SIMULATOR_SINGLETON_H

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "physical_model.h"

#include "ImageInput.h"

class SimulatorSingleton {
  public:
    SimulatorSingleton() : instances(0), created(false), read(NULL), write(NULL),
        img_fwd(NULL), img_dwn(NULL), img_copy_start(false), img_copy_done(false) {}
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
    void sim_init();
    void sim_keyboard(unsigned char);
    void sim_display();
    void sim_reshape(int, int);
    const IplImage* get_image(ImageDirection);

    physical_model attitude();
    void add_position(world_vector);
    void add_orientation(orientation);
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
    IplImage *img_fwd, *img_dwn;
    volatile bool img_copy_start, img_copy_done;
    ImageDirection img_dir;
};

#endif
