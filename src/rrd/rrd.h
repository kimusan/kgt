/* $Id$ */

#ifndef KGT_RRD_H
#define KGT_RRD_H

#include "../ast.h"

struct box_size {
	int w;
	int h;
};

struct node {
	enum {
		NODE_SKIP,
		NODE_LITERAL,
		NODE_RULE,
		NODE_ALT,
		NODE_SEQ,
		NODE_LOOP
	} type;

	union {
		const char *literal; /* TODO: point to ast_literal instead */
		const char *name;    /* TODO: point to ast_rule instead */

		struct node *alt;
		struct node *seq;

		struct {
			struct node *forward;
			struct node *backward;
			unsigned int min;
			unsigned int max;
		} loop;
	} u;

	int y;
	struct box_size size;

	struct node *next;
};

/* node traversal - visit functions are passed a pointer to the pointer that ties
 * the visited node into the tree.
 * they are free to replace the node they visited via said pointer. cf. beautify
 */
struct node_walker {
	int (*visit_skip   )(struct node *, struct node **, int, void *);
	int (*visit_name   )(struct node *, struct node **, int, void *);
	int (*visit_literal)(struct node *, struct node **, int, void *);
	int (*visit_alt    )(struct node *, struct node **, int, void *);
	int (*visit_seq    )(struct node *, struct node **, int, void *);
	int (*visit_loop   )(struct node *, struct node **, int, void *);
};

struct node *ast_to_rrd(const struct ast_rule *);

#endif
