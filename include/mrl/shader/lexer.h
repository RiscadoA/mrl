#ifndef MRL_SHADER_LEXER_H
#define MRL_SHADER_LEXER_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mgl/type.h>

#include <mrl/api_utils.h>

	// Token types
	enum
	{
		MRL_SHADER_TOKEN_PUNCTUATION,
		MRL_SHADER_TOKEN_OPERATOR,
		MRL_SHADER_TOKEN_TYPE,
		MRL_SHADER_TOKEN_SAMPLER,
		MRL_SHADER_TOKEN_DSAMPLER,
		MRL_SHADER_TOKEN_IDENTIFIER,

		// Literals
		MRL_SHADER_TOKEN_TRUE,
		MRL_SHADER_TOKEN_FALSE,
		MRL_SHADER_TOKEN_INT_LITERAL,
		MRL_SHADER_TOKEN_FLOAT_LITERAL,
		MRL_SHADER_TOKEN_STRING_LITERAL,

		// Other keywords
		MRL_SHADER_TOKEN_VERSION,
		MRL_SHADER_TOKEN_INPUT,
		MRL_SHADER_TOKEN_OUTPUT,
		MRL_SHADER_TOKEN_UNIFORM,
	};

	// Punctuation types
	enum
	{
		MRL_SHADER_PUNCTUATION_OPEN_BRACE, // {
		MRL_SHADER_PUNCTUATION_CLOSE_BRACE, // }
		MRL_SHADER_PUNCTUATION_OPEN_PARENTHESIS, // (
		MRL_SHADER_PUNCTUATION_CLOSE_PARENTHESIS, // )
		MRL_SHADER_PUNCTUATION_OPEN_BRACKETS, // [
		MRL_SHADER_PUNCTUATION_CLOSE_BRACKETS, // ]
		MRL_SHADER_PUNCTUATION_PERIOD, // .
		MRL_SHADER_PUNCTUATION_COMMA, // ,
		MRL_SHADER_PUNCTUATION_SEMICOLON, // ;
		MRL_SHADER_PUNCTUATION_COLON, // :
	};

	// Operators
	enum
	{
		MRL_SHADER_OPERATOR_FIELD_SELECTOR, // . (this token is not generated by the lexer, instead it generates MRL_SHADER_PUNCTUATION_PERIOD)
		MRL_SHADER_OPERATOR_ADD, // +
		MRL_SHADER_OPERATOR_SUBTRACT, // -
		MRL_SHADER_OPERATOR_MULTIPLY, // *
		MRL_SHADER_OPERATOR_DIVIDE, // /
		MRL_SHADER_OPERATOR_PREFIX_INCREMENT, // ++
		MRL_SHADER_OPERATOR_PREFIX_DECREMENT, // --
		MRL_SHADER_OPERATOR_POSTFIX_INCREMENT, // ++ (this token is not generated by the lexer, instead it generates MRL_SHADER_OPERATOR_POSTFIX_INCREMENT)
		MRL_SHADER_OPERATOR_POSTFIX_DECREMENT, // -- (this token is not generated by the lexer, instead it generates MRL_SHADER_OPERATOR_PREFIX_DECREMENT)
		MRL_SHADER_OPERATOR_UNARY_PLUS, // + (this token is not generated by the lexer, instead it generates MRL_SHADER_OPERATOR_ADD)
		MRL_SHADER_OPERATOR_UNARY_MINUS, // - (this token is not generated by the lexer, instead it generates MRL_SHADER_OPERATOR_SUBTRACT)
		MRL_SHADER_OPERATOR_BOOLEAN_NEGATION, // !
		MRL_SHADER_OPERATOR_LESS_THAN, // <
		MRL_SHADER_OPERATOR_GREATER_THAN, // >
		MRL_SHADER_OPERATOR_LESS_THAN_OR_EQUAL_TO, // <=
		MRL_SHADER_OPERATOR_GREATER_THAN_OR_EQUAL_TO, // >=
		MRL_SHADER_OPERATOR_EQUALITY, // ==
		MRL_SHADER_OPERATOR_NOT_EQUALITY, // !=
		MRL_SHADER_OPERATOR_AND, // &&
		MRL_SHADER_OPERATOR_OR, // ||
		MRL_SHADER_OPERATOR_XOR, // ^^
		MRL_SHADER_OPERATOR_ASSIGNMENT, // =
		MRL_SHADER_OPERATOR_ADDITION_ASSIGNMENT, // +=
		MRL_SHADER_OPERATOR_SUBTRACTION_ASSIGNMENT, // -=
		MRL_SHADER_OPERATOR_MULTIPLICATION_ASSIGNMENT, // *=
		MRL_SHADER_OPERATOR_DIVISION_ASSIGNMENT, // /=
	};

	// Types
	enum
	{
		MRL_SHADER_TYPE_VOID,

		MRL_SHADER_TYPE_BOOL,
		MRL_SHADER_TYPE_I32,
		MRL_SHADER_TYPE_U32,
		MRL_SHADER_TYPE_F32,

		MRL_SHADER_TYPE_BOOLV2,
		MRL_SHADER_TYPE_I32V2,
		MRL_SHADER_TYPE_U32V2,
		MRL_SHADER_TYPE_F32V2,

		MRL_SHADER_TYPE_BOOLV3,
		MRL_SHADER_TYPE_I32V3,
		MRL_SHADER_TYPE_U32V3,
		MRL_SHADER_TYPE_F32V3,

		MRL_SHADER_TYPE_BOOLV4,
		MRL_SHADER_TYPE_I32V4,
		MRL_SHADER_TYPE_U32V4,
		MRL_SHADER_TYPE_F32V4,

		MRL_SHADER_TYPE_F32M2X2,
		MRL_SHADER_TYPE_F32M2X3,
		MRL_SHADER_TYPE_F32M2X4,
		MRL_SHADER_TYPE_F32M3X2,
		MRL_SHADER_TYPE_F32M3X3,
		MRL_SHADER_TYPE_F32M3X4,
		MRL_SHADER_TYPE_F32M4X2,
		MRL_SHADER_TYPE_F32M4X3,
		MRL_SHADER_TYPE_F32M4X4,
	};

	// Sampler types
	enum
	{
		MRL_SHADER_I32_SAMPLER_1D,
		MRL_SHADER_U32_SAMPLER_1D,
		MRL_SHADER_F32_SAMPLER_1D,
		MRL_SHADER_I32_SAMPLER_2D,
		MRL_SHADER_U32_SAMPLER_2D,
		MRL_SHADER_F32_SAMPLER_2D,
		MRL_SHADER_I32_SAMPLER_3D,
		MRL_SHADER_U32_SAMPLER_3D,
		MRL_SHADER_F32_SAMPLER_3D,
		MRL_SHADER_I32_SAMPLER_CUBE,
		MRL_SHADER_U32_SAMPLER_CUBE,
		MRL_SHADER_F32_SAMPLER_CUBE,
	};

	// Depth sampler types
	enum
	{
		MRL_SHADER_DSAMPLER_1D,
		MRL_SHADER_DSAMPLER_2D,
		MRL_SHADER_DSAMPLER_3D,
		MRL_SHADER_DSAMPLER_CUBE,
	};

	/// <summary>
	///		Shader token type information.
	/// </summary>
	typedef struct
	{
		mgl_enum_u8_t id;
		union
		{
			mgl_enum_u8_t punctuation_id;
			mgl_enum_u8_t operator_id;
			mgl_enum_u8_t type_id;
			mgl_enum_u8_t sampler_id;
			mgl_enum_u8_t dsampler_id;
		};
		const mgl_chr8_t* name;

	} mrl_shader_token_type_info_t;

	/// <summary>
	///		Shader token.
	/// </summary>
	typedef struct
	{
		const mrl_shader_token_type_info_t* tinfo;
		mgl_chr8_t* attribute;
	} mrl_shader_token_t;

	// Punctuation
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_OPEN_BRACE = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_OPEN_BRACE, u8"{" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_CLOSE_BRACE = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_CLOSE_BRACE, u8"}" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_OPEN_PARENTHESIS = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_OPEN_PARENTHESIS, u8"(" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_CLOSE_PARENTHESIS = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_CLOSE_PARENTHESIS, u8")" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_OPEN_BRACKETS = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_OPEN_BRACKETS, u8"[" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_CLOSE_BRACKETS = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_CLOSE_BRACKETS, u8"]" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_PERIOD = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_PERIOD, u8"." };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_COMMA = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_COMMA, u8"," };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_SEMICOLON = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_SEMICOLON, u8";" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_COLON = { MRL_SHADER_TOKEN_PUNCTUATION, MRL_SHADER_PUNCTUATION_COLON, u8":" };

	// Operators
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_FIELD_SELECTOR = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_FIELD_SELECTOR, u8"." };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_ADD = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_ADD, u8"+" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_SUBTRACT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_SUBTRACT, u8"-" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_MULTIPLY = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_MULTIPLY, u8"*" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_DIVIDE = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_DIVIDE, u8"/" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_PREFIX_INCREMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_PREFIX_INCREMENT, u8"++" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_PREFIX_DECREMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_PREFIX_DECREMENT, u8"--" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_POSTFIX_INCREMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_POSTFIX_INCREMENT, u8"++" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_POSTFIX_DECREMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_POSTFIX_DECREMENT, u8"--" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_UNARY_PLUS = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_UNARY_PLUS, u8"+" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_UNARY_MINUS = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_UNARY_MINUS, u8"-" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_BOOLEAN_NEGATION = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_BOOLEAN_NEGATION, u8"!" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_LESS_THAN = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_LESS_THAN, u8"<" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_GREATER_THAN = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_GREATER_THAN, u8">" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_LESS_THAN_OR_EQUAL_TO = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_LESS_THAN_OR_EQUAL_TO, u8"<=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_GREATER_THAN_OR_EQUAL_TO = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_GREATER_THAN_OR_EQUAL_TO, u8">=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_EQUALITY = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_EQUALITY, u8"==" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_NOT_EQUALITY = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_NOT_EQUALITY, u8"!=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_AND = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_AND, u8"&&" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_OR = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_OR, u8"||" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_XOR = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_XOR, u8"^^" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_ASSIGNMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_ASSIGNMENT, u8"=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_ADDITION_ASSIGNMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_ADDITION_ASSIGNMENT, u8"+=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_SUBTRACTION_ASSIGNMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_SUBTRACTION_ASSIGNMENT, u8"-=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_MULTIPLICATION_ASSIGNMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_MULTIPLICATION_ASSIGNMENT, u8"*=" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_DIVISION_ASSIGNMENT = { MRL_SHADER_TOKEN_OPERATOR, MRL_SHADER_OPERATOR_DIVISION_ASSIGNMENT, u8"/=" };

	// Types
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_VOID = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_VOID, u8"void" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_BOOL = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_BOOL, u8"bool" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_I32, u8"i32" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_U32, u8"u32" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32, u8"f32" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_BOOLV2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_BOOLV2, u8"boolv2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32V2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_I32V2, u8"i32v2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32V2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_U32V2, u8"u32v2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32V2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32V2, u8"f32v2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_BOOLV3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_BOOLV3, u8"boolv3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32V3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_I32V3, u8"i32v3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32V3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_U32V3, u8"u32v3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32V3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32V3, u8"f32v3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_BOOLV4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_BOOLV4, u8"boolv4" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32V4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_I32V4, u8"i32v4" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32V4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_U32V4, u8"u32v4" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32V4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32V4, u8"f32v4" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M2X2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M2X2, u8"f32m2x2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M2X3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M2X3, u8"f32m2x3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M2X4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M2X4, u8"f32m2x4" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M3X2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M3X2, u8"f32m3x2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M3X3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M3X3, u8"f32m3x3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M3X4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M3X4, u8"f32m3x4" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M4X2 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M4X2, u8"f32m4x2" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M4X3 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M4X3, u8"f32m4x3" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32M4X4 = { MRL_SHADER_TOKEN_TYPE, MRL_SHADER_TYPE_F32M4X4, u8"f32m4x4" };

	// Sampler types
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32_SAMPLER_1D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_I32_SAMPLER_1D, u8"i32sampler1d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32_SAMPLER_1D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_U32_SAMPLER_1D, u8"u32sampler1d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32_SAMPLER_1D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_F32_SAMPLER_1D, u8"f32sampler1d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32_SAMPLER_2D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_I32_SAMPLER_2D, u8"i32sampler2d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32_SAMPLER_2D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_U32_SAMPLER_2D, u8"u32sampler2d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32_SAMPLER_2D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_F32_SAMPLER_2D, u8"f32sampler2d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32_SAMPLER_3D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_I32_SAMPLER_3D, u8"i32sampler3d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32_SAMPLER_3D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_U32_SAMPLER_3D, u8"u32sampler3d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32_SAMPLER_3D = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_F32_SAMPLER_3D, u8"f32sampler3d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_I32_SAMPLER_CUBE = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_I32_SAMPLER_CUBE, u8"i32sampler_cube" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_U32_SAMPLER_CUBE = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_U32_SAMPLER_CUBE, u8"u32sampler_cube" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_F32_SAMPLER_CUBE = { MRL_SHADER_TOKEN_SAMPLER, MRL_SHADER_F32_SAMPLER_CUBE, u8"f32sampler_cube" };

	// Depth sampler types
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_DSAMPLER_1D = { MRL_SHADER_TOKEN_DSAMPLER, MRL_SHADER_DSAMPLER_1D, u8"dsampler1d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_DSAMPLER_2D = { MRL_SHADER_TOKEN_DSAMPLER, MRL_SHADER_DSAMPLER_2D, u8"dsampler2d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_DSAMPLER_3D = { MRL_SHADER_TOKEN_DSAMPLER, MRL_SHADER_DSAMPLER_3D, u8"dsampler3d" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_DSAMPLER_CUBE = { MRL_SHADER_TOKEN_DSAMPLER, MRL_SHADER_DSAMPLER_CUBE, u8"dsampler_cube" };

	// Literals
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_TRUE = { MRL_SHADER_TOKEN_TRUE, 0, u8"true" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_FALSE = { MRL_SHADER_TOKEN_FALSE, 0, u8"false" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_INT_LITERAL = { MRL_SHADER_TOKEN_INT_LITERAL, 0, u8"int_literal" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_FLOAT_LITERAL = { MRL_SHADER_TOKEN_FLOAT_LITERAL, 0, u8"float_literal" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_STRING_LITERAL = { MRL_SHADER_TOKEN_STRING_LITERAL, 0, u8"string_literal" };

	// Others
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_IDENTIFIER = { MRL_SHADER_TOKEN_IDENTIFIER, 0, u8"identifier" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_VERSION = { MRL_SHADER_TOKEN_VERSION, 0, u8"version" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_INPUT = { MRL_SHADER_TOKEN_INPUT, 0, u8"input" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_OUTPUT = { MRL_SHADER_TOKEN_OUTPUT, 0, u8"output" };
	static const mrl_shader_token_type_info_t MRL_SHADER_TINFO_UNIFORM = { MRL_SHADER_TOKEN_UNIFORM, 0, u8"uniform" };

	typedef struct
	{
		/// <summary>
		///		Allocator used to allocate string literals and identifiers.
		/// </summary>
		void* allocator;

		/// <summary>
		///		Error buffer (optional, can be NULL).
		/// </summary>
		mgl_chr8_t* error_buffer;

		/// <summary>
		///		Error buffer size (should be at least 256).
		/// </summary>
		mgl_u64_t error_buffer_size;

		/// <summary>
		///		Shader source code.
		/// </summary>
		const mgl_chr8_t* src;

		/// <summary>
		///		Max output token count.
		/// </summary>
		mgl_u64_t max_token_count;

		/// <summary>
		///		Pointer to output token count.
		/// </summary>
		mgl_u64_t* out_token_count;

		/// <summary>
		///		Pointer to output tokens.
		/// </summary>
		mrl_shader_token_t* out_tokens;
	} mrl_shader_lexer_params_t;

	/// <summary>
	///		Runs the shader lexer.
	/// </summary>
	/// <param name="params">Lexer params</param>
	/// <returns>MGL_TRUE if the operation was a success, otherwise MGL_FALSE</returns>
	MRL_API mgl_bool_t mrl_run_shader_lexer(const mrl_shader_lexer_params_t* params);

	/// <summary>
	///		Deallocates the string literals and identifiers allocated by the lexer.
	/// </summary>
	/// <param name="tokens">Pointer to tokens</param>
	/// <param name="token_count">Token count</param>
	/// <param name="allocator">Allocator used</param>
	MRL_API void mrl_deallocate_shader_lexer_tokens(mrl_shader_token_t* tokens, mgl_u64_t token_count, void* allocator);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
