# compiler & Linker settings
CXX = g++
CXXFLAGS = -I ./inc -I ./third-party/CImg -I ./third-party/libjpeg -I ./Data-Loader -std=c++11
OPTFLAGS = -march=native -flto -funroll-loops -finline-functions -ffast-math -O3
WARNINGS = -g -Wall
LINKER = -L/usr/X11R6/lib -lm -lpthread -L./third-party/libjpeg -ljpeg -lpng

# Valgrind for memory issue
CHECKCC = valgrind
CHECKFLAGS = --leak-check=full -s --show-leak-kinds=all --track-origins=yes

# Cppcheck for static analysis
CPPCHECK = cppcheck
CPPCHECKFLAGS = --enable=all --inconclusive --suppress=missingIncludeSystem \
                -I ./inc -I ./third-party/CImg -I ./third-party/libjpeg -I ./Data-Loader

# Source files and object files
SRCDIR = src
OBJDIR = obj
INCDIR = inc
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(SRCS))

# Control the build verbosity
ifeq ("$(VERBOSE)","1")
    Q :=
    VECHO = @true
else
    Q := @
    VECHO = @printf
endif

.PHONY: all install check cppcheck analyze clean

# Name of the executable
TARGET = Image_Processing Data_Loader_Example

all: $(TARGET)

$(OBJDIR):
	@mkdir $(OBJDIR)

Image_Processing: main.cpp $(OBJS) $(OBJDIR)/data_loader.o
	$(VECHO) "	LD\t$@\n"
	$(Q)$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) $^ -o $@ $(LINKER)

Data_Loader_Example: data_loader_demo.cpp $(OBJDIR)/data_loader.o
	$(VECHO) "	LD\t$@\n"
	$(Q)$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) $^ -o $@ $(LINKER)

# Include generated dependency files
-include $(DEPS)

# Compilation rule for object files with automatic dependency generation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) Makefile
	$(VECHO) "	CC\t$@\n"
	$(Q)$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) -MMD -c $< -o $@

# Compilation rule for data_loader.o with explicit dependencies
$(OBJDIR)/data_loader.o: ./Data-Loader/data_loader.cpp ./Data-Loader/data_loader.h | $(OBJDIR) Makefile
	$(VECHO) "	CC\t$@\n"
	$(Q)$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) -MMD -c $< -o $@



install:
	$(VECHO) "Installing third party dependencies\n"
	$(Q)chmod +x scripts/clone_env.sh  
	$(Q)./scripts/clone_env.sh  > /dev/null 2>&1
	$(VECHO) "Finished installing third party dependencies!!\n"

check: Image_Processing
	$(VECHO) "Running Valgrind (dynamic analysis)...\n"
	printf "3\n1 0\n0\n" | $(CHECKCC) $(CHECKFLAGS) ./Image_Processing 2>&1 | tee valgrind_report.txt
	$(VECHO) "Valgrind report saved to valgrind_report.txt\n"

cppcheck:
	$(VECHO) "Running cppcheck (static analysis)...\n"
	$(CPPCHECK) $(CPPCHECKFLAGS) $(SRCDIR)/ Data-Loader/ main.cpp data_loader_demo.cpp 2>&1 | tee cppcheck_report.txt
	$(VECHO) "Cppcheck report saved to cppcheck_report.txt\n"

analyze: check cppcheck

clean:
	rm -rf $(OBJDIR) $(TARGET)
