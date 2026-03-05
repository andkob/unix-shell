#ifndef SCANNER_H
#define SCANNER_H

typedef void *Scanner;

/**
 * Create a new Scanner that tokenizes the given string by whitespace.
 *
 * @param s Null-terminated input string (a copy is made internally).
 * @return  Opaque Scanner handle.
 */
extern Scanner newScanner(char *s);

/**
 * Free all memory associated with a Scanner.
 *
 * @param scan The Scanner to free.
 */
extern void freeScanner(Scanner scan);

/**
 * Advance to the next whitespace-delimited token and return it.
 *
 * @param scan The Scanner.
 * @return     The next token string, or NULL if input is exhausted.
 */
extern char *nextScanner(Scanner scan);

/**
 * Return the current token without advancing.  If no token has been
 * read yet, automatically calls nextScanner() first.
 *
 * @param scan The Scanner.
 * @return     The current token string, or NULL if input is exhausted.
 */
extern char *currScanner(Scanner scan);

/**
 * Compare the current token to a given string.
 *
 * @param scan The Scanner.
 * @param s    The string to compare against.
 * @return     1 if the current token equals s, 0 otherwise.
 */
extern int cmpScanner(Scanner scan, char *s);

/**
 * If the current token matches s, consume it (advance) and return 1;
 * otherwise return 0 and leave the scanner position unchanged.
 *
 * @param scan The Scanner.
 * @param s    The expected token string.
 * @return     1 if matched and consumed, 0 otherwise.
 */
extern int eatScanner(Scanner scan, char *s);

/**
 * Return the current byte offset within the original input string.
 *
 * @param scan The Scanner.
 * @return     0-based character position.
 */
extern int posScanner(Scanner scan);

#endif
