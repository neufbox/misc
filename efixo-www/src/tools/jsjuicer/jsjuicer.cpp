
/* jsjuicer 1.2: Safe whitespace stripping and obfuscation of JavaScript.
 *
 * 	Copyright (c) 2007 Adrian Johnston (adrian3@gmail.com)
 *
 *	Copyright (c) 2002 Douglas Crockford  (www.crockford.com)
 *	  Author of jsmin.c which the minifcation code is based on.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy of
 *	this software and associated documentation files (the "Software"), to deal in
 *	the Software without restriction, including without limitation the rights to
 *	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *	of the Software, and to permit persons to whom the Software is furnished to do
 *	so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	The Software shall be used for Good, not Evil.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

//  * The 1.2 release fixed a VC6 compile bug, opens the output file as binary
//	  instead of text (no \r in output) and avoids changing Prototype's $
//	  varaiable.
//
//	* The 1.1 release added support for IE's conditional comments.
//
// Conceptually this program has the following compiler equivalents.
//
//	preprocessing     ->  read_next_stripped_and_obfuscated_character
//	lexical analysis  ->  minimize_character
//	parsing           ->  minimize_file (Has an LR(1) BNF equivalent.)

#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>

// --------------------------------------------------------------------------

class JavaScript_Juicer
{
public:
	JavaScript_Juicer( const char* command_name );
	~JavaScript_Juicer( void );

	void enable_stripping_comments( bool enabled );
	void enable_stripping_debug( bool enabled );	// lines starting with ;;;
	void enable_minimize( bool enabled );           // delete unnesicary characters
	void enable_obfuscation( bool enabled );        // rename identifiers starting with '$'

	void set_output_file( const char* write_file_name );  // defaults to stdout
	void process_file( const char* read_file_name );      // NULL implies stdin
	void insert_comment( const char* text );

private:
	// Minimization operations.
	enum
	{
		EMIT_A,
		DELETE_A,
		DELETE_B
	};

	typedef std::map<std::vector<char>, std::vector<char> > Obfuscation_Table;

	void parse_error( const char* msg );
	int is_identifier_character( int c );

	int read_next_character_no_lookahead( void );
	int read_next_character( void );
	int peek_next_character( unsigned index );
	int read_next_stripped_and_obfuscated_character( void );

	std::vector<char>& generate_obfuscation_id( void );
	void obfuscate_identifier( void );

	void minimize_check_string_literal( void );
	void minimize_check_regular_expression( void );
	void minimize_check_conditional_comment( void );
	void minimize_character( int command );
	void minimize_file( void );

	const char*       m_command_name;

	// file handling
	FILE*             m_read_file;
	FILE*             m_write_file;
        const char*       m_read_file_name;
	int               m_read_file_line;

	// stripping
	bool              m_strip_comments;
	bool              m_strip_debug;      // remove lines starting with ;;;

	// obfuscation
	bool              m_obfuscate;
	Obfuscation_Table m_obfuscation_table;
	int               m_current_obfuscation_id;
	std::vector<char> m_obfuscation_id_string;
	std::vector<char> m_obfuscation_temp_string;

	// minimization
	bool              m_minimize;
	int               m_a;
	int               m_b;
	std::vector<int>  m_lookahead_buffer;
	bool              m_b_was_deleted;        // Avoids joining "+ +" or "- -"
};

// --------------------------------------------------------------------------

JavaScript_Juicer::JavaScript_Juicer( const char* command_name )
{
	m_command_name = command_name;
	m_write_file = stdout;
	m_strip_comments = false;
	m_strip_debug = false;
	m_minimize = false;
	m_obfuscate = false;
	m_current_obfuscation_id = 0;
}

// --------------------------------------------------------------------------

JavaScript_Juicer::~JavaScript_Juicer( void )
{
	if( m_write_file != stdout )
	{
		fclose( m_write_file );
	}
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::enable_stripping_comments( bool enabled )
{
	m_strip_comments = enabled;
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::enable_stripping_debug( bool enabled )
{
	m_strip_debug = enabled;
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::enable_minimize( bool enabled )
{
	m_minimize = enabled;
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::enable_obfuscation( bool enabled )
{
	m_obfuscate = enabled;
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::set_output_file( const char* write_file_name )
{
	m_write_file = fopen( write_file_name, "w" );
	if( m_write_file == NULL )
	{
		fprintf( stderr, "%s error: Cannot open file for writing: %s\n", m_command_name, write_file_name );
		exit( -1 );
	}
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::process_file( const char* read_file_name )
{
	if( read_file_name != NULL )
	{
		m_read_file = fopen( read_file_name, "r" );
		if( m_read_file == NULL )
		{
			fprintf( stderr, "%s error: Cannot open file for reading: %s\n", m_command_name, read_file_name );
			exit( -1 );
		}
		m_read_file_name = read_file_name;
	}
	else
	{
		m_read_file = stdin;
		read_file_name = "<stdin>";
	}

	m_read_file_line = 1;

	m_lookahead_buffer.clear();

	if( m_minimize )
	{
		m_strip_comments = true; // minimization code assumes this.
		minimize_file();
	}
	else
	{
		int c;
		while( ( c = read_next_stripped_and_obfuscated_character() ) != EOF )
		{
			putc( c, m_write_file );
		}
	}

	if( m_read_file != stdin )
	{
		fclose( m_read_file );
	}
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::insert_comment( const char* text )
{
	// Add the '//'s just to be safe.
	fprintf( m_write_file, "// %s\n", text );
}

// --------------------------------------------------------------------------

void JavaScript_Juicer::parse_error( const char* msg )
{
	fprintf( stderr, "%s error: %s: %d: %s.\n", m_command_name, m_read_file_name, m_read_file_line, msg );
	exit( -1 );
}

// --------------------------------------------------------------------------
// is_identifier_character -- return true if the character is a letter, digit, underscore,
//		dollar sign, or non-ASCII character. Or '\\' ?!?
int JavaScript_Juicer::is_identifier_character( int c )
{
	return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'Z') || c == '_' || c == '$' || c == '\\' || c > 126;
}

// --------------------------------------------------------------------------
// read_next_character_no_lookahead -- read and filter next character from
//		file without using the lookahead.
int JavaScript_Juicer::read_next_character_no_lookahead( void )
{
	int c = getc( m_read_file );

	if( c == '\n' )
	{
		++m_read_file_line;
	}

	if( !m_minimize || ( c >= ' ' && c != 127 ) || c == '\n' || c == EOF )
	{
		return c;
	}

	if( c == '\r')
	{
		return '\n';
	}

	return ' ';
}


// --------------------------------------------------------------------------
// read_next_character -- return the next character character from stdin
//		or lookahead. If the character is not printable translate
//		it to a space or linefeed.
int JavaScript_Juicer::read_next_character( void )
{
	if( !m_lookahead_buffer.empty() )
	{
		int c = m_lookahead_buffer.back();
		m_lookahead_buffer.pop_back();
		return c;
	}

	return read_next_character_no_lookahead();
}

// --------------------------------------------------------------------------
// peek_next_character -- read the next character or (optionally some number
//		of characters ahead) without removing it.

int JavaScript_Juicer::peek_next_character( unsigned index )
{
	if( index == 0 )
	{
		// Fast path for common case.
		if( !m_lookahead_buffer.empty() )
		{
			return m_lookahead_buffer.back();
		}

		int x = read_next_character_no_lookahead();
		m_lookahead_buffer.push_back( x );
		return x;
	}

	while( m_lookahead_buffer.size() <= index )
	{
		m_lookahead_buffer.insert( m_lookahead_buffer.begin(), read_next_character_no_lookahead() );
	}

	return *( m_lookahead_buffer.end() - ( index + 1 ) );
}

// --------------------------------------------------------------------------
// read_next_stripped_and_obfuscated_character -- get the next character
//		after stripping and obfuscation is done.
int JavaScript_Juicer::read_next_stripped_and_obfuscated_character( void )
{
	int c = read_next_character();

	if( m_strip_comments && c == '/' )
	{
		switch( peek_next_character( 0 ) )
		{
			// this kind of comment
			case '/':
			{
				for( ; ; )
				{
					c = read_next_character();
					if( c <= '\n') {
						return c;
					}
				}
			}

			/* this kind of comment */
			case '*':
			{
				// Ignore conditional IE comments: /*@ ... @*/
				if( peek_next_character( 1 ) == '@' )
				{
					break;
				}

				int line = m_read_file_line;
				read_next_character();

				for( ; ; )
				{
					switch( read_next_character() )
					{
						case '*':
						{
							if( peek_next_character( 0 ) == '/' )
							{
								read_next_character();
								return ' ';
							}
							break;
						}
						case EOF:
						{
							m_read_file_line = line;
							parse_error("Unterminated JavaScript comment");

						}
					}
				}
			}
		}
	}
	else if( m_obfuscate && c == '$' )
	{
		obfuscate_identifier();
	}
	else if( m_strip_debug && c == ';'
			&& peek_next_character( 0 ) == ';'
			&& peek_next_character( 1 ) == ';' )
	{
		// strip debug statements following ';;;'
		for( ; ; )
		{
			c = read_next_character();
			if( c <= '\n')
			{
				return c;
			}
		}
	}

	return c;
}

// --------------------------------------------------------------------------
// emit a new uniqe identifier.
std::vector<char>& JavaScript_Juicer::generate_obfuscation_id( void )
{
	// JavaScript is case sensitive.
	static const char s_symbols[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
		'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z'
	};
	
	static const char s_symbols_size = sizeof s_symbols / sizeof *s_symbols;

	m_obfuscation_id_string.clear();

	int obfuscation_id = m_current_obfuscation_id;
	++m_current_obfuscation_id;

	if( m_current_obfuscation_id <= obfuscation_id )
	{
		// Should be impossible.
		parse_error( "Obfuscated JavaScript identifier overflow" );
	}

	do
	{
		int v = obfuscation_id % s_symbols_size;
		obfuscation_id /= s_symbols_size;

		m_obfuscation_id_string.push_back( s_symbols[v] );
	}
	while( obfuscation_id != 0 );

	// The vector is actually backwards right now but it gets reversed coming off the lookahead.
	return m_obfuscation_id_string;
}

// --------------------------------------------------------------------------
// obfuscate_identifier -- Obfuscate the identifier to be read from the file
//		and leave it in the lookahead buffer.
void JavaScript_Juicer::obfuscate_identifier( void )
{
		// avoids constant buffer reallocation
		m_obfuscation_temp_string.clear();
	
		for( ; ; )
		{
			int c = read_next_character();
			if( is_identifier_character( c ) )
			{
				m_obfuscation_temp_string.push_back( c );
			}
			else
			{
				m_lookahead_buffer.push_back( c );
				break;
			}
		}

		if( m_obfuscation_temp_string.empty() )
		{
			return; // leave the $ identifier unmodified.
		}

		// In a single lookup either find or add an entry for this identifier.
		std::pair<Obfuscation_Table::iterator, bool> insertion = m_obfuscation_table.insert(
			Obfuscation_Table::value_type( m_obfuscation_temp_string, std::vector<char>() ) );
		if( insertion.second == true )
		{
			// Identifier did not already exist, so generate it's obfuscation.
			insertion.first->second = generate_obfuscation_id();
		}

		std::vector<char>& obfuscated_identifier = insertion.first->second;

		// m_obfuscation_temp_string was actually generated backwards, so this works out correctly.
		for( int i=0; i != obfuscated_identifier.size(); ++i )
		{
			// insert was breaking on VC6.
			m_lookahead_buffer.push_back( obfuscated_identifier[i] );
		}
}

// --------------------------------------------------------------------------
// minimize_check_string_literal -- check for a string literal during
//		character minimization operations, between DELETE_A and DELETE_B
//		when m_a == m_b.
void JavaScript_Juicer::minimize_check_string_literal( void )
{
	// Check for a string literal and avoid stripping or obfuscating it.
	if( m_a == '\'' || m_a == '"')
	{
		for( ; ; )
		{
			putc( m_a, m_write_file );
			m_a = read_next_character();
			if( m_a == m_b)
			{
				break;
			}
			if( m_a <= '\n')
			{
				parse_error("Unterminated JavaScript string literal");
			}
			if( m_a == '\\')
			{
				putc( m_a, m_write_file );
				m_a = read_next_character();
			}
		}
	}
}

// --------------------------------------------------------------------------
// minimize_check_string_literal -- check for a regular expression during
//		character minimization operations, after DELETE_B.
void JavaScript_Juicer::minimize_check_regular_expression( void )
{
	// Check for regular expression and avoid stripping or obfuscating it.
	if( m_b == '/' && ( m_a == '(' || m_a == ',' || m_a == '=' ||
						m_a == ':' || m_a == '[' || m_a == '!' || 
						m_a == '&' || m_a == '|' || m_a == '?') )
	{
		putc( m_a, m_write_file );
		putc( m_b, m_write_file );

		for( ; ; )
		{
			m_a = read_next_character();
			if( m_a == '/' )
			{
				break;
			}
			else if( m_a =='\\' )
			{
				putc( m_a, m_write_file );
				m_a = read_next_character();
			}
			else if( m_a <= '\n' )
			{
				parse_error( "Unterminated JavaScript Regular Expression literal" );
			}
			putc( m_a, m_write_file );
		}
		m_b = read_next_stripped_and_obfuscated_character();
	}
}

// --------------------------------------------------------------------------
// minimize_check_conditional_comment -- check for a conditional comment
//		during character minimization operations, after DELETE_B.
//		This really just disables minimization inside conditional comments.
void JavaScript_Juicer::minimize_check_conditional_comment( void )
{
	// Do not minimize conditional IE comments: /*@ ... @*/
	if( m_a == '/' && m_b == '*' && peek_next_character( 0 ) == '@' )
	{
		int line = m_read_file_line;

		// Force a newline at the beginning of a conditional comment.
		putc( '\n', m_write_file );

		putc( m_a, m_write_file );   // '/'
		putc( m_b, m_write_file );   // '*'
		m_a = read_next_character(); // '@'

		m_b = read_next_stripped_and_obfuscated_character();

		for( ; ; )
		{
			// EMIT_A
			putc( m_a, m_write_file );

			// DELETE_A
			m_a = m_b;

			minimize_check_string_literal();

			// DELETE_B
			m_b = read_next_stripped_and_obfuscated_character();

			minimize_check_regular_expression();

			if( m_a == '@' && m_b == '*' && peek_next_character( 0 ) == '/' )
			{
				// There is a rule in the minimizer to preserve the
				// newline at the end of the comment after the '/'.

				putc( m_a, m_write_file ); // '@'
				putc( m_b, m_write_file ); // '*'
				m_a = read_next_character(); // '/'
				m_b = read_next_stripped_and_obfuscated_character();

				return;
			}
			else if( m_a == EOF )
			{
				m_read_file_line = line;
				parse_error( "Unterminated conditional comment" );
			}
		}
	}
}

// --------------------------------------------------------------------------
// minimize_character -- determined by the argument:
//
//		EMIT_A     Output A. Copy B to A. Get the next B.
//		DELETE_A   Copy B to A. Get the next B. (Delete A).
//		DELETE_B   Get the next B. (Delete B).\
//
//	treats a string literal as a single character
//	skips regular expressions preserving the character befoe them.
//	avoids joining "+ +" or "- -"

void JavaScript_Juicer::minimize_character( int command )
{
	switch( command )
	{
		case EMIT_A:
		{
			putc(m_a, m_write_file);

			if( m_b_was_deleted )
			{
				if( m_a == m_b && ( m_a == '+' || m_a == '-' ) )
				{
					// '+' and '-' can change meaning when made adjacent.
					putc( ' ', m_write_file);
				}
			}

			// FALL THROUGH //
		}
		case DELETE_A:
		{
			m_a = m_b;

			minimize_check_string_literal();

			// FALL THROUGH //
		}
		case DELETE_B:
		{
			m_b = read_next_stripped_and_obfuscated_character();

			minimize_check_regular_expression();

			minimize_check_conditional_comment();
		}
	}

	// Remember if emitting A will remove whitespace between it and B.
	m_b_was_deleted = ( command == DELETE_B );
}

// --------------------------------------------------------------------------
// minimize_file -- Copy the input to the output, deleting the characters which are
//		insignificant to JavaScript.
//
// Converts control characters to spaces. 
// Collapses multiple spaces or linefeeds.
// Keep only spaces preceeding and following an is_identifier_character().
//		Spaces between two '+' or '-' are restored by minimize_character.
// Keep only '\n' that are both:
//		Preceeded by an is_identifier_character(), '{', '[', '(', '+', or '-'
//		Followed  by an is_identifier_character(), '}', ']', ')', '+', '-', '"', or '\''
// 
// The conservitive '\n' handling should avoid bugs due to omitted ';'

void JavaScript_Juicer::minimize_file( void )
{
	m_b_was_deleted = false;

	m_a = '\n';
	minimize_character( DELETE_B );
	minimize_character( DELETE_A );

	while( m_a != EOF )
	{
		switch( m_a )
		{
			case ' ':
			{
				if( is_identifier_character( m_b ) || m_b == '@' )
				{
					minimize_character( EMIT_A );
				}
				else
				{
					minimize_character( DELETE_A );
				}
				break;
			}
			case '\n':
			{
				switch( m_b )
				{
					case '{':
					case '[':
					case '(':
					case '+':
					case '-':
						minimize_character( EMIT_A );
						break;
					case ' ':
						minimize_character( DELETE_B );
						break;
					default:
						if( is_identifier_character( m_b ) )
						{
							minimize_character( EMIT_A );
						}
						else
						{
							minimize_character( DELETE_A );
						}
				}
				break;
			}
			default:
			{
				switch( m_b )
				{
					case ' ':
						if( is_identifier_character( m_a ) )
						{
							minimize_character( EMIT_A );
							break;
						}
						minimize_character( DELETE_B );
						break;
					case '\n':
						switch( m_a )
						{
							case '}':
							case ']':
							case ')':
							case '+':
							case '-':
							case '"':
							case '\'':
							case '/': // possible end of conditional comment.
								minimize_character( EMIT_A );
								break;
							default:
								if( is_identifier_character( m_a ) )
								{
									minimize_character( EMIT_A );
								}
								else
								{
									minimize_character( DELETE_B );
								}
						}
						break;
					default:
						minimize_character( EMIT_A );
						break;
				}
			}
		}
	}
}

// --------------------------------------------------------------------------
// main

const char* s_command_name = "jsjuicer";

const char* s_usage =
	"usage: %s [-sdmoch] [comment] [output file] [input file]...\n"
	"\n"
	"\t-s Strip comments.\n"
	"\t-d Strip debug code starting with \";;;\".\n"
	"\t-m Safely remove unneeded whitespace and control characters.\n"
	"\t-o Obfuscate symbols starting with \"$\".\n"
	"\t-c Inject comment.\n"
	"\t-h Just print this usage and exit.\n"
	"\n"
	"\tIf no output file is specified then output is written to standard out.\n"
	"\tIf no input files are specified then input is read from to standard in.\n"
	"\tUsing -m implies -s and removes control characters from strings.\n"
	"\tDefaults to outputting the input files unmodified.\n";

int main( int argc, char* argv[] )
{
	fprintf( stderr, "%s version 1.2.\n", s_command_name );

	JavaScript_Juicer javascript_juicer( s_command_name );

	bool has_comment = false;
	bool has_header = true;

	int arg_index = 1;
	if( argc > 1 && argv[1][0] == '-' )
	{
		int opt_char = 1;
		while( argv[1][opt_char] != 0 )
		{
			switch( argv[1][opt_char] )
			{
				case 's':
					javascript_juicer.enable_stripping_comments( true );
					break;
				case 'd':
					javascript_juicer.enable_stripping_debug( true );
					break;
				case 'm':
					javascript_juicer.enable_minimize( true );
					break;
				case 'o':
					javascript_juicer.enable_obfuscation( true );
					break;
				case 'c':
					has_comment = true;
					break;
				case 'h':
					fprintf( stderr, s_usage, s_command_name );
					exit( -1 );
					break;
				default:
					fprintf( stderr, "%s error: illegal option: '%c'.  Use -h for usage.\n",
										s_command_name, argv[1][opt_char] );
					exit( -1 );
					break;
			}
			++opt_char;
		}

		++arg_index;
	}

	if( has_comment )
	{
		if( argc <= arg_index )
		{
			fprintf( stderr, "%s error: Comment missing.\n", s_command_name );
			exit( -1 );
		}
		++arg_index;
	}

	if( argc > arg_index )
	{
		javascript_juicer.set_output_file( argv[arg_index] );
		++arg_index;
	}

	if( has_comment )
	{	
		fprintf(stderr, "coucoucoucocuocuocuocuoc hekokoj\n");
		javascript_juicer.insert_comment( argv[2] );
	}

	if( argc > arg_index )
	{
		while( argc > arg_index )
		{
			javascript_juicer.process_file( argv[arg_index] );
			++arg_index;
		}
	}
	else
	{
		javascript_juicer.process_file( NULL ); // stdin
	}

	return 0;
}
