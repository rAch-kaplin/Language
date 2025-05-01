CXX = g++

CXXFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
           -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
           -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd \
           -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
           -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default \
           -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
           -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
           -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla \
		  -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

INCLUDES = -I./frontend/include -I./libs/logger -I./libs/arg_parser -I./common -I./DSL

BUILD_DIR = build
EXECUTABLE = do

SOURCES = frontend/src/main.cpp frontend/src/file_read.cpp frontend/src/tree_func.cpp frontend/src/lexical_analysis.cpp frontend/src/main.cpp

OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:%.cpp=%.o))
DEPENDS = $(OBJECTS:.o=.d)

LOGGER_LIB = libs/logger/build/logger.o
ARG_PARSER_LIB = libs/arg_parser/build/arg_parser.o

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(LOGGER_LIB) $(ARG_PARSER_LIB)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
	@echo -e "\033[33m Build complete: $@ \033[0m"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

$(LOGGER_LIB):
	@$(MAKE) -C libs/logger

$(ARG_PARSER_LIB):
	@$(MAKE) -C libs/arg_parser

clean:
	@rm -rf $(BUILD_DIR) $(EXECUTABLE)
	@$(MAKE) -C libs/logger clean
	@$(MAKE) -C libs/arg_parser clean

clean-img:
	@rm -f graphiz/img/*.png
	@echo -e "\033[36m Deleted all PNG files in graphiz/img \033[0m"

clean-dot:
	@rm -f graphiz/dot/*.dot
	@echo -e "\033[36m Deleted all DOT files in graphiz/dot \033[0m"

-include $(DEPENDS)
