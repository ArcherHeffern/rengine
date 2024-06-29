#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "rengine.h"

static char *ERROR_MSG = NULL;
#define EXT_ERR_SYNTAX 1

int main(int argc, char **argv)
{
	char *regex = "jsadf&\\fd||~!$";
	RegEx *compiled_regex = regex__compile(regex);
	// char *target = "abcde";
	// regex__match(compiled_regex, target);
}

bool renode__is_infinite(ReNode *renode)
{
	return renode->max == -1;
}
bool renode__is_assertion(ReNode *renode)
{
	return renode->max == 0;
}

void renode__print(ReNode *renode)
{
	if (renode__is_assertion(renode))
	{
		printf("(%c: Assertion)", renode->characters[0]);
	}
	else if (renode__is_infinite(renode))
	{
		printf("(%s; range: %d-infinity, greedy: %s)", renode->characters, renode->min, renode->greedy ? "True" : "False");
	}
	else
	{
		printf("(%s; range: %d-%d, greedy: %s)", renode->characters, renode->min, renode->max, renode->greedy ? "True" : "False");
	}
}


RegEx *regex__compile(char *regex)
{
	TokenList *tokens = tokens__tokenize(regex);
	if (tokens == NULL)
	{
		printf("Syntax Error: %s", ERROR_MSG);
		exit(EXT_ERR_SYNTAX);
	}
	tokens__print(tokens);
	RegEx* parsed_regex = regex__parse(tokens);
	regex__print(parsed_regex);

	exit(0);
}

RegEx* regex__init() {
	RegEx* regex = malloc(sizeof(RegEx));
	regex->renodearray = renodearray__init();
}

RegEx* regex__parse(TokenList *tokens) {
	RegEx* regex = regex__init();
}

void regex__print(RegEx *regex)
{
	renodearray__print(regex->renodearray);
}

bool regex__match(RegEx *regex, char *s)
{
	return 1;
}

ReNodeArray* renodearray__init() {
	ReNodeArray* renodearray = malloc(sizeof(ReNodeArray));
	renodearray->capacity = 8;
	renodearray->size = 0;
	renodearray->renode_array = malloc(sizeof(ReNode*)*8);
	return renodearray;
}

void renodearray__insert(ReNodeArray* renodearray, ReNode* renode) {
	if (renodearray->size == renodearray->capacity)
	{
		renodearray->capacity *= 2;
		renodearray->renode_array = realloc(renodearray->renode_array, sizeof(ReNode*) * renodearray->capacity);
	}
	renodearray->renode_array[renodearray->size++] = renode;
}

void renodearray__print(ReNodeArray *renodearray)
{
	for (int i = 0; i < renodearray->size; i++)
	{
		renode__print(renodearray->renode_array[i]);
	}
}

void renodearray__destroy(ReNodeArray* renodearray) {
	for (int i = 0; i < renodearray->size; i++) {
		free(renodearray->renode_array[i]);
	}
	free(renodearray->renode_array);
	free(renodearray);
}

char *tokentype__to_string(enum TokenType tokentype)
{
	switch (tokentype)
	{
	case TOKEN_CHARACTER:
		return "TOKEN_CHARACTER";
	case TOKEN_L_PAREN:
		return "TOKEN_L_PAREN";
	case TOKEN_R_PAREN:
		return "TOKEN_R_PAREN";
	case TOKEN_L_CURLY:
		return "TOKEN_L_CURLY";
	case TOKEN_R_CURLY:
		return "TOKEN_R_CURLY";
	case TOKEN_VERT:
		return "TOKEN_VERT";
	case TOKEN_QUESTION:
		return "TOKEN_QUESTION";
	case TOKEN_ADD:
		return "TOKEN_ADD";
	case TOKEN_STAR:
		return "TOKEN_STAR";
	case TOKEN_CARAT:
		return "TOKEN_CARAT";
	case TOKEN_AMPERSAND:
		return "TOKEN_AMPERSAND";
	default:
		return "UNDEFINED";
	}
}

Token *token__init(enum TokenType tokentype, char *literal)
{
	Token *token = malloc(sizeof(Token));
	token->literal = literal;
	token->type = tokentype;
	return token;
}

void token__print(Token *token)
{
	printf("(type=%s, literal=%s)\n", tokentype__to_string(token->type), token->literal);
}

TokenList *tokenlist__init(int capacity)
{
	if (capacity <= 0)
	{
		capacity = 8;
	}
	TokenList *token_list = malloc(sizeof(TokenList));
	token_list->capacity = capacity;
	token_list->size = 0;
	token_list->tokens = malloc(sizeof(Token *) * capacity);
	return token_list;
}

void tokenlist__insert(TokenList *tokenlist, Token *token)
{
	if (tokenlist->size >= tokenlist->capacity)
	{
		tokenlist->capacity *= 2;
		tokenlist->tokens = realloc(tokenlist->tokens, sizeof(Token *) * tokenlist->capacity);
	}
	tokenlist->tokens[tokenlist->size++] = token;
}

/**
 * @return List of tokens null terminated
 */
TokenList *tokens__tokenize(char *s)
{
	TokenList *token_list = tokenlist__init(8);
	Scanner *scanner = scanner__init(s);
	StringBuilder *stringbuilder = stringbuilder__init();
	enum TokenType tokentype;
	char *literal;
	Token *token;
	while (!scanner__at_end(scanner))
	{
		char ch = scanner__consume_next(scanner);
		switch (ch)
		{
		case '(':
			tokentype = TOKEN_L_PAREN;
			break;
		case ')':
			tokentype = TOKEN_R_PAREN;
			break;
		case '{':
			tokentype = TOKEN_L_CURLY;
			break;
		case '}':
			tokentype = TOKEN_R_CURLY;
			break;
		case '|':
			tokentype = TOKEN_VERT;
			break;
		case '?':
			tokentype = TOKEN_QUESTION;
			break;
		case '+':
			tokentype = TOKEN_ADD;
			break;
		case '*':
			tokentype = TOKEN_STAR;
			break;
		case '^':
			tokentype = TOKEN_CARAT;
			break;
		case '&':
			tokentype = TOKEN_AMPERSAND;
			break;
		case '\\':
			if (!scanner__has_next(scanner))
			{
				asprintf(&ERROR_MSG, "Expected character after \\ at character %llu but found nothing\n", scanner->curr + 1);
				return NULL;
			}
			ch = scanner__consume_next(scanner);
			tokentype = TOKEN_CHARACTER;
			break;
		default:
			tokentype = TOKEN_CHARACTER;
			break;
		}
		stringbuilder__append(stringbuilder, ch);
		literal = stringbuilder__to_string(stringbuilder);
		token = token__init(tokentype, literal);
		tokenlist__insert(token_list, token);
	}
	return token_list;
}

void tokens__print(TokenList *token_list)
{
	for (int i = 0; i < token_list->size; i++)
	{
		token__print(token_list->tokens[i]);
	}
	// TODO: What was wrong with this code?
	// Token *token = token_list->tokens[0];
	// while (token != NULL)
	// {
	// 	token__print(token);
	// 	token++;
	// }
}

Scanner *scanner__init(char *string)
{
	Scanner *scanner = malloc(sizeof(Scanner));
	scanner->string = string;
	scanner->curr = -1;
	scanner->end = strlen(string);
	return scanner;
}

bool scanner__at_end(Scanner *scanner)
{
	return scanner->curr + 1 == scanner->end;
}

bool scanner__has_next(Scanner *scanner)
{
	return scanner->curr + 1 < scanner->end;
}

char scanner__peek_next(Scanner *scanner)
{
	return scanner->string[scanner->curr + 1];
}

char scanner__consume_next(Scanner *scanner)
{
	return scanner->string[++scanner->curr];
}

StringBuilder *stringbuilder__init()
{
	StringBuilder *stringbuilder = malloc(sizeof(StringBuilder));
	stringbuilder->capacity = 8;
	stringbuilder->size = 0;
	stringbuilder->string = malloc(8);
	return stringbuilder;
}

void stringbuilder__append(StringBuilder *stringbuilder, char c)
{
	if (stringbuilder->size + 1 == stringbuilder->capacity)
	{
		stringbuilder->capacity *= 2;
		stringbuilder->string = realloc(stringbuilder->string, stringbuilder->capacity);
	}
	stringbuilder->string[stringbuilder->size++] = c;
}

/**
 * Resets the stringbuilder
 * @return built string
 */
char *stringbuilder__to_string(StringBuilder *stringbuilder)
{
	stringbuilder->string[stringbuilder->size] = '\0';
	char *string = stringbuilder->string;
	stringbuilder->size = 0;
	stringbuilder->capacity = 8;
	stringbuilder->string = malloc(8);
	return string;
}

void stringbuilder__free(StringBuilder *stringbuilder)
{
	free(stringbuilder->string);
	free(stringbuilder);
}