/*  Standard controllers library.
 *  
 *  This is free-software, distributed under the terms of the GNU-LGPL. 
 *  Please note that this is a library of standard controllers, and not
 *    a standard library of controllers.
 */

#ifndef STDCTL_H
#define STDCTL_H

//#include <IQmathCpp.h>
typedef double iq;
#define MAX_IQ_POS +1e300
#define MAX_IQ_NEG -1e300
#define IQ(x) double(x)

/// Namespace containing all classes
namespace stdctl {

/// PI-Type controller.
class PI_controller {
	/// Integrator memory refered to the output.
	iq Mi;

	public:
	/// Proportional gain.
	iq Kp;
	/// Integrator gain.
	iq Ki;

	/** Default constructor
	 *
	 *  @param nKp is the proportional gain.
	 *  @param nKi is the integral gain.
	 *  @param nMi is the initial value of the output, if assuming zero error.
	 */
	inline PI_controller(iq nKp, iq nKi, iq nMi=0)
		: Kp(nKp), Ki(nKi), Mi(nMi)
	{	
		if(nKi) Mi/=nKi;
	}

	/** Controller main function
	 *
	 *  @param e    is the error input as reference-measured value.
	 *  @param cond is a condition to enable the integrator. If this evaluates
	 *              to false the integrator memory will not be updated.
	 *  @returns    the output of the controller.
	 */
	inline iq operator() (iq e, bool cond=true) {
		if (cond) Mi += Ki*e;
		return Kp*e + Mi;
	}

	/** Reset the integrator memory.
	 *
	 *  @param nMi is the new value for the integrator memory. Defaults to zero.
	 */
	inline void reset(iq nMi=0) {
		Mi=nMi;
	}
};

/// PI-Type controller with bounded integrator and output.
class Bounded_PI_controller {
	/// Integrator memory refered to the output.
	iq Mi;

	public:
	/// Proportional gain.
	iq Kp;
	/// Integrator gain.
	iq Ki;
	/// Low boundary for the integrator memory.
	iq MiMin;
	/// High boundary for the integrator memory.
	iq MiMax;
	/// Low boundary for the output.
	iq Min;
	/// High boundary for the output.
	iq Max;

	/** Default constructor
	 *
	 *  @param nKp is the proportional gain.
	 *  @param nKi is the integral gain.
	 *  @param nMi is the initial value of the output, if assuming zero error.
	 *  @param nMiMin is the minimum value allowed for the integrator memory.
	 *  @param nMiMax is the maximum value allowed for the integrator memory.
	 *  @param nMin is the minimum value allowed for the output.
	 *  @param nMax is the maximum value allowed for the output.
	 */
	inline Bounded_PI_controller (
		iq nKp, iq nKi, iq nMi=0,
		iq nMiMin=MAX_IQ_NEG, iq nMiMax=MAX_IQ_POS,
		iq nMin=MAX_IQ_NEG, iq nMax=MAX_IQ_POS
	) : Kp(nKp), Ki(nKi), Mi(nMi), 
	    MiMin(nMiMin), MiMax(nMiMax),
	    Min(nMin), Max(nMax)
	{	
		if(nKi) Mi/=nKi;
	}

	/** Controller main function
	 *
	 *  @param e    is the error input as reference-measured value.
	 *  @param cond is a condition to enable the integrator. If this evaluates
	 *              to false the integrator memory will not be updated.
	 *  @returns    the output of the controller.
	 */
	inline iq operator() (iq e, bool cond=true) {
		if (cond) {
			Mi += Ki*e;
			if (Mi>MiMax) Mi=MiMax;
			if (Mi<MiMin) Mi=MiMin;
		}
		iq r = Kp*e + Mi;
		if (r>Max) return Max;
		if (r<Min) return Min;
		return r;
	}

	/** Reset the integrator memory.
	 *
	 *  @param nMi is the new value for the integrator memory. Defaults to zero.
	 */
	inline void reset(iq nMi=0) {
		Mi=nMi;
	}
};

/// PI-Type controller with anti-windup and bounded output.
class AntiWindup_PI_controller {
	/// Integrator memory refered to the output.
	iq Mi;
	/// Anti-windup error.
	iq awe;

	public:
	/// Proportional gain.
	iq Kp;
	/// Integrator gain.
	iq Ki;
	/// Anti windup gain.
	iq kaw;
	/// Low boundary for the output.
	iq Min;
	/// High boundary for the output.
	iq Max;

	/** Default constructor
	 *
	 *  @param nKp  is the proportional gain.
	 *  @param nKi  is the integral gain.
	 *  @param nkaw is the anti-windup gain.
	 *  @param nMi  is the initial value of the output, if assuming zero error.
	 *  @param nMin is the minimum value allowed for the output.
	 *  @param nMax is the maximum value allowed for the output.
	 */
	inline AntiWindup_PI_controller (
		iq nKp, iq nKi, iq nkaw, iq nMi=0,
		iq nMin=MAX_IQ_NEG, iq nMax=MAX_IQ_POS
	) : Kp(nKp), Ki(nKi), Mi(nMi), 
	    kaw(nkaw), awe(0),
	    Min(nMin), Max(nMax)
	{	
		if(nKi) Mi/=nKi;
	}

	/** Controller main function
	 *
	 *  @param e    is the error input as reference-measured value.
	 *  @returns    the output of the controller.
	 */
	inline iq operator() (iq e) {
		Mi += Ki*(e-kaw*awe);
		iq r = Kp*e + Mi;
		
		if (r>Max) {
			awe = Max-r;
			return Max;
		}
		if (r<Min) {
			awe = Min-r;
			return Min;
		}
		awe = IQ(0);
		return r;
	}

	/** Reset the integrator memory.
	 *
	 *  @param nMi is the new value for the integrator memory. Defaults to zero.
	 */
	inline void reset(iq nMi=0) {
		Mi  = nMi;
		awe = IQ(0);
	}
};

}

#endif // #ifndef STDCTL_H

