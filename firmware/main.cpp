/**************************************************
*                                                 *
*            Safecast Geiger Counter              *
*                                                 *
**************************************************/

#include "wirish_boards.h"
#include "power.h"
#include "safecast_config.h"

//#define DISABLE_ACCEL
#include "UserInput.h"
#include "Geiger.h"
#include "GUI.h"
#include "Controller.h"
#include <stdint.h>
#include "flashstorage.h"
#include "rtc.h"
#include "accel.h"
#include "realtime.h"
#include "serialinterface.h"
#include "switch.h"
#include "buzzer.h"
#include <stdio.h>
#include <string.h>

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void
premain() {
  gpio_init_all();
  init();
  delay_us(100000);
}

int main(void) {

    Geiger g;
    power_initialise();
    flashstorage_initialise();
    display_initialise();
    buzzer_initialise();
    realtime_initialise();
    g.initialise();


    delay_us(10000);  // can be removed?

    #ifndef DISABLE_ACCEL
    accel_init();
    #endif

    Controller c(g);
    switch_initialise();

    // if we woke up on an alarm, we're going to be sending the system back.
    if(power_get_wakeup_source() == WAKEUP_RTC) {
      c.m_sleeping = true;
    } else {
      buzzer_nonblocking_buzz(1);
    }

    GUI m_gui(c);
    c.set_gui(m_gui);
    UserInput  u(m_gui);
    u.initialise();
    serial_initialise();
  

    // Need to refactor out stored settings
    if(c.m_sleeping == false) {   
      const char *sbright = flashstorage_keyval_get("BRIGHTNESS");
      if(sbright != 0) {
        unsigned int c;
        sscanf(sbright, "%u", &c);
        display_brightness(c+6);
      }
 
      const char *sbeep = flashstorage_keyval_get("GEIGERBEEP");
      if(sbeep != 0) {
        if(strcmp(sbeep,"true") == 0) g.set_beep(true);
                                 else g.set_beep(false);
      }

      const char *language = flashstorage_keyval_get("LANGUAGE");
      if(language != 0) {
        if(strcmp(language,"English" ) == 0) m_gui.set_language(LANGUAGE_ENGLISH);
        if(strcmp(language,"Japanese") == 0) m_gui.set_language(LANGUAGE_JAPANESE);
      }
    }


    m_gui.jump_to_screen(1);
    m_gui.push_stack(0,1);
    for(;;) {
      c.update();
      m_gui.render();

      serial_eventloop();

      // It might be a good idea to move the following code to Controller.
      // Hack to check that captouch is ok, and reset it if not.
      bool c = cap_check();
      if(c == false) {
        display_draw_text(0,90,"CAPFAIL",0);
        cap_init(); 
      }

      // Screen lock code
      uint32_t release_time = cap_last_press();
      uint32_t   press_time = cap_last_release();
      uint32_t current_time = realtime_get_unixtime();
      if((release_time != 0) && (release_time < press_time) && ((current_time-press_time) > 3)) {
        system_gui->toggle_screen_lock();
        cap_clear_press();
      }

      power_wfi();
    }

    // should never get here
    for(int n=0;n<60;n++) {
      delay_us(100000);
      buzzer_blocking_buzz(1000);
    }
    return 0;
}
