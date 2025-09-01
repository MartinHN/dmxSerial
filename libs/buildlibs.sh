scriptFile="$(readlink -f "$0")"
ftdiDir="$(dirname $scriptFile)/libftdi"
installDir=$ftdiDir/install
mkdir -p $installDir
echo $installDir

pushd $ftdiDir/build
cmake  -DCMAKE_INSTALL_PREFIX="$installDir" ../
make
make install
popd 
