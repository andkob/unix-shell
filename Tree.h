#ifndef TREE_H
#define TREE_H

typedef struct T_sequence *T_sequence;
typedef struct T_pipeline *T_pipeline;
typedef struct T_command  *T_command;
typedef struct T_words    *T_words;
typedef struct T_word     *T_word;

struct T_sequence {
  T_pipeline pipeline;
  char *op;			/* ; or & */
  T_sequence sequence;
};

struct T_pipeline {
  T_command command;
  T_pipeline pipeline;
};

struct T_command {
  T_words words;
  char *filein;
  char *fileout;
};

struct T_words {
  T_word word;
  T_words words;
};

struct T_word {
  char *s;
};

/**
 * Allocate and zero-initialize a new T_sequence node.
 *
 * @return Pointer to a heap-allocated T_sequence.
 */
extern T_sequence new_sequence();

/**
 * Allocate and zero-initialize a new T_pipeline node.
 *
 * @return Pointer to a heap-allocated T_pipeline.
 */
extern T_pipeline new_pipeline();

/**
 * Allocate and zero-initialize a new T_command node.
 *
 * @return Pointer to a heap-allocated T_command.
 */
extern T_command  new_command();

/**
 * Allocate and zero-initialize a new T_words node.
 *
 * @return Pointer to a heap-allocated T_words.
 */
extern T_words    new_words();

/**
 * Allocate and zero-initialize a new T_word node.
 *
 * @return Pointer to a heap-allocated T_word.
 */
extern T_word     new_word();

#endif
