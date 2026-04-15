CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -O2


SRC_DIR    := src
OBJ_DIR    := obj
BIN_DIR    := bin
SFML_PATH  := lib/SFML


TARGET_NAME := app.exe
TARGET      := $(BIN_DIR)/$(TARGET_NAME)


SOURCES       := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS       := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))


INCLUDES := -Isrc -I$(SFML_PATH)/include
LIBS     := -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32


FIX_PATH = $(subst /,\,$1)
RM       = del /Q /S


COPY_CMD = xcopy /y /i "$(call FIX_PATH,$(SFML_PATH)/bin/*.dll)" "$(call FIX_PATH,$(BIN_DIR))" > nul 2>&1

.PHONY: all clean run

all: $(TARGET)


$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)
	@$(COPY_CMD)
	@echo Build successful! Run: $(TARGET)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@



$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)


clean:
	@if exist $(OBJ_DIR) $(RM) $(call FIX_PATH,$(OBJ_DIR)\*.o)
	@if exist $(BIN_DIR) $(RM) $(call FIX_PATH,$(BIN_DIR)\$(TARGET_NAME))
	@echo Cleaned up.

run: all
	./$(TARGET)