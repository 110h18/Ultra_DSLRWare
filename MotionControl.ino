#include <AccelStepper.h>

#define STEPPER_E1 10
#define STEPPER_E2 11

AccelStepper stepper(AccelStepper::FULL2WIRE, 8, 9);


void init_stepper()
{

	stepper.setEnablePin(STEPPER_E1);   
	stepper.setEnablePin(STEPPER_E2); 
	stepper.disableOutputs();

}

void prepareStepperParameters(byte mode)
{
	byte curpos=0;
	uint32_t stepperTargetPosition=0;

	stepper.setCurrentPosition(curpos);
//	float linear_to_circular=stepsPerRevolution*getFloat(redRatio,redRatioPrecision)/PI/getFloat(pulleyPitch,pulleyPitchPrecision);

	switch (mode)
	{
		case MODE_CONTINIOUS:
			
			stepper.setMaxSpeed(getFloat(continiousVelocity,continiousVelocityPrecision)*linear_to_circular()/secInMin);
			stepper.setAcceleration(steperAcceleration);
			stepperTargetPosition=(float)getFloat(continiousDistance,continiousDistancePrecision)*linear_to_circular();
			stepper.moveTo(stepperTargetPosition);
//			Serial.println(stepperTargetPosition);

		break;

		case MODE_TIMELAPSE:
			
			stepperTargetPosition=(float)getFloat(timelapseDistance,timelapseDistancePrecision)*linear_to_circular();
			stepper.moveTo(stepperTargetPosition);
//			Serial.println(stepperTargetPosition);
		    stepper.setMaxSpeed(1000/*stepperMaxSpeed*/);
//			stepper.setSpeed(500/*stepperMaxSpeed*/);

		break;

	}

	
	
		
}

float linear_to_circular()
{
	return (float)stepsPerRevolution*getFloat(redRatio,redRatioPrecision)/PI/getFloat(pulleyPitch,pulleyPitchPrecision);
}

float circular_to_linear()
{
	return 1/(float)stepsPerRevolution/getFloat(redRatio,redRatioPrecision)*PI*getFloat(pulleyPitch,pulleyPitchPrecision);
}

uint32_t time_to_sec(byte mode)
{
	switch (mode)
	{
		case MODE_TIMELAPSE:
		return timelapseTimeHr*secInMin*secInMin+timelapseTimeMin*secInMin+timelapseTimeSec;
		break;
	}
	return 0;
	
}



void run() 
{	
	stepper.run();
}

void runSpeed()
{
	stepper.runSpeed();
}

void stop() {
	
	stepper.stop();
	FlexiTimer2::stop();
	stepper.disableOutputs();
}

void getStepperPosition()
{
	byte a=10;	
	Position = (float)stepper.currentPosition()*circular_to_linear()*a;
//	Serial.println(stepper.speed());
}

void makeFrame()
{
}

void match_timelapse_parameters()
{
	switch (timelapseMode)
	{
	case 0:
		timelapseFrmInt=((float)(time_to_sec(MODE_TIMELAPSE))-(float)timelapseDistance/getMaxLinearSpeed()*secInMin)/timelapseFrms;
		timelapseOffcet=(float)((float)(time_to_sec(MODE_TIMELAPSE))/timelapseFrms-getFloat(timelapseFrmInt,timelapseFrmIntPrecision))/getMaxLinearSpeed()*100;
//		Serial.println((float)((float)(time_to_sec(MODE_TIMELAPSE))/timelapseFrms-getFloat(timelapseFrmInt,timelapseFrmIntPrecision))/getMaxLinearSpeed()*100);


	break;

	default:
		timelapseFrms=(float)(time_to_sec(MODE_TIMELAPSE))/((float)timelapseOffcet/getMaxLinearSpeed()*secInMin+getFloat(timelapseFrmInt,timelapseFrmIntPrecision));
		timelapseDistance=timelapseOffcet*timelapseFrms;
		break;
	}
}

float getCurrentOffcetInterval(void)
{
	byte a=100;
	timelapseOffecetInterval= (float)(time_to_sec(MODE_TIMELAPSE) - (float)timelapseDistance/getMaxLinearSpeed()*secInMin)/timelapseFrms*a;
	return timelapseOffecetInterval;
	
}

float getCurrentTimelapseLinearSpeed(void)
{
	
	timelapseVelocity=(float) timelapseOffcet/getFloat(timelapseFrmInt,timelapseFrmIntPrecision)*secInMin;
//	Serial.println((float) timelapseOffcet);
	
	return timelapseVelocity;
	
}

float getCurrentLinearSpeed(void)
{
	
	continiousVelocity=(float) continiousDistance/(continiousTimeHr*secInMin+continiousTimeMin+(float) continiousTimeSec/secInMin);
	
	return continiousVelocity;
	
}

float getFloat(uint32_t number, uint8_t precision)
{
	byte a =10;
	return number*pow(a,-precision);
}

uint32_t getMaxLinearSpeed(void)
{
	uint8_t a = 10;
	
	return (float) stepperMaxSpeed*circular_to_linear()*a*secInMin;
}

void run_stepper_interrupts(byte mode)
{
	stepper.enableOutputs();
	switch (mode)
	{
	case MODE_CONTINIOUS:
		FlexiTimer2::set(1, 1.0/stepperMaxSpeed/3, run); 
	break;
	case MODE_TIMELAPSE:
		FlexiTimer2::set(1, 1.0/stepperMaxSpeed/3, runSpeed); 
	break;

	}	
		FlexiTimer2::start();
//		Serial.println("interrupts");
}

void init_exposure(void){

	uint8_t a=10;

	switch (timelapseExposureMode)
	{
	case 0:
		timelapseExp=timelapseCameraExp;
	break;
	default :
		timelapseExp=timelapseBULBExp;
	break;

	}	

	timelapseFrmTime=(timelapseBracketing*(timelapseSafetyInt*a+timelapseExp)+timelapseSafetyInt*a)/a;
	
}


