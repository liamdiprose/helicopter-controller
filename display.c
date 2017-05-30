
#include "display.h"

// Initialise display
void display_init(void) {
	OLEDInitialise();
}

//// Update yaw reading on OLED display
//void display_update_yaw(int yaw) {
//	char line[24];
//	usprintf(line, "yaw:%d-24s", yaw);
//	OLEDStringDraw(line, 0, 0);
//}

// Update altitude reading on OLED display
void display_update(int32_t current_alt, int32_t target_alt, int32_t current_yaw, int32_t target_yaw) {
	char line[24];
	usprintf(line, "Alt: %d [%d]    \0", current_alt, target_alt);
	OLEDStringDraw(line, 3, 0);
	usprintf(line, "Yaw: %d [%d]    \0", current_yaw, target_yaw);
	OLEDStringDraw(line, 3, 1);
}
