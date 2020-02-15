import tvm,os
from tvm.contrib import util, ndk, graph_runtime as runtime
import os
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"
import os
import numpy as np
from PIL import Image
import keras
from keras.applications.mobilenet_v2 import MobileNetV2 as MobileNet
# from tutorials.autotvm.MobileNet import MobileNetV2 as MobileNet
from keras.applications import resnet50
import tvm
import tvm.relay as relay
from tvm import rpc
from tvm.contrib import util, ndk, graph_runtime as runtime
from tvm.contrib.download import download_testdata
import ssl
ssl._create_default_https_context = ssl._create_unverified_context
import nnvm

print("keras 2.2.5")

temp = util.tempdir()


loaded_json = open("/home/naruto/software/tvm/tutorials/frontend/deploy_graph.json").read()
loaded_lib = tvm.module.load("/home/naruto/software/tvm/tutorials/frontend/deploy_lib.so")
loaded_params = bytearray(open("/home/naruto/software/tvm/tutorials/frontend/deploy_param.params", "rb").read())
num_threads = 2
os.environ["TVM_NUM_THREADS"] = str(num_threads)#todo physical cores
# input_data = tvm.nd.array(np.random.uniform(size=data_shape).astype("float32"))
input_name = 'input_1'
img_path = "/home/naruto/tvm-tutorial/cat.png"#download_testdata(img_url, img_name, module='data')
image = Image.open(img_path).resize((224, 224))
dtype = 'float32'

def transform_image(image):
    image = np.array(image) - np.array([123., 117., 104.])

    # image = np.array(image) - np.array([123., 117., 104.])
    image /= np.array([58.395, 57.12, 57.375])
    image = image.transpose((2, 0, 1))
    image.astype("float32").tofile("tiger_3.bin")

    image = image[np.newaxis, :]
    print("shape:",image.shape)
    return image

x_np = transform_image(image)
print(np.max(x_np))
print(np.argmax(x_np))
x_np.astype("float32").tofile("tiger_new.bin")

fcreate = tvm.get_global_func("tvm.graph_runtime.create")
ctx = tvm.cpu(0)
gmodule = fcreate(loaded_json, loaded_lib, ctx.device_type, ctx.device_id)
set_input, get_output, run = gmodule["set_input"], gmodule["get_output"], gmodule["run"]
set_input(input_name, tvm.nd.array(x_np.astype(dtype)))
# set_input("x", tvm.nd.array(x_np))

gmodule["load_params"](loaded_params)
run()
out = tvm.nd.empty((1,1000))
get_output(0, out)
print("predict result:",str(np.argmax(out.asnumpy())),np.max(out.asnumpy()))
# get top1 result
synset_path = "/home/naruto/.tvm_test_data/data/imagenet1000_clsid_to_human.txt"#download_testdata(synset_url, synset_name, module='data')
with open(synset_path) as f:
    synset = eval(f.read())
top1 = np.argmax(out.asnumpy())
print('TVM prediction top-1: {}'.format(synset[top1]))

# print(out.asnumpy())