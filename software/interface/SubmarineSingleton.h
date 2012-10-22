#include <stdbool.h>

class SubmarineSingleton {
  public:
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
    SubmarineSingleton();
    SubmarineSingleton(SubmarineSingleton const&); // Don't implement
    void operator=(SubmarineSingleton const&);     // Don't implement

    void destroy();

    int instances = 0;
    bool created = false;
};
