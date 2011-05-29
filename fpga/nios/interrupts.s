.section .exceptions, "ax"

  call interrupt_handler
  eret
