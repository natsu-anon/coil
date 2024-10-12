#include "coil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>
#include <stdint.h>
#include "query_str.h"
/* #include "assert_macros.h" */

typedef enum Language
{
	C = 0,
	CPP,
	SIZE_LANGUAGE
} Language;

extern const char func[];
extern const char c_decl[];
extern const char cpp_decl[];

const TSLanguage *tree_sitter_c(void);
const TSLanguage *tree_sitter_cpp(void);

const TSLanguage *(*language[2])(void) = { tree_sitter_c, tree_sitter_cpp }; /* IM GAMING */

const char err_matching[5][32] = {
	"UNMATCHED ERROR TYPE",
	"Syntax",
	"Node Type",
	"Field",
	"Capture",
};

#define QUERY_ASSERT(A) if (A) { fprintf(stderr, "[TSQUERY ERROR] %s:%d  -- %s", __FILE__, __LINE__, err_matching[A]); exit(EXIT_FAILURE); }


void tree_sit(const char* str, const int len, const Language l, TSParser **parser, TSTree **tree)
{
	*parser = ts_parser_new();
	ts_parser_set_language(*parser, language[l]());
	*tree = ts_parser_parse_string(*parser, NULL, str, len);
}

char* node_text(const TSNode* n, const char* str)
{
	int b0 = ts_node_start_byte(*n);
	int b1 = ts_node_end_byte(*n);
	char* res = malloc(b1 - b0 + 1);
	strncpy(res, str + b0, b1 - b0);
	res[b1 - b0] = '\0';
	return res;
}

unsigned char node_unbound(const TSNode* n, const uint32_t r0, const uint32_t r1)
{
	return (int)ts_node_start_byte(*n) < r0 | r1 < (int)ts_node_end_byte(*n);
}


TSQuery* make_query(const char* query, const int len, const Language l)
{
	TSQueryError err;
	uint32_t err_offset;
	TSQuery* ts_q = ts_query_new(
		language[l](),
		query,
		len,
		&err_offset,
		&err);
	QUERY_ASSERT(err);
	return ts_q;
}

void coil_define_c(FILE *buf, const char *src, const int len)
{
	TSParser *parser;
	TSTree *tree;
	tree_sit(src, len, C, &parser, &tree);
	TSQuery* query = make_query(c_decl, strlen(c_decl), C);
	TSQuery* subquery = make_query(func, strlen(func), C);
	TSQueryCursor* cursor = ts_query_cursor_new();
	ts_query_cursor_exec(cursor, query, ts_tree_root_node(tree));
	TSQueryMatch match;
	TSQueryMatch submatch;
	int b0;
	int b1;
	TSNode node;
	unsigned char first = 1;
	while (ts_query_cursor_next_match(cursor, &match))
	{
		node = match.captures[0].node;
		TSQueryCursor* subcursor = ts_query_cursor_new();
		ts_query_cursor_exec(subcursor, subquery, node);
		/* unfortunately, you have to do it this way :( */
		if (ts_query_cursor_next_match(subcursor, &submatch))
		{
			b1 = ts_node_end_byte(submatch.captures[0].node);
		}
		else
		{
			continue;
		}
		ts_query_cursor_delete(subcursor);
		if (strcmp(ts_node_type(ts_node_parent(node)), "function_definition") == 0)
		{
			continue;
		}
		b0 = ts_node_start_byte(node);
		char decl[b1 - b0 + 1];
		strncpy(&decl[0], src + b0, b1 - b0);
		decl[b1 - b0] = '\0';
		if (first)
		{
			fprintf(buf, "%s\n{\n\t/* TODO */\n}\n", decl);
			first = 0;
		}
		else
		{
			fprintf(buf, "\n%s\n{\n\t/* TODO */\n}\n", decl);
		}
	}
	ts_query_cursor_delete(cursor);
	ts_query_delete(subquery);
	ts_query_delete(query);
	ts_tree_delete(tree);
	ts_parser_delete(parser);
}

void coil_define_cpp(FILE *buf, const char *src, const int len, const uint32_t r0, const uint32_t r1)
{
	TSParser *parser;
	TSTree *tree;
	tree_sit(src, len, CPP, &parser, &tree);
	TSQuery* query = make_query(cpp_decl, strlen(cpp_decl), CPP);
	TSQuery* subquery = make_query(func, strlen(func), CPP);
	TSQueryCursor* cursor = ts_query_cursor_new();
	ts_query_cursor_exec(cursor, query, ts_tree_root_node(tree));
	TSQueryMatch match;
	TSQueryMatch submatch;
	int b0;
	int b1;
	TSNode node;
	unsigned char first = 1;
	while (ts_query_cursor_next_match(cursor, &match))
	{
		/* im not using a switch because I want the frames */
		if (match.pattern_index == 0)
		{
			node = match.captures[1].node;
			if (node_unbound(&node, r0, r1))
			{
				continue;
			}
			node = match.captures[0].node;
			char* class_str = node_text(&node, src);
			node = match.captures[2].node;
			/* hopefully filters out weird macros */
			char* temp  = node_text(&node, src);
			if (strcmp(temp, class_str))
			{
				free(class_str);
				free(temp);
				continue;
			}
			free(temp);
			node = match.captures[1].node;
			char* class_decl = node_text(&node, src);
			if (first)
			{
				fprintf(buf, "%s::%s\n{\n\t/* TODO */\n}\n", class_str, class_decl);
				first = 0;
			}
			else {
				fprintf(buf, "\n%s::%s\n{\n\t/* TODO */\n}\n", class_str, class_decl);
			}
			free(class_str);
			free(class_decl);
		}
		else if (match.pattern_index == 1) {
			node = match.captures[1].node;
			if (node_unbound(&node, r0, r1))
			{
				continue;
			}
			node = match.captures[0].node;
			char* class_str = node_text(&node, src);
			node = match.captures[1].node;
			char* class_decl = node_text(&node, src);
			if (first)
			{
				fprintf(buf, "%s::%s\n{\n\t/* TODO */\n}\n", class_str, class_decl);
				first = 0;
			}
			else {
				fprintf(buf, "\n%s::%s\n{\n\t/* TODO */\n}\n", class_str, class_decl);
			}
			free(class_str);
			free(class_decl);
		}
		else if (match.pattern_index == 2) //
		{
			node = match.captures[1].node;
			if (node_unbound(&node, r0, r1))
			{
				continue;
			}
			node = match.captures[0].node;
			char* class_str = node_text(&node, src);
			node = match.captures[1].node;
			TSQueryCursor* subcursor = ts_query_cursor_new();
			ts_query_cursor_exec(subcursor, subquery, node);
			/* unfortunately, you have to do it this way :( */
			if (ts_query_cursor_next_match(subcursor, &submatch))
			{
				b0 = ts_node_start_byte(submatch.captures[0].node);
				b1 = ts_node_end_byte(submatch.captures[0].node);
			}
			else
			{
				continue;
			}
			ts_query_cursor_delete(subcursor);
			char sig[b1 - b0 + 1];
			strncpy(&sig[0], src + b0, b1 - b0);
			sig[b1 - b0] = '\0';
			b1 = b0 - 1;
			b0 = ts_node_start_byte(node);
			char ret[b1 - b0 + 1];
			strncpy(&ret[0], src + b0, b1 - b0);
			ret[b1 - b0] = '\0';
			if (first)
			{
				fprintf(buf, "%s %s::%s\n{\n\t/* TODO */\n}\n", ret, class_str, sig);
				first = 0;
			}
			else {
				fprintf(buf, "\n%s %s::%s\n{\n\t/* TODO */\n}\n", ret, class_str, sig);
			}
			free(class_str);
		}
		else // C-style func declaration
		{
			node = match.captures[0].node;
			if (node_unbound(&node, r0, r1))
			{
				continue;
			}
			TSQueryCursor* subcursor = ts_query_cursor_new();
			ts_query_cursor_exec(subcursor, subquery, node);
			/* unfortunately, you have to do it this way :( */
			if (ts_query_cursor_next_match(subcursor, &submatch))
			{
				b1 = ts_node_end_byte(submatch.captures[0].node);
			}
			else
			{
				continue;
			}
			ts_query_cursor_delete(subcursor);
			if (strcmp(ts_node_type(ts_node_parent(node)), "function_definition") == 0)
			{
				continue;
			}
			b0 = ts_node_start_byte(node);
			char decl[b1 - b0 + 1];
			strncpy(&decl[0], src + b0, b1 - b0);
			decl[b1 - b0] = '\0';
			if (first)
			{
				fprintf(buf, "%s\n{\n\t/* TODO */\n}\n", decl);
				first = 0;
			}
			else
			{
				fprintf(buf, "\n%s\n{\n\t/* TODO */\n}\n", decl);
			}
		}
	}
	ts_query_delete(subquery);
	ts_query_delete(query);
	ts_query_cursor_delete(cursor);
	ts_tree_delete(tree);
	ts_parser_delete(parser);
}
