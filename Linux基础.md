# Linux基础

## Linux文件目录

1：Linux中所见皆文件

2：linux 根目录中有 bin  boot  dev  etc  home  lib  lib64  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var

​	**bin:**存有所有命令 包括常用的ls 、cd

​	**boot：**linux的启动文件

​	**etc：**linux的配置文件 其中passwd存有用户信息

​	**home：**除root用户外新创建的用户都在这里，每一个用户一个总的文件夹

​	**lib lib64：**操作系统使用的库都在这里

​	**usr：**安装第三方应用的资源目录包括

​		bin  config  games  include  lib  lib64  libexec   

3：linux文件类型

​	**普通文件：**-

​	**目录：**d

​	**字符设备文件：**c

​	**块设备文件：**b

​	**软连接：**l

​	**管道文件：**p

​	**套接字：**s

​	**-未知文件-**

​	注意： - d l 是真正占用磁盘空间的，而c，b，p，s是伪文件

## 容易忘记的命令

​	**ls -R:** 递归查看

​	**which 命令：**查看命令所在的文件目录

​	**cp -a 目标目录 目的目录：**将目标目录 全部拷贝到目的目录 （全部拷贝包括权限时间等信息）

​	**tac 目标文件：**倒着读文件内容

​	**ln -s 源目录/文件 目录/文件：**为 源目录/文件创建一个软连接（快捷方式）**（软连接）**

​		注意可使用相对路径或绝对路径

​		若通过快捷方式对原文件进行修改要参考原文件的权限

​	**alias 别名=‘内容’** **：**用别名代替内容

​	**ln 源目录/文件 目录/文件：**为 源目录/文件创建一个硬连接	**（硬连接）**

​		操作系统会给每个文件赋予一个inode，创建的硬连接文件会和原文件有相同的inode ，对持有相同的inode的其中任何一个修改都会			使得所有文件都改变，当删除一个文件时硬连接计数减1，当减为0时，该inode被释放

​	stat **文件** 查看文件属性			

```bash
[u1@iZ0jleb69f376uvegxzt6oZ ~]$ stat /tmp/root.ss 
  File: /tmp/root.ss
  Size: 76        	Blocks: 8          IO Block: 4096   regular file
Device: fd01h/64769d	Inode: 117723111   Links: 3 #硬连接计数为3
Access: (0664/-rw-rw-r--)  Uid: ( 1001/      u1)   Gid: ( 1001/      u1)
Access: 2025-06-26 00:41:58.839793150 +0800
Modify: 2025-04-16 17:47:13.981707779 +0800
Change: 2025-06-26 00:41:44.694142809 +0800
 Birth: 2025-04-16 17:47:13.981707779 +0800

```

​	**find /path -name "filename.txt"**  	# 精确匹配文件名
​		**find /path -name "*.log"**        		# 查找所有 .log 文件
​		**find /path -iname "file*"**      	 	# -iname 忽略大小写

​	find /path -type 文件类型			#按文件类型查找

​	**grep -r ‘文件中的内容’ 路径 -n**  		#查找路径下文件内容并且显示行号

​	**ps aux | grep “过滤内容”**

​	**find ./*.conf | xargs  grep "redis"** 	 #若要用管道那么加 **xargs**

​	**strace 可执行程序**					#跟踪可执行程序运行时的系统调用

​	----------------------------------------------软件安装--------------------------------------------------------------------------

​	0.设置软件源（需联网）

​		





​	**1. 搜索软件包**

​		sudo dnf search 软件名      # 例如：sudo dnf search nginx

​	**2. 安装软件**

​		sudo dnf install 软件名     # 例如：sudo dnf install nginx

​	**3. 卸载软件**

​		sudo dnf remove 软件名      # 例如：sudo dnf remove nginx

​	**4. 更新软件包**

​		sudo dnf update            # 更新所有已安装的软件包

​		sudo dnf update 软件名      # 更新指定软件（如：sudo dnf update nginx）

​	**5. 列出已安装的软件**

​		sudo dnf list installed    # 列出所有已安装的包

​		sudo dnf list installed | grep 软件名  # 过滤特定软件

​	**6. 启用 EPEL 仓库（额外软件源）**

​		如果官方仓库没有需要的软件，先启用 EPEL（Extra Packages for Enterprise Linux）：

​		sudo dnf install epel-release

​	**7. 清理缓存**

​		sudo dnf clean all         # 清理下载的缓存包

​	**8. 其他常用选项**

​		sudo dnf groupinstall  # “开发工具” #安装软件组（如开发工具）

​		sudo dnf history   	#查看 DNF 操作历史

---------------------------------------------压缩解压---------------------------------------------------------------------------------------

​	**tar zcvf 要生成的压缩包文件名 压缩材料**

​	**tar zxvf 要解压的压缩包名**

## umask

```
[u1@iZ0jleb69f376uvegxzt6oZ ~]$ umask
0002
```

`umask`（User File Creation Mask）是 Linux/Unix 系统中用来 **控制新建文件或目录的默认权限** 的重要设置。你看到的 `0002` 是当前用户的 	`umask` 值，它的作用如下：

### **1. umask 的作用**

- **定义默认权限的“反码”**：
  `umask` 值会从系统默认权限中 **减去** 对应的权限，最终得到新文件/目录的实际权限。

- **权限计算规则**：

  - **目录的默认权限**：`777 - umask`
    （目录默认满权限 `rwxrwxrwx`，即 `777`）

  - **文件的默认权限**：`666 - umask`
    （文件默认无执行权限 `rw-rw-rw-`，即 `666`）

    #### **示例计算**

    - **新建目录**：
      `777 - 002 = 775` → 权限为 `rwxrwxr-x`
      （所有者：`rwx`，组：`rwx`，其他人：`r-x`）
    - **新建文件**：
      `666 - 002 = 664` → 权限为 `rw-rw-r--`
      （所有者：`rw-`，组：`rw-`，其他人：`r--`）
    - **注意：**新创建的文件默认没有执行权限x
  
  ### **2. 目录的权限**、
  
  ​	在 Linux/Unix 系统中，目录的权限 `rwx`（读、写、执行）与普通文件的权限含义不同，具体解释如下：
  
  ------
  
  ### **目录权限 `rwx` 的含义**
  
  | 权限            | 字母表示 | 数字表示 | 作用                                                         |
  | :-------------- | :------- | :------- | :----------------------------------------------------------- |
  | **r** (read)    | `r`      | `4`      | **列出目录内容**（如 `ls` 命令），但不能访问文件详情。       |
  | **w** (write)   | `w`      | `2`      | **修改目录内容**（创建、删除、重命名目录内的文件或子目录）。 |
  | **x** (execute) | `x`      | `1`      | **进入目录**（`cd` 命令）或访问目录内文件的元数据（如 `stat`）。 |
  
  **如何修改**：
  
  ​	**临时修改**	 umask 0022  # 设置为 **0022**
  
  ​	**永久修改**	对当前用户：编辑 `~/.bashrc` 或 `~/.bash_profile`，添加：umask 0022
  
  ​				   对所有用户：修改 `/etc/profile`（需 root 权限）。
  
  ## 配置文件
  
  | 文件                  | 作用范围 | 执行时机               | 典型用途                       |
  | :-------------------- | :------- | :--------------------- | :----------------------------- |
  | `/etc/profile`        | 全局     | 登录 Shell             | 系统级环境变量、`umask`        |
  | `/etc/profile.d/*.sh` | 全局     | 由 `/etc/profile` 加载 | 模块化管理环境变量             |
  | `~/.bash_profile`     | 用户级   | 登录 Shell             | 个人环境变量、加载 `~/.bashrc` |
  | `~/.bashrc`           | 用户级   | 非登录 Shell           | 别名、函数、终端设置           |

​		 **`/etc/bashrc**` 是系统级的shell设置

​		**3. 与 `~/.bashrc` 的关系**

| 文件          | 作用范围 | 优先级 | 加载顺序                 |
| :------------ | :------- | :----- | :----------------------- |
| `/etc/bashrc` | 所有用户 | 低     | 先执行                   |
| `~/.bashrc`   | 当前用户 | 高     | 后执行（会覆盖同名设置） |



## 动态库和静态库

​	**静态库：**将库文件编译到可执行程序中，开销大，但速度快；

​	**动态库（共享库）：**动态库会加载到内存中，当可执行程序要用到库中函数时，会调用内存中的函数，速度比静态库略慢，但是开销小，多		个可执行程序可以共用一个库，对磁盘的开销小。

### 静态库

​	**ar   rcs libxxx.a    *.o文件**：使用*.o文件创建名为libxxx.a的静态库

​	**制作方法：**

​			1.将源程序编译成为.o结尾的二进制文件（编译加 -c）

​				gcc -c xxx.c 

​			2.使用 ar   rcs libxxx.a    *.o 命令创建出名为libxxx.a的静态库

​	**使用：**		

​		要使用到库中的函数时，需要和源程序一起编译：			

​		**gcc** test.c  libxxx.a **-o** test 

### 动态库

​		**制作方法：**

​				1.将源程序编译成为.o结尾的二进制文件（编译加 -c 最后加-fPIC）

​					**gcc -c** xxx.c -o xxx **-fPIC**

​				2.使用gcc -shared制作动态库

​					**gcc** **-shared -o** libxxx.so  *.o

​				3.编译可执行程序时，指定所使用动态库。-l:指定库名-lxxx  -L：指定动态库路径 -Llibdir

​					**gcc** test.c **-o** test -**l**xxx -**L**libdir

​		**使用：**

​				直接./test 会报错：./test: error while loading shared libraries: libmylib1.so: cannot open shared object file: No 					such file or directory

​				原因：

​					链接器：工作于**链接阶段**，工作时要指定 -I -L -l。

​					动态链接器:工作于程序**运行阶段**，工作时要提供动态库路径。(设置环境变量LD_LIBRARY_PATH的值)

​					1.**vim** ~/.bashrc,在该文件中添加**export** LD_LIBRARY_PATH=./libdir

​					2.source ~/.bashrc(使得刚刚修改的生效，对该用户有效)		

​					另外一种方法 修改/etc/ld.so.conf

​								1.sudo vim /etc/ld.so.conf

​								2.写入动态库的绝对路径 ，保存

​								3.sudo ldconfig -v 使得修改生效	

​								4../test 运行成功（ldd 命令查看可执行程序运行时需要那些动态库：ldd test）				     

​		

​		

​										

​						





## gcc问题

​	gcc：

​		-I 		指定头文件所在目录

​		-c		只做预处理、编译、汇编生成二进制文件（.o）

​		-g 	       编译时添加调试语句，支持gdb调试

​		-Wall	  显示所有警告信息

​	**编译时找不到库文件：gcc: error: libmylib.a: No such file or directory**

​		**1：指定库完整路径**

​			**gcc** /path/to/libmylib.a test.c **-o** test **-Wall**

​		**2：使用-L指定库搜索路径**

​			**gcc** test.c **-L**/path/libdir libmylib.a **-o** test **-Wall**

​			若库名复合libxxx.a规范，可以用-lxxx链接：**gcc** test.c **-L**/path/libdir **-l**mylib **-o** test **-Wall**

​	**gcc链接顺序是从左到右，若a依赖于b，那么a在前b在后**

​			

## 	Linux小问题

​	1.export 命令	

​					在 Linux/Unix 系统中，export 命令用于**设置 环境变量**，但其作用域和持久性取决于 如何使用 和 在哪个文件中配置。

​					**export` 的作用**

​								**功能**：将变量提升为 **环境变量**，使它对当前 Shell 及其所有子进程可见。

​								**临时生效**：直接在终端执行 `export`，变量仅在当前 Shell 会话有效，关闭终端后失效。

​								**永久生效**：将 `export` 命令写入 Shell 的配置文件（如 `~/.bashrc`、`~/.bash_profile`），每次登录 									Shell 时自动加载。

​					**可以在不进入~/.bashrc的情况下直接修改：**

​							**echo** ‘export LD_LIBRARY_PATH=$export LD_LIBRARY:your_path’  **>>** ~/.bashrc				

​							cat << ‘EOF’ >> ~/.bashrc

​							export LD_LIBRARY_PATH=$export LD_LIBRARY:your_path

​							EOF

**文件描述符：**

​			pcb进程控制块（本质上时内核的一个结构体）有一个成员指针，这个指针维护属于该进程一个文件描述符表，<fd>  <ptr>

​			平常使用的fd，实际上是键值对的fd，ptr指向打开的文件

**inode与dentry：**

​			dentyr(目录项) 包含文件名和inode编号

​			inode：是一个结构体，成员包括文件的各种信息，包括该文件在磁盘的位置

​			创建硬链接，本质是创建一个相同inode，不同文件名的目录项

​			**创建一个文件就是创建一个dentry与磁盘相对应**

**软链接（符号链接）与硬链接区别：**

​							1，软链接是新 inode + 新 dentry，数据块里存“目标路径字符串”					

​							2，可以跨文件系统、可链目录，但删掉原文件会变“悬挂链接” (dangling)

​							3，ls -li 看 inode 号，硬链相同，软链不同

**linux32位内存模型**

<img src="E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250628185443672.png" alt="image-20250628185443672" style="zoom:50%;" />

## 虚拟地址到物理地址的转换

**MMU** 全称是 **Memory Management Unit**（内存管理单元），它是 **CPU 中的一个硬件模块**，负责 **虚拟地址 → 物理地址的转换**。

你的代码（虚拟地址）  
      	↓  
	CPU 发出虚拟地址  
    	  ↓  
	MMU 拿到虚拟地址 → 拆成 页号 + 偏移量  
     	↓  
	查页表（通常是操作系统准备的）  
   	   ↓  
	找到对应的 物理页帧号  
     	 ↓  
	组合出物理地址（页帧地址 + 偏移）  
    	  ↓  
	访问内存

**简记为：**

1️⃣ 虚拟地址 → 2️⃣ 拆成页号+偏移 → 3️⃣ 查页表 → 4️⃣ 得到物理地址 → 5️⃣ 访问内存

**总结一句话**

> **MMU 是硬件级别的“地址翻译器”，它让每个程序都可以有自己的“看起来独立”的内存空间，同时让操作系统能控制权限、分配和隔离内存。**



**当你访问 *(&a)，CPU 和 MMU 的流程大致是：**

1. CPU 拿到虚拟地址 0x804A02C
2. 拆成 页号 + 页内偏移
3. 查页表，找页号对应的物理页帧号
4. 用 页帧地址 + 偏移量 → 得到物理地址
5. 访问物理内存



**C语言真正调用链：**

​	![image-20250712020438507](E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250712020438507.png)



**如遇到有些明明包含了头文件还是报错的main.c开头加：**

​					**#define _POSIX_C_SOURCE 200809L**		 #我要启用 2008 版 POSIX API

是告诉系统头文件“我需要这些 POSIX 接口”，不写它，有些函数或类型就不暴露出来了，尤其在 VSCode 或大项目中非常重要。

## GDB调试

​	基本使用：

​			1, 编译时添加-g选项

​			2, gdb ./a.out

​	命令：

​			list（l）（n）：		列出源码（第几行）

​			break(b)  (n) :		  第n行添加断点

​			run(r) :			       运行程序

​			next(n):			     下一条指令（会跳过函数）

​			step（s):			  下一步指令（会进入函数）

​			print(p) 变量:		   查看变量的值

​			start:				程序从开头开始执行，后续可用n/s

​			finish:			       结束当前调用，返回调入点

​			continue:			执行后续代码，或执行到下一个断点

​			info b:		      	查看所有断点的信息

​			b 20 if i=5           	     当i=5的时候第20行断点生效

​			ptype 变量：		   查看变量的类型

​			bt：				  列出存活着的栈帧

​			frame 栈帧编号	      切换到对应的栈帧

​			display 变量：		跟踪变量的值（可以多个）

​			undisplay 变量编号	取消跟踪对应变量编号变量的值

​			watch var			变量被修改时暂停

​			tbreak 函数名   	       断点命中一次后自动删除

​			quit：				退出gdb调试	    

​			

​	其他用法：

​			1:直接run ，若出现段错误，会停在错的那条语句，并且有原因；

​			2:若使用带参数的main函数，刚进入gdb调试的时候，可以set args【1】args【2】args【3】... 设置参数

​			    或

​			  run args【1】args【2】args【3】 

​			3，使用gdb调试的时候，gdb只能跟踪一个进程，可以在fork调用前

​				set follow-fork-mode chilld 命令gdb在fork后跟踪子进程

​				set follow-fork-mode parent 跟踪父进程

​		

​													**栈帧**

​		<img src="E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250629132514623.png" alt="image-20250629132514623" style="zoom:67%;" />

## Makefile

```makefile
#头文件目录
INC_DIR = ./inc
#编译器
CC = gcc
#编译器参数
CFLAGS = -Wall -g -I$(INC_DIR)
#链接器参数
LDFLAGS = -g
#**Dependency Flags**：用于生成依赖文件的特殊选项，如：`-MMD -MP`（告诉 gcc 输出 `.d` 文件）这个选项在编译的时候加上会在修改头文件时会被感知
DEPFLAGS = -MMD -MP
#源文件目录
SRC_DIR = ./src

#目标文件目录
OBJ_DIR = ./obj
#应用程序目录
BUILT_DIR = ./app
#源文件
SRC = $(wildcard $(SRC_DIR)/*.c)
#目标文件
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC)) #注意这里

#要生成的可执行程序名称
TARGET = main
#伪目标
.PHONY: all clean
#最终目标
all: $(BUILT_DIR)/$(TARGET)
#链接
$(BUILT_DIR)/$(TARGET) : $(OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS) 
#通过模式规则编译
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c 
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)
# -------- 3. 让 Makefile 把生成的 .d 文件读进来 ------
-include $(OBJ:.o=.d)
clean:
	-rm -rf $(OBJ_DIR) $(BUILT_DIR)
```

注：

​	1, all 和 .phnoy 后面的：后要加一个空格

​	2，每条语句的后面不要加空格或者tab

​	3，@mkdir -p $(@D) :若目录不存在则建立

​					1)：第一个@ 表示不会把这条语句打到终端上，终端更干净

​					2)：-p表示若要创建的目录已存在，则跳过这条语句，否则就创建目录

​					3)：$ @是目标文件，

​					      $(@D)为目录部分

​					      $(@F)为文件部分

​	4,`-MMD` ：对每个 `foo.c` 同时生成 `foo.d`，内容是 “foo.o 依赖 foo.c、add.h、…”。

​	5,`-MP` ：给每个头文件生成 **dummy 目标**，防止头文件被删除时 make 报错。

​	6, -include $(OBJ:.o=.d):让 make 在读取自己的规则后，再把所有.d 文件“拼进”依赖图。用 - 前缀是为了 首次构建时 .d 文件尚不存在也不报错。

​	

​	

| 变量名     | 全称（意义）         | 用于阶段         | 作用                                                         |
| ---------- | -------------------- | ---------------- | ------------------------------------------------------------ |
| `CC`       | **C Compiler**       | 编译 & 链接      | 指定 C 编译器（如 `gcc`）                                    |
| `CFLAGS`   | **C Compiler Flags** | 编译阶段（`-c`） | 编译器选项，例如：`-Wall -g -O2 -I./inc` 等                  |
| `LDFLAGS`  | **Linker Flags**     | 链接阶段         | 链接器选项，例如：`-g -lm -lpthread -L./lib` 等              |
| `DEPFLAGS` | **Dependency Flags** | 编译阶段         | 用于生成依赖文件的特殊选项，如：`-MMD -MP`（告诉 gcc 输出 `.d` 文件） |

​					![image-20250629204857461](E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250629204857461.png)

| 变量名     | 全称（意义）         | 用于阶段         | 作用                                                         |
| ---------- | -------------------- | ---------------- | ------------------------------------------------------------ |
| `CC`       | **C Compiler**       | 编译 & 链接      | 指定 C 编译器（如 `gcc`）                                    |
| `CFLAGS`   | **C Compiler Flags** | 编译阶段（`-c`） | 编译器选项，例如：`-Wall -g -O2 -I./inc` 等                  |
| `LDFLAGS`  | **Linker Flags**     | 链接阶段         | 链接器选项，例如：`-g -lm -lpthread -L./lib` 等              |
| `DEPFLAGS` | **Dependency Flags** | 编译阶段         | 用于生成依赖文件的特殊选项，如：`-MMD -MP`（告诉 gcc 输出 `.d` 文件） |

## git

1，设置用户名和邮箱

​	git config --global user.name “hiHjy”

​	git config --global user.email “huangjiyaun123@gmail.com”

​	设置好后 输入：git config --global --list(当前用户)

​				    查看是否配置成功

2，git项目创建

​	1，在项目文件下

​		 git init

​	2，克隆远程仓库目录

​		git clone url  

3，提交代码到本地仓库

​	git status			  #

​	git add .    			#将本目录下所有文件添加到暂存区

​	编写.gitignore文件	#忽视不想提交的文件

​		*.txt			

​		!lib.txt			#lib.txt不忽略

​		/temp			#忽略根目录下的temp的全部文件或目录，子目录下的temp不会被忽略

​		build/			#忽略项目下所有名为build的目录（递归忽略）

​		doc/*.txt		   #忽略/doc 下所有 txt文件。若想递归忽略，需用 `doc/**/*.txt`。

​		

​	git commit (-m)  “注释内容”	     #提交暂存区里的文件到本地仓库。-m为提交文件后跟着的注释信息（仅本次提交的文件）	

4, 设置本地绑定SSH公钥，实现免密码登录

​	# cd ~/.ssh				

​		ssh-keygen -t rsa    		#一路回车，生成公钥（id_rsa.pub），密钥（id_rsa），在~/.ssh文件下

​	#将公钥内容复制到GitHub

其他命令：

​				

| 需求                   | 命令                                         |
| ---------------------- | -------------------------------------------- |
| 查看所有本地分支       | `git branch`                                 |
| 查看所有远程分支       | `git branch -r`                              |
| 查看本地+远程          | `git branch -a`                              |
| 显示当前分支（更简洁） | `git symbolic-ref --short HEAD`              |
| 切换到 main            | `git switch main` 或 `git checkout main`     |
| 切换到 master          | `git switch master` 或 `git checkout master` |

​	git checkout -b 分支名 		#新建一个分支并切换到该分支 

​	git branch -d（r） 分支名		#删除一个（远程）分支

​	git push origin --delete 分支名 	#删除一个远程分支

​	git merge [branch]				#合并指定分支到当前分支

​	git restore . 						#恢复当前目录的改动

​	git fetch origin            		 # 拉取远程所有分支

​	git reset HEAD~N      			#混合重置（默认，撤销提交和暂存，但保留工作区修改）

| 操作               | 命令                        | 说明             |
| ------------------ | --------------------------- | ---------------- |
| 查看状态           | `git status`                | 最常用命令       |
| 添加文件           | `git add file`              | 或 `git add .`   |
| 提交               | `git commit -m "msg"`       |                  |
| 查看日志           | `git log --oneline`         | 简洁查看提交记录 |
| 撤销最近提交       | `git reset --soft HEAD~1`   | 保留改动         |
| 丢弃最近提交和改动 | `git reset --hard HEAD~1`   | 慎用             |
| 查看未 push 提交   | `git log origin/main..main` | 推送前检查       |
| 推送               | `git push`                  |                  |
| 拉取               | `git pull`                  |                  |
| 克隆项目           | `git clone URL`             |                  |

## 虚拟机使用代理

![image-20250630223615371](E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250630223615371.png)

port: 7890

socks-port: 7891

allow-lan: true

bind-address: 0.0.0.0

mode: Rule

log-level: info

external-controller: 0.0.0.0:9090



![image-20250630223718003](E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250630223718003.png)



![image-20250630223756748](E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250630223756748.png)





![image-20250630223857044](E:\360MoveData\Users\HJY\Desktop\Linux基础.assets\image-20250630223857044.png)



**设置ssh代理的~/.ssh/config**

​	Host github.com
  	HostName ssh.github.com
​	  Port 443
​	  User git
  	ProxyCommand nc -X 5 -x 192.168.1.8:7890 %h %p

​	`192.168.1.8` 是你 Windows 主机 IP

`		7980` 是 Clash 的 mixed-port

​	让 OpenSSH 通过 SOCKS5 代理连接 GitHub

**Clash 设置要求**

​	allow-lan: true

​	mixed-port: 7980（你也可以加 socks-port: 7891 但不是必须）

​	确保配置文件生效（默认配置 / 当前使用配置）





**测试命令** 

​	ssh -T git@github.com

​		输出 `Hi xxx!` 即成功。





































