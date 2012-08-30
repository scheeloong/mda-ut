SIM_DIR=./sim/
MOTORS_DIR=./motors/
VISION_DIR=./vision/

all: motors vision sim

motors:
	$(MAKE) -C $(MOTORS_DIR) clean
	$(MAKE) -C $(MOTORS_DIR)

vision:
	$(MAKE) -C $(VISION_DIR)

sim:
	$(MAKE) -C $(SIM_DIR)

clean:
	$(MAKE) -C $(MOTORS_DIR) clean
	$(MAKE) -C $(VISION_DIR) clean
	$(MAKE) -C $(SIM_DIR) clean

.PHONY: all clean motors vision sim
