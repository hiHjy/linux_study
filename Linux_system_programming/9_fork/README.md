# 8_dup_dup2

## 🧠 学习目标
- 理解 `dup` 和 `dup2` 的作用和区别
- 掌握文件描述符的复制与重定向原理

## 🔧 主要函数

| 函数名 | 功能 |
|--------|------|
| `dup(oldfd)` | 复制 `oldfd`，返回新的最小可用 fd |
| `dup2(oldfd, newfd)` | 把 `newfd` 重定向到 `oldfd`，若 `newfd` 已打开会自动关闭 |
| `fcntl(fd, F_SETFD, FD_CLOEXEC)` | 设置 exec 时关闭 fd 的标志 |

## 🚀 编译 & 运行
```bash
make
./main test.txt
cat test.txt
