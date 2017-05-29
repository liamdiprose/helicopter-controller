#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdint.h""
#include "OrbitOLEDInterface.h"
#include "utils/ustdlib.h"  // usprintf()



// Initialise display
void display_init(void);

// Update altitude reading on OLED display
void display_update_alt(int32_t current, int32_t target);


void display_update_yaw(int32_t current, int32_t target);


#endif /* DISPLAY_H_ */
