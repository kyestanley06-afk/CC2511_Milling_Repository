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

  while (true)
  {
    //  Repeated code here
    if (manual_mode)
    {
      // handle_manual_mode();
    }
    else
    {
      // handle_command_mode();
    }
  }
}
