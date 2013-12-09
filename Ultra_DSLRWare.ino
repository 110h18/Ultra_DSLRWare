#include <LiquidCrystal.h>
#include "M2tk.h"
#include "utility/m2ghlc.h"
#include <FlexiTimer2.h>


////////////////////////////////////////
//  Declare LCD
////////////////////////////////////////

#define LCD_RS A4
#define LCD_EN 6
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2
#define LCD_BKLT 7

#define lcd_rows 2
#define lcd_columns 16

LiquidCrystal lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

byte lcd_bklt_state=HIGH;

#define PI 3.14
const uint8_t secInMin=60;


////////////////////////////////////////
//  Declare States
////////////////////////////////////////

#define NONE 0
#define REFRESH_POSITION_LCD 1
#define BLINK_CONTINIOUS_CURRENT_MAX_SPEED_LCD 2
#define BLINK_TIMELAPSE_CURRENT_MAX_SPEED_LCD 3
#define BLINK_TIMELAPSE_CURRENT_MIN_FRM_TIME 4
#define START_STEPPER_CONTINIOUS 5
#define START_STEPPER_TIMELAPSE 6
#define MAKE_FRAME 7
#define PAUSE_STEPPER_TIMELAPSE 8
#define STOP_STEPPER_TIMELAPSE 9

#define MODE_CONTINIOUS 1
#define MODE_TIMELAPSE 2



uint8_t state=NONE;
unsigned long refresh;


////////////////////////////////////////
//  Declare Motion Control 
////////////////////////////////////////

// Common Variables

uint32_t spdIndicator;
uint8_t spdIndicatorPrecision=1;

uint32_t frmTimeIndicator;
uint8_t frmTimeIndicatorPrecision=2;

uint32_t Position = 0;

bool indicatorState=true;

//Continious mode

const char *fn_idx_to_color(uint8_t idx)
{
  if ( idx == 0 )
    return "V&Dist";
  else if (idx == 1 )
    return "V&t";
  return "Dist&t";
}

uint8_t continiousMode = 2;
uint32_t continiousVelocity = 2000;
uint8_t continiousVelocityPrecision=1;
uint32_t continiousDistance = 1000;
uint8_t continiousDistancePrecision=1;


uint8_t continiousTimeHr=0;
uint8_t continiousTimeMin=1;
uint8_t continiousTimeSec=0;



//Timelapse mode

const char *fn_idx_to_timelapse_mode(uint8_t idx)
{
  if ( idx == 0 )
    return "Dist&t&#Frms";
   
  return "Offct&t&FrmInt";
}

uint8_t timelapseMode = 1;

uint32_t timelapseDistance = 500;
uint8_t timelapseDistancePrecision=1;
uint32_t timelapseOffcet = 10;
uint8_t timelapseOffcetPrecision=1;
uint32_t timelapseFrmInt = 100;
uint8_t timelapseFrmIntPrecision=2;

uint32_t timelapseVelocity = 0;
uint8_t timelapseVelocityPrecision=1;

uint32_t timelapseFrms=999;
uint32_t timelapseFrmsDone=0;

uint32_t timelapseFrmTime=100;
uint32_t timelapseOffecetInterval;
uint8_t timelapseFrmTimePrecision=2;

uint32_t timelapseMinFrmTime=50;
uint8_t timelapseMinFrmTimePrecision=2;


uint8_t timelapseTimeHr=0;
uint8_t timelapseTimeMin=30;
uint8_t timelapseTimeSec=0;

uint8_t timelapseBracketing=1;

uint32_t timelapseSafetyInt=10;   //0.1 sec
uint32_t timelapseMaxSafetyInt=25;
uint32_t timelapseMinSafetyInt=10;
uint8_t timelapseSafetyIntPrecision=2;

uint32_t timelapseBULBExp=1500;

uint32_t timelapseCameraExp=500;
uint32_t timelapseExp=0;

uint8_t timelapseExpPrecision=3;



const char *fn_idx_to_timelapse_exposure_mode(uint8_t idx)
{
  if ( idx == 0 )
    return "Camera Driven";
   
  return "BULB";
}

uint8_t timelapseExposureMode=0;

//Camera Options

uint32_t CameraMinBULBExp=1000;
uint32_t CameraFrameSec=37;
uint8_t CameraFrameSecPrecision=1;
uint8_t CameraShootSignals = 1;

const char *fn_idx_to_shoot_signals(uint8_t idx)
{
  if ( idx == 0 )
    return "Shoot";
   
  return "Shoot&Focus";
}


//Stepper Options

uint32_t redRatio = 1373;
uint8_t redRatioPrecision=2;
uint32_t pulleyPitch = 4456;
uint8_t pulleyPitchPrecision=3;
uint32_t stepperMaxSpeed = 1500;
uint8_t stepperMaxSpeedPrecision=0;
uint32_t stepsPerRevolution =200;
uint8_t stepsPerRevolutionPrecision=0;
uint32_t steperAcceleration =800;
uint8_t steperAccelerationPrecision=0;



////////////////////////////////////////
//  Common UI Elements
////////////////////////////////////////

M2_BUTTON(el_ok, NULL, "ok", fn_ok);
//M2_BUTTON(el_stop, NULL, "stop", fn_stop);


////////////////////////////////////////
//  UI Forms
////////////////////////////////////////

//*************************
// Continious
//*************************

//  Continious Mode Form

M2_LABEL(el_label_Mode, NULL, "Mode:");
M2_COMBO(el_combo_Mode_Continious, NULL, &continiousMode, 3, fn_idx_to_color);
M2_LIST(list_1_1) = { 
    &el_label_Mode, &el_combo_Mode_Continious, 
    &el_ok
};
M2_GRIDLIST(list_element_1_1, "c2",list_1_1);

//  Continious Velocity Form

M2_LABEL(el_label_continiousVelocity, NULL, "Velocity, cm/min");
M2_U32NUMFN(el_s32num_continiousVelocity,"a1c4.1",&u32_cb_Velocity);
M2_LIST(list_continiousVelocity) = { &el_s32num_continiousVelocity,&el_ok };
M2_HLIST(list_element_continiousVelocity, NULL,list_continiousVelocity);
M2_LIST(list_continiousVelocityForm) = { &el_label_continiousVelocity, &list_element_continiousVelocity};
M2_VLIST(list_element_continiousVelocityForm, NULL,list_continiousVelocityForm);

//  Continious Distance Form

M2_LABEL(el_label_continiousDistance, NULL, "Distance, cm");
M2_U32NUM(el_s32num_continiousDistance,"a1c4.1",&continiousDistance);
M2_LIST(list_continiousDistance) = { &el_s32num_continiousDistance,&el_ok };
M2_HLIST(list_element_continiousDistance, NULL,list_continiousDistance);
M2_LIST(list_continiousDistanceForm) = { &el_label_continiousDistance, &list_element_continiousDistance};
M2_VLIST(list_element_continiousDistanceForm, NULL,list_continiousDistanceForm);

//  Continious Time Form

//M2_LABEL(el_label_continiousTime, NULL, "t:");
M2_LABEL(el_label_hr, NULL, "h");
M2_LABEL(el_label_min, NULL, "m");
M2_LABEL(el_label_sec, NULL, "s");
M2_U8NUM(el_s8num_continiousTimeHr,"a1c2",NULL,99,&continiousTimeHr);
M2_U8NUM(el_s8num_continiousTimeMin,"a1c2",NULL,59,&continiousTimeMin);
M2_U8NUM(el_s8num_continiousTimeSec,"a1c2",NULL,59,&continiousTimeSec);
M2_LIST(list_continiousTime) = { &el_s8num_continiousTimeHr, &el_label_hr, &el_s8num_continiousTimeMin, &el_label_min, &el_s8num_continiousTimeSec,&el_label_sec};
M2_HLIST(list_element_continiousTime, NULL,list_continiousTime);
M2_LIST(list_continiousTimeForm) = { &list_element_continiousTime, &el_ok};
M2_VLIST(list_element_continiousTimeForm, NULL,list_continiousTimeForm);

//  Continious Distance Form with MAX Speed control

M2_LABEL(el_label_continiousDistanceSpdControl, NULL, "Dist.,cm");
M2_LIST(list_continiousDistanceSpdControlLine1) = { &el_label_continiousDistanceSpdControl, &el_s32num_continiousDistance};
M2_HLIST(list_element_continiousDistanceSpdControlLine1, NULL,list_continiousDistanceSpdControlLine1);
M2_U32NUM(el_s32num_continiousSpdIndicator,"r1c4.1",&spdIndicator);
M2_LABEL(el_label_cm_min, NULL, "cm/mn");
M2_LIST(list_continiousDistanceSpdControlLine2) = { &el_s32num_continiousSpdIndicator, &el_label_cm_min,&el_ok};
M2_HLIST(list_element_continiousDistanceSpdControlLine2, NULL,list_continiousDistanceSpdControlLine2);
M2_LIST(list_continiousDistanceSpdControlForm) = { &list_element_continiousDistanceSpdControlLine1, &list_element_continiousDistanceSpdControlLine2};
M2_VLIST(list_element_continiousDistanceSpdControlForm, NULL,list_continiousDistanceSpdControlForm);

//  Continious Time Form with MAX Speed control

M2_LIST(list_continiousTimeSpdControlForm) = { &list_element_continiousTime, &list_element_continiousDistanceSpdControlLine2};
M2_VLIST(list_element_continiousTimeSpdControlForm, NULL,list_continiousTimeSpdControlForm);

//  Execute Form


M2_LABEL(el_label_MaxSpeedExceeded, NULL, "MAX Spd exceeded");
M2_LIST(list_MaxSpeedExceeded) = { 
    &el_label_MaxSpeedExceeded, &el_ok
};
M2_GRIDLIST(list_element_MaxSpeedExceeded, "c1",list_MaxSpeedExceeded);


M2_U32NUM(el_s32num_testDistance,"r1c4.1",&Position);
M2_LIST(list_testPosition) = { &el_s32num_testDistance,&el_ok };
M2_HLIST(list_element_testPosition, NULL,list_testPosition);
M2_LIST(list_testPositionForm) = { &el_label_continiousDistance, &list_element_testPosition};
M2_VLIST(list_element_testPositionForm, NULL,list_testPositionForm);

//*************************
// Timelapse
//*************************

//  Timelapse Mode Form

M2_COMBO(el_combo_Mode_Timelapse, NULL, &timelapseMode, 2, fn_idx_to_timelapse_mode);
M2_LIST(list_timelapseModeLine1) = { 
    &el_label_Mode, &el_ok};
M2_HLIST(list_element_timelapseModeLine1, NULL,list_timelapseModeLine1);
M2_LIST(list_timelapseMode) = {&list_element_timelapseModeLine1, &el_combo_Mode_Timelapse};
M2_VLIST(list_element_timelapseMode, NULL,list_timelapseMode);

//  Timelapse Offcet

M2_LABEL(el_label_timelapseOffcet, NULL, "Offcet,cm");
M2_U32NUM(el_s32num_timelapseOffcet,"a1c3.1",&timelapseOffcet);
M2_LIST(list_timelapseOffcetLine1) = { &el_label_timelapseOffcet,&el_s32num_timelapseOffcet};
M2_HLIST(list_element_timelapseOffcetLine1, NULL,list_timelapseOffcetLine1);
M2_LIST(list_timelapseOffcetLine2) = { &el_s32num_continiousSpdIndicator,&el_label_cm_min,&el_ok};
M2_HLIST(list_element_timelapseOffcetLine2, NULL,list_timelapseOffcetLine2);
M2_LIST(list_timelapseOffcetForm) = { &list_element_timelapseOffcetLine1, &list_element_timelapseOffcetLine2};
M2_VLIST(list_element_timelapseOffcetForm, NULL,list_timelapseOffcetForm);

//  Timelapse Frame Interval

M2_LABEL(el_label_timelapseFrmInt, NULL, "FrmInt,s");
M2_U32NUM(el_s32num_timelapseFrmInt,"a1c4.2",&timelapseFrmInt);
M2_LIST(list_timelapseFrmInt) = {&el_label_timelapseFrmInt, &el_s32num_timelapseFrmInt };
M2_HLIST(list_element_timelapseFrmInt, NULL,list_timelapseFrmInt);
M2_LIST(list_timelapseFrmIntForm) = {&list_element_timelapseFrmInt,&list_element_timelapseOffcetLine2};
M2_VLIST(list_element_timelapseFrmIntForm, NULL,list_timelapseFrmIntForm);

// Timelapse Time Form

M2_U8NUM(el_s8num_timelapseTimeHr,"a1c2",NULL,99,&timelapseTimeHr);
M2_U8NUM(el_s8num_timelapseTimeMin,"a1c2",NULL,59,&timelapseTimeMin);
M2_U8NUM(el_s8num_timelapseTimeSec,"a1c2",NULL,59,&timelapseTimeSec);
M2_LIST(list_timelapseTime) = { &el_s8num_timelapseTimeHr, &el_label_hr, &el_s8num_timelapseTimeMin, &el_label_min, &el_s8num_timelapseTimeSec,&el_label_sec};
M2_HLIST(list_element_timelapseTime, NULL,list_timelapseTime);
M2_LIST(list_timelapseTimeForm) = { &list_element_timelapseTime, &el_ok};
M2_VLIST(list_element_timelapseTimeForm, NULL,list_timelapseTimeForm);

// Timelapse Time Form with current/MIN Frame Time indicator 

M2_U32NUM(el_s32num_FrmTimeIndicator,"c6.2r1",&frmTimeIndicator);
M2_LIST(list_FrmTimeIndicator)={ &el_s32num_FrmTimeIndicator, &el_label_sec, &el_ok};
M2_HLIST(list_element_FrmTimeIndicator, NULL,list_FrmTimeIndicator);
M2_LIST(list_timelapseFrmTimeIndicator) = { &list_element_timelapseTime, &list_element_FrmTimeIndicator};
M2_VLIST(list_element_timelapseFrmTimeIndicator, NULL,list_timelapseFrmTimeIndicator);

// Timelapse Distance with current/MIN Frame Time indicator 

M2_LABEL(el_label_timelapseDistance, NULL, "Dist,cm");
M2_U32NUM(el_s32num_timelapseDistance,"a1c4.1",&timelapseDistance);
M2_LIST(list_timelapseDistance) = { &el_label_timelapseDistance,&el_s32num_timelapseDistance };
M2_HLIST(list_element_timelapseDistance, NULL,list_timelapseDistance);
M2_LIST(list_timelapseDistanceForm) = { &list_element_timelapseDistance, &list_element_FrmTimeIndicator};
M2_VLIST(list_element_timelapseDistanceForm, NULL,list_timelapseDistanceForm);

// Timelapse Frame Number with current/MIN Frame Time indicator 

M2_LABEL(el_label_timelapseFrms, NULL, "# Frames");
M2_U32NUM(el_s32num_timelapseFrms,"a1c4",&timelapseFrms);
M2_LIST(list_timelapseFrms) = { &el_label_timelapseFrms,&el_s32num_timelapseFrms };
M2_HLIST(list_element_timelapseFrms, NULL,list_timelapseFrms);
M2_LIST(list_timelapseFrmsForm) = { &list_element_timelapseFrms, &list_element_FrmTimeIndicator};
M2_VLIST(list_element_timelapseFrmsForm, NULL,list_timelapseFrmsForm);

// Timelapse Frame Number with current/MIN Frame Time indicator 

M2_LABEL(el_label_MinFrameTimeError, NULL, "MIN FrmTime err.");
M2_LIST(list_MinFrameTimeError) = {
    &el_label_MinFrameTimeError, &el_ok
};
M2_GRIDLIST(list_element_MinFrameTimeError, "c1",list_MinFrameTimeError);

// Timelapse Exposure MODE Form

M2_LABEL(el_label_timelapseExposureMode, NULL, "Exp. Mode:");
M2_COMBO(el_combo_timelapseExposureMode, NULL, &timelapseExposureMode, 2, fn_idx_to_timelapse_exposure_mode);
M2_LIST(list_timelapseExposureModeLine1) = { 
    &el_label_timelapseExposureMode, &el_ok};
M2_HLIST(list_element_timelapseExposureModeLine1, NULL,list_timelapseExposureModeLine1);
M2_LIST(list_timelapseExposureMode) = {&list_element_timelapseExposureModeLine1, &el_combo_timelapseExposureMode};
M2_VLIST(list_element_timelapseExposureMode, NULL,list_timelapseExposureMode);

// Timelapse BULB Exposure Form

M2_LABEL(el_label_timelapseBULBExp, NULL, "BULB Exp., sec");
//M2_COMBO(el_combo_timelapseExposureMode, NULL, &timelapseExposureMode, 2, fn_idx_to_timelapse_exposure_mode);
M2_U32NUMFN(el_s32num_timelapseBULBExp,"a1c5.3",&u32_cb_timelapseBULBExp);
M2_LIST(list_timelapseBULBExpLine2) = { &el_s32num_timelapseBULBExp, &el_ok};
M2_HLIST(list_element_timelapseBULBExpLine2, NULL,list_timelapseBULBExpLine2);
M2_LIST(list_timelapseBULBExp) = {&el_label_timelapseBULBExp,&list_element_timelapseBULBExpLine2};
M2_VLIST(list_element_timelapseBULBExp, NULL,list_timelapseBULBExp);

// Timelapse Camera Exposure Form

M2_LABEL(el_label_timelapseCameraExp, NULL, "Camera Exp., sec");
//M2_COMBO(el_combo_timelapseExposureMode, NULL, &timelapseExposureMode, 2, fn_idx_to_timelapse_exposure_mode);
M2_U32NUMFN(el_s32num_timelapseCameraExp,"a1c5.3",&u32_cb_timelapseCameraExp);
M2_LIST(list_timelapseCameraExpLine2) = {&el_s32num_timelapseCameraExp,&el_ok};
M2_HLIST(list_element_timelapseCameraExpLine2, NULL,list_timelapseCameraExpLine2);
M2_LIST(list_timelapseCameraExp) = {&el_label_timelapseCameraExp,&list_element_timelapseCameraExpLine2};
M2_VLIST(list_element_timelapseCameraExp, NULL,list_timelapseCameraExp);



//  Timelapse Bracketing Form

M2_LABEL(el_label_timelapseBracketing, NULL, "Bracketing:");
M2_U8NUM(el_s8num_timelapseBracketing,"a1c1",1,8,&timelapseBracketing);
M2_LIST(list_timelapseBracketing) = { &el_s8num_timelapseBracketing, &el_ok};
M2_HLIST(list_element_timelapseBracketing, NULL,list_timelapseBracketing);
M2_LIST(list_timelapseBracketingForm) = {&el_label_timelapseBracketing, &list_element_timelapseBracketing};
M2_VLIST(list_element_timelapseBracketingForm, NULL,list_timelapseBracketingForm);

//  Timelapse Safety Interval Form

M2_LABEL(el_label_timelapseSafetyInt, NULL, "Safety Int, sec");
M2_U32NUMFN(el_s32num_timelapseSafetyInt,"a1c3.2",&u32_cb_SafetyInt);
M2_LIST(list_timelapseSafetyInt) = { &el_s32num_timelapseSafetyInt,&el_ok };
M2_HLIST(list_element_timelapseSafetyInt, NULL,list_timelapseSafetyInt);
M2_LIST(list_timelapseSafetyIntForm) = { &el_label_timelapseSafetyInt, &list_element_timelapseSafetyInt};
M2_VLIST(list_element_timelapseSafetyIntForm, NULL,list_timelapseSafetyIntForm);

//*************************
// Camera Options
//*************************

//  Camera Min BULB Exposure Form

M2_LABEL(el_label_CameraMinBULBExp, NULL, "Min BULB Exp,sec");
M2_U32NUM(el_s32num_CameraMinBULBExp,"a1c5.3",&CameraMinBULBExp);
M2_LIST(list_CameraMinBULBExp) = { &el_s32num_CameraMinBULBExp,&el_ok };
M2_HLIST(list_element_CameraMinBULBExp, NULL,list_CameraMinBULBExp);
M2_LIST(list_CameraMinBULBExpForm) = { &el_label_CameraMinBULBExp, &list_element_CameraMinBULBExp};
M2_VLIST(list_element_CameraMinBULBExpForm, NULL,list_CameraMinBULBExpForm);

//  Camera Frame/sec capability Form

M2_LABEL(el_label_CameraFrameSec, NULL, "Camera frame/sec");
M2_U32NUM(el_s32num_CameraFrameSec,"a1c2.1",&CameraFrameSec);
M2_LIST(list_CameraFrameSec) = { &el_s32num_CameraFrameSec,&el_ok };
M2_HLIST(list_element_CameraFrameSec, NULL,list_CameraFrameSec);
M2_LIST(list_CameraFrameSecForm) = { &el_label_CameraFrameSec, &list_element_CameraFrameSec};
M2_VLIST(list_element_CameraFrameSecForm, NULL,list_CameraFrameSecForm);


//*************************
// Stepper Options
//*************************

// Pulley Pitch Diameter

M2_LABEL(el_label_pulleyPitch, NULL, "Pulley Pitch, cm");
M2_U32NUM(el_s32num_pulleyPitch,"c4.3",&pulleyPitch);
M2_LIST(list_pulleyPitch) = { &el_s32num_pulleyPitch,&el_ok };
M2_HLIST(list_element_pulleyPitch, NULL,list_pulleyPitch);
M2_LIST(list_pulleyPitchForm) = { &el_label_pulleyPitch, &list_element_pulleyPitch};
M2_VLIST(list_element_pulleyPitchForm, NULL,list_pulleyPitchForm);

// Stepper MAX Speed (steps/sec)

M2_LABEL(el_label_stepperMaxSpeed, NULL, "MAX Spd stps/sec");
M2_U32NUM(el_s32num_stepperMaxSpeed,"c4",&stepperMaxSpeed);
M2_LIST(list_stepperMaxSpeed) = { &el_s32num_stepperMaxSpeed,&el_ok };
M2_HLIST(list_element_stepperMaxSpeed, NULL,list_stepperMaxSpeed);
M2_LIST(list_stepperMaxSpeedForm) = { &el_label_stepperMaxSpeed, &list_element_stepperMaxSpeed};
M2_VLIST(list_element_stepperMaxSpeedForm, NULL,list_stepperMaxSpeedForm);

// Reductor Ratio

M2_LABEL(el_label_redRatio, NULL, "Reductor Ratio:");
M2_U32NUM(el_s32num_redRatio,"c4.2",&redRatio);
M2_LIST(list_redRatio) = { &el_s32num_redRatio,&el_ok };
M2_HLIST(list_element_redRatio, NULL,list_redRatio);
M2_LIST(list_redRatioForm) = { &el_label_redRatio, &list_element_redRatio};
M2_VLIST(list_element_redRatioForm, NULL,list_redRatioForm);



////////////////////////////////////////
//  Main Menu Structure
////////////////////////////////////////

M2_EXTERN_HLIST(top_el_expandable_menu);

// Left entry: Menu name. Submenus must have a '.' at the beginning
// Right entry: Reference to the target Form
m2_xmenu_entry m2_xmenu_data[] = 
{  
  { "Continious", NULL, NULL },
	 { ". Mode", &list_element_1_1, NULL },
	 { ".", NULL /*&list_element_continiousVelocityForm*/, xmenu_value2 },
	 { ".", NULL /*&list_element_continiousDistanceForm*/, xmenu_value3 },
	 { ". Smooth strt", &top_el_expandable_menu, NULL },
	 { ". Execute", NULL/*&list_element_testPositionForm*/, continious_execute },
  { "Timelapse", NULL, NULL },
	 { ". Mode", &list_element_timelapseMode, NULL },
	 { ".", NULL, xmenu_timelapse_value2 },
	 { ".", NULL, xmenu_timelapse_value3 },
	 { ". Time", NULL, xmenu_timelapse_value4 },
	 { ". Exp. Mode", &list_element_timelapseExposureMode, NULL },
	 { ".", NULL, *xmenu_timelapse_value5 },
	 { ". Brcktng", &list_element_timelapseBracketingForm, NULL },
	 { ". Safe Int", &list_element_timelapseSafetyIntForm, NULL },
//	 { ". Frm dur inf", &list_element_timelapseSafetyIntForm, NULL },
	 { ". Execute", NULL, timelapse_execute },
  { "Camera Opt", NULL, NULL },
	 { ". Min BULB", &list_element_CameraMinBULBExpForm, NULL },
	 { ". Cam frms/s", &list_element_CameraFrameSecForm, NULL },
	 { ". On Shoot", &list_element_CameraMinBULBExpForm, NULL },
  { "Cntrller Opt", NULL, NULL },
  { "Stepper Opt", &top_el_expandable_menu, NULL },
	 { ". Pulley Ptch", &list_element_pulleyPitchForm, NULL },
	 { ". MAX Speed", &list_element_stepperMaxSpeedForm, NULL },
	 { ". Red. Ratio", &list_element_redRatioForm, NULL },

  { NULL, NULL, NULL },
};


// The first visible line and the total number of visible lines.
// Both values are written by M2_2LMENU and read by M2_VSB
uint8_t m2_xmenu_first;
uint8_t m2_xmenu_cnt;

// M2_2LMENU definition
// Option l4 = four visible lines
// Option e1 = first column has a width of 1 char
// Option w12 = second column has a width of 12 chars

M2_X2LMENU(el_xmenu,"l2e1w12",&m2_xmenu_first,&m2_xmenu_cnt, m2_xmenu_data,'+','-','\0');
M2_VSB(el_vsb, "l2w1r1", &m2_xmenu_first, &m2_xmenu_cnt);
M2_LIST(list_xmenu) = { &el_xmenu, &el_vsb };
M2_HLIST(top_el_expandable_menu, NULL, list_xmenu);

// m2 object and constructor
M2tk m2(&top_el_expandable_menu, m2_es_arduino_analog_input, m2_eh_6bs, m2_gh_lc);

void setup(){
  digitalWrite(A1,HIGH);
  pinMode(LCD_BKLT,OUTPUT);
  digitalWrite(LCD_BKLT,lcd_bklt_state);

  m2_SetLiquidCrystal(&lcd, lcd_columns, lcd_rows);
  init_stepper();
  init_exposure();
  Serial.begin(9600);
  
//  m2.setPin(M2_KEY_SELECT, uiKeySelectPin);
//  m2.setPin(M2_KEY_NEXT, uiKeyNextPin);

  	
  
}

void loop() {
  
  m2.checkKey();
  m2.checkKey();
  if ( m2.handleKey() )
    m2.draw();
  m2.checkKey();
  m2.checkKey();
  process_state();
}

void process_state(void)
{
	switch(state) 
	{
	case REFRESH_POSITION_LCD: if (millis()-refresh>250){getStepperPosition();m2.draw();refresh=millis();} break;
    case BLINK_CONTINIOUS_CURRENT_MAX_SPEED_LCD: if (millis()-refresh>1000){continiousSpdIndicator();refresh=millis();} break;
	case BLINK_TIMELAPSE_CURRENT_MAX_SPEED_LCD: if (millis()-refresh>1000){timelapseSpdIndicator();refresh=millis();} break;
	case BLINK_TIMELAPSE_CURRENT_MIN_FRM_TIME: if (millis()-refresh>1000){timelapseFrmTimeIndicator();refresh=millis();} break;
	case START_STEPPER_CONTINIOUS:  prepareStepperParameters(MODE_CONTINIOUS); run_stepper_interrupts(MODE_CONTINIOUS); set_state(REFRESH_POSITION_LCD); break;
	case START_STEPPER_TIMELAPSE: run_stepper_interrupts(MODE_TIMELAPSE); set_state(PAUSE_STEPPER_TIMELAPSE);break;
	case PAUSE_STEPPER_TIMELAPSE:  getStepperPosition(); if (Position>=timelapseOffcet*timelapseFrmsDone){stop(); set_state(MAKE_FRAME); if (millis()-refresh>250){m2.draw();refresh=millis();}} break;
	case STOP_STEPPER_TIMELAPSE: stop(); set_state(NONE); break;
	case MAKE_FRAME: 
					 if (timelapseFrmsDone<timelapseFrms)
					 {
						 for(byte i=0;i<timelapseBracketing;i++){
							 delay(timelapseSafetyInt*10);
							 digitalWrite(13,HIGH);
							 delay(timelapseExp);
							 digitalWrite(13,LOW);
						 }
						 delay(timelapseSafetyInt*10);
//						 Serial.println("delay");
						 timelapseFrmsDone++;
						 set_state(START_STEPPER_TIMELAPSE);
					 } 
					 else
					 {
						 set_state(STOP_STEPPER_TIMELAPSE);
					 }
					 
	break;
    default: break;
	}
}

void set_state(uint8_t state_id)
{
	refresh=millis();
	state=state_id;
}



/*
  M2tklib Test Board
  R Div R Div Scaled    Center Value
  0.000     190                 104
  0.203     362                 303
  0.389     529                 479
  0.548     693                 633
  0.688     858                 864
  
*/
int read_analog_buttons()
{
  uint16_t adc_key_in = analogRead(1);      // read the value from the sensor 
 // Serial.println(adc_key_in);
  if (adc_key_in < 276)  return M2_KEY_SELECT;  
  if (adc_key_in < 445)  return M2_KEY_NEXT; 
  if (adc_key_in < 611)  return M2_KEY_DATA_DOWN; 
  if (adc_key_in < 775)  return M2_KEY_DATA_UP;
  if (adc_key_in < 900)  return M2_KEY_PREV;   
  return M2_KEY_NONE;  // default
 
}

uint8_t m2_es_arduino_analog_input(m2_p ep, uint8_t msg) 
{
  switch(msg)
  {
    case M2_ES_MSG_GET_KEY:
      return read_analog_buttons();
    case M2_ES_MSG_INIT:
      return 0;
  }
  return 0;
}

void fn_ok(m2_el_fnarg_p fnarg) {
	
	//stop everithing
	stop();
	
	set_state(NONE);
	
	//goto root menu
	m2_Clear();
	m2_SetRoot(&top_el_expandable_menu);
	init_exposure();
  
}



uint32_t u32_cb_Velocity(m2_rom_void_p element, uint8_t msg, uint32_t val)
{
  
	if ( msg == M2_U32_MSG_SET_VALUE )
  {
	  if(val<getMaxLinearSpeed() && val>0){continiousVelocity = val;}

  }
//	Serial.println(getMaxLinearSpeed());
  return continiousVelocity;
}



uint32_t u32_cb_SafetyInt(m2_rom_void_p element, uint8_t msg, uint32_t val)
{
  
	if ( msg == M2_U32_MSG_SET_VALUE )
  {
	  if(val<=timelapseMaxSafetyInt && val>=timelapseMinSafetyInt){timelapseSafetyInt = val;}

  }
//	Serial.println(getMaxLinearSpeed());
  return timelapseSafetyInt;
}

uint32_t u32_cb_timelapseBULBExp(m2_rom_void_p element, uint8_t msg, uint32_t val)
{
  
	if ( msg == M2_U32_MSG_SET_VALUE )
  {
	  if(val<=CameraMinBULBExp){timelapseBULBExp = val;}

  }

	init_exposure();

//	Serial.println(getMaxLinearSpeed());
  return timelapseBULBExp;
}

uint32_t u32_cb_timelapseCameraExp(m2_rom_void_p element, uint8_t msg, uint32_t val)
{

	if ( msg == M2_U32_MSG_SET_VALUE )
  {
	  if(val>=1000/getFloat(CameraFrameSec,CameraFrameSecPrecision)){timelapseCameraExp = val;}

  }

	init_exposure();

//	Serial.println(getMaxLinearSpeed());
  return timelapseCameraExp;
}




void continiousSpdIndicator(){
//	u32_cb_Distance(&el_s32num_testDistance, M2_U32_MSG_SET_VALUE, NULL);
	indicatorState=!indicatorState;
	if (indicatorState==true) {spdIndicator=getCurrentLinearSpeed();}
	if (indicatorState==false) {spdIndicator=getMaxLinearSpeed();}
	m2.draw();
}

void timelapseSpdIndicator(){
//	u32_cb_Distance(&el_s32num_testDistance, M2_U32_MSG_SET_VALUE, NULL);
	indicatorState=!indicatorState;
	if (indicatorState==true) {spdIndicator=getCurrentTimelapseLinearSpeed();}
	if (indicatorState==false) {spdIndicator=getMaxLinearSpeed();}
	m2.draw();
}

void timelapseFrmTimeIndicator(){
//	u32_cb_Distance(&el_s32num_testDistance, M2_U32_MSG_SET_VALUE, NULL);
	indicatorState=!indicatorState;
	if (indicatorState==true) {frmTimeIndicator=getCurrentOffcetInterval();}
	if (indicatorState==false) {frmTimeIndicator=timelapseFrmTime;}
	m2.draw();
}

const char *xmenu_value2(uint8_t idx, uint8_t msg)
{  
  if ( msg == M2_STRLIST_MSG_GET_STR ) {
	  switch (continiousMode)	{
		 case 0:
			return " Velocity";
		 case 1:
			return " Velocity";
		 default:
			return " Distance";
	  }
	}
   if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   switch (continiousMode)	{
		 case 0:			
			m2_SetRoot(&list_element_continiousVelocityForm);
		 break;
		 case 1:
			m2_SetRoot(&list_element_continiousVelocityForm);
		 break;
		 default:
//			FlexiTimer2::set(1000, spdIndicator); 
//			FlexiTimer2::start();
			set_state(BLINK_CONTINIOUS_CURRENT_MAX_SPEED_LCD);
			m2_SetRoot(&list_element_continiousDistanceSpdControlForm);	
		 break;
	  }
	}

  
  return "";
}

const char *xmenu_value3(uint8_t idx, uint8_t msg)
{  
  if ( msg == M2_STRLIST_MSG_GET_STR ) {
	  switch (continiousMode)	{
		 case 0:
			return " Distance";
		 case 1:
			return " Time";
		 default:
			return " Time";
	  }
	}
   if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   switch (continiousMode)	{
		 case 0:			
			m2_SetRoot(&list_element_continiousDistanceForm);
		 break;
		 case 1:
			m2_SetRoot(&list_element_continiousTimeForm);
		 break;	
		 default:
//			FlexiTimer2::set(1000, spdIndicator); // 500ms period
//			FlexiTimer2::start();
			
			set_state(BLINK_CONTINIOUS_CURRENT_MAX_SPEED_LCD);
			m2_SetRoot(&list_element_continiousTimeSpdControlForm);	
		break;
	}
   }

  
  return "";
}



const char *continious_execute(uint8_t idx, uint8_t msg)
{  
     if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   if (continiousVelocity>getMaxLinearSpeed())	
	   {
		 m2_SetRoot(&list_element_MaxSpeedExceeded);	
       }
	   else
	   {
		   set_state(START_STEPPER_CONTINIOUS);
		   m2_SetRoot(&list_element_testPositionForm);
	   }
	}

  
  return "";
}

const char *timelapse_execute(uint8_t idx, uint8_t msg)
{  
     if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   if (timelapseVelocity>getMaxLinearSpeed())	
	   {
		 m2_SetRoot(&list_element_MaxSpeedExceeded);	
       }
	   else if(timelapseFrmTime<timelapseMinFrmTime)
	   {
		 m2_SetRoot(&list_element_MinFrameTimeError);
	   }
	   else
	   {
//		   Serial.println("!");
		   match_timelapse_parameters();
		   prepareStepperParameters(MODE_TIMELAPSE);
		   set_state(MAKE_FRAME);
		   m2_SetRoot(&list_element_testPositionForm);
	   }
	}

  
  return "";
}



const char *xmenu_timelapse_value2(uint8_t idx, uint8_t msg)
{  
  if ( msg == M2_STRLIST_MSG_GET_STR ) {
	  switch (timelapseMode)	{
		 case 0:
			return " Distance";
		
		 default:
			 return " Offcet";
	  }
	}
   if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   switch (timelapseMode)	{
		 case 0:
			set_state(BLINK_TIMELAPSE_CURRENT_MIN_FRM_TIME);
			m2_SetRoot(&list_element_timelapseDistanceForm);
		 break;
		 default:
			set_state(BLINK_TIMELAPSE_CURRENT_MAX_SPEED_LCD);
			m2_SetRoot(&list_element_timelapseOffcetForm);	
		 break;
	  }
	}

  
  return "";
}

const char *xmenu_timelapse_value3(uint8_t idx, uint8_t msg)
{  
  if ( msg == M2_STRLIST_MSG_GET_STR ) {
	  switch (timelapseMode)	{
		 case 0:
			 return " # Frames";
		
		 default:
			 return " Frame Int";
	  }
	}
   if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   switch (timelapseMode)	{
		 case 0:
			set_state(BLINK_TIMELAPSE_CURRENT_MIN_FRM_TIME);
			m2_SetRoot(&list_element_timelapseFrmsForm);
		 break;
		 default:
			set_state(BLINK_TIMELAPSE_CURRENT_MAX_SPEED_LCD);
			m2_SetRoot(&list_element_timelapseFrmIntForm);
		 break;
	  }
	}
  
  return "";
}

const char *xmenu_timelapse_value4(uint8_t idx, uint8_t msg)
{  
    if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   switch (timelapseMode)	{
		 case 0:
			set_state(BLINK_TIMELAPSE_CURRENT_MIN_FRM_TIME);
			m2_SetRoot(&list_element_timelapseFrmTimeIndicator);
		 break;
		 default:
			m2_SetRoot(&list_element_timelapseTimeForm);
		 break;
	  }
	}

  
  return "";
}

const char *xmenu_timelapse_value5(uint8_t idx, uint8_t msg)
{  
   if ( msg == M2_STRLIST_MSG_GET_STR ) {
	  switch (timelapseExposureMode)	{
		 case 0:
			 return " Camera Exp.";
		
		 default:
			 return " BULB Exp.";
	  }
	}

	if ( msg == M2_STRLIST_MSG_SELECT ){
		m2_Clear();
	   switch (timelapseExposureMode)	{
		 case 0:
			
			m2_SetRoot(&list_element_timelapseCameraExp);
		 break;
		 default:
			
			m2_SetRoot(&list_element_timelapseBULBExp);
		 break;
	  }
	}
 
  return "";
}