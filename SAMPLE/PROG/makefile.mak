.autodepend
CC=bcc32
CFLAG=-W  -3 -O -w- -pc -k -y 
DIR = exec
INC = -I"$(DXSDK_DIR)Include"
TARGET=exec\SAMPLE.EXE

OBJ1 = $(DIR)\task.obj
OBJ2 = $(DIR)\main.obj
OBJ3 = $(DIR)\game.obj

LIB = "C:\borland\bcc55\Lib\dx\d3d9.lib"
LIB = $(LIB) "C:\borland\bcc55\Lib\dx\d3d9d.lib"
LIB = $(LIB) "C:\borland\bcc55\Lib\dx\d3dx9_27.lib"
LIB = $(LIB) "C:\borland\bcc55\Lib\dx\d3dx9d_27.lib"
LIB = $(LIB) "C:\borland\bcc55\Lib\dx\dsound.lib"
LIB = $(LIB) "C:\borland\bcc55\Lib\dx\dmusic.lib"


TARGET: $(TARGET)

$(TARGET): $(OBJ1) $(OBJ2) $(OBJ3)
    $(CC) $(CFLAG) -e$(TARGET) $(OBJ1) $(OBJ2) $(OBJ3) $(LIB)

$(OBJ1): task.cpp
    $(CC) $(CFLAG) -n$(DIR) $(INC) -c task.cpp

$(OBJ2): main.cpp
    $(CC) $(CFLAG) -n$(DIR) $(INC) -c main.cpp

$(OBJ3): game.cpp
    $(CC) $(CFLAG) -n$(DIR) $(INC) -c game.cpp
