#ifndef INCLUDE_POINT_H_
#define INCLUDE_POINT_H_

#include <vector>

namespace diversity {

// Clase que representa un elemento multidimensional en el espacio.
class Point {
 public:
  Point(int id, const std::vector<double>& coordinates);

  int GetId() const { return id_; }
  const std::vector<double>& GetCoordinates() const { return coordinates_; }
  
  // Calcula la distancia euclídea hacia otro punto.
  double DistanceTo(const Point& other) const;

 private:
  int id_;
  std::vector<double> coordinates_;
};

}  // namespace diversity

#endif  // INCLUDE_POINT_H_