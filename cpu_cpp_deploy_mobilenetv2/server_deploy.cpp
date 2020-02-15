#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <time.h>
#include <iostream>
#include <string>

#include <unistd.h>
using namespace std;

void Mat_to_CHW(float* data, cv::Mat& frame) {
  assert(data && !frame.empty());
  unsigned int volChl = 224 * 224;

  for (int c = 0; c < 3; ++c) {
    for (unsigned j = 0; j < volChl; ++j)
      data[c * volChl + j] = static_cast<float>(float(frame.data[j * 3 + c]));

    //        data[c*volChl + j] = static_cast<float>(float(frame.data[j * 3 + c]) / 255.0);
  }
}
int out_class = 1000;
const string input_name = "input_1";
int in_ndim = 4;
int64_t in_shape[4] = {1, 3, 224, 224};
int out_ndim = 2;
int64_t out_shape[2] = {
    1,
    out_class,
};
const int warmup_iter = 300;
const int infer_iters = 1000;
const int out_index = 0;

const string DATAFILE = "model/tiger_new.bin";
const string JSONFILE = "model/deploy_graph.json";
const string LIBFILE = "model/deploy_lib.so";
const string PARAMSFILE = "model/deploy_param.params";

int main() {
  // tvm params ,to change is your device is not server cpu!!!
  int dtype_code = kDLFloat;
  int dtype_bits = 32;
  int dtype_lanes = 1;
  int device_type = kDLCPU;
  int device_id = 0;

  const int64 start = cv::getTickCount();
  sleep(1);  //暂停3秒
  const int64 start_forward = cv::getTickCount();
  double duration_before = (start_forward - start) / cv::getTickFrequency();
  std::cout << "test cv2 Time func: " << duration_before << "s" << std::endl;



  // tvm module for compiled functions
  tvm::runtime::Module mod_syslib = tvm::runtime::Module::LoadFromFile(LIBFILE);

  // json graph
  std::ifstream json_in(JSONFILE, std::ios::in);
  std::string json_data((std::istreambuf_iterator<char>(json_in)),
                        std::istreambuf_iterator<char>());
  json_in.close();

  // parameters in binary
  std::ifstream params_in(PARAMSFILE, std::ios::binary);
  std::string params_data((std::istreambuf_iterator<char>(params_in)),
                          std::istreambuf_iterator<char>());
  params_in.close();

  // parameters need to be TVMByteArray type to indicate the binary data
  TVMByteArray params_arr;
  params_arr.data = params_data.c_str();
  params_arr.size = params_data.length();

  // get global function module for graph runtime
  tvm::runtime::Module mod = (*tvm::runtime::Registry::Get("tvm.graph_runtime.create"))(
      json_data, mod_syslib, device_type, device_id);

  DLTensor* x = nullptr;
  TVMArrayAlloc(in_shape, in_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id, &x);

  //
  //    // load image data saved in binary
  std::ifstream data_fin(DATAFILE, std::ios::binary);
//  if (!data_fin) throw std::runtime_error("Could not open: " + data_filename);
  if (!data_fin) throw std::runtime_error("Could not open:file " );

  data_fin.read(static_cast<char*>(x->data),
                in_shape[0] * in_shape[1] * in_shape[2] * in_shape[3] * dtype_bits / 8);

  //    cv::Mat image,frame,input;
  //    image = cv::imread("/home/naruto/tvm-tutorial/cat.png");
  ////    cv::cvtColor(image, frame, cv::COLOR_BGR2RGB);
  //    cv::resize(image, input,  cv::Size(224,224));
  //    float data[224 * 224 * 3];
  //    // 在这个函数中 将OpenCV中的图像数据转化为CHW的形式
  //    Mat_to_CHW(data, input);
  //    memcpy(x->data, &data, 3 * 224 * 224 * sizeof(float));

  //    int out_class=1000;
  //    int INPUT_SIZE = 224;
  //    cv::Mat raw_image = cv::imread("/home/naruto/tvm-tutorial/cat.png");

  // get the function from the module(set input data)
  tvm::runtime::PackedFunc set_input = mod.GetFunction("set_input");

  // test input
  //    auto x_iter = static_cast<float*>(x->data);
  //    auto max_iter = std::max_element(x_iter, x_iter + 3*224*224);
  //    auto max_index = std::distance(x_iter, max_iter);
  //    std::cout << "The maximum x position in output vector is: " << max_index << std::endl;
  //    std::cout << "The maximum  vector is: " << *max_iter << std::endl;

  set_input(input_name, x);

  // get the function from the module(load patameters)
  tvm::runtime::PackedFunc load_params = mod.GetFunction("load_params");
  load_params(params_arr);

  // get the function from the module(run it)
  tvm::runtime::PackedFunc run = mod.GetFunction("run");
  run();

  DLTensor* y = nullptr;
  TVMArrayAlloc(out_shape, out_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id,
                &y);

  // get the function from the module(get output data)
  tvm::runtime::PackedFunc get_output = mod.GetFunction("get_output");
  get_output(out_index, y);

  //    // get the maximum position in output vector
  //    auto y_iter = static_cast<float*>(y->data);
  //    auto max_iter = std::max_element(y_iter, y_iter + 1000);
  //    auto max_index = std::distance(y_iter, max_iter);
  //    std::cout << "The maximum position in output vector is: " << max_index << std::endl;
  //    std::cout << "The maximum  vector is: " << *max_iter << std::endl;

  // warm up
  for (int i = 0; i < warmup_iter; ++i) {
    mod.GetFunction("set_input")(input_name, x);
    mod.GetFunction("run")();
    //        set_input("data", &x);
    //        mod.GetFunction("run")();
    //        run();
  }

  // cal time
  const int64 start1 = cv::getTickCount();
  for (int i = 0; i < infer_iters; ++i) {
    //        mod.GetFunction("set_input")("input_1", x);
    //        mod.GetFunction("run")();
    set_input(input_name, x);
    run();
    get_output(out_index, y);
    //        mod.GetFunction("run")();
  }
  double duration_before1 =( cv::getTickCount() - start1) / cv::getTickFrequency() * (1.0 / (double)infer_iters);
  std::cout << "avg infer Time: " << duration_before1 << "s" << std::endl;

  tvm::runtime::NDArray res = get_output(out_index);

  cv::Mat vector(out_shape[1], out_shape[0], CV_32F);
  memcpy(vector.data, res->data, out_shape[1]* out_shape[0] * dtype_bits / 8);

  // get the maximum position in output vector
  auto y_iter = static_cast<float*>(y->data);
  auto max_iter = std::max_element(y_iter, y_iter + out_class);
  auto max_index = std::distance(y_iter, max_iter);
  std::cout << "The maximum   is: " << *max_iter << "    max index:" << max_index
            << std::endl;
  std::cout << "expected max val: 0.25041685  max index:282" << std::endl;

  //    cv::Mat _l2;
  //     normlize
  //    cv::multiply(vector,vector,_l2);
  //    float l2 =  cv::sqrt(cv::sum(_l2).val[0]);
  //    vector = vector / l2;

  //    // get the maximum position in output vector
  //    auto y_iter = static_cast<float*>(y->data);
  //    auto max_iter = std::max_element(y_iter, y_iter + 3);
  //    std::cout<< y_iter <<std::endl;
  //    std::cout<< (float*)y->data <<std::endl;
  //
  //
  //    auto max_index = std::distance(y_iter, max_iter);
  //    std::cout << "The maximum position in output vector is: " << max_index << std::endl;

  TVMArrayFree(x);
  TVMArrayFree(y);

  return 0;
}