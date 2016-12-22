# force-cop-estimator

This module estimates the center of contct in finger's frame of reference and the aplied force in Newtons from the response of the tactile sensors of the iCub robot. This modul is used in Task 4-1 to detect contacts. 


## Dependencies
[iCub: YARP, icub-main and icub-contrib](http://wiki.icub.org/wiki/ICub_Software_Installation)

[GURLS](https://github.com/LCSL/GURLS)


## How to compile
Use cmake to compile for your OS distribution. Under Linux you can compile it as follows:

```
mkdir build
cd build
ccmake ..
make install
```

## How to run 

```
force-cop-estimator --from <config-file>
```
### Ports created

/force-cop-estimator/force:o
/force-cop-estimator/cop:o

### Control port (YARP rpc port)

```
/force-cop-estimator/rpc:i
```

commands:

`Help` display available commands

 

