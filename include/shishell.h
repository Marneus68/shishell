#ifndef __SHISHELL_H__
#define __SHISHELL_H__

typedef enum emode {
    EXMODE_NORMAL = 0,
    EXMODE_BACKGR = 1
} emode;

int shishell(char* e_input);

int shiexec(char** e_input_arr, emode e_mode);

#endif /* __SHISHELL_H__ */
