CXX = g++
SDL_LIB = -LC:\SDL\x86_64-w64-mingw32\lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
SDL_INCLUDE = -IC:\SDL\x86_64-w64-mingw32\include
SDL_LIB32 = -LC:\SDL\i686-w64-mingw32\lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
SDL_INCLUDE32 = -IC:\SDL\i686-w64-mingw32\include
CXXFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE)
CXXFLAGS32 = -Wall -m32 -c -std=c++11 $(SDL_INCLUDE32)
LDFLAGS = -lmingw32 -mwindows -mconsole $(SDL_LIB)
LDFLAGS32 = -lmingw32 -mwindows -mconsole -m32 $(SDL_LIB32)
ODIR = obj/
ODIR32 = obj32/
SDIR = src/
IDIR = include/
EXE = bin/game.exe
EXE32 = bin32/game.exe
binDIR = bin/
changeDir = cd bin
OBJS := $(patsubst $(SDIR)%.cpp,$(ODIR)%.o,$(wildcard $(SDIR)*.cpp))
OBJS32 := $(patsubst $(SDIR)%.cpp,$(ODIR32)%.o,$(wildcard $(SDIR)*.cpp))
INCLUDES := $(wildcard $(IDIR)*.h)

all: $(EXE)

# O -> EXE 
$(EXE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

build32: $(EXE32)

$(EXE32): $(OBJS32)
	$(CXX) $(OBJS32) $(LDFLAGS32) -o $@

$(ODIR32)%.o: $(SDIR)%.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS32) $< -o $@

# CPP -> O
$(ODIR)%.o: $(SDIR)%.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) $< -o $@

# RUN 
run: $(binDIR)game.exe $(OBJS)
	$(changeDir) & game.exe

# CLEAN
clean:
	del obj\*.o && del bin\*.exe