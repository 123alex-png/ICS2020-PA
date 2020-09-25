APP=gen-expr

$(APP): gen-expr.c
	gcc -g -O2 -Wall -Werror -o $@ $<

.PHONY: clean
clean:
	-rm $(APP) 2> /dev/null
