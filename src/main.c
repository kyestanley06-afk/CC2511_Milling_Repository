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

typedef enum{
  MODE_MANUAL = 0,
  MODE_COMMAND = 1,
} machine_mode_t;

type def struct{
  float x;
  float y;
  float z;

  float home_x;
  float home_y;
  float home_z;

  int spindle_pwm;
  bool spindle_on;

  int absolute_mode; // 0 for relative, 1 for absolute
  int units_mm;
  float feed_rate;
  machine_mode_t mode;

}machine_state_t;


// Utility functions//

void uppercase_string(char* str)
{
  //convert string to uppercase
}

void clear_gcode_paramamters(g_code_params_t *params)
{
  // Reset G-code parameters
}


void print_manual_help(void)
{
  // Will print manual code control 
}

void print_command_help(void)
{
  // Will print the list of supported G-code commands
}

void print_machine_status(const machine_state_t *machine)
{
  // Print current machine status
}


//Machine State functions//

void machine_init(machine_state_t *machine)
{
  // Initialize machine state
}

// Hardware facing helpers functions //

void spindle_on(machine_state_t *machine)
{
  // Turn on spindle
}

void step_axis(int axis, int dir, int feed_rate)
{
  // Step the specified axis by the given distance at the specified feed rate
}

float(convert_to_mm(float value, bool units_mm))
{
  // Convert value to mm if units are in inches
}

// Motion Helpers //

void move_relative(machine_state_t *machine, float dx, float dy, float dz)
{
  // Move machine relative to current position
}

void move_absolute(machine_state_t *machine, float x, float y, float z)
{
  // Move machine to absolute position
}

void set_home(machine_state_t *machine)
{
  // Set current position as home
}

void return_home(machine_state_t *machine)
{
  // Move machine to home position
}

//Manual Mode//

void handle_manual_key(machine_state_t *machine, char key)
{
  // Handle manual control key input
}


//G-code parsing//


//Command Mode//


//Main User Interface loop//