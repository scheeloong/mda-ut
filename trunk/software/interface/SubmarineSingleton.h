/* SubmarineSingleton

  This class implements the communication interface to the submarine, via its FPGA. This communication is done through the nios2-terminal,
  and the nuances of communicating through an interactive shell is encapsulated by this singleton. The FPGA is responsible for gathering
  attitude data, for relaying motor control commands to the submarine motors, and power management. This singleton allows a high-level
  program to interface with the FPGA.
*/

#ifndef SUBMARINE_SINGLETON_H
#define SUBMARINE_SINGLETON_H

#include <stdbool.h>

class SubmarineSingleton {
  public:
    SubmarineSingleton() : registered(false), created(false) {}
    ~SubmarineSingleton() { destroy(); }

    static SubmarineSingleton& get_instance()
    {
      static SubmarineSingleton instance;
      return instance;
    }
    void register_instance()
    {
      if (registered) {
        return;
      }
      registered = true;
      create();
    }
  private:
    SubmarineSingleton(SubmarineSingleton const&); // Don't implement
    void operator=(SubmarineSingleton const&);     // Don't implement

    void create();
    void destroy();

    bool registered, created;
};

#endif
