# SMASH

This is the repository for the development of a new transport
approach for the dynamical description of heavy ion reactions:
SMASH (Simulating Many Accelerated Strongly-interacting Hadrons).

See this file on how to compile it and CONTRIBUTING for development hints.

 *    If Pythia cite
 *    T. Sjöstrand, S. Mrenna and P. Skands, JHEP05 (2006) 026,
 *                          Comput. Phys. Comm. 178 (2008) 852.

 <!-- /*!\Userguide -->

 \page install Installation

 Quick instructions how to build smash.

 Prerequisites
 -------------

 SMASH is known to compile and work with one of these compilers (which have the
 required C++11 features):
 - gcc >= 4.8
 - clang >= 3.2

 It requires the following tools & libraries:
 - cmake >= 2.8.11
 - the GNU Scientific Library >= 1.15
 - the Eigen3 library for linear algebra (see http://eigen.tuxfamily.org)
 - boost filesystem >= 1.49

 Further tools that can be helpful (mainly for development):
 - clang-format = 6.0
 - ROOT >= 5.34
 - doxygen >= 1.8.4
 - valgrind
 - cpplint
 - cppcheck
 - codespell

 Support for ROOT output is only enabled if a suitable version of ROOT is found
 on the system.


 Building SMASH
 --------------

 Build SMASH in a separate directory:

     mkdir build
     cd build
     cmake ..
     make

 Run it with specific settings:

     vi config.yaml
     ./smash


 Run SMASH with example input files
 ------------------------------------

 SMASH ships with example configuration files for the collider, box,
 sphere and list modus. By default, i.e. by running ./smash, the simulation is
 set up by means of a collider configuration file, called "config.yaml" and
 the default particles and decaymodes files, "particles.txt" and
 "decaymodes.txt". They are located in /input.
 Additionally, example configuration files for the box, sphere and list modus can
 be found in the respective directories /input/{box,sphere,list}. In case
 of a box simulation, the default particles and decaymodes files need to be
 modified to allow for equilibration. These are also stored in
 /input/box. For the list modus, an input list file to be read in is
 required. This file, "example_list0", is located in /input/list.

 To run SMASH with a manually specified configuration file, use the "-i" command.
 For example, for the sphere or list example file:

     ./smash -i ../input/sphere/config.yaml

     ./smash -i ../input/list/config.yaml


 To further use non-default particles and decaymodes files, the "-p"
 and "-d" options are necessary. For the default box, this means:

     ./smash -i ../input/box/config.yaml -p ../input/box/particles.txt
     -d ../input/box/decaymodes.txt

 All command line options can be viewed with

     ./smash -h


 Size of the code
 ----------------

 Please note that after compilation the 'smash' directory (including 'build')
 has a size of about 4GB. If disk space is restricted, one suggestion is to
 just run

     make smash

 which will only compile the main code. As a default the unit tests are also
 compiled which leads to a large portion of the disk space consumption. It is
 still recommended to run the unit tests at least once, when compiling in
 a new environment to ensure that everything works as expected.


 Changing the compiler
 ---------------------

 In order to use a particular compiler, you can set the following environment
 variables:

     export CC=gcc
     export CXX=g++

 Alternatively the compiler can also be specified to cmake like this:

     cmake .. -DCMAKE_CXX_COMPILER=g++


 Using the offical clang version on macOS
 ----------------------------------------

 The clang compiler version shipped with XCode or the Command Line Tools from
 Apple in the past (before version 8.1) did not always support all C++ features
 that were used. If problems with compiling on macOS arise, try the official
 clang version. This can be done for example with Homebrew (http://brew.sh):

     brew install llvm

 After that you have to instruct cmake that it should use the newly installed
 clang compiler and also tell the linker where the libraries are by altering some
 cmake flags. Enter the cmake command like below:

     cmake .. -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++
     -DCMAKE_EXE_LINKER_FLAGS="-L/usr/local/opt/llvm/lib -lc++abi"
     -DCMAKE_CXX_FLAGS=-I/usr/local/opt/llvm/include

 Note: FPE environment only works with gcc, so e.g. you won't get backtraces from
 floating point traps with clang in general.


 Disabling ROOT support
 ---------------------------

 Producing ROOT output requires ROOT installed (see http://root.cern.ch).
 If ROOT is found, the support for ROOT output is automatically enabled.
 In order to disable it, one can do the follwoing:

     cmake -DUSE_ROOT=OFF <source_dir>
     make

 Building the Documentation
 --------------------------

 Build the code documentation:

     make doc
     firefox doc/html/index.html

 Build the user documentation:

     make user
     firefox doc/user/index.html


 Running Tests
 -------------

 Run the various unit tests:

     make test

 Another way to do this is:

     ctest


 This has the advantage that it can also be used for running tests in parallel on
 a multicore machine, e.g. via

     ctest -j4

 (on a quad-core machine).

 If a test crashes, there might be some leftover in the "test_output" folder,
 causing the test to always fail when run again. To fix this problem, just remove
 the folder.


 Installing binaries as a user
 -----------------------------

 If you don't have administrator privileges on the machine you are using, you can
 still install software locally. Just copy the binary (in this example
 "clang-format") to a local folder and update your path:

     mkdir ~/bin
     cp ./clang-format ~/bin
     echo 'export PATH=$PATH:~/bin' >> ~/.bashrc
     source ~/.bashrc

 After this, you can just copy executable files to `~/bin` to install them. This
 also works for other exectuables like cpplint. You might have to set them to be
 executable with "chmod u+x ~/bin/my-binary".


 Installing clang-format
 -----------------------

 clang-format is a part of the clang compiler. You can download the most recent
 binaries here:

     http://releases.llvm.org/download.html

 Make sure to pick a pre-built binary for your system. For example, for Ubuntu
 you could run:

     $ lsb_release -a
     No LSB modules are available.
     Distributor ID: Ubuntu
     Description:    Ubuntu 16.04.2 LTS
     Release:        16.04
     Codename:       xenial

 This tells you to download "Clang for x86_64 Ubuntu 16.04". (You might have to
 look for an older version to get pre-built binaries.)

 It is sufficient to unpack the archive with `tar xf` and to copy only the
 binary you need (`clang-format` in the `bin` folder of the archive), see
 "Installing binaries as a user" above.


 Installing cpplint
 ------------------

 We use cpplint to enforce some of our style guide lines as part of our tests.
 You can install it like this:

     pip install --user cpplint

 You might have to add `~/.local/bin` to your `$PATH`, see "Installing binaries
 as a user".


 Installing cppcheck
 -------------------

 You can use cppcheck to find some problems in the code, it has quite a few
 false positives though. Download and compile the latest version:

     git clone git://github.com/danmar/cppcheck.git
     cd cppcheck
     make

 You can then copy it to your local binary folder, see "Installing binaries
 as a user".


 Installing codespell
 --------------------

 If you want to check the spelling in comments, try codespell. You can install
 it like this:

     pip install --user codespell

 It is the same as installing cpplint.


 Including Eigen header files from custom location
 -------------------------------------------------

 Let's assume Eigen headers will be unpacked in $HOME.

 - Download latest package [latest-eigen].tar.gz from http://eigen.tuxfamily.org

 - unpack: tar -xf [latest-eigen].tar.gz -C $HOME

 - in smash/build/, create build files with

   cmake -DCMAKE_INSTALL_PREFIX=$HOME/[latest-eigen]/ ..


 Using a custom GSL build
 ------------------------

 Run the following:

     wget ftp://ftp.gnu.org/gnu/gsl/gsl-latest.tar.gz
     tar -zxvf gsl-latest.tar.gz

 This creates a folder named "gsl-[version_number]" called $GSL here.

     cd $GSL
     ./configure --prefix $GSL
     make -jN
     make install

 Here N is the number of cores to use in the "make" command. When compiling
 SMASH, run cmake with

     cmake -DGSL_ROOT_DIR=$GSL ..

 Note: In case of problems, make sure to start with a clean build folder.
 */
