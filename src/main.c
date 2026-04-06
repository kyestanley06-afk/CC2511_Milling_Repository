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

typedef struct{
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

void uppercase_string(char* str) //Converts a string to uppercase
{
  for (int i = 0; strt[i] != '\0'; i++)
    {
        text[i] = (char)toupper((unsigned char)text[i]);
    }
}

void clear_gcode_paramamters(g_code_params_t *params) // Reset G-code parameters
{

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
void parse_gcode_params(const char* line, g_code_params_t *params)
{
  // Parse a line of G-code and populate the params structure
}


//G-code execution//
void execute_move_position(machine_state_t *machine, const g_code_params_t *params)
{
  // Execute a move to the specified position based on G-code parameters
}

void execute_g0(machine_state_t *machine, const g_code_params_t *params)
{
  // Execute G00
}

void execute_g1(machine_state_t *machine, const g_code_params_t *params)
{
  // Execute G01
}

void handle_gcode_line(char* line, machine_state_t *machine)
{
  // Handle a line of G-code command
}

//Main User Interface loop//
int main(void)
{
  stdio_init_all();
  machine_state_t machine;
  machine_init(&machine);

  while (true) {
    if (machine.mode == MODE_MANUAL) {
      // Handle manual mode input
    } else if (machine.mode == MODE_COMMAND) {
      // Handle command mode input
    }
  }

  return 0;
}