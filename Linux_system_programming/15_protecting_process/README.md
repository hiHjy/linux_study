#1 会话
    1，创建会话的进程不能是进程组长
    2，创建一个会话，取自己的id为进程组id，同时也是新会话的id
    3, 新会话会丢弃原有的终端，该会话没有控制终端

        pid_t setsid(); 

            @brief  创建一个会话
            @retval
                成功返回进程的会话id 
                失败 -1，errno

        pid_t getpgid(pid_t pid);

            @brief 得到pid的进程组id，传0表示获取当前进程的组id

            @retval 成功返回进程组id
                    失败 -1 ，errno    
        
        pid_t getsid(pid_t pid);

            @brief 获得pid的会话id，传0获得当前进程的会话id

            @retval 成功：会话id
                    失败：(pid_t) -1
-----------------------------------------------------------------------------------------------
#2 守护进程
    1，一般以.d结尾
    2,创建步骤
        （1）fork子进程， 让父进程终止
        （2）子进程setsid创建会话，让子进程独立，三id合一（pid，sid，gid）
        （3）通常根据需要，改变工作目录位置 chdir（）
        （4）通常根据需要，重设umask
        （5）通常根据需要，关闭或者重定向文件描述符，
        （6）守护进程业务逻辑。while（1）

    int chdir(const char *path);

        @brief 改变工作目录
        @retval 成功：0 失败：-1 errno
                
    mode_t umask(mode_t mask);

        @brief 设置会话的umask
        @retval 返回上一次的umask值，总是成功

