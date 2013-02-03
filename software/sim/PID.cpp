#include "PID.h"
#include <stdio.h>

PID::PID(){
	Kp =  Ki =  Kd =  alpha = 0;
	
	gettimeofday(&t,NULL);
	unsigned long long t_us = t.tv_sec*1000000 + t.tv_usec;
	
	for(int i=0; i<PID_NUM_OLD_VALUES; i++){
		times[i] = (t_us-i);	//initialize to now, with an offset such that 1/(t[i]-t[i+1]) != inf
		for(int j=0; j<PID_DEG_FREEDOM; j++){
			oldValues[j][i] = 0;
		}
	}
	for(int k=0; k<PID_DEG_FREEDOM; k++) P[k] = D[k] = I[k] = 0;
}

PID::PID(float _Kp, float _Ki, float _Kd, float _alpha){
	Kp = _Kp; Ki = _Ki; Kd = _Kd; alpha = _alpha;

	gettimeofday(&t,NULL);
	unsigned long long t_us = t.tv_sec*1000000 + t.tv_usec;
	
	for(int i=0; i<PID_NUM_OLD_VALUES; i++){
		times[i] = (t_us-i);	//initialize to now, with an offset such that 1/(t[i]-t[i+1]) != inf
		for(int j=0; j<PID_DEG_FREEDOM; j++){
			oldValues[j][i] = 0;
		}
	}
	for(int k=0; k<PID_DEG_FREEDOM; k++) P[k] = D[k] = I[k] = 0;
}


PID::~PID(){}

void PID::setK(float _Kp, float _Ki, float _Kd){
	Kp = _Kp; Ki = _Ki; Kd = _Kd;
}

void PID::setAlpha(float _alpha){
	alpha = _alpha;
}

void PID::PID_Reset(){
/*Resets controller while preserving PID constants*/

	gettimeofday(&t,NULL);
	unsigned long long t_us = t.tv_sec*1000000 + t.tv_usec;

	for(int i=0; i<PID_NUM_OLD_VALUES; i++){
		times[i] = (t_us-i);	//initialize to now, with an offset such that 1/(t[i]-t[i+1]) != inf
		for(int j=0; j<PID_DEG_FREEDOM; j++){
			oldValues[j][i] = 0;
		}
	}
	for(int k=0; k<PID_DEG_FREEDOM; k++) P[k] = D[k] = I[k] = 0;
}

void PID::PID_Update(float *values){	
/*Updates PID signals internally for each controlled value
  Expects that values is of size PID_DEG_FREEDOM*/

	for(int j=PID_NUM_OLD_VALUES-1; j>0; j--){
		for (int i=0; i<PID_DEG_FREEDOM; i++) oldValues[i][j] = oldValues[i][j-1];
		times[j] = times[j-1];
	}
	gettimeofday(&t,NULL);
	unsigned long long t_us = t.tv_sec*1000000 + t.tv_usec;

	times[0] = t_us;

	for(int i=0; i<PID_DEG_FREEDOM; i++){
		oldValues[i][0] = values[i];

		I[i] = I[i] * (1-alpha) + ((times[0] - times[1])*(oldValues[i][0] + oldValues[i][1])/2) / 1000000;  //Trapezoidal integration
		if (times[0] - times[1] == 0) {
			D[i] = 0;
		} else {
			D[i] = (oldValues[i][0] - oldValues[i][1])/(times[0]-times[1]) * 1000000;  //Backwards difference differentiation
		}
		P[i] = values[i];
	}
}

float* PID::PID_Output(){
/*Output control signals for each controlled vlaue
  Output is of length PID_DEG_FREEDOM*/

	float* output = new float[PID_DEG_FREEDOM];
	for (int i=0; i<PID_DEG_FREEDOM; i++){
		output[i] = Kp*P[i] + Kd*D[i] + Ki*I[i];
	}
	return (output);
}

void PID::debug(){
	printf("P: %f I: %f D: %f\n", P[0], I[0], D[0]);
	printf(" times: ");
	for(int j=0; j<PID_NUM_OLD_VALUES; j++){
		printf("%lld ",times[j]);
	}
	printf("\n");
}
