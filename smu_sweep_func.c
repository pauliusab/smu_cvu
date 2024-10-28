/* USRLIB MODULE INFORMATION

	MODULE NAME: smu_sweep_func
	MODULE RETURN TYPE: double 
	NUMBER OF PARMS: 14
	ARGUMENTS:
		force_id,	int,	Input,	,	,	
		measure_id,	int,	Input,	,	,	
		cviv_id,	int,	Input,	,	,	
		StartV,	double,	Input,	0,	,	
		StopV,	double,	Input,	1,	,	
		StepV,	double,	Input,	0.1,	,	
		dualSweep,	int,	Input,	1,	,	
		I_cc,	double,	Input,	0.001,	,	
		V_range,	double,	Input,	20,	,	
		I_range,	double,	Input,	0.01,	,	
		MeasureDelay,	double,	Input,	0.020,	,	
		V_programmed,	double *,	Output,	,	,	
		I_meas,	double *,	Output,	,	,	
		T_meas,	double *,	Output,	,	,	
	INCLUDES:
#include "keithley.h"
	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "keithley.h"

/* USRLIB MODULE MAIN FUNCTION */
double smu_sweep_func( int force_id, int measure_id, int cviv_id, double StartV, double StopV, double StepV, int dualSweep, double I_cc, double V_range, double I_range, double MeasureDelay, double *V_programmed, double *I_meas, double *T_meas )
{
/* USRLIB MODULE CODE */
int status;

int n_steps;
n_steps = (StopV - StartV) / StepV;

int n_data = n_steps + 1;
if (dualSweep == 1)
	n_data = n_steps * 2 + 1;


devint();
Sleep(100);
// connect switch pins
status = cviv_config(cviv_id, 1, KI_CVIV_SMU);
status = cviv_config(cviv_id, 2, KI_CVIV_SMU);
status = cviv_config(cviv_id, 3, KI_CVIV_OPEN);
status = cviv_config(cviv_id, 4, KI_CVIV_OPEN);
if (status)
	return status;

Sleep(100);

//make SMU return measured values when CC or range limit is reached
status = setmode(force_id, KI_LIM_MODE, KI_VALUE);
status = setmode(measure_id, KI_LIM_MODE, KI_VALUE);
if (status)
    return status;




// apply CC limits
status = limiti(force_id, I_cc);
if (status)
	return status;


// select ranges to prevent autoranging
status = rangev(force_id, V_range);
status = rangev(measure_id, V_range);
status = rangei(force_id, I_range);
status = rangei(measure_id, I_range);
if (status)
	return status;


//make sure both channels are grounded at the start
forcev(force_id, StartV);
forcev(measure_id, 0.0);


//when sweep is executed fills input array with programmed voltage values
status = rtfary(V_programmed);
if (status)
	return status;


//prepare measurements
status = smeast(measure_id, T_meas);
status = smeasi(measure_id, I_meas);
if (status)
	return status;


//execute sweep
status = sweepv(force_id, StartV, StopV, n_steps, MeasureDelay);
if (status)
	return status;

if (dualSweep == 1){
	status = sweepv(force_id, StopV-StepV, StartV, n_steps-1, MeasureDelay);
	if (status)
		return status;
}


return 0;
/* USRLIB MODULE END  */
} 		/* End smu_sweep_func.c */

