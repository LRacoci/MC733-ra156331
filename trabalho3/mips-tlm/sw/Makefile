# ####################################################
# Hello World
# ####################################################

#changed as this option was given in path
CC = mips-newlib-elf-gcc
CFLAGS = -msoft-float -specs=archc
LDFLAGS = -lm

TARGET = hello_world.mips hist.mips
INC_DIR := -I.

#------------------------------------------------------
.SILENT:

#------------------------------------------------------
.SUFFIXES: .o .c .h

#------------------------------------------------------
all: $(TARGET)
# Regras Genéricas
%.mips: %.o
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(INC_DIR) -c $< -o $@
#------------------------------------------------------
#------------------------------------------------------
clean:
	rm -f *.o *~ $(TARGET)
#------------------------------------------------------
distclean: clean
#------------------------------------------------------
.c.o:
	$(CC) -c $(CFLAGS) $(INC_DIR) -c $<


