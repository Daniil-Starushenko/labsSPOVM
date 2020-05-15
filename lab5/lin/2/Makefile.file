CC = gcc
CFLAGS = -c -Wall -pedantic
SHAREDFLAG = -shared

ifeq ($(OS), Windows_NT)

STAND:=  -std=c99
APP = lb5
LIB = libWin.dll
APP_SOURCES = lab5_win.c
APP_OBJECTS = $(APP_SOURCES:.c=.o)
LIB_SOURCES = lib.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)

MAIN_SOURCES = main.c
MAIN_OBJECTS = $(MAIN_SOURCES:.c=.o)

all: $(LIB) $(APP) 

$(LIB): $(LIB_OBJECTS) 
	$(CC) $(LIB_OBJECTS) $(SHAREDFLAG) $(STAND) -o $@

$(APP): $(APP_OBJECTS) $(MAIN_OBJECTS)
	$(CC) head.h $(APP_OBJECTS) $(MAIN_OBJECTS) $(STAND) -o $@
	
.c.o:
	$(CC) $(CFLAGS) $(STAND) $< -o $@

.PHONY: clean
clean:
	
	del /f /q *.o
	del /f /q *.exe
	del /f /q *.dll
	
else

APP = lb5

LIB = libj
SHAREDFLAG = -shared
LIB_SOURCES = lib_lin.c
STAND:=  -std=gnu99


APP_SOURCES = lab5_lin.c
APP_OBJECTS = $(APP_SOURCES:.c=.o)

MAIN_SOURCES = main.c
MAIN_OBJECTS = $(MAIN_SOURCES:.c=.o)



all: $(LIB) $(APP)

lib_lin.o:lib_lin.c
	$(CC) $(CFLAGS) $(STAND) lib_lin.c -fPIC

$(LIB):lib_lin.o
		$(CC) -shared -std=gnu99 -o $(LIB) lib_lin.o 

$(APP): $(APP_OBJECTS) $(MAIN_OBJECTS)
	$(CC) head.h $(APP_OBJECTS) $(MAIN_OBJECTS) $(STAND) -o $@ -ldl -pthread 
	
.c.o:
	$(CC) $(CFLAGS) $(STAND) $< -o $@

.PHONY: clean
clean:
	rm -rf $(APP_OBJECTS) $(LIB_OBJECTS)
	rm -rf $(APP)
	rm -rf $(LIB)
	rm -rf *.o
endif

