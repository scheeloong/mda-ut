#ifndef _PID_H
#define _PID_H


#include "physical_model.h"
#include <ctime>

#define PID_NUM_OLD_VALUES 3
#define PID_DEG_FREEDOM 2		//Number of properties being controlled: 0=>y, 1=>yaw

class PID {
	protected:
		float P[PID_DEG_FREEDOM], I[PID_DEG_FREEDOM], D[PID_DEG_FREEDOM];
		float Kp, Ki, Kd;						//PID constants

		clock_t t;								//Record real world time of execution for numerical integration/differentiation
		double times[PID_NUM_OLD_VALUES];

		float oldValues[PID_DEG_FREEDOM][PID_NUM_OLD_VALUES];	//store old values
		float alpha;											//decay constant for I

	public:
		PID();
		PID(float _Kp, float _Ki, float _Kd, float _alpha);
		~PID();

		void setK(float _Kp, float _Ki, float _Kd);
		void setAlpha(float _alpha);

		void PID_Reset();

		void   PID_Update(float *values);	//performs direct update on the model values, based on the target
		float *PID_Output();
};
#endif