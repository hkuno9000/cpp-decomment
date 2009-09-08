/**@file decomment.cpp --- CPP�\�[�X����R�����g�Ƌ󔒂���������.
 * $Id: decomment.cpp,v 1.7 2002/10/01 08:09:01 hkuno Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <ctype.h>
//using namespace std;

//------------------------------------------------------------------------
// �^�A�萔�A�O���[�o���ϐ��̒�`
//........................................................................
// typedef and constants
/** �����������^�̕ʖ� */
typedef unsigned char uchar;

/** ������long�^�̕ʖ� */
typedef unsigned long ulong;

const int LINESIZE=8192;

/** C++�\�[�X���͍s��� */
enum cpp_state_e {
	BLANK,				// space
	IDNAME,				// identifier or reserved-word
	PAREN,				// parenthesis. "[", "]", "{", "}", "(", ")"
	SEPARATOR,			// separator or char/string closer. ",", ";", "'", """
	OPERATOR,			// operator or others. "=", "==", "+", "+=", "++", ...
	CPP_COMMENT,		// C++ single comment. "//..."
	C_COMMENT,			// C comment block.    "/* ... */"
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

/** -q: remove quoted string */
bool gIsRemoveQuotedString = false;

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
const char* gUsage  = "usage :decomment [-h?biqnsr] [-d<DIR>] file1.cpp file2.cpp ...\n";

/** detail help-message for options and version */
const char* gUsage2 =
	"  version: 1.8\n"
	"  -h -?      this help\n"
	"  -b         keep blank line\n"
	"  -i         keep indent spaces\n"
	"  -q         remove quoted string\n"
	"  -n         print line number of input-file\n"
	"  -s         output to stdout instend of files(*.decomment)\n"
	"  -r         recursive search under the input-file's folder(wildcard needed)\n"
	"  -d<DIR>    output to DIR\n"
	"  fileN.cpp  input-files. wildcard OK\n";

//------------------------------------------------------------------------
/** �p���������肷�� */
inline bool IsAlnum(int c)
{
	return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/** ���ʖ�����(�����܂��͉p����)�����肷�� */
inline bool IsIdNameChar(int c)
{
	return (c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/** �󔒕��������肷�� */
inline bool IsSpace(int c)
{
	return (c <= ' '); // ��_�Ȕ��肾���A�����Ώۂ̓e�L�X�g�Ȃ̂ł���ŏ\��.
}

/** ���ʕ��������肷�� */
inline bool IsParen(int c)
{
	return (c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')');
}

//------------------------------------------------------------------------
/** C++�\�[�X�Ƃ���"�]���ȋ�"��"�R�����g"�Ɖ��s����������. */
void DecommentLine(const char* fname, int line, cpp_state_e& state, char* d, const char* s)
{
	int c;
	char* top = d;
	cpp_state_e lastToken = state;
	bool needSpace = false;
	while ((c = (uchar)*s++) != '\0') {

		if (c == '\\' && *s == '\n') { // �s�̕����w��.
			if (state == CPP_COMMENT) {
				// �P�s�R�����g���玟�s�ւ̍s�����w��͉����̌��Ȃ̂Ōx�����o���Ė�������B
				// �������o�C�g�̕����R�[�h�� 0x5c �̏ꍇ���E���Ă��܂����A
				// �p����ŃR���p�C������ƕ����w�舵���ɂȂ�������P�[�X�Ȃ̂ŁA�x�����o���Ă����B
				fprintf(stderr, "%s(%d) !!!warning: line-marge-mark '\\' at end of single comment. ignore it.\n",
					fname, line);
				state = BLANK;
			}
			else if (state == C_COMMENT) {
				// �u���b�N�R�����g���ɂāA���s�ւ̍s�����w��͂�͂���Ǝv���邪�A
				// ���s�������R�����g���Ȃ̂ŁA���������Ƃ���Ŗ��͂Ȃ��B����Ė�������B
			}
			else {
				*d++ = c;	// c='\\'
			}
			++s; continue; // �s���L��'\n'�̎��֐i��(�����炭������)
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
				lastToken = state = STRING_CONSTANT;
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
					needSpace = true; // IDNAME�Ԃ̋󔒂͋�؂�L���Ƃ��ĈӖ�������̂ŕK�v�ł���.
				}
				lastToken = state = IDNAME;
				break;
			}
			else {
				if (lastToken == OPERATOR && state == BLANK && d > top) {
					// a. "i++ + j" �� "i + ++j" �͉��Z�q�Ԃ̋󔒂���������Ƃǂ����"i+++j"�ƂȂ莮�̈Ӗ����ς��Ă��܂�.
					// b. "a / *p;" �͉��Z�q�Ԃ̋󔒂���������Ǝ����u���b�N�R�����g�J�n�L���ɉ����Ă��܂�.
					// c. "a & &i;" �͉��Z�q�Ԃ̋󔒂����������"&"��"&&"�ɉ����āA���̈Ӗ����ς��Ă��܂�.
					// d. "a ? b : ::c;" �͉��Z�q�Ԃ̋󔒂����������": ::c"��":::c"�ƂȂ蕶�@�G���[�ɂȂ�.
					// e. "vector<vector<int> >" ��">"�Ԃ̋󔒂���������ƕ��@�G���[�ɂȂ�.
					// �����̋L���Ԃ̋󔒂͋�؂�L���Ƃ��ĈӖ�������̂ŕK�v�ł���.
					// �u���̂��鉉�Z�q�̑g�ݍ��킹�v�������Ɍ��o����͓̂���̂�,
					// ��肪���肻���ȃP�[�X�ɑ΂��āu�󔒂�K�v�v�Ɣ��肷�邱�Ƃɂ���.
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
	if (state == CPP_COMMENT)
		state = BLANK;	// ���S��.
}


//------------------------------------------------------------------------
/** fin����̓��͍s�ɑ΂��ăR�����g�Ɨ]���ȋ󔒂��������Afout�ɏo�͂���. */
void DecommentFile(const char* fname, FILE* fin, FILE* fout)
{
	char buf[LINESIZE];
	char line[LINESIZE];
	cpp_state_e cppState = BLANK;

	for (int i = 0; fgets(buf, sizeof(buf), fin) != NULL; ++i) {
		char* s = buf;
		if (gIsKeepIndent) {
			while (*s == ' ' || *s == '\t')
				++s;
		}

		DecommentLine(fname, i+1, cppState, line, s);

		if (!gIsKeepBlankLine && !line[0])
			continue;
		if (gIsPrintNumber)
			fprintf(fout, "%07d:", i+1);
		if (buf < s)
			fwrite(buf, 1, s - buf, fout); // indent
		fputs(line, fout); // decommented line
		putc('\n', fout);
	}
}

//------------------------------------------------------------------------
/** ���̓t�@�C���I�[�v��.
 * �I�[�v�����s���ɂ̓��^�[�����Ȃ��ŁA�����ŏI������.
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
/** �o�̓t�@�C���I�[�v��.
 * �o�̓t�@�C�����́A�n���ꂽ�t�@�C�����̖����� extname ��ǉ��������̂Ƃ���.
 * �I�[�v�����s���ɂ̓��^�[�����Ȃ��ŁA�����ŏI������.
 */
FILE* OpenOutput(const char* inputfname, const char* extname)
{
	char fname[_MAX_PATH+100];	// �g���q�ɂ�100�������݂Ă����Ώ\��
	if (gOutDir) {
		char base[_MAX_PATH];
		char ext[_MAX_PATH];
		_splitpath(inputfname, NULL, NULL, base, ext);
		_makepath(fname, NULL, gOutDir, base, ext);
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
/** usage�ƃG���[���b�Z�[�W��\����ɁAexit���� */
void error_abort(const char* msg)
{
	fputs(gUsage, stderr);
	if (msg)
		fputs(msg, stderr);
	exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------
/** s1��s2�͓�������? */
inline bool strequ(const char* s1, const char* s2)
{
	return strcmp(s1, s2) == 0;
}

//------------------------------------------------------------------------
/** fname��ǂݍ��݁A�R�����g�Ɨ]���ȋ󔒂��������Afname+".decomment"�ɏo�͂���. */
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
/** ���C���h�J�[�h�W�J�ƍċA�T���t���� DecommentMain */
void DecommentWildMain(const char* fname)
{
	if (strpbrk(fname, "*?") == NULL) {
		//----- ���C���h�J�[�h���܂܂Ȃ��p�X���̏���
		DecommentMain(fname);
	}
	else {
		//----- ���C���h�J�[�h���܂ރp�X���̏���
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

		// �T�u�t�H���_���������A���ꂼ��ɑ΂��čċA����
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
				DecommentWildMain(path); // �ċA�Ăяo��.
			} while (_findnext(h, &find) == 0);
			_findclose(h);
		}
	}
}

//------------------------------------------------------------------------
/** ���C���֐� */
int main(int argc, char* argv[])
{
	//--- �R�}���h���C����̃I�v�V��������͂���.
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
				case 'q':
					gIsRemoveQuotedString = true;
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
					gOutDir = sw+1;
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

	//--- �R�}���h���C����̊e���̓t�@�C������������.
	for (int i = 1; i < argc; i++)
		DecommentWildMain(argv[i]);

	return EXIT_SUCCESS;
}

// decomment.cpp - end.
