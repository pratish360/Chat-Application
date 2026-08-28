CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pthread -Iinclude
SRC_DIR := src
BUILD_DIR := build

COMMON_SRCS := $(SRC_DIR)/Socket.cpp $(SRC_DIR)/Message.cpp
COMMON_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(COMMON_SRCS))

.PHONY: all clean

all: chat_server chat_client

chat_server: $(BUILD_DIR)/main_server.o $(BUILD_DIR)/ChatServer.o $(BUILD_DIR)/ChatLogger.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

chat_client: $(BUILD_DIR)/main_client.o $(BUILD_DIR)/ChatClient.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) chat_server chat_client
