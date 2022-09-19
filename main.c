#include "mcp/syscalls.h"
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_a2560_disp.h"

int main() {
	lv_init();
	lv_a2560_disp_init();
	lv_demo_widgets();
	
	while(1) {
		/* Periodically call the lv_task handler.
		* It could be done in a timer interrupt or an OS task too.*/
		lv_timer_handler();
		long delay = sys_time_jiffies() + 5000;
		while (delay > sys_time_jiffies());
	}

	return 0;
}

void _Stub_assert() {
	// NO-OP
}
