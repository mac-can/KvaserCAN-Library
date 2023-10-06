BINARIES = ../../Binaries
INCLUDES = -I../../INCLUDES

TARGET1 = can_recv
SOURCE1 = $(TARGET1).cpp $(BINARIES)/libKvaserCAN.a
DEFINES1 = -DCHANNEL=0 -DBAUDRATE=CANBTR_INDEX_250K

TARGET2 = can_send
SOURCE2 = $(TARGET2).cpp $(BINARIES)/libUVCANKVL.a
DEFINES2 = -DCHANNEL=1 -DBAUDRATE=CANBTR_INDEX_250K

CXXFLAGS =
LDFLAGS = -Wl,-framework -Wl,IOKit -Wl,-framework -Wl,CoreFoundation \
	-rpath /usr/local/lib -lpthread

CXX = clang++
LD = clang++
RM = rm -f

all: $(TARGET1) $(TARGET2)

$(TARGET1):	$(SOURCE1)
	$(CXX) $(SOURCE1) $(DEFINES1) $(INCLUDES) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET1)

$(TARGET2):	$(SOURCE2)
	$(CXX) $(SOURCE2) $(DEFINES2) $(INCLUDES) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET2)

clean:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2)
