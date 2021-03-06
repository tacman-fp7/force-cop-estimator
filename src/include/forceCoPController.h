// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_forceCoPController
#define YARP_THRIFT_GENERATOR_forceCoPController

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class forceCoPController;


class forceCoPController : public yarp::os::Wire {
public:
  forceCoPController();
  virtual bool train();
  virtual bool quit();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
