#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "OrbitOLEDInterface.h"
#include "utils/ustdlib.h"  // usprintf()



// Initialise display
void display_init(void);

void display_update_yaw(int yaw);

// Update altitude reading on OLED display
void display_update_alt(int altitude);


#endif /* DISPLAY_H_ */
