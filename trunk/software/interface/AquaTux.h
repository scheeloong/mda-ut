#include "AttitudeInput.h"
#include "ImageInput.h"
#include "ControlAlgorithm.h"

class AquaTux {
  public:
    AquaTux(const char *);
    void work();
    ~AquaTux();
  private:
    AttitudeInput *m_attitude_input;
    ImageInput *m_image_input;
    ControlAlgorithm *m_control_algorithm;
};
