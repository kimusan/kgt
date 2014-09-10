/* Generated by lx */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include LX_HEADER

static enum lx_ebnf_token z1(struct lx_ebnf_lx *lx);
static enum lx_ebnf_token z2(struct lx_ebnf_lx *lx);
static enum lx_ebnf_token z3(struct lx_ebnf_lx *lx);
static enum lx_ebnf_token z4(struct lx_ebnf_lx *lx);

static int
lx_getc(struct lx_ebnf_lx *lx)
{
	int c;

	assert(lx != NULL);
	assert(lx->lgetc != NULL);

	if (lx->c != EOF) {
		c = lx->c, lx->c = EOF;
	} else {
		c = lx->lgetc(lx);
		if (c == EOF) {
			return EOF;
		}
	}

	lx->end.byte++;
	lx->end.col++;

	if (c == '\n') {
		lx->end.line++;
		lx->end.col = 1;
	}

	return c;
}

static void
lx_ebnf_ungetc(struct lx_ebnf_lx *lx, int c)
{
	assert(lx != NULL);
	assert(lx->c == EOF);

	lx->c = c;

	if (lx->pop != NULL) {
		lx->pop(lx);
	}

	lx->end.byte--;
	lx->end.col--;

	if (c == '\n') {
		lx->end.line--;
		lx->end.col = 0; /* XXX: lost information */
	}
}

int
lx_ebnf_fgetc(struct lx_ebnf_lx *lx)
{
	assert(lx != NULL);
	assert(lx->opaque != NULL);

	return fgetc(lx->opaque);
}

int
lx_ebnf_sgetc(struct lx_ebnf_lx *lx)
{
	char *s;

	assert(lx != NULL);
	assert(lx->opaque != NULL);

	s = lx->opaque;
	if (*s == '\0') {
		return EOF;
	}

	return lx->opaque = s + 1, *s;
}

int
lx_ebnf_agetc(struct lx_ebnf_lx *lx)
{
	struct lx_arr *a;

	assert(lx != NULL);
	assert(lx->opaque != NULL);

	a = lx->opaque;

	assert(a != NULL);
	assert(a->p != NULL);

	if (a->len == 0) {
		return EOF;
	}

	return a->len--, *a->p++;
}

int
lx_ebnf_dgetc(struct lx_ebnf_lx *lx)
{
	struct lx_fd *d;

	assert(lx != NULL);
	assert(lx->opaque != NULL);

	d = lx->opaque;
	assert(d->fd != -1);
	assert(d->p != NULL);

	if (d->len == 0) {
		ssize_t r;

		assert((fcntl(d->fd, F_GETFL) & O_NONBLOCK) == 0);

		d->p = (char *) d + sizeof *d;

		r = read(d->fd, d->p, d->bufsz);
		if (r == -1) {
			assert(errno != EAGAIN);
			return EOF;
		}

		if (r == 0) {
			return EOF;
		}

		d->len = r;
	}

	return d->len--, *d->p++;
}

int
lx_ebnf_dynpush(struct lx_ebnf_lx *lx, char c)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);
	assert(c != EOF);

	t = lx->buf;

	assert(t != NULL);

	if (t->p == t->a + t->len) {
		size_t len;
		char *tmp;

		if (t->len == 0) {
			assert(LX_DYN_LOW > 0);
			len = LX_DYN_LOW;
		} else {
			len = t->len * LX_DYN_FACTOR;
			if (len < t->len) {
				errno = ERANGE;
				return -1;
			}
		}

		tmp = realloc(t->a, len);
		if (tmp == NULL) {
			return -1;
		}

		t->p   = tmp + (t->p - t->a);
		t->a   = tmp;
		t->len = len;
	}

	assert(t->p != NULL);
	assert(t->a != NULL);

	*t->p++ = c;

	return 0;
}

void
lx_ebnf_dynpop(struct lx_ebnf_lx *lx)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);
	assert(t->a != NULL);
	assert(t->p >= t->a);

	if (t->p == t->a) {
		return;
	}

	t->p--;
}

int
lx_ebnf_dynclear(struct lx_ebnf_lx *lx)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);

	if (t->len > LX_DYN_HIGH) {
		size_t len;
		char *tmp;

		len = t->len / LX_DYN_FACTOR;

		tmp = realloc(t->a, len);
		if (tmp == NULL) {
			return -1;
		}

		t->a   = tmp;
		t->len = len;
	}

	t->p = t->a;

	return 0;
}

void
lx_ebnf_dynfree(struct lx_ebnf_lx *lx)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);

	free(t->a);
}
int
lx_ebnf_fixedpush(struct lx_ebnf_lx *lx, char c)
{
	struct lx_fixedbuf *t;

	assert(lx != NULL);
	assert(c != EOF);

	t = lx->buf;

	assert(t != NULL);
	assert(t->p != NULL);
	assert(t->a != NULL);

	if (t->p == t->a + t->len) {
		errno = ENOMEM;
		return -1;
	}

	*t->p++ = c;

	return 0;
}

void
lx_ebnf_fixedpop(struct lx_ebnf_lx *lx)
{
	struct lx_fixedbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);
	assert(t->a != NULL);
	assert(t->p >= t->a);

	if (t->p == t->a) {
		return;
	}

	t->p--;
}

int
lx_ebnf_fixedclear(struct lx_ebnf_lx *lx)
{
	struct lx_fixedbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);
	assert(t->p != NULL);
	assert(t->a != NULL);

	t->p = t->a;

	return 0;
}

static enum lx_ebnf_token
z1(struct lx_ebnf_lx *lx)
{
	int c;

	enum {
		S1, S2, S3
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S3;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1: /* e.g. "\"" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z = z4, TOK_LITERAL;
			}

		case S2: /* e.g. "a" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_CHAR;
			}

		case S3: /* start */
			switch (c) {
			case '\"': state = S1;      continue;
			default:  state = S2;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_LITERAL;
	case S2: return TOK_CHAR;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_ebnf_token
z2(struct lx_ebnf_lx *lx)
{
	int c;

	enum {
		S1, S2, S3
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S3;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1: /* e.g. "`" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z = z4, TOK_LITERAL;
			}

		case S2: /* e.g. "a" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_CHAR;
			}

		case S3: /* start */
			switch (c) {
			case '`': state = S1;      continue;
			default:  state = S2;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_LITERAL;
	case S2: return TOK_CHAR;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_ebnf_token
z3(struct lx_ebnf_lx *lx)
{
	int c;

	enum {
		S1, S2, S3, S4
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S4;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
		case S1:
		case S2:
		case S3:
		case S4:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1: /* e.g. "*)" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z = z4, lx->z(lx);
			}

		case S2: /* e.g. "*" */
			switch (c) {
			case ')': state = S1;      continue;
			default:  lx_ebnf_ungetc(lx, c); return lx->z(lx);
			}

		case S3: /* e.g. "a" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z(lx);
			}

		case S4: /* start */
			switch (c) {
			case '*': state = S2;      continue;
			default:  state = S3;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_EOF;
	case S2: return TOK_EOF;
	case S3: return TOK_EOF;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_ebnf_token
z4(struct lx_ebnf_lx *lx)
{
	int c;

	enum {
		S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, 
		S11, S12, S13, S14, S15, S16, S17, S18, S19, S20, 
		S21, S22, S23
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S23;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
		case S2:
		case S3:
		case S4:
		case S18:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1: /* e.g. "a\x09" */
			switch (c) {
			case '\t':	          continue;
			case '\n':	          continue;
			case '\v':	          continue;
			case '\f':	          continue;
			case '\r':	          continue;
			case ' ':	          continue;
			case '-': state = S19;      continue;
			case '0': state = S19;      continue;
			case '1': state = S19;      continue;
			case '2': state = S19;      continue;
			case '3': state = S19;      continue;
			case '4': state = S19;      continue;
			case '5': state = S19;      continue;
			case '6': state = S19;      continue;
			case '7': state = S19;      continue;
			case '8': state = S19;      continue;
			case '9': state = S19;      continue;
			case 'A': state = S19;      continue;
			case 'B': state = S19;      continue;
			case 'C': state = S19;      continue;
			case 'D': state = S19;      continue;
			case 'E': state = S19;      continue;
			case 'F': state = S19;      continue;
			case 'G': state = S19;      continue;
			case 'H': state = S19;      continue;
			case 'I': state = S19;      continue;
			case 'J': state = S19;      continue;
			case 'K': state = S19;      continue;
			case 'L': state = S19;      continue;
			case 'M': state = S19;      continue;
			case 'N': state = S19;      continue;
			case 'O': state = S19;      continue;
			case 'P': state = S19;      continue;
			case 'Q': state = S19;      continue;
			case 'R': state = S19;      continue;
			case 'S': state = S19;      continue;
			case 'T': state = S19;      continue;
			case 'U': state = S19;      continue;
			case 'V': state = S19;      continue;
			case 'W': state = S19;      continue;
			case 'X': state = S19;      continue;
			case 'Y': state = S19;      continue;
			case 'Z': state = S19;      continue;
			case '_': state = S19;      continue;
			case 'a': state = S19;      continue;
			case 'b': state = S19;      continue;
			case 'c': state = S19;      continue;
			case 'd': state = S19;      continue;
			case 'e': state = S19;      continue;
			case 'f': state = S19;      continue;
			case 'g': state = S19;      continue;
			case 'h': state = S19;      continue;
			case 'i': state = S19;      continue;
			case 'j': state = S19;      continue;
			case 'k': state = S19;      continue;
			case 'l': state = S19;      continue;
			case 'm': state = S19;      continue;
			case 'n': state = S19;      continue;
			case 'o': state = S19;      continue;
			case 'p': state = S19;      continue;
			case 'q': state = S19;      continue;
			case 'r': state = S19;      continue;
			case 's': state = S19;      continue;
			case 't': state = S19;      continue;
			case 'u': state = S19;      continue;
			case 'v': state = S19;      continue;
			case 'w': state = S19;      continue;
			case 'x': state = S19;      continue;
			case 'y': state = S19;      continue;
			case 'z': state = S19;      continue;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}

		case S2: /* e.g. "(*" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z = z3, lx->z(lx);
			}

		case S3: /* e.g. "\x09" */
			switch (c) {
			case '\t':	          continue;
			case '\n':	          continue;
			case '\v':	          continue;
			case '\f':	          continue;
			case '\r':	          continue;
			case ' ':	          continue;
			default:  lx_ebnf_ungetc(lx, c); return lx->z(lx);
			}

		case S4: /* e.g. "\"" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z = z1, lx->z(lx);
			}

		case S5: /* e.g. "(" */
			switch (c) {
			case '*': state = S2;      continue;
			case '/': state = S16;      continue;
			case ':': state = S20;      continue;
			default:  lx_ebnf_ungetc(lx, c); return TOK_STARTGROUP;
			}

		case S6: /* e.g. ")" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_ENDGROUP;
			}

		case S7: /* e.g. "*" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_STAR;
			}

		case S8: /* e.g. "," */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_CAT;
			}

		case S9: /* e.g. "-" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_EXCEPT;
			}

		case S10: /* e.g. "/" */
			switch (c) {
			case ')': state = S17;      continue;
			default:  lx_ebnf_ungetc(lx, c); return TOK_ALT;
			}

		case S11: /* e.g. "0" */
			switch (c) {
			case '0':	          continue;
			case '1':	          continue;
			case '2':	          continue;
			case '3':	          continue;
			case '4':	          continue;
			case '5':	          continue;
			case '6':	          continue;
			case '7':	          continue;
			case '8':	          continue;
			case '9':	          continue;
			default:  lx_ebnf_ungetc(lx, c); return TOK_COUNT;
			}

		case S12: /* e.g. ":" */
			switch (c) {
			case ')': state = S22;      continue;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}

		case S13: /* e.g. "." */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_SEP;
			}

		case S14: /* e.g. "=" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_EQUALS;
			}

		case S15: /* e.g. "?" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_OPT;
			}

		case S16: /* e.g. "[" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_STARTOPT;
			}

		case S17: /* e.g. "]" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_ENDOPT;
			}

		case S18: /* e.g. "`" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return lx->z = z2, lx->z(lx);
			}

		case S19: /* e.g. "a" */
			switch (c) {
			case '\t': state = S1;      continue;
			case '\n': state = S1;      continue;
			case '\v': state = S1;      continue;
			case '\f': state = S1;      continue;
			case '\r': state = S1;      continue;
			case ' ': state = S1;      continue;
			case '-':	          continue;
			case '0':	          continue;
			case '1':	          continue;
			case '2':	          continue;
			case '3':	          continue;
			case '4':	          continue;
			case '5':	          continue;
			case '6':	          continue;
			case '7':	          continue;
			case '8':	          continue;
			case '9':	          continue;
			case 'A':	          continue;
			case 'B':	          continue;
			case 'C':	          continue;
			case 'D':	          continue;
			case 'E':	          continue;
			case 'F':	          continue;
			case 'G':	          continue;
			case 'H':	          continue;
			case 'I':	          continue;
			case 'J':	          continue;
			case 'K':	          continue;
			case 'L':	          continue;
			case 'M':	          continue;
			case 'N':	          continue;
			case 'O':	          continue;
			case 'P':	          continue;
			case 'Q':	          continue;
			case 'R':	          continue;
			case 'S':	          continue;
			case 'T':	          continue;
			case 'U':	          continue;
			case 'V':	          continue;
			case 'W':	          continue;
			case 'X':	          continue;
			case 'Y':	          continue;
			case 'Z':	          continue;
			case '_':	          continue;
			case 'a':	          continue;
			case 'b':	          continue;
			case 'c':	          continue;
			case 'd':	          continue;
			case 'e':	          continue;
			case 'f':	          continue;
			case 'g':	          continue;
			case 'h':	          continue;
			case 'i':	          continue;
			case 'j':	          continue;
			case 'k':	          continue;
			case 'l':	          continue;
			case 'm':	          continue;
			case 'n':	          continue;
			case 'o':	          continue;
			case 'p':	          continue;
			case 'q':	          continue;
			case 'r':	          continue;
			case 's':	          continue;
			case 't':	          continue;
			case 'u':	          continue;
			case 'v':	          continue;
			case 'w':	          continue;
			case 'x':	          continue;
			case 'y':	          continue;
			case 'z':	          continue;
			default:  lx_ebnf_ungetc(lx, c); return TOK_IDENT;
			}

		case S20: /* e.g. "{" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_STARTSTAR;
			}

		case S21: /* e.g. "!" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_ALT;
			}

		case S22: /* e.g. "}" */
			switch (c) {
			default:  lx_ebnf_ungetc(lx, c); return TOK_ENDSTAR;
			}

		case S23: /* start */
			switch (c) {
			case '\t': state = S3;      continue;
			case '\n': state = S3;      continue;
			case '\v': state = S3;      continue;
			case '\f': state = S3;      continue;
			case '\r': state = S3;      continue;
			case ' ': state = S3;      continue;
			case '!': state = S21;      continue;
			case '\"': state = S4;      continue;
			case '(': state = S5;      continue;
			case ')': state = S6;      continue;
			case '*': state = S7;      continue;
			case ',': state = S8;      continue;
			case '-': state = S9;      continue;
			case '.': state = S13;      continue;
			case '/': state = S10;      continue;
			case '0': state = S11;      continue;
			case '1': state = S11;      continue;
			case '2': state = S11;      continue;
			case '3': state = S11;      continue;
			case '4': state = S11;      continue;
			case '5': state = S11;      continue;
			case '6': state = S11;      continue;
			case '7': state = S11;      continue;
			case '8': state = S11;      continue;
			case '9': state = S11;      continue;
			case ':': state = S12;      continue;
			case ';': state = S13;      continue;
			case '=': state = S14;      continue;
			case '?': state = S15;      continue;
			case 'A': state = S19;      continue;
			case 'B': state = S19;      continue;
			case 'C': state = S19;      continue;
			case 'D': state = S19;      continue;
			case 'E': state = S19;      continue;
			case 'F': state = S19;      continue;
			case 'G': state = S19;      continue;
			case 'H': state = S19;      continue;
			case 'I': state = S19;      continue;
			case 'J': state = S19;      continue;
			case 'K': state = S19;      continue;
			case 'L': state = S19;      continue;
			case 'M': state = S19;      continue;
			case 'N': state = S19;      continue;
			case 'O': state = S19;      continue;
			case 'P': state = S19;      continue;
			case 'Q': state = S19;      continue;
			case 'R': state = S19;      continue;
			case 'S': state = S19;      continue;
			case 'T': state = S19;      continue;
			case 'U': state = S19;      continue;
			case 'V': state = S19;      continue;
			case 'W': state = S19;      continue;
			case 'X': state = S19;      continue;
			case 'Y': state = S19;      continue;
			case 'Z': state = S19;      continue;
			case '[': state = S16;      continue;
			case ']': state = S17;      continue;
			case '`': state = S18;      continue;
			case 'a': state = S19;      continue;
			case 'b': state = S19;      continue;
			case 'c': state = S19;      continue;
			case 'd': state = S19;      continue;
			case 'e': state = S19;      continue;
			case 'f': state = S19;      continue;
			case 'g': state = S19;      continue;
			case 'h': state = S19;      continue;
			case 'i': state = S19;      continue;
			case 'j': state = S19;      continue;
			case 'k': state = S19;      continue;
			case 'l': state = S19;      continue;
			case 'm': state = S19;      continue;
			case 'n': state = S19;      continue;
			case 'o': state = S19;      continue;
			case 'p': state = S19;      continue;
			case 'q': state = S19;      continue;
			case 'r': state = S19;      continue;
			case 's': state = S19;      continue;
			case 't': state = S19;      continue;
			case 'u': state = S19;      continue;
			case 'v': state = S19;      continue;
			case 'w': state = S19;      continue;
			case 'x': state = S19;      continue;
			case 'y': state = S19;      continue;
			case 'z': state = S19;      continue;
			case '{': state = S20;      continue;
			case '|': state = S21;      continue;
			case '}': state = S22;      continue;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S2: return TOK_EOF;
	case S3: return TOK_EOF;
	case S4: return TOK_EOF;
	case S5: return TOK_STARTGROUP;
	case S6: return TOK_ENDGROUP;
	case S7: return TOK_STAR;
	case S8: return TOK_CAT;
	case S9: return TOK_EXCEPT;
	case S10: return TOK_ALT;
	case S11: return TOK_COUNT;
	case S13: return TOK_SEP;
	case S14: return TOK_EQUALS;
	case S15: return TOK_OPT;
	case S16: return TOK_STARTOPT;
	case S17: return TOK_ENDOPT;
	case S18: return TOK_EOF;
	case S19: return TOK_IDENT;
	case S20: return TOK_STARTSTAR;
	case S21: return TOK_ALT;
	case S22: return TOK_ENDSTAR;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

const char *
lx_ebnf_name(enum lx_ebnf_token t)
{
	switch (t) {
	case TOK_COUNT: return "COUNT";
	case TOK_IDENT: return "IDENT";
	case TOK_OPT: return "OPT";
	case TOK_STAR: return "STAR";
	case TOK_EXCEPT: return "EXCEPT";
	case TOK_ENDGROUP: return "ENDGROUP";
	case TOK_STARTGROUP: return "STARTGROUP";
	case TOK_EQUALS: return "EQUALS";
	case TOK_CAT: return "CAT";
	case TOK_ENDSTAR: return "ENDSTAR";
	case TOK_STARTSTAR: return "STARTSTAR";
	case TOK_ENDOPT: return "ENDOPT";
	case TOK_STARTOPT: return "STARTOPT";
	case TOK_SEP: return "SEP";
	case TOK_ALT: return "ALT";
	case TOK_LITERAL: return "LITERAL";
	case TOK_CHAR: return "CHAR";
	case TOK_EOF:     return "EOF";
	case TOK_ERROR:   return "ERROR";
	case TOK_UNKNOWN: return "UNKNOWN";
	default: return "?";
	}
}

const char *
lx_ebnf_example(enum lx_ebnf_token (*z)(struct lx_ebnf_lx *), enum lx_ebnf_token t)
{
	assert(z != NULL);

	if (z == z1) {
		switch (t) {
		case TOK_LITERAL: return "\"";
		case TOK_CHAR: return "a";
		default: goto error;
		}
	} else
	if (z == z2) {
		switch (t) {
		case TOK_LITERAL: return "`";
		case TOK_CHAR: return "a";
		default: goto error;
		}
	} else
	if (z == z3) {
		switch (t) {
		default: goto error;
		}
	} else
	if (z == z4) {
		switch (t) {
		case TOK_COUNT: return "0";
		case TOK_IDENT: return "a";
		case TOK_OPT: return "?";
		case TOK_STAR: return "*";
		case TOK_EXCEPT: return "-";
		case TOK_ENDGROUP: return ")";
		case TOK_STARTGROUP: return "(";
		case TOK_EQUALS: return "=";
		case TOK_CAT: return ",";
		case TOK_ENDSTAR: return "}";
		case TOK_STARTSTAR: return "{";
		case TOK_ENDOPT: return "]";
		case TOK_STARTOPT: return "[";
		case TOK_SEP: return ".";
		case TOK_ALT: return "/";
		default: goto error;
		}
	}

error:

	errno = EINVAL;
	return NULL;
}

void
lx_ebnf_init(struct lx_ebnf_lx *lx)
{
	static const struct lx_ebnf_lx lx_default;

	assert(lx != NULL);

	*lx = lx_default;

	lx->c = EOF;
	lx->z = NULL;

	lx->end.byte = 0;
	lx->end.line = 1;
	lx->end.col  = 1;
}

enum lx_ebnf_token
lx_ebnf_next(struct lx_ebnf_lx *lx)
{
	enum lx_ebnf_token t;

	assert(lx != NULL);

	if (lx->lgetc == NULL) {
		return TOK_EOF;
	}

	if (lx->z == NULL) {
		lx->z = z4;
	}

	t = lx->z(lx);

	if (lx->push != NULL) {
		if (-1 == lx->push(lx, '\0')) {
			return TOK_ERROR;
		}
	}

	if (lx->lgetc == NULL && lx->free != NULL) {
		lx->free(lx);
	}

	return t;
}

