/***************************************************************************
 *   Copyright (C) 2008 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// System includes
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <semaphore.h>

// My libraries
#include "arg_tool.h"
#include "straux.h"
#include "kepco.h"
#include "matv4.h"
#include "smalt.h"
#include "load_dat.h"
#include "progressbar.h"
#include "destroyer.h"

// PV generator related includes
#include "pvgen.h"
#include "pvgen_sc.h"
#include "pvgen_mc.h"
#include "pvgen_mpp_I.h"
#include "pvgen_models.h"
#include "pvgen_setup.h"
#include "mppt_inccond.h"
#include "mppt_mlam.h"
#include "mppt_mlamhf.h"
#include "mppt_temperature.h"
#include "mppt_temperaturehf.h"
#include "pvgen_model_test.h"

// Other local includes
#include "pid_file_handler.h"
#include "denis_sensors.h"

// Debug header, must be last include in file
#define DEBUG
#include "debug.h"

#define stricmp(x,y) strcmp((x),(y))

using namespace std;

// Arglist-related variables
int iOutFile, iSamplePeriod, iDuration;
int iPsuTest, iPsu1, iPsu2;
int iGeneratorTest;
int iSensorTest, iSensorAddr, iSensorPort;
int iHelp, iQuiet, iPID;
//   Simulation modifiers
int iStimuli, skip_boot, iTracker;
int generator_model, iModelTest;

arg_t args[] = {
	{"-h",        &iHelp,          ARG_FLAG},
	{"--help",    &iHelp,          ARG_FLAG},
	{"-Ts",       &iSamplePeriod,  ARG_DEFAULT},
	{"-t",        &iDuration,      ARG_DEFAULT},
	{"-p",        &iPID,           ARG_DEFAULT},
	{"-pt",       &iPsuTest,       ARG_FLAG},
	{"-psu1",     &iPsu1,          ARG_DEFAULT},
	{"-psu2",     &iPsu2,          ARG_DEFAULT},
	{"-st",       &iSensorTest,    ARG_FLAG},
	{"-sa",       &iSensorAddr,    ARG_DEFAULT},
	{"-sp",       &iSensorPort,    ARG_DEFAULT},
	{"-gt",       &iGeneratorTest, ARG_FLAG},
	{"-o",        &iOutFile,       ARG_DEFAULT},
	{"-q",        &iQuiet,         ARG_FLAG},
	
	{"--stimuli",             &iStimuli,        ARG_DEFAULT},
	{"--skip-boot",           &skip_boot,       ARG_FLAG},
	{"--tracker",             &iTracker,        ARG_DEFAULT},
	{"--generator-model",     &generator_model, ARG_DEFAULT},
	{"-mt",                   &iModelTest,      ARG_FLAG},
	{0,0,0}
};

// Global parameters
double dDuration, dSamplePeriod;
kepco_bop psu1, psu2;
denis_sensors sensor;
double startTime;
ofstream outFile;
sem_t main_wait;

// Available MPP Trackers
mppt_inccond       track_ic;
mppt_mlamhf        track_mlamhf;
mppt_temperaturehf track_temperaturehf;
double tracker_truempp      (pvGenerator &gen, double V, double I, double T) {
	return gen.V(pvgen_mpp_I(gen, 0, gen.getSourceCurrent(), 1e-4));
}
double tracker_ic           (pvGenerator &gen, double V, double I, double T) { return track_ic           (V, I          ); }
double tracker_mlamhf       (pvGenerator &gen, double V, double I, double T) { return track_mlamhf       (V, I, T-273.16); }
double tracker_mlamhf_notemp(pvGenerator &gen, double V, double I, double T) { return track_mlamhf       (V, I, 40      ); }
double tracker_temperaturehf(pvGenerator &gen, double V, double I, double T) { return track_temperaturehf(V, I, T       ); }
double tracker_mlamhf_temp  (pvGenerator &gen, double V, double I, double T) {
	return track_mlamhf       (V, I, 40      ) + track_temperaturehf(V, I, T);
}
double (*tracker)(pvGenerator &gen, double V, double I, double T) = &tracker_mlamhf;

// Handler for SIGALRM
sem_t alarm_sem;
void alarm_handler(int);

// Handler for SIGINT
volatile bool interrupt_process=false;
void interrupt_handler(int) {
	interrupt_process=true;
}

// Get time of day as a high-res(1ns) double.
double getTime() {
	struct timespec ts;
	clock_gettime(0,&ts);
	return ts.tv_sec + 1e-9*ts.tv_nsec;
}

// Destroyers
void disable_power_supplies(void *) {
	cout<<"Disabling the power supplies... "<<flush;
	psu1.close();
	psu2.close();
	cout<<" Ok."<<endl;
}
void remove_signal_handlers(void *) {
	cout<<"Removing signal handlers... "<<flush;
	signal(SIGINT,  SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	sem_destroy(&alarm_sem);
	cout<<" Ok."<<endl;
}

// Main
int main(int argc, const char *argv[]) {
	destroyer d;
	
	// Command line processing
	if (true) { // keep i local
		int i=arg_eval(argc,argv,args);
		if (i) {
			cerr<<"Error: Command line parsing failed at \""<<argv[i]<<"\"."<<endl;
			cerr<<"Use -h or --help for a list of available options."<<endl;
			return 1;
		}
	}
	
	if (iHelp) {
		cout<<"<< MPPT comparision software >>"<<endl;
		cout<<"Sorry, no help is available for now."<<endl;
		cout<<"You may have to have to look at the source."<<endl;
		
		cout<<"Available command line switches (undocumented):"<<endl;
		for (int i=0; args[i].id; ++i) {
			cout<<"  "<<setw(11)<<setfill('.')<<setiosflags(ios::left)<<args[i].id<<": ";
			cout<<resetiosflags(ios::left);
			cout<<(args[i].flags&ARG_FLAG ? "Flag" : "Default type")<<endl;
		}
		
		return 0;
	}
	
	// Check which parameter set is required
	bool bRequireSensors=false, bRequireTiming=false, bRequirePsu=false;
	if (iStimuli || iModelTest) {
		// simulation run, no hardware or timing required
		if (iSensorTest)    cerr<<"Simulation run: Ignoring -st."<<endl;
		if (iPsuTest)       cerr<<"Simulation run: Ignoring -pt."<<endl;
		if (iGeneratorTest) cerr<<"Simulation run: Ignoring -gt."<<endl;
		iSensorTest = 0;
		iPsuTest = 0;
		iGeneratorTest = 0;
		
		if (iModelTest && !iOutFile) {
			cerr<<"Model testing requires an output file."<<endl;
			return 1;
		}
	} else if (iSensorTest) {
		bRequireSensors = true;
	} else if (iPsuTest) {
		bRequirePsu = true;
	} else {
		bRequireSensors = true;
		bRequirePsu = true;
		bRequireTiming = true;
	}
	
	// Timimng parameters
	if (bRequireTiming) {
		if (iSamplePeriod) {
			dSamplePeriod = strIsFloat(argv[iSamplePeriod]) ? atof(argv[iSamplePeriod]) : -1;
			if (dSamplePeriod <= 0) {
				cerr<<"Error: -Ts requires a positive floating point parameter."<<endl;
				return 1;
			}
		} else dSamplePeriod = 0.5;
		
		if (iDuration) {
			dDuration = strIsFloat(argv[iDuration]) ? atof(argv[iDuration]) : -1;
			if (dDuration <= 0) iDuration = 0; // Triggers the error below
		} 
		if (!iDuration) {
			cerr<<"Error: -t requires a positive floating point parameter."<<endl;
			return 1;
		}
	}
	
	if (bRequirePsu && (!iPsu1 || !iPsu2)) {
		cerr<<"Error: Options -psu1 and -psu2 are required."<<endl;
		return 1;
	}
	
	// Command-line is valid

	// Disable cout if quiet mode. cerr remains untouched.
	if (iQuiet) cout.rdbuf(0);

	// Connect to the power supplies
	if (bRequirePsu) {
		cout<<"Connecting to the power supplies... "<<flush;
		psu1.open(argv[iPsu1], 9600);
		psu2.open(argv[iPsu2], 9600);
		if (!psu1 || !psu2) {
			cout<<"ERROR."<<endl;
			if (!psu1) cerr<<"Error: Failed to connect to psu1 at "<<argv[iPsu1]<<"."<<endl;
			if (!psu2) cerr<<"Error: Failed to connect to psu2 at "<<argv[iPsu2]<<"."<<endl;
			return 1;
		}
		d.add(disable_power_supplies, 0);
		if (!iGeneratorTest) {
			psu1.voltageMode();
			psu2.voltageMode();
			psu1.setVoltageAndCurrent(0,20);
			psu2.setVoltageAndCurrent(0,20);
		} else {
			psu1.currentMode();
			psu2.voltageMode();
			psu1.setVoltageAndCurrent(30, 0);
			psu2.setVoltageAndCurrent(0, 20);
		}
		cout<<"Ok."<<endl;
	}
	
	// Configure sensors
	if (bRequireSensors) {
		cout<<"Connecting to the iluminance and temperature sensors... "<<flush;
		if (!iSensorAddr) {
			// Sensor connection not specified. Emulate.
			cout<<"Emulate."<<endl;
		} else {
			if (iSensorPort) {
				sensor.open(argv[iSensorAddr], htons(atoi(argv[iSensorPort])));
			} else {
				sensor.open(argv[iSensorAddr]);
			}
			
			if (!sensor) {
				cout<<"Error."<<endl;
				cerr<<"Error: Failed to connect to the iluminance and temperature sensors."<<endl;
				return 1;
			}
			cout<<"Ok."<<endl;
		}
	}
	
	// Test sensors
	if (iSensorTest) {
		double G, T1, T2;
		usleep(100000);
		sensor.read(&G, &T1, &T2);
		cout<<"G  = "<< G  <<endl;
		cout<<"T1 = "<< T1 <<endl;
		cout<<"T2 = "<< T2 <<endl;
		return 0;
	}
	
	// Test Power Supplies
	if (iPsuTest) {
		cout<<"PSU1:"<<endl;
		cout<<"  Model Name: "<<psu1.getName()<<endl;
//		cout<<"  Voltage Range: "<<psu1.getMinVoltage()<<"V to "<<psu1.getMaxVoltage()<<"V"<<endl;
//		cout<<"  Current Range: "<<psu1.getMinCurrent()<<"A to "<<psu1.getMaxCurrent()<<"A"<<endl;
		cout<<"PSU2:"<<endl;
		cout<<"  Model Name: "<<psu2.getName()<<endl;
//		cout<<"  Voltage Range: "<<psu2.getMinVoltage()<<"V to "<<psu2.getMaxVoltage()<<"V"<<endl;
//		cout<<"  Current Range: "<<psu2.getMinCurrent()<<"A to "<<psu2.getMaxCurrent()<<"A"<<endl;
		
		psu1.voltageMode();
		psu2.voltageMode();
		psu1.setCurrent(10);
		psu2.setCurrent(10);
		double V, I;
		for (int i=0; i<=16; ++i) {
			psu1.setVoltage(i);
			psu2.setVoltage(16-i);
			
			psu1.getVoltageAndCurrent(V,I);
			psu2.getVoltageAndCurrent(V,I);
			
			sleep(1);
		}
		
		return 0;
	}
	
	// Allocate memory
// 	sv.i = 0;
// 	sv.n = int(dDuration / dSampleTime) + 1;
// 	smalt<double> mal;
// 	sv.t  = mal(sv.n);
// 	sv.G  = mal(sv.n);
// 	sv.T1 = mal(sv.n);
// 	sv.T2 = mal(sv.n);
// 	sv.V1 = mal(sv.n);
// 	sv.V2 = mal(sv.n);
// 	sv.I1 = mal(sv.n);
// 	sv.I2 = mal(sv.n);
// 	if (!mal) {
// 		cerr<<"Error: Not enough memory."<<endl;
// 		return 1;
// 	}
	
	// Create the pid file
	pid_file_handler pidfile;
	if (iPID) pidfile.create(argv[iPID], iQuiet ? 0 : &cout);
	
	// Select generator model
	const pvgen_parameters_t *genparam=0;
	if (true) {
		// Specified on command-line
		if (generator_model) {
			for (int i=0; generators[i].name; ++i) {
				if (!strcmp(argv[generator_model], generators[i].name)) {
					genparam = &generators[i];
					break;
				}
			}
			if (!genparam) cerr<<"WARINIG: Requested generator model \""<<argv[generator_model]<<"\" not found. Using default."<<endl;
		}
		// Default to KC130TM if unspecified or not found
		if (!genparam) genparam = &generators[GEN_KC130TM];
	}
	
	// Test the pv generator models
	if (iModelTest) return pvgen_model_test(genparam, argv[iOutFile]);
	
	// Prepare MPP trackers
	cout<<"Preparing MPPT trackers ("<<genparam->name<<")... "<<flush;
	if (true) {
		pvgen_model_parameters_t m = pvgen_nominal_model(genparam->nameplate);
		m.Rs += 0.16;
		track_mlamhf.dVr        = 0.01;
		
		track_temperaturehf.Vmpref = genparam->nameplate.Vmp;
		track_temperaturehf.Tref   = genparam->nameplate.Tr;
		track_temperaturehf.kVT    = genparam->nameplate.kT_Voc;
		track_temperaturehf.dVr = 0.01;

		tracker = tracker_mlamhf;
		
		const char *name = "MLAM";
		if (iTracker) name = argv[iTracker];
		
		if        (stricmp(name, "truempp") == 0) {
			tracker = &tracker_truempp;
			
		} else if (stricmp(name, "mlam") == 0) {
			track_mlamhf.dVr = 0;
			
		} else if (stricmp(name, "mlam+ic") == 0) {
			// No need to do anything
			
		} else if (stricmp(name, "mlam+real") == 0) {
			m = genparam->model;
			track_mlamhf.dVr = 0;
			
		} else if (stricmp(name, "mlam+ic+real") == 0) {
			m = genparam->model;
			
		} else if (stricmp(name, "mlam-temp") == 0) {
			tracker = &tracker_mlamhf_notemp;
			track_mlamhf.dVr = 0;
			
		} else if (stricmp(name, "mlam+ic-temp") == 0) {
			tracker = &tracker_mlamhf_notemp;
			
		} else if (stricmp(name, "mlam+real-temp") == 0) {
			tracker = &tracker_mlamhf_notemp;
			m = genparam->model;
			track_mlamhf.dVr = 0;
			
		} else if (stricmp(name, "mlam+ic+real-temp") == 0) {
			tracker = &tracker_mlamhf_notemp;
			m = genparam->model;
			
		} else if (stricmp(name, "temp") == 0) {
			tracker = &tracker_temperaturehf;
			track_temperaturehf.dVr = 0;
			
		} else if (stricmp(name, "temp+ic") == 0) {
			tracker = &tracker_temperaturehf;
			
		} else if (stricmp(name, "mlam+ic+temp") == 0) {
			tracker = &tracker_mlamhf_temp;
			track_temperaturehf.dVr    = 0;
			track_temperaturehf.Vmpref = 0;
			
		} else if (stricmp(name, "mlam+ic+real+temp") == 0) {
			tracker = &tracker_mlamhf_temp;
			m = genparam->model;
			track_temperaturehf.dVr    = 0;
			track_temperaturehf.Vmpref = 0;
			
		} else {
			cout << "Error." << endl;
			cerr << "Error: Unknown tracker \"" << name << "\"." << endl;
			return 1;
		}
			
		track_mlamhf.Iphr = m.Iph * 1000/m.G;
		track_mlamhf.mr   = m.m;
		track_mlamhf.Ior  = m.I0;
		track_mlamhf.Rs   = m.Rs;
		track_mlamhf.Rp   = m.Rp;
		track_mlamhf.Tr   = m.T - 273.16;
		track_mlamhf.Ns   = m.Ns;
	
		track_mlamhf.setMap(0, track_mlamhf.Iphr*1.5, 128, 25, 100, 4);
		if (!track_mlamhf) {
			cout<<"Error."<<endl;
			cerr<<"Error: Failed to configure MPPT trackers."<<endl;
			return 1;
		}
	}
	cout<<"Ok."<<endl;
	
	// Output file creation (automatically truncated! be aware!)
	if (iOutFile) {
		cout<<"Creating data file... "<<flush;
		outFile.open(argv[iOutFile], ios::out|ios::trunc);
		if (!outFile) {
			cout<<"Error!"<<endl;
			cerr<<"Failed to create output file \""<<argv[iOutFile]<<"\"."<<endl;
			return 1;
		}
		
		// Output file header
		time_t rawtime;
		struct tm *timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		
		if (!iGeneratorTest) {
			outFile << "  Time G T1 T2 V1 V2 Vr1 Vr2 I1 I2 P1 P2" << endl;
		} else {
			outFile << "  Time V1 V2 I1 I2" << endl;
		}
		outFile << setprecision(15) << dec;
		cout<<"Ok."<<endl;
	}
	
	// Simulation run
	if (iStimuli) {
		// Load stimuli
		cout<<"Loading stimuli... "<<flush;
		std::map<std::string, std::vector<double> > stimuli = load_dat(argv[iStimuli]);
		if (stimuli.empty()) {
			cout<<"Failed."<<endl;
			cerr<<"Stimuli file empty, invalid, or inexinstent."<<endl;
			return 1;
		}
		if (
			stimuli["Time"].empty() ||
			stimuli["G"].empty() ||
			stimuli["T"].empty()
		) {
			cout<<"Failed."<<endl;
			cerr<<"Stimuli file does not contain required variables Time, G and/or T."<<endl;
			return 1;
		}
		cout<<"Ok."<<endl;
		
		// Prepare generator model (use experimental model)
		cout<<"Preparing PV generator model ("<<genparam->name<<")... "<<flush;
		pvGenerator_sc gen;
		pvgen_setup(gen, genparam->model);
		cout<<"Ok."<<endl;
		
		// Run
		std::vector<double> &Time = stimuli["Time"];
		std::vector<double> &G    = stimuli["G"];
		std::vector<double> &T    = stimuli["T"];
		double V0=0.5, I0, W0=0, P0a=0; // For True-MPP
		double V1=0.5, I1, W1=0, P1a=0; // For IncCond
		double V2=0.5, I2, W2=0, P2a=0; // For second tracker
		
		progressBar pgb(Time.size());
		cout<<"Running simulation... "<<endl;
		for (int i=0; i<Time.size(); ++i) {
			cout<<pgb(i);
			gen.setInsolation(G[i]);
			double TK = (T[i] > 200) ? T[i] : T[i] + 273.16;
			gen.setTemperature(TK); // Already KELVIN
			
			// True MPP
			double P0, Vr0;
			I0 = gen.I(Vr0);
			P0 = V0 * I0;
			I0 = pvgen_mpp_I(gen, 0.0, gen.getSourceCurrent(), 1e-4);
			Vr0 = gen.V(I0);
			
			// IncCond
			I1 = gen.I(V1);
			double P1 = V1*I1;
			double Vr1 = track_ic(V1, I1);
			
			// Second tracker
			double P2, Vr2;
			I2 = gen.I(V2);
			P2 = V2*I2;
			Vr2 = tracker(gen, V2, I2, TK);
			
			// Saving
			if (outFile)
				outFile
					<< Time[i] << " "
					<< G[i] << " "
					<< T[i] << " "
					<< T[i] << " "
					<< V1   << " "
					<< V2   << " "
					<< Vr1  << " "
					<< Vr2  << " "
					<< I1   << " "
					<< I2   << " "
					<< P1   << " "
					<< P2   << endl;
			
			if (i) {
				if (!skip_boot || Time[i] > 100) {
					W0 += (Time[i]-Time[i-1]) * (P0+P0a)/2;
					W1 += (Time[i]-Time[i-1]) * (P1+P1a)/2;
					W2 += (Time[i]-Time[i-1]) * (P2+P2a)/2;
				}
			}
			V1 = Vr1;
			V2 = Vr2;
			P0a = P0;
			P1a = P1;
			P2a = P2;
		}
		
		cout<<pgb()<<endl;
		cout<<"Done."<<endl;
		cout<<"Energy accumulated:"<<endl;
		cout<<"  W0 = "<<W0<<"J ("<<(W0/W1*100)<<"%)"<<endl;
		cout<<"  W1 = "<<W1<<"J (100.000%)"<< endl;
		cout<<"  W2 = "<<W2<<"J ("<<(W2/W1*100)<<"%)"<<endl;
		return 0;
	}
	
	// Set signal handlers
	cout<<"Setting up signal handlers... "<<flush;
	if (signal(SIGINT,  interrupt_handler) == SIG_ERR) {
		cout<<"Error!"<<endl;
		cerr<<"Error: Failed setting up signal handler for SIGINT."<<endl;
		return 1;
	}
	if (signal(SIGTERM, interrupt_handler) == SIG_ERR) {
		cout<<"Error!"<<endl;
		cerr<<"Error: Failed setting up signal handler for SIGTERM."<<endl;
		return 1;
	}
	if (signal(SIGALRM,alarm_handler) == SIG_ERR) {
		cout<<"Error!"<<endl;
		cerr<<"Error: Failed to install SIGALRM handler."<<endl;
		return 1;
	}
	if (sem_init(&alarm_sem, 0, 1)) {
		cout<<"Error!"<<endl;
		cerr<<"Error: Failed to create SIGALRM protection semaphore."<<endl;
		return 1;
	}
	if (sem_init(&main_wait, 0, 1)) {
		cout<<"Error!"<<endl;
		cerr<<"Error: Failed to create main_wait semaphore."<<endl;
		return 1;
	}
	d.add(remove_signal_handlers, 0);
	cout<<"Ok."<<endl;
	
	// Set the alarm rate
	cout<<"Setting sample period... "<<flush;
	struct itimerval itv;
	itv.it_interval.tv_sec  = unsigned(dSamplePeriod);
	itv.it_interval.tv_usec = unsigned((dSamplePeriod - itv.it_interval.tv_sec) * 1000000);
	itv.it_value.tv_sec     = 1;
	itv.it_value.tv_usec    = 0;
	if (setitimer(ITIMER_REAL,&itv, 0)) {
		cout<<"Error!"<<endl;
		cerr<<"Error: Failed to set the alarm rate."<<endl;
		return 1;
	}
	cout<<"Ok."<<endl;
	
	// Main loop
	char lops[] = "/-\\|";
	int lopn = 0;
	progressBar pgb("Running", dDuration);
	while (!interrupt_process) {
		cout<<pgb(getTime() - startTime);
		
		// Wait for a signal from the alarm handler
		struct timespec ts;
		ts.tv_sec  = 0;
		ts.tv_nsec = 100000000; //0.1s
		if (sem_timedwait(&main_wait, &ts)) {
			continue;
		}
		
//		cout<<"\rRunning... "<<lops[lopn++]<<flush;
//		if (!lops[lopn]) lopn = 0;
	}
	cout<<pgb()<<endl;
	
	return 0;
}

void alarm_handler(int) {
	sem_post(&main_wait);
	if (interrupt_process) return;
	
	// Check CPU overload
	if (sem_trywait(&alarm_sem)) {
		// Previous alarm has not finished processing
		interrupt_process = true; // Abort
		return;
	}
	
	// Time of day, of step, and of test
	static double dt=NAN, fst=NAN;
	double tod = getTime();
	if (isnan(dt)) dt = 0;
	else dt = tod - dt;
	if (isnan(fst)) fst=tod;
	double t = tod - startTime;
	
	double G, T1, T2, V1, V2, I1, I2;
	psu1.getVoltageAndCurrent(V1, I1);
	psu2.getVoltageAndCurrent(V2, I2);
	I1 *= -1; I2 *= -1;
	if (!sensor.read(&G, &T1, &T2)) {
		G = 1000;
		T1 = T2 = 40;
	}
		
	if (!iGeneratorTest) {
		// Tracking
		double Vr1 = track_ic(V1, I1);
	//	double Vr2 = track_ic(V2, I2);
		double Vr2 = track_mlamhf(V2, I2, T2);
		psu1.setVoltage(Vr1);
		psu2.setVoltage(Vr2);
	
		// Saving
		if (outFile)
			outFile
				<< t  << " "
				<< G  << " "
				<< T1 << " "
				<< T2 << " "
				<< V1 << " "
				<< V2 << " "
				<< Vr1 << " "
				<< Vr2 << " "
				<< I1 << " "
				<< I2 << " "
				<< (V1*I1) << " "
				<< (V2*I2) << endl;
	} else {
		// Generator testing
		// Just save data
		if (outFile) outFile
			<< t  << " "
			<< V1 << " "
			<< V2 << " "
			<< I1 << " "
			<< I2 << endl;
	}
	
	// Save time of day for this run, used to measure timestep.
	dt = tod;
	
	if (t>=dDuration) interrupt_process = true;
	
	// Mark processing as finished
	sem_post(&alarm_sem);
}
