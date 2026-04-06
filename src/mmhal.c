#include "mmhal.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#ifndef CNC_VERSION
#define CNC_VERSION 2
#endif

// Pin numbers arrays
const int step_pins[] = {XSTEP_PIN, YSTEP_PIN, ZSTEP_PIN};
const int dir_pins[] = {XDIR_PIN, YDIR_PIN, ZDIR_PIN};

// Multipliers for each axis, dealing with assymetric stepper directions
#if CNC_VERSION == 1
const int stepper_multipliers[] = {-1, 1, -1};
#elif CNC_VERSION == 2
const int stepper_multipliers[] = {1, -1, 1};
#else
#error "Invalid CNC_VERSION"
#endif

volatile int mmhal_high_delay_us = 2000; // Microseconds for step pulse high time
volatile int mmhal_low_delay_us = 100;   // Microseconds for step pulse low time

/**
 * @brief Initialize GPIO pins and PWM for spindle control
 */
void mmhal_init() //Initialises GPIO pins
{
  
//Initialise the pins for X, Y & Z steppers, using the step_pins and dir_pins arrays
  for (int i = 0; i < DIMCOUNT; i++) 
  {
    gpio_init(step_pins[i]);
    gpio_set_dir(step_pins[i], GPIO_OUT);
    gpio_put(step_pins[i], 0); 

    gpio_init(dir_pins[i]);
    gpio_set_dir(dir_pins[i], GPIO_OUT);
    gpio_put(dir_pins[i], 0);
  }


// Initialize spindle PWM
  gpio_set_function(SPINDLE_PIN, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(SPINDLE_PIN);
  uint channel = pwm_gpio_to_channel(SPINDLE_PIN);
}

void mmhal_set_spindle_pwm(uint16_t pwm_level)
{
  // Implement spindle PWM setting
  uint pwm_slice_num = pwm_gpio_to_slice_num(SPINDLE_PIN);
  uint channel = pwm_gpio_to_channel(SPINDLE_PIN);

  pwm_set_chan_level(pwm_slice_num, channel, pwm_level);
} 

// Implement microstepping mode setting
void mmhal_set_microstepping(int x_or_y, mmhal_microstep_mode_t mode)
{
  int mode0_pin; 
  int mode1_pin;
  int mode2_pin;

  if (x_or_y == 0) // X axis
  {
    mode0_pin = X_MODE0_PIN;
    mode1_pin = X_MODE1_PIN;
    mode2_pin = X_MODE2_PIN;
  }
  else // Y axis
  {
    mode0_pin = Y_MODE0_PIN;
    mode1_pin = Y_MODE1_PIN;
    mode2_pin = Y_MODE2_PIN;
  }

  int b0 = 0;
  int b1 = 0;
  int b2 = 0;

  //Switch Code goes here


  gpio_put(mode0_pin, b0);
  gpio_put(mode1_pin, b1);
  gpio_put(mode2_pin, b2);
}

/**
 * @brief Run motors in specified directions
 * @param dirs Array of 3 directions: -1 for negative,
 * 0 for no movement, 1 for positive
 */

// Implement motor stepping logic, using the dirs array
void mmhal_step_motors_impl(int dirs[])
{
  // determine which motors to step and in which direction
  for(int i = 0; i < DIMCOUNT; i++)
  {
    if (dirs[i] != 0) 
    {
      // Remember to use the stepper_multipliers array to handle asymmetric stepper directions
      int effective_dir = dirs[i] * stepper_multipliers[i];
      gpio_put(dir_pins[i], (effective_dir > 0) ? 1 : 0);
    }
  }


//Implement the timing for the step pulses, using mmhal_high_delay_us and mmhal_low_delay_us for the pulse timing
  for (int i = 0; i < DIMCOUNT; i++)
    {
        if (dirs[i] != 0)
        {
            gpio_put(step_pins[i], 1);
        }
    }

  sleep_us(mmhal_high_delay_us);

  for (int i = 0; i < DIMCOUNT; i++)
  {
    if (dirs[i] != 0)
    {
      gpio_put(step_pins[i], 0);
    }
  }

  sleep_us(mmhal_low_delay_us);
}

void mmhal_step_motors(int x_dir, int y_dir, int z_dir)
{
  int dirs[3] = {x_dir, y_dir, z_dir};
  mmhal_step_motors_impl(dirs);
}
