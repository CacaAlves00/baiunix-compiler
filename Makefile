baiunix-compiler: baiunix-compiler.l baiunix-compiler.y baiunix-compiler.h
				bison -d baiunix-compiler.y
				flex -o baiunix-compiler.lex.c baiunix-compiler.l
				gcc -o $@ baiunix-compiler.tab.c baiunix-compiler.lex.c baiunix-compiler-functions.c -lm -g
				@echo Parser da Linguagem estah pronto!
