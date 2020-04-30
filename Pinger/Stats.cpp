#include "Stats.h"
#include <numeric>

#ifdef _DEBUG
#include <iostream>
#endif

Stats::Stats(int samples)
{
  m_samples = samples;
  m_data = new stat_t(m_samples, 0);
}

Stats::~Stats()
{
  delete m_data;
}

void Stats::Update(float value)
{
  if (m_idx == m_samples - 1)
  {
    m_isFull = true;
  }
  m_data->at(m_idx) = value;
  m_idx = (m_idx + 1) % m_samples;
}

float Stats::GetAverage()
{
  float sum = 0;
  int size = m_isFull ? m_data->size() : m_idx;

  for (int i = 0; i < size; i++)
  {
    sum += m_data->at(i);
  }

  return sum / size;
}

#ifdef _DEBUG
void Stats::Print()
{
  for (unsigned int i = 0; i < m_data->size(); i++)
  {
    std::cout << "(" << i << ": " << m_data->at(i) << ")";
  }
  std::cout << std::endl;
}
#endif