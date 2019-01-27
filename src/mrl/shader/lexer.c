#include <mrl/shader/lexer.h>
#include <mgl/error.h>
#include <mgl/memory/allocator.h>
#include <mgl/memory/manipulation.h>
#include <mgl/stream/buffer_stream.h>

typedef struct
{
	const mrl_shader_lexer_params_t* params;
	const mgl_chr8_t* it;
	mgl_u64_t token_count;
	mgl_u64_t line_count;
	mgl_bool_t finished;
} lexer_state_t;

static mgl_bool_t is_white_space(mgl_chr8_t c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

static mgl_bool_t is_alpha(mgl_chr8_t c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static mgl_bool_t is_numeric(mgl_chr8_t c)
{
	return c >= '0' && c <= '9';
}

static mgl_bool_t put_token(lexer_state_t* state, const mrl_shader_token_t* token)
{
	if (state->token_count >= state->params->max_token_count)
	{
		if (state->params->error_buffer != NULL)
		{
			mgl_buffer_stream_t stream;
			mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
			mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
			mgl_print(&stream, u8"Failed to put token of type '");
			mgl_print(&stream, token->tinfo->name);
			mgl_print(&stream, u8"' (token number ");
			mgl_print_u64(&stream, state->token_count + 1, 10);
			mgl_print(&stream, u8", line ");
			mgl_print_u64(&stream, state->line_count + 1, 10);
			mgl_print(&stream, u8") because the token buffer is already full (try increasing the token buffer size)");
		}
		return MGL_FALSE;
	}

	state->params->out_tokens[state->token_count] = *token;
	state->token_count += 1;
	return MGL_TRUE;
}

static mgl_bool_t read_token(lexer_state_t* state)
{
	mrl_shader_token_t token;

	token.tinfo = NULL;
	token.attribute = NULL;

	mgl_u64_t attribute_it = 0;

	// Helper macros
#define SINGLE_CHAR_TOK(chr, t) else if (*state->it == chr) { token.tinfo = &t; ++state->it; }
#define TWO_CHAR_TOK(chr, t) else if (state->it[0] == chr[0] && state->it[1] == chr[1]) { token.tinfo = &t; state->it += 2; }
#define KEYWORD_TOK(str, t) if (token.tinfo == NULL && *state->it == str[0]) {\
		attribute_it = 0;\
		for (;;)\
		{\
			if (str[attribute_it] == '\0')\
			{\
				if (is_alpha(state->it[attribute_it]) ||\
					is_numeric(state->it[attribute_it]))\
					break;\
				token.tinfo = &t;\
				state->it += attribute_it;\
				break;\
			}\
			else if (state->it[attribute_it] != str[attribute_it])\
				break;\
			++attribute_it;\
		}}

	// Check whitespace
	if (is_white_space(*state->it))
	{
		if (*state->it == '\0')
			state->finished = MGL_TRUE;
		else
		{
			if (*state->it == '\n')
				++state->line_count;
			state->it += 1;
		}
		return MGL_TRUE;
	}

	// String literal
	else if (*state->it == '"')
	{
		for (attribute_it = 0;; ++attribute_it)
		{
			if (state->it[attribute_it + 1] == '"' && state->it[attribute_it] != '\\')
				break;
			if (state->it[attribute_it + 1] == '\0')
			{
				mgl_buffer_stream_t stream;
				mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
				mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
				mgl_print(&stream, u8"Failed to get string literal on line ");
				mgl_print_u64(&stream, state->line_count + 1, 10);
				mgl_print(&stream, u8", missing end '\"' ('");
				for (mgl_u64_t i = 0; i < 32 && state->it[i] != '\0'; ++i)
					mgl_put_char(&stream, state->it[i]);
				mgl_print(&stream, u8"')");
				return MGL_FALSE;
			}
		}
		mgl_error_t err = mgl_allocate(state->params->allocator, attribute_it + 1, (void**)&token.attribute);
		if (err != MGL_ERROR_NONE)
		{
			if (state->params->error_buffer != NULL)
			{
				mgl_buffer_stream_t stream;
				mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
				mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
				mgl_print(&stream, u8"Failed to allocate string literal on line ");
				mgl_print_u64(&stream, state->line_count + 1, 10);
				mgl_print(&stream, u8" (");
				mgl_print(&stream, mgl_get_error_string(err));
				mgl_print(&stream, u8")");
			}
			return MGL_FALSE;
		}

		token.tinfo = &MRL_SHADER_TINFO_STRING_LITERAL;
		token.attribute[attribute_it] = '\0';
		for (mgl_u64_t i = 0; i < attribute_it; ++i)
			token.attribute[i] = state->it[i + 1];
		state->it += attribute_it + 2;
	}

	// Punctuation
	SINGLE_CHAR_TOK('{', MRL_SHADER_TINFO_OPEN_BRACE)
	SINGLE_CHAR_TOK('}', MRL_SHADER_TINFO_CLOSE_BRACE)
	SINGLE_CHAR_TOK('(', MRL_SHADER_TINFO_OPEN_PARENTHESIS)
	SINGLE_CHAR_TOK(')', MRL_SHADER_TINFO_CLOSE_PARENTHESIS)
	SINGLE_CHAR_TOK('[', MRL_SHADER_TINFO_OPEN_BRACKETS)
	SINGLE_CHAR_TOK(']', MRL_SHADER_TINFO_CLOSE_BRACKETS)
	SINGLE_CHAR_TOK('.', MRL_SHADER_TINFO_PERIOD)
	SINGLE_CHAR_TOK(',', MRL_SHADER_TINFO_COMMA)
	SINGLE_CHAR_TOK(';', MRL_SHADER_TINFO_SEMICOLON)
	SINGLE_CHAR_TOK(':', MRL_SHADER_TINFO_COLON)

	// Operators
	TWO_CHAR_TOK("++", MRL_SHADER_TINFO_PREFIX_INCREMENT)
	TWO_CHAR_TOK("--", MRL_SHADER_TINFO_PREFIX_DECREMENT)
	TWO_CHAR_TOK("<=", MRL_SHADER_TINFO_LESS_THAN_OR_EQUAL_TO)
	TWO_CHAR_TOK(">=", MRL_SHADER_TINFO_GREATER_THAN_OR_EQUAL_TO)
	TWO_CHAR_TOK("==", MRL_SHADER_TINFO_EQUALITY)
	TWO_CHAR_TOK("!=", MRL_SHADER_TINFO_NOT_EQUALITY)
	TWO_CHAR_TOK("&&", MRL_SHADER_TINFO_AND)
	TWO_CHAR_TOK("||", MRL_SHADER_TINFO_OR)
	TWO_CHAR_TOK("^^", MRL_SHADER_TINFO_XOR)
	TWO_CHAR_TOK("+=", MRL_SHADER_TINFO_ADDITION_ASSIGNMENT)
	TWO_CHAR_TOK("-=", MRL_SHADER_TINFO_SUBTRACTION_ASSIGNMENT)
	TWO_CHAR_TOK("*=", MRL_SHADER_TINFO_MULTIPLICATION_ASSIGNMENT)
	TWO_CHAR_TOK("/=", MRL_SHADER_TINFO_DIVISION_ASSIGNMENT)
	SINGLE_CHAR_TOK('+', MRL_SHADER_TINFO_ADD)
	SINGLE_CHAR_TOK('-', MRL_SHADER_TINFO_SUBTRACT)
	SINGLE_CHAR_TOK('*', MRL_SHADER_TINFO_MULTIPLY)
	SINGLE_CHAR_TOK('/', MRL_SHADER_TINFO_DIVIDE)
	SINGLE_CHAR_TOK('!', MRL_SHADER_TINFO_BOOLEAN_NEGATION)
	SINGLE_CHAR_TOK('<', MRL_SHADER_TINFO_LESS_THAN)
	SINGLE_CHAR_TOK('>', MRL_SHADER_TINFO_GREATER_THAN)
	SINGLE_CHAR_TOK('=', MRL_SHADER_TINFO_ASSIGNMENT)

	// Types
	KEYWORD_TOK(u8"void", MRL_SHADER_TINFO_VOID)
	KEYWORD_TOK(u8"bool", MRL_SHADER_TINFO_BOOL)
	KEYWORD_TOK(u8"i32", MRL_SHADER_TINFO_I32)
	KEYWORD_TOK(u8"u32", MRL_SHADER_TINFO_U32)
	KEYWORD_TOK(u8"f32", MRL_SHADER_TINFO_F32)
	KEYWORD_TOK(u8"boolv2", MRL_SHADER_TINFO_BOOLV2)
	KEYWORD_TOK(u8"i32v2", MRL_SHADER_TINFO_I32V2)
	KEYWORD_TOK(u8"u32v2", MRL_SHADER_TINFO_U32V2)
	KEYWORD_TOK(u8"f32v2", MRL_SHADER_TINFO_F32V2)
	KEYWORD_TOK(u8"boolv3", MRL_SHADER_TINFO_BOOLV3)
	KEYWORD_TOK(u8"i32v3", MRL_SHADER_TINFO_I32V3)
	KEYWORD_TOK(u8"u32v3", MRL_SHADER_TINFO_U32V3)
	KEYWORD_TOK(u8"f32v3", MRL_SHADER_TINFO_F32V3)
	KEYWORD_TOK(u8"boolv4", MRL_SHADER_TINFO_BOOLV4)
	KEYWORD_TOK(u8"i32v4", MRL_SHADER_TINFO_I32V4)
	KEYWORD_TOK(u8"u32v4", MRL_SHADER_TINFO_U32V4)
	KEYWORD_TOK(u8"f32v4", MRL_SHADER_TINFO_F32V4)
	KEYWORD_TOK(u8"f32m2x2", MRL_SHADER_TINFO_F32M2X2)
	KEYWORD_TOK(u8"f32m2x3", MRL_SHADER_TINFO_F32M2X3)
	KEYWORD_TOK(u8"f32m2x4", MRL_SHADER_TINFO_F32M2X4)
	KEYWORD_TOK(u8"f32m3x2", MRL_SHADER_TINFO_F32M3X2)
	KEYWORD_TOK(u8"f32m3x3", MRL_SHADER_TINFO_F32M3X3)
	KEYWORD_TOK(u8"f32m3x4", MRL_SHADER_TINFO_F32M3X4)
	KEYWORD_TOK(u8"f32m4x2", MRL_SHADER_TINFO_F32M4X2)
	KEYWORD_TOK(u8"f32m4x3", MRL_SHADER_TINFO_F32M4X3)
	KEYWORD_TOK(u8"f32m4x4", MRL_SHADER_TINFO_F32M4X4)

	// Sampler types
	KEYWORD_TOK(u8"i32sampler1d", MRL_SHADER_TINFO_I32_SAMPLER_1D)
	KEYWORD_TOK(u8"u32sampler1d", MRL_SHADER_TINFO_U32_SAMPLER_1D)
	KEYWORD_TOK(u8"f32sampler1d", MRL_SHADER_TINFO_F32_SAMPLER_1D)
	KEYWORD_TOK(u8"i32sampler2d", MRL_SHADER_TINFO_I32_SAMPLER_2D)
	KEYWORD_TOK(u8"u32sampler2d", MRL_SHADER_TINFO_U32_SAMPLER_2D)
	KEYWORD_TOK(u8"f32sampler2d", MRL_SHADER_TINFO_F32_SAMPLER_2D)
	KEYWORD_TOK(u8"i32sampler3d", MRL_SHADER_TINFO_I32_SAMPLER_3D)
	KEYWORD_TOK(u8"u32sampler3d", MRL_SHADER_TINFO_U32_SAMPLER_3D)
	KEYWORD_TOK(u8"f32sampler3d", MRL_SHADER_TINFO_F32_SAMPLER_3D)
	KEYWORD_TOK(u8"i32sampler_cube", MRL_SHADER_TINFO_I32_SAMPLER_CUBE)
	KEYWORD_TOK(u8"u32sampler_cube", MRL_SHADER_TINFO_U32_SAMPLER_CUBE)
	KEYWORD_TOK(u8"f32sampler_cube", MRL_SHADER_TINFO_F32_SAMPLER_CUBE)

	// Depth sampler types
	KEYWORD_TOK(u8"dsampler1d", MRL_SHADER_TINFO_DSAMPLER_1D)
	KEYWORD_TOK(u8"dsampler2d", MRL_SHADER_TINFO_DSAMPLER_2D)
	KEYWORD_TOK(u8"dsampler3d", MRL_SHADER_TINFO_DSAMPLER_3D)
	KEYWORD_TOK(u8"dsampler_cube", MRL_SHADER_TINFO_DSAMPLER_CUBE)

	// Boolean literals
	KEYWORD_TOK(u8"true", MRL_SHADER_TINFO_TRUE)
	KEYWORD_TOK(u8"false", MRL_SHADER_TINFO_FALSE)

	// Other keywords
	KEYWORD_TOK(u8"version", MRL_SHADER_TINFO_VERSION)
	KEYWORD_TOK(u8"input", MRL_SHADER_TINFO_INPUT)
	KEYWORD_TOK(u8"output", MRL_SHADER_TINFO_OUTPUT)
	KEYWORD_TOK(u8"uniform", MRL_SHADER_TINFO_UNIFORM)


	// Float literal
	if (token.tinfo == NULL && is_numeric(*state->it))
	{
		mgl_bool_t found_dot = MGL_FALSE;
		mgl_bool_t is_float = MGL_FALSE;

		// Get size
		for (attribute_it = 1;; ++attribute_it)
		{
			if (state->it[attribute_it] == '.')
			{
				if (found_dot)
					break;
				else
					found_dot = MGL_TRUE;
			}
			else if (!is_numeric(state->it[attribute_it]))
			{
				if (is_alpha(state->it[attribute_it]) && state->it[attribute_it] != 'f')
					break;
				if (found_dot && state->it[attribute_it - 1] != '.')
					is_float = MGL_TRUE;
				break;
			}
		}

		if (is_float)
		{
			// Allocate attribute
			mgl_error_t err = mgl_allocate(state->params->allocator, attribute_it + 1, (void**)&token.attribute);
			if (err != MGL_ERROR_NONE)
			{
				if (state->params->error_buffer != NULL)
				{
					mgl_buffer_stream_t stream;
					mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
					mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
					mgl_print(&stream, u8"Failed to allocate float literal on line ");
					mgl_print_u64(&stream, state->line_count + 1, 10);
					mgl_print(&stream, u8" (");
					mgl_print(&stream, mgl_get_error_string(err));
					mgl_print(&stream, u8")");
				}
				return MGL_FALSE;
			}

			token.tinfo = &MRL_SHADER_TINFO_FLOAT_LITERAL;
			token.attribute[attribute_it] = '\0';
			for (mgl_u64_t i = 0; i < attribute_it; ++i)
				token.attribute[i] = state->it[i];
			state->it += attribute_it;
			if (state->it[0] == 'f')
				++state->it;
		}
	}

	// Int literal
	if (token.tinfo == NULL && is_numeric(*state->it))
	{
		mgl_bool_t is_int = MGL_FALSE;

		// Get size
		for (attribute_it = 1;; ++attribute_it)
			if (!is_numeric(state->it[attribute_it]))
			{
				if (is_alpha(state->it[attribute_it]))
					break;
				is_int = MGL_TRUE;
				break;
			}

		if (is_int)
		{
			// Allocate attribute
			mgl_error_t err = mgl_allocate(state->params->allocator, attribute_it + 1, (void**)&token.attribute);
			if (err != MGL_ERROR_NONE)
			{
				if (state->params->error_buffer != NULL)
				{
					mgl_buffer_stream_t stream;
					mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
					mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
					mgl_print(&stream, u8"Failed to allocate int literal on line ");
					mgl_print_u64(&stream, state->line_count + 1, 10);
					mgl_print(&stream, u8" (");
					mgl_print(&stream, mgl_get_error_string(err));
					mgl_print(&stream, u8")");
				}
				return MGL_FALSE;
			}

			token.tinfo = &MRL_SHADER_TINFO_INT_LITERAL;
			token.attribute[attribute_it] = '\0';
			for (mgl_u64_t i = 0; i < attribute_it; ++i)
				token.attribute[i] = state->it[i];
			state->it += attribute_it;
		}
	}

	// Identifier
	if (token.tinfo == NULL && (is_alpha(*state->it) || *state->it == '_'))
	{
		// Get size
		for (attribute_it = 1;; ++attribute_it)
			if (!is_alpha(state->it[attribute_it]) &&
				!is_numeric(state->it[attribute_it]) &&
				state->it[attribute_it] != '_')
				break;

		// Allocate attribute
		mgl_error_t err = mgl_allocate(state->params->allocator, attribute_it + 1, (void**)&token.attribute);
		if (err != MGL_ERROR_NONE)
		{
			if (state->params->error_buffer != NULL)
			{
				mgl_buffer_stream_t stream;
				mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
				mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
				mgl_print(&stream, u8"Failed to allocate identifier on line ");
				mgl_print_u64(&stream, state->line_count + 1, 10);
				mgl_print(&stream, u8" (");
				mgl_print(&stream, mgl_get_error_string(err));
				mgl_print(&stream, u8")");
			}
			return MGL_FALSE;
		}

		token.tinfo = &MRL_SHADER_TINFO_IDENTIFIER;
		token.attribute[attribute_it] = '\0';
		for (mgl_u64_t i = 0; i < attribute_it; ++i)
			token.attribute[i] = state->it[i];
		state->it += attribute_it;
	}

	if (token.tinfo == NULL)
	{
		if (state->params->error_buffer != NULL)
		{
			mgl_buffer_stream_t stream;
			mgl_mem_set(state->params->error_buffer, state->params->error_buffer_size, 0);
			mgl_init_buffer_stream(&stream, state->params->error_buffer, state->params->error_buffer_size);
			mgl_print(&stream, u8"Unknown token found on line ");
			mgl_print_u64(&stream, state->line_count + 1, 10);
			mgl_print(&stream, u8" ('");
			for (mgl_u64_t i = 0; i < 32 && state->it[i] != '\0'; ++i)
				mgl_put_char(&stream, state->it[i]);
			mgl_print(&stream, u8"')");
		}
		return MGL_FALSE;
	}

	return put_token(state, &token);
}

MRL_API mgl_bool_t mrl_run_shader_lexer(const mrl_shader_lexer_params_t * params)
{
	MGL_DEBUG_ASSERT(params != NULL);
	MGL_DEBUG_ASSERT(params->src != NULL && params->out_tokens != NULL && params->out_token_count != NULL && params->max_token_count > 0);

	lexer_state_t state;
	state.params = params;
	state.it = params->src;
	state.token_count = 0;
	state.finished = MGL_FALSE;
	state.line_count = 0;

	// For every token
	while (!state.finished)
		// Read next token
		if (!read_token(&state))
		{
			// Deallocate token attributes
			for (mgl_u64_t i = 0; i < state.token_count; ++i)
				if (state.params->out_tokens[i].attribute != NULL)
					mgl_deallocate(params->allocator, state.params->out_tokens[i].attribute);
			return MGL_FALSE;
		}

	*params->out_token_count = state.token_count;

	return MGL_TRUE;
}

MRL_API void mrl_deallocate_shader_lexer_tokens(mrl_shader_token_t * tokens, mgl_u64_t token_count, void * allocator)
{
	MGL_DEBUG_ASSERT(tokens != NULL && allocator != NULL);

	for (mgl_u64_t i = 0; i < token_count; ++i)
		if (tokens[i].attribute != NULL)
			MGL_DEBUG_ASSERT(mgl_deallocate(allocator, tokens[i].attribute) == MGL_ERROR_NONE);
}
