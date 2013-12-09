#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Uno
#define __AVR_ATmega328P__
#define 
#define ARDUINO 102
#define ARDUINO_MAIN
#define __AVR__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

const char *fn_idx_to_color(uint8_t idx);
const char *fn_idx_to_timelapse_mode(uint8_t idx);
const char *fn_idx_to_timelapse_exposure_mode(uint8_t idx);
const char *fn_idx_to_shoot_signals(uint8_t idx);
//
//
void process_state(void);
void set_state(uint8_t state_id);
int read_analog_buttons();
uint8_t m2_es_arduino_analog_input(m2_p ep, uint8_t msg);
void fn_ok(m2_el_fnarg_p fnarg);
uint32_t u32_cb_Velocity(m2_rom_void_p element, uint8_t msg, uint32_t val);
uint32_t u32_cb_SafetyInt(m2_rom_void_p element, uint8_t msg, uint32_t val);
uint32_t u32_cb_timelapseBULBExp(m2_rom_void_p element, uint8_t msg, uint32_t val);
uint32_t u32_cb_timelapseCameraExp(m2_rom_void_p element, uint8_t msg, uint32_t val);
void continiousSpdIndicator();
void timelapseSpdIndicator();
void timelapseFrmTimeIndicator();
const char *xmenu_value2(uint8_t idx, uint8_t msg);
const char *xmenu_value3(uint8_t idx, uint8_t msg);
const char *continious_execute(uint8_t idx, uint8_t msg);
const char *timelapse_execute(uint8_t idx, uint8_t msg);
const char *xmenu_timelapse_value2(uint8_t idx, uint8_t msg);
const char *xmenu_timelapse_value3(uint8_t idx, uint8_t msg);
const char *xmenu_timelapse_value4(uint8_t idx, uint8_t msg);
const char *xmenu_timelapse_value5(uint8_t idx, uint8_t msg);
void init_stepper();
void prepareStepperParameters(byte mode);
float linear_to_circular();
float circular_to_linear();
uint32_t time_to_sec(byte mode);
void run();
void runSpeed();
void stop();
void getStepperPosition();
void makeFrame();
void match_timelapse_parameters();
float getCurrentOffcetInterval(void);
float getCurrentTimelapseLinearSpeed(void);
float getCurrentLinearSpeed(void);
float getFloat(uint32_t number, uint8_t precision);
uint32_t getMaxLinearSpeed(void);
void run_stepper_interrupts(byte mode);
void init_exposure(void);

#include "C:\arduino-1.0.2\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\arduino-1.0.2\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\Karmishin\Documents\Arduino\Ultra_DSLRWare\Ultra_DSLRWare.ino"
#include "C:\Users\Karmishin\Documents\Arduino\Ultra_DSLRWare\MotionControl.ino"
#endif
