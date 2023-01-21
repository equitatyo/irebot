#ifndef _CLICKS_H_
#define _CLICKS_H_

#include "dr_wav.h"
#include "macro.h"

typedef struct _Clicks {
    int count;
    int start;
    drwav* files;
} Clicks;

Clicks** load_clicks(char* path);
int generate_clicks(char* path, Macro* macro, Clicks** player1, Clicks** player2, float lsoft_clicks, float nsoft_clicks, float volume1, float volume2);
void unload_clicks(Clicks** files);

#endif //_CLICKS_H_