# Independent test makefile (doesn't use EPICS rules)

CXX = g++
CXXFLAGS = -g -O0 -Wall -Wextra -std=c++11 -DCONFIG_DEFAULT_LOGLEVEL=1
LDFLAGS = 

# Paths relative to tests/ directory
TOP = $(abspath ../..)
APP_TOP = $(abspath ..)
TEST_TOP = $(APP_TOP)/tests
APP_SRC = $(APP_TOP)/src
TEST_SRC = $(TEST_TOP)/src

# Include directories
INC_DIRS = $(TOP)/ADS/AdsLib/
INC_DIRS += $(TOP)/doctest/doctest/
INC_DIRS += $(TEST_SRC)
INC_DIRS += $(APP_SRC)

# Source directories for VPATH
SRC_DIRS = $(APP_SRC) $(TEST_SRC)
VPATH = $(SRC_DIRS)

# Sources
SOURCES_NEEDED_BUT_NOT_BEING_TESTED = \
  AdsDef.cpp \
  AdsLib.cpp \
  AmsConnection.cpp \
  AmsNetId.cpp \
  AmsPort.cpp \
  AmsRouter.cpp \
  Log.cpp \
  NotificationDispatcher.cpp \
  Sockets.cpp \
  Frame.cpp

SOURCES_UNDER_TEST = \
  AdsBulkBucket.cpp \
  AdsRequestType.cpp \
  AdsBulkParameter.cpp

# Test sources (basenames only)
TEST_SOURCES = $(notdir $(wildcard $(TEST_SRC)/*.cpp))

# All sources to compile (basenames)
ALL_SOURCES = $(SOURCES_NEEDED_BUT_NOT_BEING_TESTED) \
              $(SOURCES_UNDER_TEST) \
              $(TEST_SOURCES)

# -I commands
INCLUDES = $(addprefix -I, $(INC_DIRS))

# Object files (in build directory)
BUILD_DIR = $(TEST_TOP)/build
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(ALL_SOURCES))

# Test executable
TEST_EXEC = test_runner

.PHONY: all test clean show-includes show-source debug

all: test

test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(OBJECTS)
	@echo "Linking: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Generic rule for all .cpp files (uses VPATH to find them)
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "Compiling: $< (found via VPATH)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TEST_EXEC)

show-includes:
	@echo "Include directories:"
	@echo "$(INC_DIRS)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "Compiler flags:"
	@echo "$(INCLUDES)" | tr ' ' '\n' | sed 's/^/  /'
  
show-source:
	@echo "VPATH: $(VPATH)"
	@echo ""
	@echo "All sources (basenames):"
	@echo "$(ALL_SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "Object files needed:"
	@echo "$(OBJECTS)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "Checking if sources exist:"
	@for src in $(ALL_SOURCES); do \
		found=false; \
		for dir in $(VPATH); do \
			if [ -f "$$dir/$$src" ]; then \
				echo "  ✓ $$src (in $$dir)"; \
				found=true; \
				break; \
			fi; \
		done; \
		if [ "$$found" = "false" ]; then \
			echo "  ✗ $$src (NOT FOUND)"; \
		fi; \
	done

debug: show-includes show-source
	@echo ""
	@echo "Build directory exists: $(shell [ -d $(BUILD_DIR) ] && echo YES || echo NO)"
	@echo ""
	@echo "Make will search for sources in:"
	@for dir in $(VPATH); do echo "  $$dir"; done