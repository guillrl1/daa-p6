#include "../include/bb_solver.h"
#include <queue>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>

namespace diversity {

// Estructura ligera del Nodo para el árbol B&B
struct Node {
  int level;            // Profundidad actual (índice del elemento a decidir)
  int count;            // Número de elementos seleccionados hasta ahora
  double current_z;     // Diversidad Z acumulada en los elementos seleccionados
  double upper_bound;   // Cota superior (optimista) de esta rama
  uint64_t selected;    // Máscara de bits de los elementos elegidos
};

BBSolver::BBSolver(const Instance& instance, int m, const Solution& initial_sol, BranchingStrategy strategy)
    : Solver(instance, m), 
      initial_sol_(initial_sol),  // Guardamos la solución completa
      strategy_(strategy),
      nodes_generated_(0),
      best_mask_(0),
      best_z_(initial_sol.total_diversity) {
  Precompute();
}

void BBSolver::Precompute() {
  int n = instance_.GetNumElements();
  const auto& elements = instance_.GetElements();

  // 1. Precomputar Matriz de Distancias en O(n^2)
  dist_matrix_.assign(n, std::vector<double>(n, 0.0));
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      double dist = elements[i].DistanceTo(elements[j]);
      dist_matrix_[i][j] = dist_matrix_[j][i] = dist;
    }
  }

  // 2. Precomputar Máximas Conexiones Internas por Sufijo
  // Útil para la Cota Superior: Suma de los mejores K(K-1)/2 ejes posibles.
  // cuánto aportaría cada elemento candidato si lo uniera a los elementos que ya he seleccionado.
  max_inner_edges_.assign(n + 1, std::vector<double>(m_ + 1, 0.0));
  for (int level = 0; level < n; ++level) {
    std::vector<double> edges;
    for (int i = level; i < n; ++i) {
      for (int j = i + 1; j < n; ++j) {
        edges.push_back(dist_matrix_[i][j]);
      }
    }
    // Ordenar de mayor a menor para selección voraz
    std::sort(edges.rbegin(), edges.rend());
    
    for (int k = 1; k <= m_ && level + k <= n; ++k) {
      int num_edges = k * (k - 1) / 2;
      double sum = 0.0;
      int limit = std::min(static_cast<int>(edges.size()), num_edges);
      for (int e = 0; e < limit; ++e) {
        sum += edges[e];
      }
      max_inner_edges_[level][k] = sum;
    }
  }
}

double BBSolver::CalculateUpperBound(uint64_t selected_mask, int count, int level) const {
  int needed = m_ - count;
  if (needed == 0) return 0.0; // Ya se han seleccionado todos
  
  int n = instance_.GetNumElements();

  // 1. Máximas conexiones posibles entre elementos seleccionados (S) y candidatos (U)
  std::vector<double> connections(n - level, 0.0);
  for (int u = level; u < n; ++u) {
    double sum_to_S = 0.0;
    
    // Iteración sencilla: comprobamos los bits desde 0 hasta level-1
    for (int s = 0; s < level; ++s) {
      // Desplazamos 's' posiciones y aplicamos máscara AND con 1
      if ((selected_mask >> s) & 1ULL) { 
        sum_to_S += dist_matrix_[s][u];
      }
    }
    
    connections[u - level] = sum_to_S; // Guardamos la suma de conexiones S-U para el candidato u
  }
  
  // Tomamos rápidamente los 'needed' mejores candidatos de conexión (S-U)
  if (needed < (n - level)) {
    std::nth_element(connections.begin(), connections.begin() + needed, 
                     connections.end(), std::greater<double>());
  }
  
  double max_S_U = 0.0;
  for (int i = 0; i < needed; ++i) {
    max_S_U += connections[i];
  }

  // 2. Máximas conexiones internas entre los candidatos elegidos (U-U)
  double max_U_U = max_inner_edges_[level][needed];

  return max_S_U + max_U_U;
}

std::vector<int> BBSolver::MaskToVector(uint64_t mask) const {
  std::vector<int> selected_ids;
  int id = 0;
  while (mask > 0) {
    if (mask & 1) {
      selected_ids.push_back(id);
    }
    mask >>= 1;
    ++id;
  }
  return selected_ids;
}

template <typename Compare>
void BBSolver::ExploreTree(Compare comp) {
  std::priority_queue<Node, std::vector<Node>, Compare> pq(comp);
  
  // Nodo Raíz
  Node root{0, 0, 0.0, 0.0, 0};
  root.upper_bound = CalculateUpperBound(root.selected, root.count, root.level);
  pq.push(root);
  nodes_generated_++;

  int n = instance_.GetNumElements();

  while (!pq.empty()) {
    // --- INICIO DE LA MODIFICACIÓN: Ramificar el 2º mejor nodo ---
    Node first_best = pq.top();
    pq.pop();

    Node current;
    if (!pq.empty()) {
      current = pq.top();      // Extraemos el segundo mejor nodo
      pq.pop();
      pq.push(first_best);     // Devolvemos el mejor nodo original a la cola
    } else {
      current = first_best;    // Si solo hay un elemento (ej. el nodo raíz), lo usamos
    }
    // --- FIN DE LA MODIFICACIÓN ---

    // Poda por cota
    if (current.upper_bound <= best_z_) {
      continue;
    }

    // Poda por factibilidad: si no quedan suficientes elementos
    if (current.count + (n - current.level) < m_) {
      continue; 
    }

    // Rama 1: INCLUIR el elemento actual
    if (current.count < m_) {
      Node include_node = current;
      include_node.selected |= (1ULL << include_node.level); // Enciende el bit
      
      // Calcular el incremento de Z
      double added_z = 0.0;
      for (int s = 0; s < current.level; ++s) {
        if ((current.selected >> s) & 1ULL) {
          added_z += dist_matrix_[s][include_node.level];
        }
      }
      
      include_node.current_z += added_z;
      include_node.count++;
      include_node.level++;
      
      // Si llegamos a M elementos, evaluamos solución completa
      if (include_node.count == m_) {
        include_node.upper_bound = include_node.current_z;
        // Si es mejor que la mejor solución encontrada, actualizamos
        if (include_node.current_z > best_z_) {
          best_z_ = include_node.current_z;
          best_mask_ = include_node.selected;
        }
        // No se encola porque ya es una solución completa
      } else {
        include_node.upper_bound = include_node.current_z + 
            CalculateUpperBound(include_node.selected, include_node.count, include_node.level);
      }

      // Si sobrevive a la poda, se encola
      if (include_node.upper_bound > best_z_) {
        pq.push(include_node);
        nodes_generated_++;
      }
    }

    // Rama 2: EXCLUIR el elemento actual
    Node exclude_node = current;
    exclude_node.level++;
    
    // Verificamos de nuevo la factibilidad por si al excluirlo nos quedamos cortos
    if (exclude_node.count + (n - exclude_node.level) >= m_) {
      exclude_node.upper_bound = exclude_node.current_z + 
          CalculateUpperBound(exclude_node.selected, exclude_node.count, exclude_node.level);
      
      if (exclude_node.upper_bound > best_z_) {
        pq.push(exclude_node);
        nodes_generated_++;
      }
    }
  }
}

Solution BBSolver::Solve() {
  nodes_generated_ = 0; // Reseteamos el contador 
  best_z_ = lower_bound_; // Cota inferior del algoritmo constructivo voraz 
  best_mask_ = 0;
  for (int id : initial_sol_.selected_ids) {
    best_mask_ |= (1ULL << id); // Encendemos los bits de la solución inicial
  }

  // Determinamos el comparador para la cola de prioridad según la estrategia pedida 
  if (strategy_ == BranchingStrategy::kExpandMinUpper) {
    auto cmp_min_upper = [](const Node& a, const Node& b) { 
      return a.upper_bound > b.upper_bound; // El menor arriba (Min-Heap)
    };
    ExploreTree(cmp_min_upper);
  } else {
    auto cmp_max_upper = [](const Node& a, const Node& b) { 
      return a.upper_bound < b.upper_bound; // El mayor arriba (Max-Heap)
    };
    ExploreTree(cmp_max_upper);
  }

  // Empaquetar resultados
  Solution best_sol;
  best_sol.total_diversity = best_z_;
  best_sol.selected_ids = MaskToVector(best_mask_);
  
  std::cout << "-> Nodos generados en B&B: " << nodes_generated_ << "\n";
  
  return best_sol;
}

}  // namespace diversity