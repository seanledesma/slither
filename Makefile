# Compiler and Directories
COMPILER = clang
CFILES = src/*.c
SOURCE_LIBS = -Ilib/

# macOS-specific options
OSX_OPT = -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OUTPUT = bin/build_osx

# Build Target
build_osx: 
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OPT) -o $(OUTPUT)

# Run Target
run: build_osx
	./$(OUTPUT)

# Clean Target
clean:
	rm -f $(OUTPUT)
