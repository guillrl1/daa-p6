#ifndef INCLUDE_SOLVER_H_
#define INCLUDE_SOLVER_H_

#include <vector>
#include "instance.h"

namespace diversity {

// Estructura para almacenar la solución final.
struct Solution {
  std::vector<int> selected_ids;
  double total_diversity = 0.0;
};

// Interfaz base para los algoritmos de resolución.
class Solver {
 public:
  Solver(const Instance& instance, int m) 
      : instance_(instance), m_(m) {}
  virtual ~Solver() = default;

  // Método puro que deben implementar las subclases.
  virtual Solution Solve() = 0;

 protected:
  const Instance& instance_;
  int m_; // Tamaño del subconjunto a buscar

  // Método de utilidad para calcular la diversidad total de un conjunto.
  double CalculateDiversity(const std::vector<int>& selected_indices) const;
};

}  // namespace diversity

#endif  // INCLUDE_SOLVER_H_