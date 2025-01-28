#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#define BUFLEN 256

void error(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
}

void compile_number(int n) {
	int c;
	while ((c = getc(stdin)) != EOF) {
		if (isspace(c))
			// 空白なら数字の区切り
			break;
		if(!isdigit(c))
			// 数字以外が混じっているのでNG
			error("Invalid character in number: '%c'", c);
		n = n * 10 + (c - '0');
	}
	printf(".text\n\t"
                ".global intfn\n"
                "intfn:\n\t"
		"mov $%d, %%rax\n\t"
		"ret\n", n);
}
void compile_string(void) {
	char buf[BUFLEN];
	int i = 0;
	for (;;) {
		int c = getc(stdin);
		if (c == EOF)
			// ダブルクォーテーションなしはエラー
			error("Unterminated string");
		if (c == '"')
			// ダブルクォーテーション"は文字列の終わり
			break;
		if (c == '\\') {
			c = getc(stdin);
			if( c == EOF) error("Unterminated \\");
		}
		buf[i++] = c;
		if (i == BUFLEN -1)
			error("String too long");
	}
	buf[i] = '\0';
	printf("\t.data\n\t"
		".mydata:\n\t"
		".string \"%s\"\n\t"
		".text\n\t"
		".global stringfn\n"
		"stringfn:\n\t"
		"lea .mydata(%%rip), %%rax\n\t"
		"ret\n", buf);
}

void compile(void) {
	int c = getc(stdin);
	if (isdigit(c))
		// 数字
		return compile_number(c - '0');
	if (c == '"')
		// 文字列
		return compile_string();
	error("Don't know how to handle '%c'", c);
}

int main(int argc, char **argv) {
	compile();
	return 0;
}

