#pragma once

static const unsigned VERSION_MAJOR = 1;
static const unsigned VERSION_MINOR = 0;
static const unsigned VERSION_REVISION = 0;

static inline void PrintVersion()
{
  std::cout << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION << std::endl;
}

static inline void PrintBuildType()
{
  std::cout << "Build: " << (_DEBUG ? "DEBUG" : "RELEASE") << std::endl;
}