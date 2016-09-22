/**@file decomment.cpp --- CPPソースからコメントと空白を除去する.
 * -*- coding: utf-8 -*-
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <io.h>
#else // may be UNIX
	#include <libgen.h>
	#define _MAX_PATH FILENAME_MAX
#endif
#include <ctype.h>
//using namespace std;


//------------------------------------------------------------------------
// 型、定数、グローバル変数の定義
//........................................................................
// typedef and constants
/** 無符号文字型の別名 */
typedef unsigned char uchar;

/** 無符号long型の別名 */
typedef unsigned long ulong;

const int LINESIZE=8192;

/** C++ソース入力行状態 */
enum cpp_state_e {
	BLANK,				// space
	IDNAME,				// identifier or reserved-word
	PAREN,				// parenthesis. "[", "]", "{", "}", "(", ")"
	SEPARATOR,			// separator or char/string closer. ",", ";", "'", """
	OPERATOR,			// operator or others. "=", "==", "+", "+=", "++", ...
	CPP_COMMENT,		// C++ single comment. "//..."
	C_COMMENT,			// C comment block.    "/* ... */"
	RAWSTRING_CONSTANT,	// raw string constant head. "R"opt(...)opt""
	STRING_CONSTANT,	// string constant. ""...""
	CHAR_CONSTANT,		// char constant.   "'...'"
	STRING_ESCAPE,		// after "\" in string constant.
	CHAR_ESCAPE,		// after "\" in char constant.
};

//........................................................................
// global variables

/** -b: keep blank line */
bool gIsKeepBlankLine = false;

/** -i: keep indent spaces */
bool gIsKeepIndent = false;

/** -m: keep minimum space */
bool gIsKeepMinimumSpace = false;

/** -q: remove quoted string */
bool gIsRemoveQuotedString = false;

/** -g: ignore backslash at end of single line comment */
bool gIsIgnoreBackslashEndOfCppComment = false;

/** -n: print line number */
bool gIsPrintNumber = false;

/** -s: output to stdout */
bool gIsStdout = false;

/** -r: recursive search */
bool gIsRecursive = false;

/** -d<DIR>: output folder */
const char* gOutDir = NULL;

//........................................................................
// messages
/** short help-message */
const char* gUsage  = "usage :decomment [-h?bimqgnsr] [-d<DIR>] file1.cpp file2.cpp ...\n";

/** detail help-message for options and version */
const char* gUsage2 =
	"  version: 2.0\n"
	"  -h -?      this help\n"
	"  -b         keep blank line\n"
	"  -i         keep indent spaces\n"
	"  -m         keep minimum space\n"
	"  -q         remove quoted string\n"
	"  -g         ignore backslash at end of single line comment\n"
	"  -n         print line number of input-file\n"
	"  -s         output to stdout instend of output-files\n"
	"  -r         [WIN32 only] recursive search under the input-file's folder(wildcard needed)\n"
	"  -d<DIR>    output to DIR\n"
	"  fileN.cpp  input-files. wildcard OK. The output-file is 'fileN.cpp.decomment'\n"
	"\n  supports and source codes at: https://github.com/hkuno9000/cpp-decomment/\n";
	;

//------------------------------------------------------------------------
/** 英数字か判定する */
inline bool IsAlnum(int c)
{
	return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/** 識別名文字(下線または英数字)か判定する */
inline bool IsIdNameChar(int c)
{
	return (c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/** 空白文字か判定する */
inline bool IsSpace(int c)
{
	return (c <= ' '); // 大胆な判定だが、扱う対象はテキストなのでこれで十分.
}

/** 括弧文字か判定する */
inline bool IsParen(int c)
{
	return (c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')');
}

/** 単項演算子か判定する */
inline bool IsUnaryOp(int c)
{
	return (c == '&' || c == '*' || c == '+' || c == '-' || c == '~' || c == '!');
}

/** 文字列が全て空白文字か判定する */
bool IsAllSpaces(const char* s)
{
	while (*s) {
		if (!IsSpace(*s++)) return false;
	}
	return true;
}

//------------------------------------------------------------------------
/** remove C/C++ comment and white spaces.
 * @param fname  input file name
 * @param line   line number of input file
 * @param state  curent syntax state
 * @param d      output source line
 * @param s      input C/CPP source line
 */
void DecommentLine(const char* fname, int line, cpp_state_e& state, char* d, const char* s)
{
	int c;
	char* top = d;
	cpp_state_e lastToken = state;
	static char   rawStringDelimiterStr[16+1+1];	///@todo should be member of cpp_state
	static size_t rawStringDelimiterLen = 0;		///@todo shuold be member of cpp_state
	bool needSpace = false;
	bool isMacro = false;
	while ((c = (uchar)*s++) != '\0') {

		if (c == '\\' && *s == '\n' && state != RAWSTRING_CONSTANT) { // backslash at end of line.
			if (state == CPP_COMMENT) {
				// At single line comment.
				// That may be a mistake or 2nd byte code of multi-byte-encoding.
				// So, print warning message.
				if (gIsIgnoreBackslashEndOfCppComment) {
					state = BLANK; // ignore it.
				}
				fprintf(stderr, "%s(%d) !!!warning: line-merge-mark '\\' at end of single line comment. %s\n",
					fname, line,
					state == BLANK
						? "ignore it by -g option on, next line is not a comment."
						: "accept it by -g opiton off, next line is a comment."
					);
			}
			else if (state == C_COMMENT) {
				// In block comment.
				// That may be a mistake or 2nd byte code of multi-byte-encoding.
				// But, next line is a comment for any case. So, do not care.
			}
			else {
				*d++ = c;	// c='\\'
			}
			++s; continue; // fetch '\n'. next char may be '\0'
		}

		switch (state) {
		case C_COMMENT:
			if (c == '*' && *s == '/') {
				state = BLANK; ++s;
			}
			continue;

		case CPP_COMMENT:
			if (c == '\n') {
				state = BLANK;
			}
			continue;

		case RAWSTRING_CONSTANT:
			if (c == ')' && strncmp(s, rawStringDelimiterStr, rawStringDelimiterLen) == 0) {
				*d++ = c;
				strncpy(d, s, rawStringDelimiterLen);
				s += rawStringDelimiterLen;
				d += rawStringDelimiterLen;
				state = SEPARATOR;
				continue;
			}
			else if (gIsRemoveQuotedString && c != '\n')
				continue; // skip output
			break;
		case STRING_CONSTANT:
			if (c == '"')
				state = SEPARATOR;
			else {
				if (c == '\\')
					state = STRING_ESCAPE;
				if (gIsRemoveQuotedString)
					continue; // skip output
			}
			break;

		case CHAR_CONSTANT:
			if (c == '\'')
				state = SEPARATOR;
			else if (c == '\\')
				state = CHAR_ESCAPE;
			break;

		case STRING_ESCAPE:
			state = STRING_CONSTANT;
			if (gIsRemoveQuotedString)
				continue; // skip output
			break;

		case CHAR_ESCAPE:
			state = CHAR_CONSTANT;
			break;

		case BLANK:
			if (c == '\n' || IsSpace(c))
				continue;
			if (c == '#') {
				isMacro = true;
			}
			if (isMacro && lastToken == IDNAME && (c == '"' || c == '\'' || c == '(' || IsUnaryOp(c)) && d > top) {
				// Don't remove a space after ID/if in following cases:
				//  #define ID "abc"
				//  #define ID 'c'
				//  #define ID (-1)
				//  #define ID -1
				//  #if -1
				needSpace = true;
			}
			if (gIsKeepMinimumSpace && d > top) {
				needSpace = true;
			}
parse_token:
			if (c == '/' && *s == '*') {
				state = C_COMMENT; ++s;
				continue;
			}
			else if (c == '/' && *s == '/') {
				state = CPP_COMMENT; ++s;
				continue;
			}
			else if (c == '"') {
				if (lastToken == IDNAME && d > top && d[-1] == 'R') { // "R" is parsed as IDNAME.
					size_t n = strcspn(s, "(");
					if (s[n] == '(' && n <= 16)
						strncpy(rawStringDelimiterStr, s, n);	// copy opt in 'R"opt(...'
					else
						n = 0;
					strcpy(rawStringDelimiterStr + n, "\"");
					rawStringDelimiterLen = n + 1;
					*d++ = c;
					strncpy(d, s, n + 1);
					s += n + 1;
					d += n + 1;
					lastToken = state = RAWSTRING_CONSTANT;
					continue;
				}
				else {
					lastToken = state = STRING_CONSTANT;
				}
				break;
			}
			else if (c == '\'') {
				lastToken = state = CHAR_CONSTANT;
				break;
			}
			else if (IsParen(c)) {
				lastToken = state = PAREN;
				break;
			}
			else if (c == ',' || c == ';') {
				lastToken = state = SEPARATOR;
				break;
			}
			else if (IsIdNameChar(c)) {
				if (lastToken == IDNAME && state == BLANK && d > top) {
					needSpace = true; // IDNAME間の空白は区切り記号として意味があるので必要である.
				}
				lastToken = state = IDNAME;
				break;
			}
			else {
				if (lastToken == OPERATOR && state == BLANK && d > top) {
					// a. "i++ + j" と "i + ++j" は演算子間の空白を除去するとどちらも"i+++j"となり式の意味が変ってしまう.
					// b. "a / *p;" は演算子間の空白を除去すると式がブロックコメント開始記号に化けてしまう.
					// c. "a & &i;" は演算子間の空白を除去すると"&"が"&&"に化けて、式の意味が変ってしまう.
					// d. "a ? b : ::c;" は演算子間の空白を除去すると": ::c"が":::c"となり文法エラーになる.
					// e. "vector<vector<int> >" は">"間の空白を除去すると文法エラーになる.
					// これらの記号間の空白は区切り記号として意味があるので必要である.
					// 「問題のある演算子の組み合わせ」を厳密に検出するのは難しいので,
					// 問題がありそうなケースに対して「空白を必要」と判定することにした.
					int c0 = (uchar) d[-1];
					if ((c0 == '/' && c == '*') || (c0 == '*' && c == '/')
							|| c0 == c  /* "+ +", "& &", ": ::", "> >"... */) {
						needSpace = true;
					}
				}
				lastToken = state = OPERATOR;
				break;
			}

		case PAREN:
		case SEPARATOR:
		case IDNAME:
		case OPERATOR:
			if (c == '\n' || IsSpace(c)) {
				state = BLANK;
				continue;
			}
			goto parse_token;
		}//.endswitch state
		if (needSpace) {
			*d++ = ' '; needSpace = false;
		}
		*d++ = c;
	}//.endwhile s
	*d = '\0';
}


//------------------------------------------------------------------------
/** input from fin, remove comment and white spaces, and output to fout. */
void DecommentFile(const char* fname, FILE* fin, FILE* fout)
{
	char buf[LINESIZE];
	char line[LINESIZE];
	cpp_state_e cppState = BLANK;

	for (int i = 0; fgets(buf, sizeof(buf), fin) != NULL; ++i) {
		char* s = buf;
		if (gIsKeepIndent && cppState == BLANK) {
			while (*s == ' ' || *s == '\t')
				++s;
		}

		DecommentLine(fname, i+1, cppState, line, s);

		if (!gIsKeepBlankLine && IsAllSpaces(line) && cppState != RAWSTRING_CONSTANT)
			continue;
		if (gIsPrintNumber)
			fprintf(fout, "%07d:", i+1);
		if (buf < s)
			fwrite(buf, 1, s - buf, fout); // indent
		fputs(line, fout); // decommented line
		if (cppState != RAWSTRING_CONSTANT)
			putc('\n', fout);
	}
}

//------------------------------------------------------------------------
/** 入力ファイルオープン.
 * オープン失敗時にはリターンしないで、ここで終了する.
 */
FILE* OpenInput(const char* fname)
{
	FILE* fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "can't open input file: %s\n", fname);
		exit(EXIT_FAILURE);
	}
	return fp;
}

//------------------------------------------------------------------------
/** 出力ファイルオープン.
 * 出力ファイル名は、渡されたファイル名の末尾に extname を追加したものとする.
 * オープン失敗時にはリターンしないで、ここで終了する.
 */
FILE* OpenOutput(const char* inputfname, const char* extname)
{
	char fname[_MAX_PATH+100];	// 拡張子には100文字もみておけば十分
	if (gOutDir) {
#ifdef _WIN32
		char base[_MAX_PATH];
		char ext[_MAX_PATH];
		_splitpath(inputfname, NULL, NULL, base, ext);
		_makepath(fname, NULL, gOutDir, base, ext);
#else // may be UNIX
		char input[_MAX_PATH];
		strcpy(input, inputfname);
		snprintf(fname, _MAX_PATH, "%s/%s", gOutDir, basename(input));
#endif
	}
	else {
		strcpy(fname, inputfname);
	}
	strcat(fname, extname);

	FILE* fp = fopen(fname, "w");
	if (fp == NULL) {
		fprintf(stderr, "can't open output file: %s\n", fname);
		exit(EXIT_FAILURE);
	}
	return fp;
}

//------------------------------------------------------------------------
/** usageとエラーメッセージを表示後に、exitする */
void error_abort(const char* msg)
{
	fputs(gUsage, stderr);
	if (msg)
		fputs(msg, stderr);
	exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------
/** s1とs2は等しいか? */
inline bool strequ(const char* s1, const char* s2)
{
	return strcmp(s1, s2) == 0;
}

//------------------------------------------------------------------------
/** fnameを読み込み、コメントと余分な空白を除去し、fname+".decomment"に出力する. */
void DecommentMain(const char* fname)
{
	FILE* fin = OpenInput(fname);
	FILE* fout;
	if (gIsStdout) {
		fout = stdout;
		printf("<<< %s >>> begin.\n", fname);
	}
	else {
		fout = OpenOutput(fname, ".decomment");
	}

	DecommentFile(fname, fin, fout);

	fclose(fin);
	if (gIsStdout) {
		printf("<<< %s >>> end.\n\n", fname);
	}
	else {
		fclose(fout);
	}
}

//------------------------------------------------------------------------
/** ワイルドカード展開と再帰探索付きの DecommentMain */
void DecommentWildMain(const char* fname)
{
#ifdef _WIN32
	if (strpbrk(fname, "*?") == NULL) {
		//----- ワイルドカードを含まないパス名の処理
		DecommentMain(fname);
	}
	else {
		//----- ワイルドカードを含むパス名の処理
		char path[_MAX_PATH + 1000];
		char drv[_MAX_DRIVE];
		char dir[_MAX_PATH + 1000];
		char base[_MAX_PATH];
		char ext[_MAX_PATH];
		_splitpath(fname, drv, dir, base, ext);

		_finddata_t find;
		long h = _findfirst(fname, &find);
		if (h != -1) {
			do {
				if (find.attrib & _A_SUBDIR)
					continue;
				_makepath(path, drv, dir, find.name, NULL);
				// fprintf(stderr, "decomment: %s\n", find.name);
				DecommentMain(path);
			} while (_findnext(h, &find) == 0);
			_findclose(h);
		}
		if (!gIsRecursive)
			return;

		// サブフォルダを検索し、それぞれに対して再帰する
		_makepath(path, drv, dir, "*.*", NULL);
		h = _findfirst(path, &find);
		if (h != -1) {
			do {
				if (!(find.attrib & _A_SUBDIR))
					continue;
				if (strequ(find.name, ".") || strequ(find.name, ".."))
					continue;
				_makepath(path, drv, dir, find.name, NULL);
				strcat(path, "\\");
				strcat(path, base);
				strcat(path, ext);
				// fprintf(stderr, "decomment recursive: %s\n", path);
				DecommentWildMain(path); // 再帰呼び出し.
			} while (_findnext(h, &find) == 0);
			_findclose(h);
		}
	}
#else // may be UNIX: already glob by shell. don't care.
	DecommentMain(fname);
#endif
}

//------------------------------------------------------------------------
/** メイン関数 */
int main(int argc, char* argv[])
{
	//--- コマンドライン上のオプションを解析する.
	while (argc > 1 && argv[1][0]=='-') {
		char* sw = &argv[1][1];
		if (strcmp(sw, "help") == 0)
			goto show_help;
		else {
			do {
				switch (*sw) {
				case 'h': case '?':
show_help:			error_abort(gUsage2);
					break;
				case 'b':
					gIsKeepBlankLine = true;
					break;
				case 'i':
					gIsKeepIndent = true;
					break;
				case 'm':
					gIsKeepMinimumSpace = true;
					break;
				case 'q':
					gIsRemoveQuotedString = true;
					break;
				case 'g':
					gIsIgnoreBackslashEndOfCppComment = true;
					break;
				case 'n':
					gIsPrintNumber = true;
					break;
				case 's':
					gIsStdout = true;
					break;
				case 'r':
					gIsRecursive = true;
					break;
				case 'd':
					gOutDir = sw+1; // "-d<DIR>"
					if (*gOutDir == '\0') gOutDir = *argv++; // "-d <DIR>"
					goto next_arg;
				default:
					error_abort("unknown option.\n");
					break;
				}
			} while (*++sw);
		}
next_arg:
		++argv;
		--argc;
	}
	if (argc == 1) {
		error_abort("please specify input file.\n");
	}

	//--- コマンドライン上の各入力ファイルを処理する.
	for (int i = 1; i < argc; i++)
		DecommentWildMain(argv[i]);

	return EXIT_SUCCESS;
}

// decomment.cpp - end.
