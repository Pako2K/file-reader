# file-reader

C++ Classes to read CSV and Properties (Key/Value) files

## Properties file reader
- Class to read a JAVA-like properties file.
- Each property key is separated from the value by a configurable character ('=' by default).
- Property keys can be duplicated.
- Commented lines (starting with '#' or '!') and invalid lines are discarded.
- Leading and trailing spaces are removed.
- Only ASCII characters are supported!

**Usage example:**
```
  #include "properties_file_reader.h"
  
  using namespace utils;
  
  PropertiesFileReader fr("file.prop", ':');

  // Get all the values with key = "key", as strings 
  for (auto& v : fr.values("key")) std::cout << v << std::endl;

  // Get value with key "key", as string
  std::cout << fr["key"] << std::endl;
  std::cout << fr.value("key") << std::endl;

  // Get value casted to type "double" for key "key"
  std::cout << fr.value<double>("key") << std::endl;
```

## CSV file reader
- CSV file reader class. 
- The field types can be provided as template parameters. 
- Each field is separated by a configurable character. 
- Commented lines (starting with '#' or '!') are discarded.
- Only ASCII characters are supported!

**Usage example 1:**
```
  #include "csv_file_reader.h"
  
  using namespace utils;
  
  // Read CSV file with all the 5 fields as strings. Separator character: ';'
  CSVFileReader<std::string> csv("test.csv", ';', 5);

  // Read CSV file with all fields as strings. Separator character: ';'
  CSVFileReader<std::string> csv("test.csv",';');
      
  // Iterate through all the records and fields  using iterators
  for (auto& rec : csv) {
    for (auto& val : rec)
      std::cout << val << " ";
    std::cout << std::endl;
  }

  // Iterate through all the records and fields using index
  for (size_t i = 0; i < csv.size(); ++i) {
    for (size_t col = 0; col < csv.cols(); ++col)
      std::cout << csv[i][col] << " ";
    std::cout << std::endl;
  }
```
**Usage example 2:**
```
  #include "csv_file_reader.h"
  
  using namespace utils;
  
  // Read CSV file with 4 fields of different types. Separator character: ';'
  CSVFileReader<int, std::string, double, std::string> csv("test.csv", ';');

  // Iterate through all the records and fields  using iterators
  for (auto& rec : csv) {
    auto [ivar, strvar, dvar, str2var] = rec;
    std::cout << ivar << " " << strvar << " " << dvar << " " << str2var << std::endl;
  }
```
