CXX = clang++
TARGET = main
SRCS = $(wildcard src/*.cpp)
INCLUDES = -I ./include/
LIBS = -L ./libs/ -lraylib -lgdi32 -lopengl32 -lwinmm

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(INCLUDES) $(LIBS) -o $(TARGET).exe

run: $(TARGET).exe
	./$(TARGET).exe

clean:
	rm -f $(TARGET).exe