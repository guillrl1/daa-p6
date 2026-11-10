#include "../include/point.h"
#include <cmath>

namespace diversity {

Point::Point(int id, const std::vector<double>& coordinates)
    : id_(id), coordinates_(coordinates) {}

double Point::DistanceTo(const Point& other) const {
  double sum = 0.0;
  for (size_t i = 0; i < coordinates_.size(); ++i) {
    double diff = coordinates_[i] - other.coordinates_[i];
    sum += diff * diff;
  }
  return std::sqrt(sum);
}

}  // namespace diversity