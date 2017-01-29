#ifndef CARD_H_
#define CARD_H_

#ifdef __cplusplus
extern "C" {
#endif

bool card_init();

void card_ls_root();
void card_cat_file(const char *file);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CARD_H_ */
