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

void skip_space(void) {
	int c;
	while ((c = getc(stdin)) != EOF) {
		if (isspace(c))
			// 空白を飛ばす
			continue;
		// 空白以外ならストリームに一文字押し戻す
		ungetc(c, stdin);
		return;
	}
}

int read_number(int n) {
	int c;
	while ((c = getc(stdin)) != EOF) {
		if(!isdigit(c)) {
			ungetc(c, stdin);
			return n;
		}
		n = n * 10 + (c - '0');
	}
}

void compile_expr2(void) {
	for (;;) {
		// 空白を読み飛ばす
		skip_space();
		int c = getc(stdin);
		if (c == EOF) {
			printf("ret\n");
			exit(0);
		}
		// 演算子の特定
		char *op;
		if (c == '+') op = "add";
		else if (c == '-') op = "sub";
		else error("Operator expected, but got '%c'", c);

		// 演算子に続く数字を得る(空白は読み飛ばす)
		skip_space();
		c = getc(stdin);
		if (!isdigit(c))
			// 数字出ない場合はエラー終了
			error("Number expected, but got '%c'", c);

		// オペレータと数字を出力する
		printf("%s $%d, %%rax\n\t", op, read_number(c - '0'));
	}
}

void compile_expr(int n) {
	n = read_number(n);
	printf(".text\n\t"
                ".global intfn\n"
                "intfn:\n\t"
		"mov $%d, %%rax\n\t", n);
	compile_expr2();
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
	exit(0);
}

void compile(void) {
	int c = getc(stdin);
	if (isdigit(c))
		// 数字
		compile_expr(c - '0');
	else if (c == '"')
		// 文字列
		compile_string();
	else
		error("Don't know how to handle '%c'", c);
}

int main(int argc, char **argv) {
	compile();
	return 0;
}

