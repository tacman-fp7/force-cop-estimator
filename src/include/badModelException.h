#ifndef BAD_MODEL_EXCEPTION_HEADER
#define BAD_MODEL_EXCEPTION_HEADER

#include <iostream>
#include <exception>

class Bad_ModelException: public std::exception{
    virtual const char* what() const throw()
      {
        return "The model file is invalid or cannot be found.";
      }
}bad_model;

#endif
