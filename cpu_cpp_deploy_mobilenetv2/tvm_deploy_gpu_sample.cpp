
#include <string>
#include <fstream>

#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/packed_func.h>


int main() {
    //setup tvm
    const std::string json_file("deploy.json");
    const std::string param_file("deploy.params");
    tvm::runtime::Module mod_syslib = tvm::runtime::Module::LoadFromFile("deploy.dll");
    
    std::ifstream json_in(json_file.c_str(), std::ios::in);
    std::string json_data((std::istreambuf_iterator<char>(json_in)), std::istreambuf_iterator<char>());
    json_in.close();
    
    std::ifstream params_in(param_file.c_str(), std::ios::binary);
    std::string params_data((std::istreambuf_iterator<char>(params_in)), std::istreambuf_iterator<char>());
    params_in.close();
    
    TVMByteArray params_arr;
    params_arr.data = params_data.data();
    params_arr.size = params_data.length();
    
    constexpr int dtype_code = kDLFloat;
    constexpr int dtype_bits = 32;
    constexpr int dtype_lanes = 1;
    constexpr int device_type = kDLGPU;
    constexpr int device_id = 0;
    
    tvm::runtime::Module mod = (*tvm::runtime::Registry::Get("tvm.graph_runtime.create"))(json_data, mod_syslib, device_type, device_id);
    
    tvm::runtime::PackedFunc load_params = mod.GetFunction("load_params");
    load_params(params_arr);
    
    DLTensor* x = nullptr;
    DLTensor* y = nullptr;
    const int in_ndim = 4;
    const int out_ndim = in_ndim;
    const int num_slice = 1;
    const int num_class = 4;
    const int shrink_size[] = { 256, 256 };
    const int64_t in_shape[] = { num_slice, 1, shrink_size[0], shrink_size[1] };
    const int64_t out_shape[] = { num_slice, num_class, shrink_size[0], shrink_size[1] };
    TVMArrayAlloc(in_shape, in_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id, &x);
    TVMArrayAlloc(out_shape, out_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id, &y);

    const int n_samples = 100;
    const size_t in_size = in_shape[0] * in_shape[1] * in_shape[2] * in_shape[3];
    const size_t out_size = out_shape[0] * out_shape[1] * out_shape[2] * out_shape[3];
    std::vector<float> tvm_input(n_samples * in_size, 0);
    std::vector<float> tvm_output(n_samples * out_size, 0);
    
    tvm::runtime::PackedFunc set_input = mod.GetFunction("set_input");
    tvm::runtime::PackedFunc run = mod.GetFunction("run");
    tvm::runtime::PackedFunc get_output = mod.GetFunction("get_output");

    const std::string input_name("data");

    for (int i = 0; i < n_samples; ++i) {
    	TVMArrayCopyFromBytes(x, &tvm_input[i * in_size], in_size * sizeof(float));
    	set_input(input_name.c_str(), x);
    	run();
        get_output(0, y);
    	TVMArrayCopyToBytes(y, &tvm_output[i * out_size],  out_size * sizeof(float));
    }
        
    TVMArrayFree(x);
    TVMArrayFree(y);

    return 0;
}

