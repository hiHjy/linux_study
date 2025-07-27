#头文件目录
#INC_DIR = ./inc
#编译器
CC = gcc
#编译器参数
CFLAGS = -Wall 
LDFLAGS = 
PTHREAD = -pthread
#DEPFLAGS = -MMD -MP
#源文件目录
SRC_DIR = .

#目标文件目录
OBJ_DIR = ./obj
#应用程序目录
BUILT_DIR = .
#源文件
SRC = $(wildcard $(SRC_DIR)/*.c)
#目标文件
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC)) #注意这里

#要生成的可执行程序名称
TARGET = server
#伪目标
.PHONY: all clean
#最终目标
all: $(BUILT_DIR)/$(TARGET)
#链接
$(BUILT_DIR)/$(TARGET) : $(OBJ) 
	@mkdir -p $(@D) 
	$(CC) $^ -o $@ $(LDFLAGS) $(PTHREAD)
#通过模式规则编译
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c 
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS) $(PTHREAD)
# -------- 3. 让 Makefile 把生成的 .d 文件读进来 ------
-include $(OBJ:.o=.d)
clean:
	-rm -rf $(OBJ_DIR) $(BUILT_DIR)/main