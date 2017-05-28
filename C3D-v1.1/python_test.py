import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import sys

sys.path.append("/home/yzhang/workspace/ActivitySegmentation/C3D/C3D-v1.1/python")




import caffe

caffe.set_device(0)
caffe.set_mode_gpu()
