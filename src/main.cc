#include <iostream>
#include <iomanip>
#include "../include/instance.h"
#include "../include/greedy_solver.h"
#include "../include/bb_solver.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Uso: " << argv[0] << " <archivo_datos.txt> <m>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];
  int m = std::stoi(argv[2]);

  try {
    // 1. Cargar instancia
    diversity::Instance instance(filename);
    std::cout << "Instancia cargada: n=" << instance.GetNumElements() 
              << ", K=" << instance.GetDimensions() << "\n\n";

    // 2. Ejecutar Algoritmo Voraz
    diversity::GreedySolver greedy(instance, m);
    diversity::Solution greedy_sol = greedy.Solve();
    
    std::cout << "=== Resultados Constructivo Voraz ===\n";
    std::cout << "Diversidad Z (Cota Inferior): " << std::fixed 
              << std::setprecision(4) << greedy_sol.total_diversity << "\n";
    std::cout << "Nodos seleccionados: ";
    for (int id : greedy_sol.selected_ids) {
      std::cout << id << " ";
    }
    std::cout << "\n\n";

    // 3. Ejecutar B&B con cota inferior del Voraz (Estrategia: Cota Mínima)
    // Expande el nodo con cota superior más pequeña.
    diversity::BBSolver bb_min_upper(instance, m, greedy_sol, 
                                     diversity::BranchingStrategy::kExpandMinUpper);
    
    std::cout << "=== Resultados B&B (Expandir Cota Mínima) ===\n";
    diversity::Solution bb_min_sol = bb_min_upper.Solve();
    std::cout << "Diversidad Z (Óptima): " << std::fixed 
              << std::setprecision(4) << bb_min_sol.total_diversity << "\n";
    std::cout << "Nodos seleccionados: ";
    for (int id : bb_min_sol.selected_ids) {
      std::cout << id << " ";
    }
    std::cout << "\n\n";

    // 4. Ejecutar B&B con cota inferior del Voraz (Estrategia: Cota Máxima)
    // Expande el nodo con cota superior más grande.
    diversity::BBSolver bb_max_upper(instance, m, greedy_sol, 
                                     diversity::BranchingStrategy::kExpandMaxUpper);
    
    std::cout << "=== Resultados B&B (Expandir Cota Máxima) ===\n";
    diversity::Solution bb_max_sol = bb_max_upper.Solve();
    std::cout << "Diversidad Z (Óptima): " << std::fixed 
              << std::setprecision(4) << bb_max_sol.total_diversity << "\n";
    std::cout << "Nodos seleccionados: ";
    for (int id : bb_max_sol.selected_ids) {
      std::cout << id << " ";
    }
    std::cout << "\n\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
