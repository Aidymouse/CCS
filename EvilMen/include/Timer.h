#ifndef _TIMER_H_
#define _TIMER_H_

typedef struct Timer {
	char active;
	float time_remaining;
	float default_time;
} Timer;

float timer_tick(Timer *timer, float tick);
void timer_stop(Timer *timer);
void timer_resume(Timer *timer):
void timer_reset(Timer *timer);
void timer_set(Timer *timer, float new_time);

#ifdef _TIMER_IMPLEMENTATION_

float timer_tick(Timer *timer, float tick) {
	if (timer->active == 1) {
		timer->time_remaining -= tick;
	}	
	return timer->time_remaining;
}

void timer_stop(Timer *timer) {
	timer->active = 0;
}

void timer_resume(Timer *timer) {
	timer->active = 1;
}

void timer_reset(Timer *timer) {
	timer->time_remaining = timer->default_time;
}

void timer_set(Timer *timer, float new_time) {
	timer->time_remaining = new_time;
}

void t

#endif

#endif
