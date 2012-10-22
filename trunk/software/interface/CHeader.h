#ifndef CHEADER_H
#define CHEADER_H

#ifdef __cplusplus
extern "C" {
  // C functions from scripts/scripts.h and fpga/ui/utils.h
  void init();
  void exit_safe();

  void power_on();
  void power_off();
  int  power_status();

  int  get_depth();
}
#endif

#endif
