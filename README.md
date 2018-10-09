# pick-robot

This project contains the software application that runs on the pick robot.

### How do I get set up? ###

* Checkout the repo

```
git clone git@bitbucket.org:freshrealm/pick-robot.git

```

* Load C/C++ IDE of your choice. Eclipse is suggested.
* Download the following packages from Eclipse Help -> Install New Software option:
	* Programming Languages>C/C++ Development Tools
	* Mobile and Device Development> GCC C/C++ Cross Compiler Support
* Download the Raspberry Pi tool chain, or build it yourself (should be named armv8-rpi3-linux-gnueabihf)
	* For Mac: https://s3.amazonaws.com/jaredwolff/xtools-2016-09-01.dmg  should work
	* Install the tool chain somewhere in your system path
* Import the project, set up the project to use the tool chain as your cross compiler (link to its path, prefix should be armv8-rpi3-linux-gnueabihf-)


### How to setup up Doxygen documentation generator ###

_Compiling from source on UNIX_

To download Doxygen from source distribution, you need at least the following to build the executable:

* `flex`, `bison`, `libiconv`, and `strip`. 
* In order to generate the `Makefile` for your platform you need [cmake](https://cmake.org) version 2.8.12 or later.

To download the GUI interface Doxygenwizard, you will also need to install:

* QT Software's GUI toolkit [Qt](https://www.qt.io/developers/) version 4.3 or higher (current Qt 5.x is not yet supported). 

__Note:__ We recommend using `brew` to install these applications: 

* `brew install flex`
* `brew install bison`
* `brew install libiconv`
* `brew install strip`

Since Qt5 is incompatable, the brew installation is a little different:

```
brew tap cartr/qt4
brew tap-pin cartr/qt4
brew install qt@4
```



Latex is also need to compile, the easiest is to use brew to install.

```
brew cask install mactex 

```
or

```
brew cask install basictex
```

Mactex is a much larger package (but is guaranteed to work).
Basictex is about 90MBs, but has not been tested with Doxygen.

Checkout the repo:

```
git clone https://github.com/doxygen/doxygen.git
cd dosxygen 
```

After that:

```
mkdir build
cd build
cmake -G "Unix Makefiles" ..

```

To enable GUI front end (Recommended):

``` 
cmake -Dbuild_wizard=YES .. 

```

Compile the program by running make:

```
make 

```
To install Doxygen:

```
make install
```

_NOTE: a more comprehensive list of instruction can also be found at the_ [Doxygen](https://www.stack.nl/~dimitri/doxygen/manual/install.html#install_src_unix) _website._

### How to run Doxygen? ###

Doxygen wizard can now be started from your terminal window. To open type:

```
doxywizard
```
it a terminal window, which will open a GUI interface.

We now need to load in the `Doxyfile` from the Pick-Robot repo:
* File->Open
* Navigate to your repo installation and select the `Doxyfile`
* Click Open

This should load all the required setting into the Doxygen interface for you. All that's left to do
is navigate to the run tab and click `Run Doxygen`. After the completed run, select `Show HTML Output` to display 
documentation.