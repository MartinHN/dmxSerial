scriptFile="$(readlink -f "$0")"
ftdiDir="$(dirname $scriptFile)/libftdi"
installDir=$ftdiDir/install

mkdir -p $installDir
mkdir -p $ftdiDir/build
echo $installDir

pushd $ftdiDir/build
FTDIOPTS="-DSHAREDLIBS=OFF -DBUILD_TESTS=OFF -DDOCUMENTATION=OFF -DEXAMPLES=OFF -DFTDIPP=OFF -DFTDI_EEPROM=OFF -DPYTHON_BINDINGS=OFF -DLINK_PYTHON_LIBRARY=OFF"
cmake  -DCMAKE_INSTALL_PREFIX="$installDir" $FTDIOPTS ../
make
make install
popd 
