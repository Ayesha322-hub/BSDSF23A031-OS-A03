#ifndef IF_THEN_H
#define IF_THEN_H

// Read and handle a multi-line if-then-else-fi block.
// first_line is the line that started with "if" (it may contain the condition or
// just "if" if the user pressed Enter after typing "if").
void handle_if_block(char *first_line);

#endif // IF_THEN_H
