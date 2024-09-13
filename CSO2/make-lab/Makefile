# Makefile for the guesser project

# These are the variables, the CC is the compiler command tool, the CFLAGS is the options for clang like the warnings,
# and the LDFLAGS is just for specifying the linker flags, which we just left as blank for now.
CC = clang
# And these are the options, using wall since we want the current and extra warnings enabled.
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS =

# And this is another variable, which is the file that's our goal to make an executable from.
TARGET = guesser

# Then these are the object files
OBJS = cheer.o grunt.o guesser.o

# Same for that library file
LIBRARY = libsay.a

# Builds into the target file, which in our case is guesser
all: $(TARGET)

# So then towards the target (guesser), we write to the .o file associated with it using the flags for compilation
$(TARGET): $(LIBRARY) guesser.o
	$(CC) $(LDFLAGS) -o $(TARGET) guesser.o -L. -lsay

# Then we build the static librar.
$(LIBRARY): cheer.o grunt.o
	ar rcs $(LIBRARY) cheer.o grunt.o

# And then these are the rules to build object files from source files. So we do the following, where we build the object files
# into the .o extensions using clang with our compiler rules. We do the same thing 3 times,
# once for each object file. 
cheer.o: cheer.c say.h
	$(CC) $(CFLAGS) -c cheer.c

grunt.o: grunt.c say.h
	$(CC) $(CFLAGS) -c grunt.c

guesser.o: guesser.c say.h
	$(CC) $(CFLAGS) -c guesser.c

# Then we clean up any of those random "build artifacts" from what's left over from compiling, found from stack overflow somewhere.
clean:
	rm -f $(TARGET) $(OBJS) $(LIBRARY)

# Then we specify that these two "all" and "clean" are commands for our Makefile instead of file inputs.
.PHONY: all clean

