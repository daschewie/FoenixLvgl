EXEC = lvgdemo

BUILD_VER = 0.1

# Common source files
ASM_SRCS =
CSRCS = main.c lv_a2560_disp.c
 
MODEL = --code-model=large --data-model=large
LIB_MODEL = lc-ld
CFLAGS = -Iinclude -DA2560=1 -DUSE_DL=0 -DNO_WCHAR=1 -DBUILD_VER="\"$(BUILD_VER)\""

include lvgl/lvgl.mk

FOENIX_LIB = lib/foenix-$(LIB_MODEL).a
A2560K_RULES = lib/a2560k-simplified.scm

# Object files
OBJS = $(ASM_SRCS:%.s=%.o) $(CSRCS:%.c=%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=%-debug.o) $(CSRCS:%.c=%-debug.o)

all: $(EXEC).pgz

%.o: %.s
	as68k --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

%.o: %.c
	cc68k $(CFLAGS) --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

%-debug.o: %.s
	as68k --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

%-debug.o: %.c
	cc68k $(CFLAGS) --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

$(EXEC).pgz:  $(OBJS)
	ln68k -o $@ $^ $(A2560K_RULES) clib-68000-$(LIB_MODEL).a $(FOENIX_LIB) --output-format=pgz --list-file=$(EXEC).lst --cross-reference --rtattr printf=float --rtattr scanf=float --rtattr cstartup=Foenix_user --stack-size=65536 --heap-size=262144

$(EXEC).elf:  $(OBJS)
	ln68k -o $@ $^ $(A2560K_RULES) --debug clib-68000-$(LIB_MODEL).a $(FOENIX_LIB) --list-file=$(EXEC).lst --cross-reference --rtattr printf=float --rtattr scanf=float --rtattr cstartup=Foenix_user --stack-size=65536 --heap-size=262144

clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst)
	-rm $(EXEC).pgz $(EXEC).elf $(EXEC).lst 
