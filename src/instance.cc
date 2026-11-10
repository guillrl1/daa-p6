#include "../include/instance.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace diversity {

Instance::Instance(const std::string& filename) {
  LoadFromFile(filename);
}

void Instance::LoadFromFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("No se pudo abrir el archivo: " + filename);
  }

  file >> n_ >> k_;
  std::string line;
  std::getline(file, line); // Consumir salto de línea

  int id = 0;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    
    // Reemplazar comas por puntos para std::stod (formato español a inglés)
    std::replace(line.begin(), line.end(), ',', '.');
    
    std::stringstream ss(line);
    std::vector<double> coords;
    double val;
    while (ss >> val) {
      coords.push_back(val);
    }
    
    if (coords.size() == k_) {
      elements_.emplace_back(id++, coords);
    }
  }
}

}  // namespace diversity