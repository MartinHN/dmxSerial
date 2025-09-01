./libs/buildlibs.sh

BUILD_TYPE="${1:-Release}"
echo ">>>>>>>>>"$BUILD_TYPE
mkdir -p build

pushd build

cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build .

popd
