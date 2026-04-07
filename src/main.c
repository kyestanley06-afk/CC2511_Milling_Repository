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

#define STEPS_PER_UNIT 10.0f
#define MANUAL_STEP_UNITS 1.0f
#define DEFAULT_FEED_RATE 100.0f
#define DEFAULT_SPINDLE_PWM 128
#define MAX_LINE_LENGTH 100

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

void move_relative(machine_state_t *machine, float dx, float dy, float dz)   // Move machine relative to current position
{
  int x_steps = (int)(dx * STEPS_PER_UNIT);
  int y_steps = (int)(dy * STEPS_PER_UNIT);
  int z_steps = (int)(dz * STEPS_PER_UNIT);

    if (x_steps > 0)
    { 
        step_axis(XDIM, 1, x_steps);
    }
    else if (x_steps < 0)
    {
        step_axis(XDIM, -1, -x_steps);
    }

    if (y_steps > 0)
    {
        step_axis(YDIM, 1, y_steps);
    }
    else if (y_steps < 0)
    {
        step_axis(YDIM, -1, -y_steps);
    }

    if (z_steps > 0)
    {
        step_axis(ZDIM, 1, z_steps);
    }
    else if (z_steps < 0)
    {
        step_axis(ZDIM, -1, -z_steps);
    }

    machine->x += dx;
    machine->y += dy;
    machine->z += dz;
}

void move_absolute(machine_state_t *machine, float target_x, float target_y, float target_z) // Move machine to absolute position
{
  float dx = target_x - machine->x;
  float dy = target_y - machine->y;
  float dz = target_z - machine->z;

  move_relative(machine, dx, dy, dz);
}

void set_home(machine_state_t *machine)   // Set current position as home
{
  machine->home_x = machine->x;
  machine->home_y = machine->y;
  machine->home_z = machine->z;
}

void return_home(machine_state_t *machine) // Move machine to home position
{
  move_absolute(machine, machine->home_x, machine->home_y, machine->home_z);
}

//Manual Mode//

void handle_manual_key(machine_state_t *machine, char key)   // Handle manual control key input
{
  float step = MANUAL_STEP_UNITS;

  switch (key)
  {
    case 'a':
    case 'A':
        move_relative(machine, -step, 0.0f, 0.0f);
        printf("Moved left\r\n");
        break;

    case 'd':
    case 'D':
        move_relative(machine, step, 0.0f, 0.0f);
        printf("Moved right\r\n");
        break;

    case 'w':
    case 'W':
        move_relative(machine, 0.0f, step, 0.0f);
        printf("Moved forward\r\n");
        break;

    case 's':
    case 'S':
        move_relative(machine, 0.0f, -step, 0.0f);
        printf("Moved back\r\n");
        break;

    case 'r':
    case 'R':
        move_relative(machine, 0.0f, 0.0f, step);
        printf("Moved up\r\n");
        break;

    case 'f':
    case 'F':
        move_relative(machine, 0.0f, 0.0f, -step);
        printf("Moved down\r\n");
        break;

    case '+':
        machine->spindle_pwm += 10;
        if (machine->spindle_pwm > 255)
        {
          machine->spindle_pwm = 255;
        }
        spindle_on(machine);
        printf("Spindle PWM = %d\r\n", machine->spindle_pwm);
        break;

    case '-':
        machine->spindle_pwm -= 10;
        if (machine->spindle_pwm < 0)
        {
          machine->spindle_pwm = 0;
        }
        spindle_on(machine);
        printf("Spindle PWM = %d\r\n", machine->spindle_pwm);
        break;

    case 'o':
    case 'O':
        machine->spindle_on = true;
        spindle_on(machine);
        printf("Spindle ON\r\n");
        break;

    case 'p':
    case 'P':
        machine->spindle_on = false;
        spindle_on(machine);
        printf("Spindle OFF\r\n");
        break;

    case 'c':
    case 'C':
        machine->mode = MODE_COMMAND;
        printf("\r\nSwitched to command mode\r\n");
        print_command_help();
        break;

    case 'h':
    case 'H':
        print_manual_help();
        break;

    default:
        break;
    }

    print_machine_status(machine);
}

//G-code parsing//
void parse_gcode_params(const char* line, g_code_params_t *params) // Parse a line of G-code and populate the params structure
{
  clear_gcode_params(params);

  char temp[MAX_LINE_LENGTH];
  strncpy(temp, line, MAX_LINE_LENGTH - 1);
  temp[MAX_LINE_LENGTH - 1] = '\0';

  char *token = strtok(temp, " ");
  bool first_token = true;

  while (token != NULL)
  {
    if (first_token)
      {
        first_token = false;
      }
    else
      {
        char prefix = token[0];
        float value = (float)atof(&token[1]);

          switch (prefix)
          {
            case 'X':
                params->has_x = true;
                params->x = value;
                break;
            case 'Y':
                params->has_y = true;
                params->y = value;
                break;
            
            case 'Z':
                params->has_z = true;
                params->z = value;
                break;
                
            case 'F':
                params->has_f = true;
                params->f = value;
                break;
                
            case 'S':
                params->has_s = true;
                params->s = value;
                break;
                
            case 'P':
                params->has_p = true;
                params->p = value;
                break;

            default:
                break;
            }
        }

        token = strtok(NULL, " ");
    }
}


//G-code execution//
void execute_move_position(machine_state_t *machine, const g_code_params_t *params)   // Execute a move to the specified position based on G-code parameters
{
  float target_x = machine->x;
  float target_y = machine->y;
  float target_z = machine->z;

  if (machine->absolute_mode)
  {
    if (params->has_x) target_x = convert_to_mm(params->x, machine->units_mm);
    if (params->has_y) target_y = convert_to_mm(params->y, machine->units_mm);
    if (params->has_z) target_z = convert_to_mm(params->z, machine->units_mm);
  }
  else
  {
    if (params->has_x) target_x += convert_to_mm(params->x, machine->units_mm);
    if (params->has_y) target_y += convert_to_mm(params->y, machine->units_mm);
    if (params->has_z) target_z += convert_to_mm(params->z, machine->units_mm);
  }

  move_absolute(machine, target_x, target_y, target_z);
}

void execute_g0(machine_state_t *machine, const g_code_params_t *params)
{
  execute_position_move(machine, params);
  printf("G0 complete\r\n");
  print_machine_status(machine);
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

  sleep_ms(2000);

  printf("\r\nCC2511 Milling Machine Controller\r\n");
  printf("Text-based terminal UI running on Pi Pico\r\n");
  printf("Manual mode active\r\n");
  print_manual_help();
  print_machine_status(&machine);

  while (true) {
    if (machine.mode == MODE_MANUAL) { // Handle manual mode input
      int ch = getchar_timeout_us(0);
          if (ch != PICO_ERROR_TIMEOUT)
          {
              handle_manual_key((char)ch, &machine);
          }
    }
    else  // Handle command mode input
    {      
      char line[MAX_LINE_LENGTH];
      printf("CMD> ");
      read_line(line, sizeof(line));
      handle_gcode_line(line, &machine);
    }
  }

  return 0;
}