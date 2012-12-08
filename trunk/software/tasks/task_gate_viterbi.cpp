#include "mda_tasks.h"
#include "mda_vision.h"

// TODO: add left and right states within SEEN_GATE
enum HIDDEN_STATES {
  START = 0,
  SEEN_GATE,
  DONE_GATE,
  SUCCEED,
  FAIL_TO_START,
  FAIL_TO_COMPLETE,
  NUM_STATES
};

MDA_TASK_GATE_VITERBI:: MDA_TASK_GATE_VITERBI (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
  v = new Viterbi(NUM_STATES);

  // Assign state transition probabilities (these can be tuned!)
  //                                START SEEN_GATE DONE_GATE SUCCEED FAIL_TO_START FAIL_TO_COMPLETE
  double start_trans[] =            {0.45,     0.45,     0.00,   0.05,         0.05,            0.00};
  double seen_gate_trans[] =        {0.05,     0.55,     0.30,   0.05,         0.00,            0.05};
  double done_gate_trans[] =        {0.00,     0.15,     0.60,   0.20,         0.00,            0.05};
  double succeed_trans[] =          {0.00,     0.00,     0.00,   1.00,         0.00,            0.00};
  double fail_to_start_trans[] =    {0.00,     0.00,     0.00,   0.00,         1.00,            0.00};
  double fail_to_complete_trans[] = {0.00,     0.00,     0.00,   0.00,         0.00,            1.00};

  v->set_transition_prob(START, start_trans);
  v->set_transition_prob(SEEN_GATE, seen_gate_trans);
  v->set_transition_prob(DONE_GATE, done_gate_trans);
  v->set_transition_prob(SUCCEED, succeed_trans);
  v->set_transition_prob(FAIL_TO_START, fail_to_start_trans);
  v->set_transition_prob(FAIL_TO_COMPLETE, fail_to_complete_trans);
}

MDA_TASK_GATE_VITERBI:: ~MDA_TASK_GATE_VITERBI ()
{
  delete v;
}

MDA_TASK_RETURN_CODE MDA_TASK_GATE_VITERBI:: run_task() {
    puts("Press q to quit");

    // Use gate and path vision
    MDA_VISION_MODULE_GATE gate_vision;
    MDA_VISION_MODULE_PATH path_vision;

    bool done_task = false;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    while (!done_task) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        const IplImage* down_frame = image_input->get_image(DWN_IMG);
        if (!frame || !down_frame) {
            ret_code = TASK_ERROR;
            break;
        }

        MDA_VISION_RETURN_CODE gate_vcode = gate_vision.filter(frame);
        MDA_VISION_RETURN_CODE path_vcode = path_vision.filter(down_frame);

        // TODO: fill in more emission probabilities!
        // Assign emission probabilities (these can also be tuned!)
        //                                START SEEN_GATE DONE_GATE SUCCEED FAIL_TO_START FAIL_TO_COMPLETE
        double default_emission[] =       {0.20,     0.20,     0.20,   0.20,         0.20,            0.20};
        double path_found_emission[] =    {0.35,     0.20,     0.00,   0.80,         0.00,            0.00};

        if (path_vcode == ONE_SEGMENT || path_vcode == FULL_DETECT || path_vcode == UNKNOWN_TARGET) {
            v->update_emission_prob(path_found_emission);
        } else {
            v->update_emission_prob(default_emission);
        }

        // update emission probabilities and compute the current state
        int state = v->optimal_state();

        switch(state) {
            case START:
                actuator_output->set_attitude_change(FORWARD);
                break;
            case SEEN_GATE:
                // complicated control code, done after switch statement
                break;
            case DONE_GATE:
                actuator_output->set_attitude_change(FORWARD);
                break;
            case SUCCEED:
                done_task = true;
                ret_code = TASK_DONE;
                break;
            case FAIL_TO_START:
                done_task = true;
                ret_code = TASK_MISSING;
                break;
            case FAIL_TO_COMPLETE:
                done_task = true;
                ret_code = TASK_REDO;
                break;
        }

        if (state == SEEN_GATE) {
            if (gate_vcode == NO_TARGET || gate_vcode == UNKNOWN_TARGET) {
                actuator_output->set_attitude_change(FORWARD);
            }
            else if (gate_vcode == ONE_SEGMENT) {
                int ang_x = gate_vision.get_angular_x();

                actuator_output->set_attitude_change(RIGHT, ang_x);

                if (fabs(ang_x) < 5.0) {
                    actuator_output->set_attitude_change(FORWARD);
                }
            } 
            else if (gate_vcode == FULL_DETECT) {
                // if we can see full gate and range is less than 400 we are done the gate part
                if (gate_vision.get_range() < 400) {
                    // set target yaw to current yaw and go forward
                    actuator_output->set_attitude_absolute(YAW, attitude_input->yaw());
                    actuator_output->set_attitude_change(FORWARD);
                    continue;
                }

                int ang_x = gate_vision.get_angular_x();

                actuator_output->set_attitude_change(RIGHT, ang_x);

                if (fabs(ang_x) < 5.0) {
                    actuator_output->set_attitude_change(FORWARD);
                }
            }
        }

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(3);
        if (c != -1) {
            CharacterStreamSingleton::get_instance().write_char(c);
        }
        if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q'){
            ret_code = TASK_QUIT;
            break;
        }
    }

    actuator_output->stop();

    return ret_code;
}
