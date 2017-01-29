#ifndef SCREEN_H_
#define SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

void screen_init();
void screen_update();

bool screen_button_pushed();

void screen_enterwait(const char *message);
void screen_leavewait();
void screen_entermenu();
void screen_draw();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SCREEN_H_ */
