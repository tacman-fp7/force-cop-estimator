#pragma once
#include <iostream>
#include <exception>

class Bad_ModelException: public std::exception{
    virtual const char* what() const throw()
      {
        return "The model file is invalid or cannot be found.";
      }
}bad_model;
