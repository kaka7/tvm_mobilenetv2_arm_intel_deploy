#TVM环境安装
趁这段时间在家办公,整理安装过程,其中有些地方可能有误,还请见谅,
总体来说安装pc端的tvm较简单,按照官网给的教程来问题不大,在移动端rpc编译和部署时问题比较多,另外个人是非常看好tvm的,近期也在研究tvm,有想深入tvm的欢迎一起探讨交流,相互学习.
##版本信息
    版本:tvm 0.6-dev
    Os:Ubuntu 16.04
    Python 3.5.2
    llvm:8.0
##安装
官方说如果需要模型在CPU上运行，则必须安装LLVM。==难道其他target就不用llvm?==
==推荐ubuntu用apt直接安装,然后验证是否安装成功==
手动安装llvm,参考https://apt.llvm.org/ 比较难,推荐用ninja,而且编译结果比较大,还要装clang,不建议此方式,除非要用llvm开发
下载llvm源码: wget http://llvm.org/releases/6.0.0/llvm-6.0.0.src.tar.xz



###安装tvm
####下载TVM
    git clone --recursive https://github.com/apache/incubator-tvm tvm

####安装依赖
```
sudo apt-get update
sudo apt-get install -y python3 python3-dev python3-setuptools gcc libtinfo-dev zlib1g-dev build-essential cmake libedit-dev libxml2-dev
```



####配置
    cd tvm      
    mkdir build      
    cp cmake/config.cmake  build    
    
    修改build/config.cmake
    set(USE_LLVM ON)
    set(USE_LLVM /path/to/your/llvm/bin/llvm-config)
 
    编译
    cd build         
    cmake ..         
    make -j4 

设置python路径：
```
export TVM_HOME=/path/to/tvm 
export PYTHONPATH=$TVM_HOME/python:$TVM_HOME/topi/python:$TVM_HOME/nnvm/python:${PYTHONPATH} 
```

Python 能 import  tvm 说明安装成功



#Android tvm RPC app构建
env:AS3.4
apps/android_rpc/工程,请严格按照官方的教程走一遍
##安卓环境准备
###下载AS3.4
百度有很多教程
###安装NDK和SDK
    Android NDK 是一组使您能将 C 或 C++（“原生代码”）嵌入到 Android 应用中的工具。
    1.在平台之间移植其应用。
    2.重复使用现有库，或者提供其自己的库供重复使用。
    3.在某些情况下提高性能，特别是像游戏这种计算密集型应用。 
    下载地址：
    https://developer.android.com/ndk/downloads/index.html
    将其解压到/opt 文件夹下，并修改环境变量： 
    export NDKROOT=/opt/ndk/android-ndk-r12b
    export PATH=$NDKROOT:$PATH 
    保存更新环境变量：
    source  ~/.bashrc 

###安装SDK
    下载  http://dl.google.com/android/android-sdk_r24.4.1-linux.tgz
    解压到/opt 目录下。
    （在外网利用android studio 下载好所有SDK文件，再拷贝到内网。） 
    环境变量
    export ANDROID_HOME=/opt/android-sdk-linuxexport PATH=$ANDROID_HOME/tools:$PATHexport PATH=$ANDROID_HOME/platform-tools:$PATH
    


###安装jdk
    下载：http://www.android-studio.org/
    tar zxvf jdk-8u77-linux-x64.tar.gz -C /opt
    bashrc里添加环境变量：
    export JAVA_HOME=/opt/jdk1.8.0_77
    export JRE_HOME=${JAVA_HOME}/jre
    export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib
    export PATH=.:${JAVA_HOME}/bin:$PATH 
    测试：
    java –version


###配置Maven
    下载：https://maven.apache.org/download.cgi
    解压到 /opt
    设置环境变量
    export MAVEN_HOME=/opt/maven
    export PATH=$MAVEN_HOME/bin:$PATH
###配置gradle
    下载：https://gradle.org/install/
    mkdir /opt/gradle
    unzip -d /opt/gradle gradle-5.5.1-bin.zip
    环境变量
    export PATH=$PATH:/opt/gradle/gradle-5.5.1/bin 

###编译TVM4J
    以上环境配好后就可以编译java 的tvm 运行环境了(tvm4j)。
    项目根目录下运行：
    make jvmpkg
    make jvminstall
    这里需要先编译好x86  linux下的tvm运行环境 libtvm_runtime.so 。
    利用TVM_NDK_CC=/opt/ndk/bin/aarch64-linux-android-ld
    (Linux与android的交叉编译工具链) 将linux下的libtvm_runtime.so转成TVM4J。
    注意：
    在项目的根目录下运行 make jvmpkg的时候会一直等待下载文件(因为是在内网)。
    解决办法：
    先在外网把需要的东西下载好，再到内网搭建个仓库，把文件拷贝到仓库。
    生成tvm4j后，修改配置文件：
    cd apps/android_deploy/app/src/main/jni
    cp make/config.mk .
    vim config.mk
    (根据自己的设备属性修改)
    APP_ABI = arm64-v8a
    APP_PLATFORM = android-17
    "# whether enable OpenCL during compile"
    USE_OPENCL = 0
    现在使用Gradle编译JNI，解析Java依赖关系，并与tvm4j一起构建Android应用程序。运行以下脚本生成apk文件。
    cd apps/android_deploy
    gradle clean build
    运行 gradle clean build 同样会遇到上述类似下载问题，解决方法与上类似。
    这样就生成了能在安卓手机上直接运行的apk文件。

##构建rpc apk并安装
###构建
    export ANDROID_HOME=[Path to your Android SDK, e.g., ~/Android/sdk]
    cd apps/android_rpc
    gradle clean build 
In app/build/outputs/apk you'll find pp-release-unsigned.apk, use ev_tools/gen_keystore.sh to generate a ignature and use dev_tools/sign_apk.sh to get the signed apk file app/build/outputs/apk/release/tvmrpc-release.apk.
###安装
插入安卓设备,Upload tvmrpc-release.apk to your Android device and install it:
```
$ANDROID_HOME/platform-tools/adb install pp/build/outputs/apk/release/tvmrpc-release.apk
```
成功安装tvm rpc app
通过adb install 安装容易出现其他问题,比如已经安装过同名的apk等,百度就可以解决,另外我已修改成通过AS就可以安装apk省去很多问题
###测试
    在手机rpc app上输入ip 和端口后运行
    启动
    python -m tvm.exec.rpc_tracker --port 9190
    显示INFO:root:RPCServer: bind to 0.0.0.0:9090

    查询
    python -m tvm.exec.query_rpc_tracker --host=0.0.0.0 --port=9190


Queue Status
key     | total|  free | pending|
--------| -----|  --   |-----|
android |  1   |   1   |   0 |

如上结果代表正确检测到设备
```
Then checkout android_rpc/tests/android_rpc_test.py and run, # Specify the RPC trackerexport TVM_TRACKER_HOST=0.0.0.0export TVM_TRACKER_PORT=[PORT]# Specify the standalone Android C++ compilerexport TVM_NDK_CC=/opt/android-toolchain-arm64/bin/aarch64-linux-android-g++
```
python android_rpc_test.py
出现如下代表rpc 按正确安装
```
Run CPU test ...
0.000962932 secs/op
```
