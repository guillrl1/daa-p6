#include "../include/greedy_solver.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace diversity {

GreedySolver::GreedySolver(const Instance& instance, int m) 
    : Solver(instance, m) {}

double Solver::CalculateDiversity(const std::vector<int>& selected_indices) const {
  double z = 0.0;
  const auto& elements = instance_.GetElements();
  for (size_t i = 0; i < selected_indices.size(); ++i) {
    for (size_t j = i + 1; j < selected_indices.size(); ++j) {
      z += elements[selected_indices[i]].DistanceTo(elements[selected_indices[j]]);
    }
  }
  return z;
}

Solution GreedySolver::Solve() {
  std::vector<int> elem(instance_.GetNumElements());
  for (int i = 0; i < instance_.GetNumElements(); ++i) elem[i] = i; // 1: Elem = S; 
  
  std::vector<int> s_sol; // 2: S = \emptyset; 
  
  std::vector<double> sc = CalculateCenterOfGravity(elem); // 3: Obtener sc = centro(Elem); 

  while (s_sol.size() < static_cast<size_t>(m_)) { // 4: repeat / 9: until (|S| = m) 
    int s_star = FindFurthestElement(elem, sc); // 5: Obtener s* alejado de sc 
    s_sol.push_back(s_star); // 6: S = S U {s*} [cite: 49]
    
    elem.erase(std::remove(elem.begin(), elem.end(), s_star), elem.end()); // 7: Elem = Elem - {s*} 
    
    sc = CalculateCenterOfGravity(s_sol); // 8: Obtener sc = centro(S) 
  }

  Solution sol;
  sol.selected_ids = s_sol;
  sol.total_diversity = CalculateDiversity(s_sol);
  return sol;
}

std::vector<double> GreedySolver::CalculateCenterOfGravity(const std::vector<int>& indices) const {
  std::vector<double> center(instance_.GetDimensions(), 0.0);
  if (indices.empty()) return center;

  const auto& elements = instance_.GetElements();
  for (int idx : indices) {
    for (int k = 0; k < instance_.GetDimensions(); ++k) {
      center[k] += elements[idx].GetCoordinates()[k];
    }
  }

  for (int k = 0; k < instance_.GetDimensions(); ++k) {
    center[k] /= indices.size();
  }
  return center;
}

int GreedySolver::FindFurthestElement(const std::vector<int>& candidates, 
                                      const std::vector<double>& center) const {
  int furthest_id = -1;
  double max_dist = -1.0;
  
  const auto& elements = instance_.GetElements();
  for (int idx : candidates) {
    double dist = 0.0;
    for (int k = 0; k < instance_.GetDimensions(); ++k) {
      double diff = elements[idx].GetCoordinates()[k] - center[k];
      dist += diff * diff;
    }
    dist = std::sqrt(dist);

    if (dist > max_dist) {
      max_dist = dist;
      furthest_id = idx;
    }
  }
  return furthest_id;
}

}  // namespace diversity