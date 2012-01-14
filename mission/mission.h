#define DEFAULT_SPEED 1

enum MOVE_DIR {
  STOP,
  FORWARD,
  REVERSE,
  LEFT,
  RIGHT,
  SINK,
  RISE
};

class Mission {
  public:
    void move(MOVE_DIR dir, int speed);
    void move(MOVE_DIR dir) { move(dir, DEFAULT_SPEED); }
  private:
    int v_forward, v_depth, v_yaw;
};
