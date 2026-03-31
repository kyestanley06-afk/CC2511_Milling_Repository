/**************************************************************
 * main.c
 * rev 1.0 13-Dec-2025 Bruce
 * milling_basic
 * ***********************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h> // printf
#include "hardware/pwm.h"
#include "mmhal.h"

bool manual_mode = true; // Start in manual mode for testing

int main(void)
{
  // Initialise components and variables
  stdio_init_all();
  mmhal_init();

  mmhal_set_spindle_pwm(128);
  sleep_ms(1000);
  mmhal_set_spindle_pwm(0);

  while (true)
  {
    mmhal_step_motors(1, 0, 0); // Step X axis in positive direction
    sleep_ms(200);
    mmhal_step_motors(-1, 0, 0);// Step X axis in negative direction
    sleep_ms(200);
    mmhal_step_motors(0, 1, 0); // Step Y axis in positive direction
    sleep_ms(200);
    mmhal_step_motors(0, -1, 0); // Step Y axis in negative direction
    sleep_ms(200);
    
    //  Repeated code here
    //if (manual_mode)
    //{
      // handle_manual_mode();
    //}
    //else
    //{
      // handle_command_mode();
    //}
  }

  return 0;
}
