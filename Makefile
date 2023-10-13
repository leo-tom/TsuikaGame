
OPTION = $(shell pkg-config --cflags gtk4 )
OPTION += $(shell pkg-config --libs gtk4 )
OPTION += "-lm"

all : executable

executable : main.c
	cc $(OPTION) -o tsuika main.c graphics.c
