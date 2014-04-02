#ifndef __SHISHELL_H__
#define __SHISHELL_H__

/* input mode */
typedef enum imode {
    INMODE_NORMAL = 0,
    INMODE_INSERT = 1
} imode;

/* execution monde */
typedef enum emode {
    EXMODE_NORMAL = 0,
    EXMODE_BACKGR = 1
} emode;

int shishell(char* e_input);

int shiexec(char** e_input_arr, emode e_mode);

#endif /* __SHISHELL_H__ */
