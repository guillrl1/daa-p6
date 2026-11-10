#ifndef INCLUDE_GREEDY_SOLVER_H_
#define INCLUDE_GREEDY_SOLVER_H_

#include "solver.h"

namespace diversity {

// Implementación del algoritmo constructivo voraz basado en centros de gravedad.
class GreedySolver : public Solver {
 public:
  GreedySolver(const Instance& instance, int m);
  Solution Solve() override;

 private:
  // Calcula el centro de gravedad de un conjunto de índices.
  std::vector<double> CalculateCenterOfGravity(const std::vector<int>& indices) const;
  
  // Encuentra el elemento más alejado del centro de gravedad dado.
  int FindFurthestElement(const std::vector<int>& candidates, 
                          const std::vector<double>& center) const;
};

}  // namespace diversity

#endif  // INCLUDE_GREEDY_SOLVER_H_