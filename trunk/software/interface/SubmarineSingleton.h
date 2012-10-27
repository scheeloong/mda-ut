#ifndef SUBMARINE_SINGLETON_H
#define SUBMARINE_SINGLETON_H

#include <stdbool.h>

class SubmarineSingleton {
  public:
    SubmarineSingleton() : instances(0), created(false) {}
    static SubmarineSingleton& get_instance()
    {
      static SubmarineSingleton instance;
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
  private:
    SubmarineSingleton(SubmarineSingleton const&); // Don't implement
    void operator=(SubmarineSingleton const&);     // Don't implement

    void destroy();

    int instances;
    bool created;
};

#endif
