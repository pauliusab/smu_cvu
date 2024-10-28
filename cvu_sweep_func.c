/* USRLIB MODULE INFORMATION

	MODULE NAME: cvu_sweep_func
	MODULE RETURN TYPE: int 
	NUMBER OF PARMS: 13
	ARGUMENTS:
		CVU,	char *,	Input,	"CVU1",	,	
		FreqStart,	double,	Input,	10e3,	1e3,	10e6
		FreqStop,	double,	Input,	100e3,	1e3,	10e6
		FreqStep,	double,	Input,	1e3,	1e3,	
		Delay,	double,	Input,	1e3,	,	
		DCV,	double,	Input,	0,	-30,	30
		ACV,	double,	Input,	0.03,	.01,	1
		Irange,	double,	Input,	0,	0,	1e-3
		V,	double *,	Output,	,	,	
		Cp,	double *,	Output,	,	,	
		Gp,	double *,	Output,	,	,	
		F,	double *,	Output,	,	,	
		Time,	double *,	Output,	,	,	
	INCLUDES:
#include "keithley.h"
	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "keithley.h"

/* USRLIB MODULE MAIN FUNCTION */
int cvu_sweep_func( char *CVU, double FreqStart, double FreqStop, double FreqStep, double Delay, double DCV, double ACV, double Irange, double *V, double *Cp, double *Gp, double *F, double *Time )
{
/* USRLIB MODULE CODE */
double stepVal = round(FreqStep/1e3) * 1e3;
long NumPts;
int status = 0;

//get a handles
int CVU_id;
getinstid(CVU, &CVU_id);
if (CVU_id == -1)
    return -1;

char cviv_name[] = "CVIV1";
int cviv_id;
getinstid(cviv_name, &cviv_id);
if (cviv_id == -1)
    return -1;

status = setmode(CVU_id, KI_CVU_AC_SRC_HI, 1);
status = setmode(CVU_id, KI_CVU_DC_SRC_HI, 1);

devint();
Sleep(100);
status = cviv_config(cviv_id, 1, KI_CVIV_CVL);
status = cviv_config(cviv_id, 2, KI_CVIV_CVH);
status = cviv_config(cviv_id, 3, KI_CVIV_OPEN);
status = cviv_config(cviv_id, 4, KI_CVIV_OPEN);
Sleep(100);

setlevel(CVU_id, ACV); /* Set AC drive voltage. */

//setauto(CVU1); /* Select auto measure range. */
// to set the current range
status = rangei(CVU_id, Irange);


status = setmode(CVU_id, KI_CVU_FREQ_STEPSIZE, stepVal);
if (status)
	return status;


status = forcev(CVU_id, DCV);
//Sleep(100);


smeasf(CVU_id, F);
smeasz(CVU_id, KI_CVU_TYPE_ZTH, KI_CVU_SPEED_NORMAL, Cp, Gp); /* Configure CpGp measurements. */
smeast(CVU_id, Time);
rtfary(V); /* Return array of force frequencies. */



status = sweepf(CVU_id, FreqStart, FreqStop, &NumPts, Delay); /* Configure and perform sweep. */
if (status)
    return status;


return status;
/* USRLIB MODULE END  */
} 		/* End cvu_sweep_func.c */

