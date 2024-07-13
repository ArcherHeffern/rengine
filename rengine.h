#include <stdint.h>

typedef struct token Token;
typedef struct tokenlist TokenList;
typedef struct renode ReNode;
typedef struct renodearray ReNodeArray;
typedef struct regex RegEx;
typedef struct scanner Scanner;
typedef struct stringbuilder StringBuilder;

enum TokenType
{
	TOKEN_L_PAREN,
	TOKEN_R_PAREN,
	TOKEN_L_CURLY,
	TOKEN_R_CURLY,
	TOKEN_VERT,
	// Urnary Operators
	TOKEN_QUESTION,
	TOKEN_ADD,
	TOKEN_STAR,
	// Assertions
	TOKEN_CARAT,
	TOKEN_AMPERSAND,
    // Literals
	TOKEN_CHARACTER,
};

enum ReNodeType 
{
	CharacterClass,
	Assertion,
	Subexpression
};

enum AssertionType 
{
	END_OF_LINE,
	START_OF_LINE,
};


struct token
{
	enum TokenType type;
	char *literal;
};

	Token* token__init(enum TokenType tokentype, char *literal);
	void token__print(Token *token);


struct renode
{
	enum ReNodeType renode_type;

	// Character Class
    char *characters;

	// Character Class && Subexpression
	int times_seen; // Used during runtime
	int min; // Inclusive; Ignore if ReNode is an assertion
	int max; // Inclusive; If -1, allows for infinites, if 0, is an assertion
	bool greedy; // Ignore if ReNode is an assertion
	
	// Subexpression
	RegEx** subexpressions;

	// Assertion
	enum AssertionType assertion_type;

	// Shared
	struct ReNode **next;
};

	bool renode__is_infinite(ReNode* renode);
	bool renode__is_assertion(ReNode* renode);
	void renode__print(ReNode* renode);


struct renodearray
{
    ReNode** renode_array;
    uint64_t size;
    uint64_t capacity;
};

	ReNodeArray* renodearray__init();
	void renodearray__insert(ReNodeArray* renodearray, ReNode* renode);
	void renodearray__print(ReNodeArray* renodearray);
	void renodearray__destroy(ReNodeArray* renodearray);


struct regex
{
    ReNodeArray* renodearray;
	int match_start;
    int match_end;
};

	RegEx* regex__init();
	RegEx* regex__parse(TokenList *tokens);
	void regex__print(RegEx* regex);
	RegEx* regex__compile(char *regex);
	bool regex__match(RegEx *regex, char *s);


struct tokenlist
{
	uint64_t size;
	uint64_t capacity;
	Token **tokens;
};

	TokenList *tokenlist__init(int capacity);
	void tokenlist__insert(TokenList *tokenlist, Token *token);
	TokenList* tokens__tokenize(char *s);
	void tokens__print(TokenList *token_list);


struct scanner
{
	char *string;
	uint64_t curr;
	uint64_t end;
};

	Scanner* scanner__init(char *string);
	bool scanner__at_end(Scanner *scanner);
	bool scanner__has_next(Scanner* scanner);
	char scanner__peek_next(Scanner* scanner);
	char scanner__consume_next(Scanner *scanner);

struct stringbuilder 
{
    char* string;
    uint64_t size;
    uint64_t capacity;
};

	StringBuilder* stringbuilder__init();
	void stringbuilder__append(StringBuilder* stringbuilder, char c);
	char* stringbuilder__to_string(StringBuilder* stringbuilder);