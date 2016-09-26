#include "app/feedback.h"

#include "system/timer.h"
#include "system/util.h"
#include "command.h"
#include "app/buzzer.h"
#include "app/led.h"
#include "app/nfc.h"
#include "app/votes.h"

#define FEEDBACK_REFRESH_RATE_IN_MS 100
#define MAX_LED_COLORS              3
#define TICKS_PER_COLOR             4

SYS_TIMER_DEF(feedbackTimer);

static int rainbow_led_counter = 0;
static int rainbow_led_ticker = 0;
static bool displaying_fun_feedback = false;
static bool initialization_completed = false;

static void display_general_user_feedback(void *data, uint16_t dataLength) {
  SYS_UNUSED_PARAMETER(data);
  SYS_UNUSED_PARAMETER(dataLength);

  if (buzzer_is_on()) {
    if (displaying_fun_feedback) {
      if (rainbow_led_ticker == 0) {
        flash_color(rainbow_led_counter++);
        rainbow_led_counter %= MAX_LED_COLORS;
      }
      rainbow_led_ticker++;
      rainbow_led_ticker %= TICKS_PER_COLOR;
    }
    return;// do nothing, wait for buzzer to stop
  }

  displaying_fun_feedback = false;

  if (!initialization_completed) {                    // init failed, display bright red
    led_red_bright();

  } else if (get_nfc_status() == NFC_STATUS_ERROR) {  // nfc is broken, display bright red
    led_red_bright();

  } else if (max_votes_recorded()) {                  // votes reached maximum, display bright red
    led_red_bright();

  } else if (!vote_config_is_set()) {                 // no vote config, display dim red
    led_red_dim();

  } else {                                            // neutral state, display dim blue
    led_blue_dim();
  }
}

bool feedback_initialize() {
  if (!execute_succeeds(create_repeated_timer(&feedbackTimer))) return false;
  if (!execute_succeeds(start_timer(&feedbackTimer,
                                    FEEDBACK_REFRESH_RATE_IN_MS,
                                    display_general_user_feedback))) return false;

  mesh_add_terminal_command("fun", "Play a song!", display_fun_feedback);

  return true;
}

void display_vote_recorded_feedback() {
  if(!initialization_completed) return;

  led_green_bright();
  uint16_t success_tones[4] = {100, 956, 200, 716};
  play_tones(success_tones, 2);
}

void display_successful_start_up_feedback() {
  led_white_bright();
  uint16_t hello_tones[8] = {200, 851, 200, 956, 200, 758, 300, 716};
  play_tones(hello_tones, 4);
  initialization_completed = true;
}

void display_successful_update_feedback() {
  if(!initialization_completed) return;

  led_green_bright();
  uint16_t neutral_tones[2] = {500, 956};
  play_tones(neutral_tones, 1);
}

void display_failure_feedback() {
  led_red_bright();
  uint16_t failure_tones[4] = {200, 1073, 600, 1276};
  play_tones(failure_tones, 2);
}

void display_fun_feedback() {
  initialization_completed = true;

  displaying_fun_feedback = true;
  rainbow_led_counter = 0;
  rainbow_led_ticker = 0;
  uint16_t countdown_tones[24] = {150, 902, 150, 1012, 500, 902, 1000, 1351, 300, 0, 150, 851, 150, 902, 200, 851, 100, 0, 200, 902, 100, 0, 600, 1012};
  play_tones(countdown_tones, 12);
}
