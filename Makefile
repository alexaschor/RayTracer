MKDIR      ?= mkdir
RM         ?= rm -f
FFMPEG     ?= ffmpeg

SOURCES    = $(wildcard src/*.cpp lib/*/*.cpp)
INCLUDES   = $(addprefix -I,$(wildcard src lib))
OBJECTS    = $(SOURCES:.cpp=.o)

CXX        = g++
CXXFLAGS   = -std=c++11 -Wall $(INCLUDES)

MOVIE      = movie.mp4
TARGET     = render
OBJ_DIR    = build
DEP        = .depend


all: fast

slow: CXXFLAGS += -g
fast: CXXFLAGS += -Ofast -g

slow: $(TARGET)
fast: $(TARGET)

.PHONY: $(DEP)
$(DEP): $(SOURCES)
	rm -f "$@"
	$(CXX) $(CXXFLAGS) -MMD $^ -MF "$@"


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	$(RM) $(TARGET) $(DEP) $(OBJECTS)

movie:
	$(FFMPEG) -y -r 30 -f image2 -s 800x600 -start_number 1 -i frames/frame.%04d.ppm -vframes 1000 -vcodec libx264 -crf 25 -pix_fmt yuv420p $(MOVIE)

para:
	$(TARGET) 0 75 & $(TARGET) 76 150 > /dev/null 2>&1 & $(TARGET) 151 225 > /dev/null 2>&1 & $(TARGET) 226 300 > /dev/null 2>&1 & wait

para8:
	$(TARGET) 0 38 & $(TARGET) 39 75 > /dev/null 2>&1 & $(TARGET) 76 113 > /dev/null 2>&1 & $(TARGET) 114 150 > /dev/null 2>&1 & $(TARGET) 151 188 > /dev/null 2>&1 & $(TARGET) 189 225 > /dev/null 2>&1 & $(TARGET) 226 263 > /dev/null 2>&1 & $(TARGET) 264 300 > /dev/null 2>&1 & wait

# include $(DEP)
