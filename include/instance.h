#ifndef INCLUDE_INSTANCE_H_
#define INCLUDE_INSTANCE_H_

#include <string>
#include <vector>
#include "point.h"

namespace diversity {

// Clase encargada de leer e instanciar los datos del problema.
class Instance {
 public:
  explicit Instance(const std::string& filename);

  int GetNumElements() const { return n_; }
  int GetDimensions() const { return k_; }
  const std::vector<Point>& GetElements() const { return elements_; }

 private:
  int n_;
  int k_;
  std::vector<Point> elements_;

  void LoadFromFile(const std::string& filename);
};

}  // namespace diversity

#endif  // INCLUDE_INSTANCE_H_