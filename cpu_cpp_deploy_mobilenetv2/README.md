
#intel cpu c++ deploy mobilenetv2
##env
    tvm 0.6-dev
    opencv;
##准备

tvm头文件包含在include中, dlpack,dmlc,tvm 三个库的头文件直接从tvm源码中拷贝出来;

    dlpack -> $TVM_HOME/3rdparty/dlpack/include/dlpack/
    dmlc -> $TVM_HOME/3rdparty/dmlc-core/include/dmlc/
    tvm -> $TVM_HOME/include/tvm/

tvm库文件包含在lib中,libtvm_runtime.so是在服务器端编译出来的;
将模型在python下保存为三个server端的文件,官方有很详细的教程

cmakelists.txt 重点

    include_directories("${CMAKE_CURRENT_LIST_DIR}/include")#//tvm 三个主要的头文件 dlpack,dmlc,tvm
    link_directories("${CMAKE_CURRENT_LIST_DIR}/lib")#//用于获取libtvm_runtime.so
    add_executable(mobilenetv2 server_deploy.cpp)


输入数据cat.png经过python numpy预处理后转化为二进制.bin文件后作为数据输入(包括预处理,在python下能正确得到结果后再保存为float32,numpy默认tofile类型是float64,否则结果不对)

##耗时和精度验证
    cpu:intel 2.5GHz
    avg infer Time: 0.00408683s
    The maximum   is: 0.250417    max index:282
    expected max val: 0.25041685  max index:282




