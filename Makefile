PWD=$(realpath ./)
SRC_DIR=$(PWD)/src
TEST_DIR=$(PWD)/tests
CXX=g++
CFLAGS= -c -std=c++11 --coverage -fprofile-arcs -g -I $(TEST_DIR)/AxxTest/include -I $(TEST_DIR)/AxxTest/cxxtest-4.4 -I $(SRC_DIR) -I $(TEST_DIR)/dependencies/PubSubClient
LDFLAGS=--coverage -fprofile-arcs
SOURCES=$(wildcard $(TEST_DIR)/fakes/*.cpp) $(TEST_DIR)/AxxTest/include/Arduino.cpp $(TEST_DIR)/AxxTest/include/AxxTest.cpp
OBJECTS=$(SOURCES:.cpp=.o) $(SRC_DIR)/MQTTSwitch.o


# Build all the Unit Tests ready to be run
all: buildTestsMQTTSwitch buildTestsMQTTSimpleSwitch buildTestsMQTTLight

# Build and run all the Unit Tests
test: testMQTTSwitch testMQTTSimpleSwitch testMQTTLight
	
# Build the Unit Under Test
unitUnderTest: $(SRC_DIR)/MQTTSwitch.cpp $(SRC_DIR)/MQTTSwitch.h
	@echo "Compile Unit Under Test"
	@$(CXX) $(CFLAGS) $(SRC_DIR)/MQTTSwitch.cpp -o $(SRC_DIR)/MQTTSwitch.o
	

###########################################
#####       MQTTSwitch Tests     ##########
###########################################

testMQTTSwitch: buildTestsMQTTSwitch
	@$(TEST_DIR)/runners/MQTTSwitchTests.exe || true
	

buildTestsMQTTSwitch: unitUnderTest updateTestsMQTTSwitch $(OBJECTS)  
	@echo "Build MQTTSwitchTests"
	@$(CXX) $(LDFLAGS) $(OBJECTS) $(TEST_DIR)/runners/MQTTSwitchTests.o -o $(TEST_DIR)/runners/MQTTSwitchTests.exe
	
	
updateTestsMQTTSwitch: $(TEST_DIR)/MQTTSwitchTests.h
	@echo "Generate MQTTSwitchTests class"
	@test -d $(TEST_DIR)/runners || mkdir $(TEST_DIR)/runners
	@python $(TEST_DIR)/AxxTest/cxxtest-4.4/bin/cxxtestgen --error-printer -w "MQTTSwitchTests" -o $(TEST_DIR)/runners/MQTTSwitchTests.cpp $(TEST_DIR)/MQTTSwitchTests.h
	@$(CXX) $(CFLAGS) $(TEST_DIR)/runners/MQTTSwitchTests.cpp -o $(TEST_DIR)/runners/MQTTSwitchTests.o
	

###########################################
#####    MQTTSimpleSwitch Tests  ##########
###########################################

testMQTTSimpleSwitch: buildTestsMQTTSimpleSwitch
	@$(TEST_DIR)/runners/MQTTSimpleSwitchTests.exe || true
	

buildTestsMQTTSimpleSwitch: unitUnderTest updateTestsMQTTSimpleSwitch $(OBJECTS)  
	@echo "Build MQTTSimpleSwitchTests"
	@$(CXX) $(LDFLAGS) $(OBJECTS) $(TEST_DIR)/runners/MQTTSimpleSwitchTests.o -o $(TEST_DIR)/runners/MQTTSimpleSwitchTests.exe
	
	
updateTestsMQTTSimpleSwitch: $(TEST_DIR)/MQTTSimpleSwitchTests.h
	@echo "Generate MQTTSimpleSwitchTests class"
	@test -d $(TEST_DIR)/runners || mkdir $(TEST_DIR)/runners
	@python $(TEST_DIR)/AxxTest/cxxtest-4.4/bin/cxxtestgen --error-printer -w "MQTTSimpleSwitchTests" -o $(TEST_DIR)/runners/MQTTSimpleSwitchTests.cpp $(TEST_DIR)/MQTTSimpleSwitchTests.h
	@$(CXX) $(CFLAGS) $(TEST_DIR)/runners/MQTTSimpleSwitchTests.cpp -o $(TEST_DIR)/runners/MQTTSimpleSwitchTests.o
	

###########################################
#####       MQTTLight Tests      ##########
###########################################

testMQTTLight: buildTestsMQTTLight
	@$(TEST_DIR)/runners/MQTTLightTests.exe || true
	

buildTestsMQTTLight: unitUnderTest updateTestsMQTTLight $(OBJECTS)  
	@echo "Build MQTTLightTests"
	@$(CXX) $(LDFLAGS) $(OBJECTS) $(TEST_DIR)/runners/MQTTLightTests.o -o $(TEST_DIR)/runners/MQTTLightTests.exe
	
	
updateTestsMQTTLight: $(TEST_DIR)/MQTTLightTests.h
	@echo "Generate MQTTLightTests class"
	@test -d $(TEST_DIR)/runners || mkdir $(TEST_DIR)/runners
	@python $(TEST_DIR)/AxxTest/cxxtest-4.4/bin/cxxtestgen --error-printer -w "MQTTLightTests" -o $(TEST_DIR)/runners/MQTTLightTests.cpp $(TEST_DIR)/MQTTLightTests.h
	@$(CXX) $(CFLAGS) $(TEST_DIR)/runners/MQTTLightTests.cpp -o $(TEST_DIR)/runners/MQTTLightTests.o
	
	

.cpp.o:
	@echo "Compile source ($<)"
	@$(CXX) $(CFLAGS) $< -o $@
	
clean:
# Clean this folder
	@- rm $(SRC_DIR)/*.o || true
	@- rm $(SRC_DIR)/*.gcno || true
	@- rm $(SRC_DIR)/*.gcda || true
	@- rm $(SRC_DIR)/*.gcov || true
	
# Clear the tests/runners folder
	@- rm -rf tests/runners