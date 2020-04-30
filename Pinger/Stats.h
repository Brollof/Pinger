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
  int m_samples = 0;
  int m_idx = 0;
  bool m_isFull = false;
  stat_t* m_data = nullptr;
};