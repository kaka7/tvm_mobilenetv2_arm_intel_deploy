#include "tvm/runtime/c_runtime_api.h"
#include <assert.h>
#include <dlfcn.h> 
#include <dlpack/dlpack.h>
#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <streambuf>

#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/operation.h>
#include <tvm/build_module.h>

#include <opencv2/opencv.hpp>
#include <ctime>

int main(void)
{
    std::ifstream graph_file("./deploy_graph.json");
    std::ifstream model_file("./deploy_param.params");

    std::string graph_content(
        (std::istreambuf_iterator<char>(graph_file)),
        std::istreambuf_iterator<char>()
    );
    std::string model_params(
        (std::istreambuf_iterator<char>(model_file)),
        std::istreambuf_iterator<char>()
    );

    tvm::runtime::Module mod_dylib  = tvm::runtime::Module::LoadFromFile("./deploy_lib.so");
    tvm::runtime::Module mod_syslib = (*tvm::runtime::Registry::Get("module._GetSystemLib"))();
    
    // int device_type = kDLCPU;
    // int device_id = 0;

    int device_type = kDLOpenCL;
    int device_id = 0;


    tvm::runtime::Module mod = 
          (*tvm::runtime::Registry::Get("tvm.graph_runtime.create"))
          (graph_content.c_str(), mod_dylib, device_type, device_id);

    int INPUT_SIZE = 300;
    cv::Mat raw_image = cv::imread("./body.jpg");
    int raw_image_height = raw_image.rows;
    int raw_image_width  = raw_image.cols; 
    cv::Mat image;
    cv::resize(raw_image, image, cv::Size(INPUT_SIZE, INPUT_SIZE));
    image.convertTo(image, CV_32FC3);
    image = (image * 2.0f / 255.0f) - 1.0f;

    TVMByteArray params;
    params.data = reinterpret_cast<const char *>(model_params.c_str());
    params.size = model_params.size();
    mod.GetFunction("load_params")(params);

    std::vector<int64_t> input_shape = {1, 3, 300, 300};

    DLTensor input;
    input.ctx = DLContext{kDLOpenCL, 0};
    // input.ctx = DLContext{kDLCPU, 0};
    input.data = image.data;
    input.ndim = 4;
    input.dtype = DLDataType{kDLFloat, 32, 1};
    input.shape = input_shape.data();
    input.strides = nullptr; 
    input.byte_offset = 0;


    //warm up
    for (int i = 0; i < 3; ++i) {
        mod.GetFunction("set_input")("normalized_input_image_tensor", &input);
        mod.GetFunction("run")();
    }

    // cal time
    int N = 10;
    std::clock_t start = std::clock();
    // mod.GetFunction("set_input")("normalized_input_image_tensor", &input);
    for (int i = 0; i < N; ++i) {
        mod.GetFunction("set_input")("normalized_input_image_tensor", &input);
        mod.GetFunction("run")();
    }
    std::clock_t end = std::clock();
    double duration = ( end - start ) * (1.0 / (double) N) / (double) CLOCKS_PER_SEC;

    std::cout<< duration<< std::endl;

    return 0;
}
