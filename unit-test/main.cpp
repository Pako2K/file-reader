#include <properties_file_reader.h>
#include <csv_file_reader.h>

#include <iostream>

int main() {
  using namespace utils;

  PropertiesFileReader fr("test.prop");

  auto resk = fr.keys();
  for (auto& v : resk) std::cout << v << std::endl;

  auto res = fr.values("key");
  for (auto& v : res) std::cout << v << std::endl;

  std::cout << fr["key"] << std::endl;

  std::string rr = fr.value("key2");
  std::cout << rr << std::endl;

  rr = fr.value("key3");
  std::cout << rr << std::endl;

  double rf = fr.value<double>("key3");
  std::cout << rf << std::endl;

  long rfl = fr.value<long>("key3");
  std::cout << rfl << std::endl;


  // TEST CSV FILES
  {
    try {
      CSVFileReader<std::string> csv("test.csv", ';', 5);
    } 
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
    CSVFileReader<std::string> csv("test.csv",';');
      
    for (auto& rec : csv) {
      for (auto& val : rec)
        std::cout << val << " ";
      std::cout << std::endl;
    }

    for (size_t i = 0; i < csv.size(); ++i) {
      for (size_t col = 0; col < csv.cols(); ++col)
        std::cout << csv[i][col] << " ";
      std::cout << std::endl;
    }

    try {
      CSVFileReaderStr csv2("test-wrong.csv", '#', 4);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  {
    CSVFileReader<int, std::string, double, std::string> csv("test.csv", ';');

    std::cout << std::get<0>(csv[2]) << std::endl;

    for (auto& rec : csv) {
      auto [ivar, strvar, dvar, str2var] = rec;
      std::cout << ivar << strvar << dvar << str2var << std::endl;
    }
  }

  {
    try {
      CSVFileReader<int, float, double, std::string> csv("test.csv", ';');
    }
    catch (std::exception& e) {
      std::cout << e.what();
    }
  }

  try {
    CSVFileReader<int, std::string, double> csv("test.csv", ';');
  }
  catch (std::exception& e) {
    std::cout << e.what();
  }
}