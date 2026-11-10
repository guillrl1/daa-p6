#ifndef INCLUDE_BB_SOLVER_H_
#define INCLUDE_BB_SOLVER_H_

#include "solver.h"
#include <vector>
#include <cstdint>

namespace diversity {

// Estrategias de ramificación requeridas por la práctica
enum class BranchingStrategy {
  kExpandMinUpper, // Expande nodo con cota superior más pequeña
  kExpandMaxUpper  // Expande nodo con cota superior más grande
};

class BBSolver : public Solver {
 public:
  BBSolver(const Instance& instance, int m, const Solution& initial_sol, BranchingStrategy strategy);
  
  // Resuelve el problema y retorna la solución óptima
  Solution Solve() override;

  // Devuelve el número de nodos generados en la última ejecución
  long long GetNodesGenerated() const { return nodes_generated_; }

 private:
  Solution initial_sol_;
  double lower_bound_;
  BranchingStrategy strategy_;
  long long nodes_generated_;

  // --- Optimizaciones de Memoria y Velocidad ---
  
  // Matriz NxN para consultas O(1) de la distancia euclídea
  std::vector<std::vector<double>> dist_matrix_;
  
  // max_inner_edges_[L][K]: Suma máxima de las K(K-1)/2 distancias 
  // entre los elementos desde el índice L hasta N-1
  std::vector<std::vector<double>> max_inner_edges_;

  // Variables globales para el recorrido
  uint64_t best_mask_;
  double best_z_;

  // --- Métodos Privados ---

  // Precalcula las matrices de distancias y cotas para acelerar la exploración
  void Precompute();

  // Calcula la cota superior de forma eficiente usando heurística de relajación
  double CalculateUpperBound(uint64_t selected_mask, int count, int level) const;

  // Extrae los índices de una máscara de bits a un std::vector
  std::vector<int> MaskToVector(uint64_t mask) const;

  // Explora el árbol de soluciones (Método parametrizado por la cola de prioridad)
  template <typename Compare>
  void ExploreTree(Compare comp);
};

}  // namespace diversity

#endif  // INCLUDE_BB_SOLVER_H_