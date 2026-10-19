/* 
    LEXER.HPP SMALLAB28 
*/ 


#include <iostream>

typedef struct rz_lexer rz_lexer;

typedef enum{
    NORMAL,
    WORD,
    NUMBER,
    ARROW,
}rz_lexer_state;

rz_lexer_state lexer(char *buffer);


struct lexer{
    rz_lexer_state lexer_state;
};
