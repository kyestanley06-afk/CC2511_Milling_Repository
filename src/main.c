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
  params->has_x = false;
  params->has_y = false;
  params->has_z = false;
  params->has_f = false;
  params->has_s = false;
  params->has_p = false;

  params->x = 0.0f;
  params->y = 0.0f;
  params->z = 0.0f;
  params->f = 0.0f;
  params->s = 0.0f;
  params->p = 0.0f; 
}

void print_manual_help(void)  // Will print manual code control 
{
  printf("\r\n=== MANUAL MODE ===\r\n");
  printf("a = left  (-X)\r\n");
  printf("d = right (+X)\r\n");
  printf("w = forward (+Y)\r\n");
  printf("s = back    (-Y)\r\n");
  printf("r = up    (+Z)\r\n");
  printf("f = down  (-Z)\r\n");
  printf("+ = spindle PWM up\r\n");
  printf("- = spindle PWM down\r\n");
  printf("o = spindle on\r\n");
  printf("p = spindle off\r\n");
  printf("c = switch to command mode\r\n");
  printf("h = show help\r\n");
  printf("===================\r\n");
}

void print_command_help(void) // Will print the list of supported G-code commands
{
  printf("\r\n=== COMMAND MODE ===\r\n");
  printf("Supported commands:\r\n");
  printf("M2 / M02           -> manual mode\r\n");
  printf("G0 / G00 X Y Z     -> rapid move\r\n");
  printf("G1 / G01 X Y Z F   -> linear move\r\n");
  printf("G4 / G04 P         -> dwell\r\n");
  printf("G28                -> return home\r\n");
  printf("G28.1              -> set home here\r\n");
  printf("G90                -> absolute mode\r\n");
  printf("G91                -> relative mode\r\n");
  printf("G20                -> inches\r\n");
  printf("G21                -> millimetres\r\n");
  printf("M3 Snnn            -> spindle on\r\n");
  printf("M5                 -> spindle off\r\n");
  printf("====================\r\n");
}

void print_machine_status(const machine_state_t *machine)   // Print current machine status
{
  printf("POS X=%.2f Y=%.2f Z=%.2f | ", machine->x, machine->y, machine->z);
  printf("HOME X=%.2f Y=%.2f Z=%.2f | ", machine->home_x, machine->home_y, machine->home_z);
  printf("MODE=%s | ", machine->mode == MODE_MANUAL ? "MANUAL" : "COMMAND");
  printf("COORD=%s | ", machine->absolute_mode ? "ABS" : "REL");
  printf("UNITS=%s | ", machine->units_mm ? "MM" : "INCH");
  printf("SPINDLE=%s PWM=%d\r\n", machine->spindle_on ? "ON" : "OFF", machine->spindle_pwm);
}


//Machine State functions//

void machine_init(machine_state_t *machine)   // Initialize machine state
{
  machine->x = 0.0f;
  machine->y = 0.0f;
  machine->z = 0.0f;

  machine->home_x = 0.0f;
  machine->home_y = 0.0f;
  machine->home_z = 0.0f;

  machine->absolute_mode = true;    // G90 default
  machine->units_mm = true;         // G21 default
  machine->feed_rate = DEFAULT_FEED_RATE;

  machine->spindle_on = false;
  machine->spindle_pwm = DEFAULT_SPINDLE_PWM;

  machine->mode = MODE_MANUAL;
}

// Hardware facing helpers functions //

void spindle_on(const machine_state_t *machine)
{
    if (machine->spindle_on)
    {
        mmhal_set_spindle_pwm((uint16_t)machine->spindle_pwm);
    }
    else
    {
        mmhal_set_spindle_pwm(0);
    }
}

void step_axis(int axis, int dir, int feed_rate) // Step the specified axis by the given distance at the specified feed rate
{
  for (int i = 0; i < feed_rate; i++)
  {
    if (axis == XDIM)
      {
        mmhal_step_motors(dir, 0, 0);
      }
    else if (axis == YDIM)
      {
        mmhal_step_motors(0, dir, 0);
      }
    else if (axis == ZDIM)
      {
         mmhal_step_motors(0, 0, dir);
      }
  }
}

float(convert_to_mm(float value, bool units_mm))   // Convert value to mm if units are in inches
{
  if (units_mm)
  {
    return value;
  }
  return value * 25.4f;
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