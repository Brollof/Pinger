#pragma once

#include <vector>

typedef std::vector<float> stat_t;

class Stats
{
public:
  Stats(int samples);
  virtual ~Stats();

  void Update(float value);
  float GetAverage();
#ifdef _DEBUG
  void Print();
#endif

private:
  int m_samples;
  int m_idx;
  stat_t* m_data;
};