CC = g++

CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
         -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
         -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd \
         -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
         -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default \
         -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
         -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
         -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla \
         -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

ASSEMBLER_SOURCES = assembler/src/main.cpp \
                    assembler/src/assembler.cpp

PROCESSOR_SOURCES = processor/src/main.cpp \
                    processor/src/disassembler.cpp \
                    processor/src/run.cpp \
                    processor/src/debug_proc.cpp \
                    stack/src/stack.cpp \
                    stack/src/debug.cpp \
                    logger/logger.cpp

ASSEMBLER_OBJECTS = $(ASSEMBLER_SOURCES:assembler/src/%.cpp=build/assembler/%.o)

PROCESSOR_OBJECTS = $(PROCESSOR_SOURCES:%.cpp=build/processor/%.o)

INCLUDES = -I./stack/include -I./processor/include -I./Common -I./logger -I./assembler/include

$(shell mkdir -p build/assembler build/processor build/bin)

build: build/bin/asm build/bin/proc

build/bin/asm: $(ASSEMBLER_OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDES) $^ -o build/bin/asm

build/bin/proc: $(PROCESSOR_OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDES) $^ -o build/bin/proc

build/assembler/%.o: assembler/src/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/processor/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: build
	@echo -e "\033[33mCompilation complete. Run the programs using './build/bin/asm' and './build/bin/proc'.\033[0m"

clean:
	rm -rf build/*
