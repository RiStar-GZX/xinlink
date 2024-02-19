#include <dsp.h>
#include <core.h>
#include <usbip.h>
#include <moonlight.h>

int dsp_init(void){
    usbip_init();
    //moonlight_init();
    return 1;
}
