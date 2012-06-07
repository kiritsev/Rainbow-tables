CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		RBT.o RainbowTable.o Database.o ChainsGenerator.o sha1.o threadpool.o cudaFunctions.o

LIBS =  -lsqlite3 -lpthread

TARGET =	RBT

TARGETC = RBTCUDA

OUT_DIR=bin

OUT_OBJS=$(addprefix $(OUT_DIR)/,$(OBJS))

all: clean $(TARGET) 

$(TARGET): $(OUT_OBJS)
	nvcc $^ -o $@ $(LIBS)

$(OUT_DIR)/%.o: threadpool/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(OUT_DIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(OUT_DIR)/%.o: Database/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(OUT_DIR)/%.o: sha1/%.c
	$(CXX) -c $(CXXFLAGS) $< -o $@
	
$(OUT_DIR)/%.o: %.cu	
	nvcc -c $< -o $@
	
clean:
	rm -f $(OUT_DIR)/*.o $(TARGET)

test:
	./$(TARGET) test

