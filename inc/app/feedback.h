#pragma once

#include <stdbool.h>
#include <stdint.h>

bool feedback_initialize(void);
void display_successful_start_up_feedback(void);
void display_vote_recorded_feedback(void);
void display_successful_update_feedback(void);
void display_failure_feedback(void);
void display_fun_feedback(void);
