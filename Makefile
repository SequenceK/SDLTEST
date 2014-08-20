CXX = g++
SDL_LIB = -LC:\SDL\x86_64-w64-mingw32\lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
SDL_INCLUDE = -IC:\SDL\x86_64-w64-mingw32\include
CXXFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE)
LDFLAGS = -lmingw32 -mwindows -mconsole $(SDL_LIB)
ODIR = obj/
SDIR = src/
IDIR = include/
EXE = bin/game.exe
binDIR = bin/
changeDir = cd bin
OBJS := $(patsubst $(SDIR)%.cpp,$(ODIR)%.o,$(wildcard $(SDIR)*.cpp))
INCLUDES := $(wildcard $(IDIR)*.h)

all: $(EXE)

# O -> EXE 
$(EXE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# CPP -> O
$(ODIR)%.o: $(SDIR)%.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) $< -o $@

# RUN 
run: $(binDIR)game.exe $(OBJS)
	$(changeDir) & game.exe

# CLEAN
clean:
	del obj\*.o && del bin\*.exe