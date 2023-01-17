#ifndef _MACRO_H_
#define _MACRO_H_

typedef struct _Action {
    int frame;
    int player1_holding; //bool
    int player2_holding; //bool
} Action;

typedef struct _Macro {
    int fps;
    Action* actions;
    int action_num;
} Macro;

Macro* parse_txt(char* path);
void Macro_free(Macro* macro);

#endif //_MACRO_H_