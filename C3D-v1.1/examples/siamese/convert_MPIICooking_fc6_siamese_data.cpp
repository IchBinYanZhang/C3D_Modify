//
// This script converts the MPII_cooking dataset to the leveldb format used
// by caffe to train siamese network.
// Usage:
//    convert_MPIICooking_fc6_data input_folders input_label_file output_db_file
// The MNIST dataset could be downloaded at
//    http://yann.lecun.com/exdb/mnist/
#include <fstream>  // NOLINT(readability/streams)
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>

#include "glog/logging.h"
#include "google/protobuf/text_format.h"
#include "stdint.h"

#include "caffe/proto/caffe.pb.h"
#include "caffe/util/format.hpp"
#include "caffe/util/math_functions.hpp"

#ifdef USE_LEVELDB
#include "leveldb/db.h"

uint32_t swap_endian(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}


void read_image(std::ifstream* image_file, std::ifstream* label_file,
        uint32_t index, uint32_t rows, uint32_t cols,
        char* pixels, char* label) {
  image_file->seekg(index * rows * cols + 16);
  image_file->read(pixels, rows * cols);
  label_file->seekg(index + 8);
  label_file->read(label, 1);
}



void read_feature(std::ifstream& feature_file,float* pixels) {
  feature_file.seekg(5*sizeof(int));
  feature_file.read((char*)pixels, 4096*sizeof(float));
}



void convert_dataset(const char* feature_folder_path, const char* feature_type,
        const char* db_filename) {
  // Open files
  //std::ifstream image_file(image_filename, std::ios::in | std::ios::binary);
  //std::ifstream label_file(label_filename, std::ios::in | std::ios::binary);
  //CHECK(image_file) << "Unable to open file " << image_filename;
  //CHECK(label_file) << "Unable to open file " << label_filename;

  // NOTICE: all the paths here are the absolute path!!!
  // obtain the video list in the datafeature set 
  std::vector<std::string> video_list;
  std::vector<std::string>::iterator ii;
  std::stringstream command;

  command << "ls -d "<< feature_folder_path <<"/*.avi > temp.txt";
  std::system(command.str().c_str());
  std::ifstream in("temp.txt");
  std::copy(std::istream_iterator<std::string> (in), 
            std::istream_iterator<std::string>(), 
            std::back_inserter(video_list));
  in.close();
  std::system("rm -f temp.txt");



  // Open leveldb
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = true;
  leveldb::Status status = leveldb::DB::Open(
      options, db_filename, &db);
  CHECK(status.ok()) << "Failed to open leveldb " << db_filename
      << ". Is it already existing?";

  // retrieve the feature files, specify the labels and write to the db file
  
  std::string value;

  caffe::Datum datum;
  datum.set_channels(2);  // one channel for each image in the pair
  datum.set_height(1);
  datum.set_width(4096);
  std::ifstream feature_file;
  int itemid = 0;
  for (ii = video_list.begin(); ii != video_list.end(); ii++){
    // retrieve the feature files in each video folder

    command.str("");
    command << "ls " << *ii << "/*." <<feature_type << " > temp.txt"; 
    std::system(command.str().c_str());
    std::vector<std::string> frame_feature_list;
    std::ifstream in("temp.txt");
    std::copy(std::istream_iterator<std::string> (in), 
              std::istream_iterator<std::string>(), 
              std::back_inserter(frame_feature_list));
    in.close();
    std::system("rm -f temp.txt");    
    // retrieve the features from each file
    for (int jj=0;jj<frame_feature_list.size()-2;jj+=2){
      float* pair_1 = new float[2*4096]; // fc features temporal neighbours
      float* pair_2 = new float[2*4096]; // fc features without neighbourhood

      // [debug] measure the escaped time
      //clock_t begin_time = std::clock();



      std::stringstream vv; // filename 
      vv << frame_feature_list[jj];
      std::cout << "-Converting: " << vv.str() <<std::endl;
      feature_file.open(vv.str().c_str(), std::ifstream::binary);
      read_feature(feature_file, pair_1);
      read_feature(feature_file, pair_2);
      vv.str("");feature_file.close();

      //std::cout << float( std::clock() - begin_time ) /CLOCKS_PER_SEC << std::endl;


      vv << frame_feature_list[jj+1];
      feature_file.open(vv.str().c_str(), std::ifstream::binary);
      read_feature(feature_file, pair_1+4096);
      vv.str("");feature_file.close();

      //std::cout << float( std::clock() - begin_time ) /CLOCKS_PER_SEC<< std::endl;


      
      vv << frame_feature_list[jj+2];
      feature_file.open(vv.str().c_str(), std::ifstream::binary);
      read_feature(feature_file, pair_2+4096);
      vv.str("");feature_file.close();

      //std::cout << float( std::clock() - begin_time ) /CLOCKS_PER_SEC<< std::endl;
      
      std::cout << "pair+:"<< pair_1[0] <<" " <<pair_1[4095]<<" "<< pair_1[4096]<<" "<< pair_1[2*4096-1]<<std::endl;
      std::cout << "pair-:"<< pair_2[0] <<" " <<pair_2[4095]<<" "<< pair_2[4096]<<" "<< pair_2[2*4096-1]<<std::endl;


      // set the data into Datum
      google::protobuf::RepeatedField<float>* datumFloatData1 = datum.mutable_float_data();
      for (int k = 0; k < 4096; k++){
        datumFloatData1->Add(pair_1[k]);
      }
      datum.set_label(1);
      datum.SerializeToString(&value);
      
      std::string key_str = caffe::format_int(itemid, 8);
      db->Put(leveldb::WriteOptions(), key_str, value);
      datumFloatData1 -> Clear();
      itemid++;

      google::protobuf::RepeatedField<float>* datumFloatData2 = datum.mutable_float_data();
      for (int k = 0; k < 4096; k++){
        datumFloatData2->Add(pair_2[k]);
      }
      datum.set_label(0);
      datum.SerializeToString(&value);
      key_str = caffe::format_int(itemid, 8);
      db->Put(leveldb::WriteOptions(), key_str, value);
      datumFloatData2 -> Clear();
      itemid++;

      // std::cout << float( std::clock() - begin_time ) /CLOCKS_PER_SEC<< std::endl;


      delete [] pair_1;
      delete [] pair_2;      

    }
  }
  delete db;
}



int main(int argc, char** argv) {
  if (argc != 4) {
    printf("This script converts the C3D fc features of MPIICooking dataset to the leveldb format used\n"
           "by caffe to train a siamese network.\n"
           "Usage:\n"
           "    convert_MPIICooking_C3DFC_data feature_path feature_type "
           "output_db_file\n");
  } else {
    google::InitGoogleLogging(argv[0]);
    convert_dataset(argv[1], argv[2], argv[3]);
  }
  return 0;
}
#else
int main(int argc, char** argv) {
  LOG(FATAL) << "This example requires LevelDB; compile with USE_LEVELDB.";
}
#endif  // USE_LEVELDB
