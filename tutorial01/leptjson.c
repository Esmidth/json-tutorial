#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
	const char* json;
} lept_context;

static void lept_parse_whitespace(lept_context* c) {
	const char* p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == 'r') {
		p++;
	}
	c->json = p;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
	EXPECT(c, 'n');
	if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l') {
		return LEPT_PARSE_INVALID_VALUE;
	}
	c->json += 3;
	v->type = LEPT_NULL;
	return 0;
}

static int lept_parse_true(lept_context* c, lept_value* v) {
	EXPECT(c, 't');
	if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e') {
		return LEPT_PARSE_INVALID_VALUE;
	}
	c->json += 3;
	v->type = LEPT_TRUE;
	return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
	EXPECT(c, 'f');
	if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e') {
		return LEPT_PARSE_INVALID_VALUE;
	}
	c->json += 4;
	v->type = LEPT_FALSE;
	return LEPT_PARSE_OK;
}
static int lept_parse_number(lept_context* c, lept_value* v) {
	char* end;
	v->n = strtod(c->json, &end);
	if (c->json == end)
		return LEPT_PARSE_INVALID_VALUE;
	c->json = end;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
	switch (*c->json) {
	case 'n':
		return lept_parse_null(c, v);
	case '\0':
		return LEPT_PARSE_EXPECT_VALUE;
	case 't':
		return lept_parse_true(c, v);
	case 'f':
		return lept_parse_false(c, v);
	default:
		return lept_parse_number(c,v);
	}
}

static int lept_parse_root_not_singular(lept_context* c) {
	if (c == NULL)
		return LEPT_PARSE_OK;
	const char* p = c->json;
	while (p != NULL && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '\0') {
		p++;
	}
	if (p != NULL && *p == '\0') {
		return LEPT_PARSE_OK;
	}
	else
		return LEPT_PARSE_ROOT_NOT_SINGULAR;
}


int lept_parse(lept_value* v, const char* json) {
	/*
	lept_context c;
	assert(v != NULL);
	c.json = json;
	v->type = LEPT_NULL;
	lept_parse_whitespace(&c);
	return lept_parse_value(&c, v);
	*/
	lept_context c;
	assert(v != NULL);
	c.json = json;
	v->type = LEPT_NULL;
	lept_parse_whitespace(&c);
	int lept_i = lept_parse_value(&c, v);
	int lept_ii = lept_parse_root_not_singular(&c);
	if (lept_ii == LEPT_PARSE_OK) {
		return lept_i;
	}
	else
		return LEPT_PARSE_ROOT_NOT_SINGULAR;
}

lept_type lept_get_type(const lept_value* v) {
	assert(v != NULL);
	return v->type;
}

double lept_get_number(const lept_value* v ) {
	assert(v != NULL && v->type == LEPT_NUMBER);
	return v->n;
}
