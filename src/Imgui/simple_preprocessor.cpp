#define _CRT_SECURE_NO_WARNINGS 1
#include <stdlib.h>
#include <stdio.h>
#include <cstring>


char *ReadEntireFIleIntoMemoryAndNullTerminate(char *filename)
{
	char *result = 0;
	FILE *file = fopen(filename, "r");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		result = (char *)malloc(fileSize + 1);
		fread(result, fileSize, 1, file);
		result[fileSize] = 0;

		fclose(file);
	}
	return result;
}

enum token_type
{
	Token_Unknown,
	Token_OpenParem,
	Token_Colon,
	Token_ClosenParen,
	Token_Semicolon,
	Token_Asterisk,
	Token_OpenBracket,
	Token_CloseBracket,
	Token_OpenBraces,
	Token_CloseBraces,

	Token_String,
	Token_Identifier,
	Token_Macro,

	Token_EndOfStream,
};

struct Token
{
	token_type type;
	size_t textLength;
	char *text;
};

struct Tokenizer
{
	char* at;
};

inline bool IsEndOfLine(char c)
{
	return (c == '\n' || c == '\r');
}

inline bool IsWhitespace(char c)
{
	bool result = ((c == ' ') || (c == '\t') || IsEndOfLine(c));
	return result;
}

inline bool IsAlpha(char c)
{
	return ((c >= 'a') && (c <= 'z') ||
		(c >= 'A') && (c <= 'Z'));
}

inline bool IsNumer(char c)
{
	return ((c >= '0') && (c <= '9'));
}


static void EatAllWhitespace(Tokenizer* tokenizer)
{
	while (1)
	{
		if (IsWhitespace(tokenizer->at[0]))
		{
			++tokenizer->at;
		}
		else if (tokenizer->at[0] == '/' && tokenizer->at[1] == '/')
		{
			tokenizer->at += 2;
			while (tokenizer->at[0] && !IsEndOfLine(tokenizer->at[0]))
			{
				++tokenizer->at;
			}
		}
		else if (tokenizer->at[0] == '/' && tokenizer->at[1] == '*')
		{
			tokenizer->at += 2;
			while (tokenizer->at[0] && !(tokenizer->at[0] == '*' && tokenizer->at[1] == '/'))
			{
				++tokenizer->at;
			}

			if (tokenizer->at[0] == '*')
			{
				tokenizer->at += 2;
			}
		}
		else
		{
			break;
		}
	}
}

static Token GetToken(Tokenizer* tokenizer)
{
	EatAllWhitespace(tokenizer);

	Token token{};
	token.textLength = 1;
	token.text = tokenizer->at;
	char c = tokenizer->at[0];
	++tokenizer->at;

	switch (c)
	{
	case '\0': token.type = Token_EndOfStream; break;png
	case '(': token.type = Token_OpenParem; break;
	case ')': token.type = Token_ClosenParen; break;
	case ':': token.type = Token_Colon; break;
	case ';': token.type = Token_Semicolon; break;
	case '*': token.type = Token_Asterisk; break;
	case '[': token.type = Token_OpenBracket; break;
	case ']': token.type = Token_CloseBracket; break;
	case '{': token.type = Token_OpenBraces; break;
	case '}': token.type = Token_CloseBraces; break;

		/*case '/':
		{

		} break;
	*/
	case '"':
	{
		token.type = Token_String;
		token.text = tokenizer->at;

		while (tokenizer->at[0] && tokenizer->at[0] != '"')
		{
			if ((tokenizer->at[0] == '\\') && tokenizer->at[1])
			{
				++tokenizer->at;
			}
			++tokenizer->at;

		}

		token.textLength = tokenizer->at - token.text;
		if (tokenizer->at[0] == '"')
		{
			++tokenizer->at;
		}
	} break;

	default:
	{
		if (IsAlpha(c))
		{
			token.type = Token_Identifier;
			while (IsAlpha(tokenizer->at[0]) ||
				IsNumer(tokenizer->at[0]) ||
				(tokenizer->at[0] == '_'))
			{
				++tokenizer->at;
				//ParseIdentifier();
			}
			token.textLength = tokenizer->at - token.text;
		}
		else if (IsNumer(tokenizer->at[0]))
		{
			// ParseNumber();
		}
		else
		{
			token.type = Token_Unknown;
		}
	}break;
	}

	return token;
}

inline bool TokenEquals(Token token, char* match)
{
	char *at = match;
	for (int i = 0; i < token.textLength; ++i, at++)
	{
		if ((*at == 0) || (token.text[i] != *at))
		{
			return false;
		}
	}
	return (*at == 0);
}

static bool RequireToken(Tokenizer* tokenizer, token_type type)
{
	Token token = GetToken(tokenizer);
	bool result = token.type == type;
	return result;
}

static void parseIntrospectionParams(Tokenizer * tokenizer)
{
	for (;;)
	{
		Token token = GetToken(tokenizer);
		if ((token.type == Token_ClosenParen) ||
			(token.type == Token_EndOfStream))
		{
			break;
		}
	}
}

static void ParseMember(Tokenizer *tokenizer, Token structTypeToken, Token memberTypeToken)
{
	bool parsing = true;
	bool isPointer = false;
	bool normalIdentifier = true;

	Token token;
	int arrayLen;
	char* start = 0;

	while (parsing)
	{
		Token currentToken = GetToken(tokenizer);

		switch (currentToken.type)
		{
		case Token_Asterisk:
			isPointer = true;
			break;

		case Token_Identifier:
			if (normalIdentifier)
			{
				// normalIdentifier = true;
				token = currentToken;
			}
			break;
		case Token_OpenBracket:
		{
			// koko on current Token!
			normalIdentifier = false;
			start = currentToken.text + 1;
		} break;
		case Token_CloseBracket:           // get number from str // func down here
		{
			char *end = currentToken.text;
			arrayLen = strtol(start, &end, 10);
		}break;

		case Token_Semicolon:
		case Token_EndOfStream:
			parsing = false;
			break;
		}
	}

	if (normalIdentifier)
	{
		printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s }, \n",
			isPointer ? "MetaMemberFlag_IsPointer" : "0",
			(int)memberTypeToken.textLength, memberTypeToken.text,
			(int)token.textLength, token.text,
			(int)structTypeToken.textLength, structTypeToken.text,
			(int)token.textLength, token.text);
	}
	else
	{
		printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s, (void*)%i }, \n",
			"MetaMemberFlag_IsArray",
			(int)memberTypeToken.textLength, memberTypeToken.text,
			(int)token.textLength, token.text,
			(int)structTypeToken.textLength, structTypeToken.text,
			(int)token.textLength, token.text,
			arrayLen);
	}
} // type nimi [4];

enum ptrType
{
	single,
	staticArray,
	dynamicArray,
};
// static void ParsePointer()
static void ParsePointer(Tokenizer *tokenizer, Token structTypeToken, Token memberTypeToken, ptrType type)
{
	bool parsing = true;
	bool isPointer = false;

	GetToken(tokenizer); // ptrSingle jne..
	Token typeToken = GetToken(tokenizer); // muuttujan tyyppi
	GetToken(tokenizer); // ptrSingle jne..
	Token variableNameToken = GetToken(tokenizer); // muuttujan nimi
	GetToken(tokenizer); // muuttujan nimi
	Token ptrSizeVariableName = GetToken(tokenizer); // pointerin koko variablen nimi

	if (type == single)
		printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s }, \n",
			"MetaMemberFlag_IsSinglePointer",
			(int)typeToken.textLength, typeToken.text,
			(int)variableNameToken.textLength, variableNameToken.text,
			(int)structTypeToken.textLength, structTypeToken.text,
			(int)variableNameToken.textLength, variableNameToken.text); // muuttujan nimi

	else if (type == staticArray)
		printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s, (void*)%.*s }, \n",
			"MetaMemberFlag_IsArrayPointer",
			(int)typeToken.textLength, typeToken.text,
			(int)variableNameToken.textLength, variableNameToken.text,
			(int)structTypeToken.textLength, structTypeToken.text,
			(int)variableNameToken.textLength, variableNameToken.text,
			(int)ptrSizeVariableName.textLength, ptrSizeVariableName.text); // muuttujan nimi

	else if (type == dynamicArray)
		printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s, (void*)(u32)&((%.*s *)0)->%.*s }, \n",
			"MetaMemberFlag_IsDynamicPointer",
			(int)typeToken.textLength, typeToken.text,
			(int)variableNameToken.textLength, variableNameToken.text,
			(int)structTypeToken.textLength, structTypeToken.text,
			(int)variableNameToken.textLength, variableNameToken.text,
			(int)structTypeToken.textLength, structTypeToken.text,
			(int)ptrSizeVariableName.textLength, ptrSizeVariableName.text); // muuttujan nimi


	while (ptrSizeVariableName.type != Token_Semicolon)
	{
		ptrSizeVariableName = GetToken(tokenizer);
	}

	// while (parsing)
	//{
	//	Token token = GetToken(tokenizer);
	//	switch (token.type)
	//	{
	//	case Token_Identifier: // tyyppi, nimi, 

	//		break;
	//	case Token_Semicolon:
	//	case Token_EndOfStream:
	//		parsing = false;
	//		break;
	//	}
	//}

	//if (type == single)
	//		{
	//			printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s }, \n",
	//				"MetaMemberFlag_IsSinglePointer",
	//				(int)memberTypeToken.textLength, memberTypeToken.text,
	//				(int)token.textLength, token.text,
	//				(int)structTypeToken.textLength, structTypeToken.text,
	//				(int)token.textLength, token.text);
	//		}
	//		else if (type == staticArray) // skippaa seuraavat uint_t, base3 <- nimi
	//		{
	//			printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s, ",
	//				"MetaMemberFlag_IsArrayPointer",
	//				(int)memberTypeToken.textLength, memberTypeToken.text,
	//				(int)token.textLength, token.text,
	//				(int)structTypeToken.textLength, structTypeToken.text,
	//				(int)token.textLength, token.text);
	//		}
	//		else if (type == dynamicArray)
	//		{
	//			/*printf("	{%s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s *)0)->%.*s } \n",
	//				"MetaMemberFlag_IsDynamicPointer",
	//				(int)memberTypeToken.textLength, memberTypeToken.text,
	//				(int)token.textLength, token.text,
	//				(int)structTypeToken.textLength, structTypeToken.text,
	//				(int)token.textLength, token.text);*/
	//		}
}

struct meta_struct
{
	char *name;
	meta_struct *next;
};
static meta_struct *firstMetaSruct;

static void ParseStruct(Tokenizer* tokenizer)
{
	Token nameToken = GetToken(tokenizer);
	if (RequireToken(tokenizer, Token_OpenBraces))
	{
		printf("member_definition membersOf_%.*s[] = \n", (int)nameToken.textLength, nameToken.text);
		printf("{\n");
		for (;;)
		{
			Token memberToke = GetToken(tokenizer);

			if (memberToke.type == Token_CloseBraces)
			{
				break;
			}
			else if (TokenEquals(memberToke, "ptrSingle"))
			{
				ParsePointer(tokenizer, nameToken, memberToke, single);
			}
			else if (TokenEquals(memberToke, "ptrDArray"))
			{
				ParsePointer(tokenizer, nameToken, memberToke, dynamicArray);
			}
			else if (TokenEquals(memberToke, "ptrSArray"))
			{
				ParsePointer(tokenizer, nameToken, memberToke, staticArray);
			}
			else if (TokenEquals(memberToke, "ignore"))
			{
				for (;;)
				{
					Token token = GetToken(tokenizer);
					if (token.type == Token_Semicolon)
						break;
				}
			}
			else
			{
				ParseMember(tokenizer, nameToken, memberToke);
			}
		}
		printf("};\n");

		meta_struct *meta = (meta_struct*)malloc(sizeof(meta_struct));
		meta->name = (char *)malloc(nameToken.textLength + 1);
		memcpy(meta->name, nameToken.text, nameToken.textLength);
		meta->name[nameToken.textLength] = 0;
		meta->next = firstMetaSruct;
		firstMetaSruct = meta;
	}
}

static void eatLineHack(Tokenizer* tokenizer)
{
	for (;;)
	{
		Token memberToke = GetToken(tokenizer);

		if (memberToke.type == Token_CloseBraces || memberToke.type == Token_ClosenParen)
			// memberToke.type == Token_co)
		{
			break;
		}
	}
}


// tesit
static void parseIntrospectable(Tokenizer *tokenizer)
{
	if (RequireToken(tokenizer, Token_OpenParem))
	{
		parseIntrospectionParams(tokenizer);

		Token typeToken = GetToken(tokenizer);
		if (TokenEquals(typeToken, "struct"))
		{
			ParseStruct(tokenizer);
		}
		else if (TokenEquals(typeToken, "#"))
		{
			// hakky 
			// eatLineHack(tokenizer);
			// break;
			return;
		}
		else
		{
			fprintf(stderr, "ERROR only supported for struct atm.\n");
		}
	}
	else
	{
		fprintf(stderr, "ERROR missing parentheses.\n");
	}
}


#define ArrayCount(array) ( sizeof(array) / sizeof((array)[0]))

int main(int ArgCount, char ** Args)
{
	char * filenames[2] =
	{
		// "I:/Dev/SDL/allegro vanhat/src/game.h",
		// "I:/Dev/SDL/allegro vanhat/src/core.h",
		"../src/game/game.h",
		"../inc/platform.h"
	};

	char * disabledWarnings[1] = 
	{
		"4302", // type cast truncation
	};

	printf("\n");
	printf("/// ******************************************* /// \n");
	printf("/// *  THIS FILE IS AUTOMATICALLY GENERATED!  * /// \n");
	printf("/// *         DO NOT MODIFY IT BY HAND!       * /// \n");
	printf("/// ******************************************* /// \n");
	printf("\n");

	printf("#pragma warning( push )\n");
	for (int i = 0; i < ArrayCount(disabledWarnings); i++)
	{
		printf("#pragma warning( disable : %s )\n", disabledWarnings[i]);
	}
	printf("\n");

	for (int i = 0; i < ArrayCount(filenames); i++)
	{
		char* Filecontents = ReadEntireFIleIntoMemoryAndNullTerminate(filenames[i]);

		Tokenizer tokenizer = {};
		tokenizer.at = Filecontents;

		bool parsing = true;
		while (parsing)
		{
			Token token = GetToken(&tokenizer);
			switch (token.type)
			{
			case Token_EndOfStream:
			{
				parsing = false;
			} break;

			case Token_Unknown: break;

			case Token_Identifier:
			{
				if (TokenEquals(token, "define"))
				{
					// macro
					int a = 0;
				}
				else if (TokenEquals(token, "ignore"))
				{
					// igonore member functions
					int a = 0;
				}
				else if (TokenEquals(token, "introspect"))
				{
					parseIntrospectable(&tokenizer);
				}
				else if (false)
				{

				}
			} break;

			default:
			{
				// printf("%d: %.*s: \n", token.type, (int)token.textLength, token.text);
			} break;
			}
		}
	}

	printf("#define META_HANDLE_TYPE_DUMD(MemberPtr, nextIntendLevel) \\\n");
	for (meta_struct* meta = firstMetaSruct; meta; meta = meta->next)
	{
		printf("	case MetaType_%s: { dumpStruct(ArrayCount(membersOf_%s),membersOf_%s, memberPtr, nextIntendLevel + 1); } break; %s\n",
			meta->name, meta->name, meta->name,
			meta->next ? "\\" : "");
	}

	// printf("#define META_HANDLE_TYPE_DUMD(MemberPtra, nextIntendLevel) \\\n");
	printf("void deSerializeStructGenerated(int memberCount, member_definition* memberDefinition, void* structPtr, char** buffer, int level = 0) \n");
	printf("{ \n");
	printf("	for (int i = 0; i < memberCount; ++i) \n");
	printf("		{ \n");
	printf("		member_definition* member = memberDefinition + i; \n");
	printf("		void *memberPtr (((uint8_t *)structPtr) + member->offset); \n");

	printf("		"); // if ptr
	printf("		switch(member->type) \n ");
	printf("		{ \n");

	for (meta_struct* meta = firstMetaSruct; meta; meta = meta->next)
	{
		printf("		case MetaType_%s: { \n \t \
		*buffer = *buffer + %i + strlen(member->name); \n\
			deSerializeStructGenerated(ArrayCount(membersOf_%s), membersOf_%s, memberPtr, buffer, level + 1); \n  \t\t } break; \n",
			meta->name, strlen(meta->name) + 1, meta->name, meta->name);
	}

	printf("		} \n");
	printf("		} \n");
	printf("		} \n");


	printf("#define META_HANDLE_SERILIZATION(MemberPtr, nextIntendLevel) \\\n");
	for (meta_struct* meta = firstMetaSruct; meta; meta = meta->next)
	{
		printf("		case MetaType_%s: { \\\n \
		*buffer = *buffer + %i + strlen(member->name); \\\n \
			deSerializeStructR(ArrayCount(membersOf_%s), memberPtr, membersOf_%s, buffer, level + 1); \\\n  \t\t } break; \\\n",
			meta->name, strlen(meta->name) + 2, meta->name, meta->name);
	}

	printf("\n");
	printf("#pragma warning( pop )\n");
	
	getchar(); // debug macro jne
}