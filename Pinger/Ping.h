#pragma once

#include <string>

typedef struct
{
  bool status;
  int latency;
} PingResult_t;

PingResult_t ping(std::string target);