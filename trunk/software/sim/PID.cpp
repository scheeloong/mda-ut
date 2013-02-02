#include "PID.h"

PID::PID(){
	Kp =  Ki =  Kd =  alpha = 0;
	t = clock();
	
	for(int i=0; i<PID_NUM_OLD_VALUES; i++){
		times[i] = ((double)t-i)/CLOCKS_PER_SEC;	//initialize to now, with an offset such that 1/(t[i]-t[i+1]) != inf
		for(int j=0; j<PID_DEG_FREEDOM; j++){
			oldValues[j][i] = 0;
		}
	}
	for(int k=0; k<PID_DEG_FREEDOM; k++) P[k] = D[k] = I[k] = 0;
}

PID::PID(float _Kp, float _Ki, float _Kd, float _alpha){
	Kp = _Kp; Ki = _Ki; Kd = _Kd; alpha = _alpha;
	t = clock();
	
	for(int i=0; i<PID_NUM_OLD_VALUES; i++){
		times[i] = ((double)t-i)/CLOCKS_PER_SEC;	//initialize to now, with an offset such that 1/(t[i]-t[i+1]) != inf
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
	t = clock();
	
	for(int i=0; i<PID_NUM_OLD_VALUES; i++){
		times[i] = ((double)t-i)/CLOCKS_PER_SEC;	//initialize to now, with an offset such that 1/(t[i]-t[i+1]) != inf
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
	t = clock();
	times[0] = ((double)t)/CLOCKS_PER_SEC;

	for(int i=0; i<PID_DEG_FREEDOM; i++){
		oldValues[i][0] = values[i];

		I[i] = I[i] * (1-alpha) + (times[0] - times[1])*(oldValues[i][0] + oldValues[i][1])/2;  //Trapezoidal integration
		D[i] = (oldValues[i][0] - oldValues[i][1])/(times[0]-times[1]);							//Backwards difference differentiation
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