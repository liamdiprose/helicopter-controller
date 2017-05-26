
#include "display.h"

// Initialise display
void display_init(void) {
	OLEDInitialise();
}

// Update yaw reading on OLED display
void display_update_yaw(int yaw) {
	char line[24];
	usprintf(line, "yaw:%d-24s", yaw);
	OLEDStringDraw(line, 0, 0);
}

// Update altitude reading on OLED display
void display_update_alt(int altitude) {
	char line[24];
	usprintf(line, "Alt: %d%%-24s", altitude);
	OLEDStringDraw(line, 0, 1);
}
