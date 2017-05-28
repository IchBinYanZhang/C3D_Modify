#mkdir -p output/c3d/v_ApplyEyeMakeup_g01_c01
#mkdir -p output/c3d/v_BaseballPitch_g01_c01
GLOG_logtosterr=1 ../../build/tools/extract_image_features.bin prototxt/MyC3DFeatureExtraction_video.prototxt conv3d_deepnetA_sport1m_iter_1900000 0 50 25 prototxt/MPIICooking_s08-d02-cam-002.avi_output_list_video.txt fc7-1 fc6-1
