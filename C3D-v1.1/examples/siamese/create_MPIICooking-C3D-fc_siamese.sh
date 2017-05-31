#!/usr/bin/env sh
# This script converts the mnist data into leveldb format.
set -e
CAFFEPATH=~/workspace/ActivitySegmentation/C3D_Modify/C3D-v1.1
EXAMPLES=$CAFFEPATH/build/examples/siamese
#DATA=./data/mnist

echo "Creating leveldb..."

rm -rf MPIICooking_train_leveldb
rm -rf $CAFFEPATH/examples/siamese/mnist_siamese_test_leveldb

$EXAMPLES/convert_MPIICooking_fc6_siamese_data.bin \
    ~/workspace/ActivitySegmentation/Data_MPIICooking \
    fc6-1 \
    $CAFFEPATH/examples/siamese/MPIICooking_train_leveldb
#$EXAMPLES/convert_mnist_siamese_data.bin \
#    $DATA/t10k-images-idx3-ubyte \
#    $DATA/t10k-labels-idx1-ubyte \
#    ./examples/siamese/mnist_siamese_test_leveldb

echo "Done."
