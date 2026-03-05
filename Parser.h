#ifndef PARSER_H
#define PARSER_H

typedef void *Tree;

/**
 * Parse a command-line string into an abstract syntax tree representing
 * sequences, pipelines, commands, words, and I/O redirections.
 *
 * @param s  Null-terminated input string to parse.
 * @return   Opaque Tree handle (T_sequence), or NULL on empty input.
 */
extern Tree parseTree(char *s);

/**
 * Recursively free the entire parse tree and all of its children.
 *
 * @param t The Tree to free.
 */
extern void freeTree(Tree t);

#endif
