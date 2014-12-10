Bing-Objectness-linux
================================================================================
linux/mac/windows easy to build.
================================================================================
Change Directory to the main project path

1 dependences
	you need to install all the packages as bellow:
	1) OpenCV >=2.4.8
	2) Cmake  >=2.8
	3) make
	4) build-enssential(g++>=4.7; gcc>=4.7)

2 build & install

1)Install into default system path
mkdir build
cd build
Cmake ../
make

sudo make install

2)Install into self-defined path
mkdir build
cd build
Cmake -DCMAKE_INSTALL_PRIFIX=path-of-self-defined
make
make install

3) Tree structut of install path
path-of-self-defined/
├── bin
│   └── BING
├── include
│   └── bing
│       ├── LibLinear
│       │   ├── blas
│       │   │   ├── blas.h
│       │   │   └── blasp.h
│       │   ├── linear.h
│       │   └── tron.h
│       └── LibObjectness
│           ├── CmFile.h
│           ├── CmShow.h
│           ├── CmTimer.h
│           ├── DataSetVOC.h
│           ├── FilterTIG.h
│           ├── ImgContrastBB.h
│           ├── kyheader.h
│           ├── Objectness.h
│           └── ValStructVec.h
├── lib
│   ├── libblas.a
│   ├── liblinear.a
│   └── libobjectness.a
└── share
    └── bing
        ├── how to build
        ├── LIBLINEAR_README.1.93.txt
        ├── LICENSE
        ├── ReadMeFromKely.txt
        └── ReadMeFromXuXiduo

4) There are many memory leak found which caused by Opencv.

5) advice
  if you want to train the mode with big data, please build it under 64bit Operting system.

  Dataset URL: http://mmcheng.net/bingreadme/
  FAQs URL:http://mmcheng.net/bing/

6) Email
xuxiduo@zju.edu.cn
