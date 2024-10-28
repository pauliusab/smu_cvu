/* USRLIB MODULE INFORMATION

	MODULE NAME: smu_cvu_combined
	MODULE RETURN TYPE: double 
	NUMBER OF PARMS: 19
	ARGUMENTS:
		force_name,	char *,	Input,	"SMU1",	,	
		measure_name,	char *,	Input,	"SMU2",	,	
		StartV,	double,	Input,	0,	,	
		StopV,	double,	Input,	,	,	
		StepV,	double,	Input,	,	0.001,	
		dualSweep,	int,	Input,	1,	,	
		I_cc,	double,	Input,	,	0.001,	
		V_range,	double,	Input,	20,	,	
		I_range,	double,	Input,	0.01,	,	
		MeasureDelay,	double,	Input,	0.020,	,	
		CVU,	char *,	Input,	"CVU1",	,	
		FreqStart,	double,	Input,	10e3,	1e3,	10e6
		FreqStop,	double,	Input,	100e3,	1e3,	10e6
		FreqStep,	double,	Input,	1e3,	1e3,	
		Delay,	double,	Input,	1e3,	,	
		DCV,	double,	Input,	0,	-30,	30
		ACV,	double,	Input,	0.03,	.01,	1
		Irange,	double,	Input,	0,	0,	1e-3
		Filename,	char *,	Input,	"testing",	,	
	INCLUDES:
#include "keithley.h"
	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "keithley.h"

/* USRLIB MODULE MAIN FUNCTION */
double smu_cvu_combined( char *force_name, char *measure_name, double StartV, double StopV, double StepV, int dualSweep, double I_cc, double V_range, double I_range, double MeasureDelay, char *CVU, double FreqStart, double FreqStop, double FreqStep, double Delay, double DCV, double ACV, double Irange, char *Filename )
{
/* USRLIB MODULE CODE */
#include <time.h>

int status;
int force_id, measure_id, cviv_id;

int n_steps_smu;
n_steps_smu = (StopV - StartV) / StepV;


double stepVal = round(FreqStep/1e3) * 1e3;
int n_data_cvu = (FreqStop - FreqStart) / stepVal + 1;
double Cp[n_data_cvu], Gp[n_data_cvu], F[n_data_cvu], V_cvu[n_data_cvu], Time[n_data_cvu];

int n_data = n_steps_smu;
if (dualSweep == 1)
	n_data = n_steps_smu * 2;

double I_meas[n_data], T_meas[n_data], V_programmed[n_data];


//get a handle for SMU channels
getinstid(force_name, &force_id);
if (force_id == -1)
    return -1;

getinstid(measure_name, &measure_id);
if (measure_id == -1)
    return -2;

//get a handle for CVIV
char cviv_name[] = "CVIV1";
getinstid(cviv_name, &cviv_id);
if (cviv_id == -1)
    return -3;


smu_sweep_func( force_id, measure_id, cviv_id, StartV, StopV, StepV, dualSweep, I_cc, V_range, I_range, MeasureDelay, V_programmed, I_meas, T_meas );
cvu_sweep_func( CVU, FreqStart, FreqStop, FreqStep, Delay, DCV, ACV, Irange, V_cvu, Cp, Gp, F, Time);



// //post data to Clarius
// PostDataDoubleBuffer("V", V_programmed, n_data);
// PostDataDoubleBuffer("I", I_meas, n_data);
// PostDataDoubleBuffer("T", T_meas, n_data);





//Save all data to file, no decimation
FILE *f;
char fn[128];
char errMsg[100];
char tmpFilename[256];
char date[80] = "\0";
char path[256] = "\0";
char tmppath[256] = "\0";
char name[256] = "\0";
char ext[256] = "\0";

strcpy(tmpFilename, Filename);

// check for path...
if ( !strlen(path) )
{
    strcpy(path, "C:\\S4200\\kiuser\\Projects\\Paulius\\csv_test");
}

else
{
    if ( !strstr(path, "\\\\") )
    {
        int i=0;
        char *p = path;
        // need to make "\" --> "\\"
        while ( *p != '\0' )
        {
            tmppath[i++] = *p++;
            if ( *p == '\\' )
                tmppath[i++] = '\\';
        }
        
        strcpy(path, tmppath);
    }
}

// check for name...
if ( !strlen(name) )
    strcpy(name, "savefile_test");
    
// check for extension...
if ( !strlen(ext) )
    strcpy(ext, "csv");



//make date
time_t rawtime;
struct tm *timeinfo;

time(&rawtime);
timeinfo = localtime(&rawtime);

strftime(date, 80, "_%Y%m%d_%H%M%S", timeinfo);


//concatenate the name of the file
sprintf(fn, "%s\\%s%s.%s", path, name, date, ext);
f = fopen(fn, "w");
if ( f != NULL )
{
    fprintf(f, "V_programmed, I_meas, T_meas\n");
    for (int j= 0; j<n_data; j++ )
    {
        fprintf(f, "%g, %g, %g\n", V_programmed[j], I_meas[j], T_meas[j]);
    }

    fclose(f);
}



return 0;
/* USRLIB MODULE END  */
} 		/* End smu_cvu_combined.c */

