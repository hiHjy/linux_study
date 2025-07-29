#! /bin/bash
var=5
var2=        #可以是空串 也可以 var2=后面什么都没有
var3=niha_ohhhasd_fasdfasdfa #默认是字符串，必须连起来
str_="我是字符串"        #标准正规军字符串或者‘’
echo $var
printf "%s\n" 111g          #竟然能直接用printf
#echo  $var2
echo $var3
var4=1 2 3 4 # 2 会被解析成命令，没有命令所以会报错 ./test.sh: 行 10: 2：未找到命令
#########################################################
math1=$((1+1))
echo $math1
(( 2 > 1 )) && echo "false" # 如果 2 > 1 为真 整个表达式的值为1，但是返回状态码为0表示执行成功， 则执行 echo “false”   *_*
math2=$(( 1 > 2 ))
echo $math2         # 0
#########################---命令---######################################

cmd1=$(ls -al)      #将ls -al 的结果集以字符串的形式保存到cmd1里
echo "$cmd1"        #将cmd1的执行结果保留格式的输出
echo $cmd1         #cmd1的执行结果一股脑的输出


#echo
echo "你好\n"   #白板笨逼echo只会把\n 当字符串输出 
echo -e "你好\n"    #加了-e的echo会解析\n
echo -n "你好\n"    #消除字符串后面的换行 输出：你好\nhjy@ubuntu:~/Documents/linux_study/shell$ 
echo -en "你好\n"

#printf
printf "[%8s] [%-8s] %08x\n" "abc" "123" "777" #shell 的printf

#cat
cat /proc/uptime

cat > notes.txt<<EOF                            ## >> 追加内容到notes.txt > 清空然后将下面的内容填入文件
你好啊啊啊啊多发点今飞凯达这架飞机阿萨达嘎达嘎达刚刚
阿斯顿发打暑假工
阿道夫大升降开关
阿打发东方精工

EOF
str1="moren"                        
str=""
#read -rep "输入一个字符串" str #读键盘输入的内容到变量str
str=${str:-$str1}       
echo "$str"          #如果str为空，则str=str1，否则则是你输入的值
######################################################################################
if [ "1" == "2" ]; then                 # if的第一种形式
    echo jj
else 
    echo oo nihao have\'n qweqweq
fi

var1=5
var2=4
#算数比较
if (( var1 == var2 )) ; then
    echo 相等
else
    echo 不等
fi

#字符串比较
if [[ "$var1" == "$var2" ]]; then
    echo 字符串相等
else
    echo 字符串不等
fi
var1=hh111
var2=4
#模式匹配

if [[ "$var1" == "$var2" ]]; then
    echo "完全相等"
elif [[ "$var1" == h* ]]; then #模式匹配
    echo "var1以h开头"

fi
echo "$1"
echo "$2"
echo true


#while
cnt=100
while (( cnt-- >= 0 )); do
    
    echo 1;
done

str=j
while [[ $str == "jj" ]]; do
    echo 1
done 

#case/in

while true; do
    read -rep "输入字符串测试case in" str
    case $str in
    1)
        echo case 1
        ;;
    33)
        echo case 33
        ;;
    exit)
        echo case exit
        break
        ;;
    *)
        echo other
        ;;
    esac
done

#for

for i in 1 2 3; do
    echo $i
done


for (( i =0; i < 5; i++ ));do
    echo $i
done


fun() {
    echo "$1"
}
fun "hello world"