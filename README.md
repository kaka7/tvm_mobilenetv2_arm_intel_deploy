#tvm_mobilenetv2_arm_intel_deploy
>年前玩过一段时间的tvm,效果非常不错,有评测tvm>tensorrt>ncnn,也用过tensorrt,相比较而言,tvm真的是大杀器


个人觉得tvm亮点:
* 1 RPC:本地python代码运行在android上
* 2 autotvm:大一统的深度学习编译器,比tensorrt ncnn更通用
* 3 transferlearning 
* 4 VTA
* 5 量化

##说明:
* tvm_install_and_RPC.md:涵盖tvm安装及RPC 建议先看
* cpu_cpp_deploy_mobilenetv2:c++服务器版本的部署工程
* tvm_android_deploy_mobilenetv2 :Android apk工程


具体使用请参考各自的README.md,并结合起来效果最佳

##内容:
* tvm通过RPC autotvm arm/intel cpu 
* Android 和server两个版本的完整的工程化部署,确保可运行起来
* 性能测试






##结果
###不同设备测试(intel cpu,arm cpu)
model | baseline (intel cpu 2.5GHz) |	tvm intel cpu (2.5GHz) |	tvm android (arm,2.0G Hz)
- | - | - | - |
mobilenetv2             |	63              |	4.6                  |	15 |
mobilenetv2_half    |	60              |	1.8	                 |  7.5 |

###不同arm测试
arm cpu主频(G) |	1.5	|1.8|	2	|2.35
- | - | - | - |-
mobilenetv2(ms)	| 136|	16.3|	15.7	|14.9
mobilenetv2_half(ms)|	66|	7.6|	7.3|	6.3


##autotvm
注意几个参数timeout,n_trial,min_repeat_ms
* resnet18

    intel  cpu上core-avx2  2.5GHz min_repeat_ms=1000 45ms 
    Mobile cpu 2.0G autotuning 73.23 ms


mobile 上autotvm log

    Tuning...
    [Task  1/12]  Current/Best:   11.03/  23.88 GFLOPS | Progress: (400/400) | 906.04 s Done.
    [Task  2/12]  Current/Best:   11.24/  21.43 GFLOPS | Progress: (360/360) | 797.05 s Done.
    [Task  3/12]  Current/Best:   16.13/  22.01 GFLOPS | Progress: (648/648) | 1341.14 s Done.
    [Task  4/12]  Current/Best:   18.40/  21.90 GFLOPS | Progress: (576/576) | 1150.77 s Done.
    [Task  5/12]  Current/Best:   13.98/  22.73 GFLOPS | Progress: (768/768) | 1673.31 s Done.
    [Task  6/12]  Current/Best:   17.19/  22.24 GFLOPS | Progress: (672/672) | 1374.25 s Done.
    [Task  7/12]  Current/Best:    2.60/  22.95 GFLOPS | Progress: (784/784) | 1626.31 s Done.
    [Task  8/12]  Current/Best:   13.49/  23.92 GFLOPS | Progress: (252/252) | 606.69 s Done.
    [Task  9/12]  Current/Best:    4.12/  18.10 GFLOPS | Progress: (784/784) | 1679.14 s Done.
    [Task 10/12]  Current/Best:    2.84/  18.04 GFLOPS | Progress: (672/672) | 1441.11 s Done.
    [Task 11/12]  Current/Best:   13.00/  23.09 GFLOPS | Progress: (576/576) | 1239.69 s Done.
    [Task 12/12]  Current/Best:    3.45/  23.61 GFLOPS | Progress: (360/360) | 881.20 s Done.
* inception
    Tune_relay_x86_inception_tf 
    core-avx2 
    min_repeat_ms=1000 0.2s　
    min_repeat_ms=2000 0.14s
    可见min_repeat_ms影响

##服务器部署
见cpu_cpp_deploy_mobilenetv2

##andriod部署
见tvm_android_deploy_mobilenetv2



联系我![](./tvm_android_deploy_mobilenetv2/wechat.JPG)