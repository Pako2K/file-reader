//  MIT License
//
//  Copyright (c) 2018 Francisco de Lanuza
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#ifndef CSV_FILE_READER_H
#define CSV_FILE_READER_H

#include <string>
#include <vector>
#include <tuple>
#include <type_traits>
#include <exception>
#include <fstream>


namespace utils
{
  /**
    *  \brief CSV file reader class. The field types are provided as template parameters. 
    *         Each field is separated by a separator character. 
    *         Commented lines (starting with '#' or '!') are discarded.
    *         Only ASCII characters are supported!
    */
  template<class... TYPES>
  class CSVFileReader {
  protected:
    /**
      *  Vector used to store in memory the content of the file
      */
    std::vector<std::tuple<TYPES...>> _records;

    /**
      *  \brief Cast and copy the string value in a vector into a tuple.
      *  @param tuple [out] tuple where the values must be copied into
      *  @param strValues [in] Vector containing the values as atrings
      */
    template<size_t POS = 0>
    void _copyToTuple(std::tuple<TYPES...>& tuple, const std::vector<std::string>& strValues);

  public:
    /**
     *  \brief Constructor
     *         Reads the file and initializes the list of records, each record is a vector of fields
     *  @param fileName [in] Name of the properties file
     *  @param separator [in] Character used as value separator. By default is a ','
     *  @throw runtime_error File cannot be opened or some record does not contain the same number of values
     */
    CSVFileReader(const std::string& fileName, char separator = ',');

    /**
      *  \brief Returns the total number of records in the csv file
      *  @return  the number of records
      */
    size_t size() const { return _records.size(); }

    /**
      *  \brief Returns the total number of values in a record
      *  @return  the number of values
      */
    constexpr size_t cols() const { return sizeof...(TYPES); }

    /**
     *  \brief Returns an iterator to the first record
     *  @return  vector iterator
     */
    auto begin() const { return _records.begin(); }

    /**
     *  \brief Returns an iterator to the first record
     *  @return  vector iterator
     */
    auto end() const { return _records.end(); }

    /**
    *  \brief Returns a tuple with all the values in the specified record number
    *  @param row [in] record number, staring at 0
    *  @return  a tuple
    *  @throw  out_of_range exception if there is no row with that number
    */
    const auto& operator[](size_t row) const { return _records[row]; }
  };


  /**
    *  \brief CSV file reader class. Specialization for records where all the values are of the same type
    */
  template<class TYPE>
  class CSVFileReader<TYPE>
  {
  protected:
    /**
      *  Vector used to store in memory the content of the file
      */
    std::vector<std::vector<TYPE>> _records;

  public:
    /**
      *  \brief Constructor
      *         Reads the file and initializes the list of records, each record is a vector of fields
      *  @param fileName [in] Name of the properties file
      *  @param separator [in] Character used as value separator. By default is a ','
      *  @param cols [in] Number of values in each record. Default value '0' means that it will be based on the content of the csv file. 
                          All records must have the same number of values.
      *  @throw runtime_error File cannot be opened or some record does not contain the same number of values
      */
    CSVFileReader(const std::string& fileName, char separator = ',', size_t cols = 0);
        
    /**
      *  \brief Returns the total number of records in the csv file
      *  @return  the number of records
      */
    size_t size() const { return _records.size(); }

    /**
      *  \brief Returns the total number of values in a record
      *  @return  the number of values
      */
    size_t cols() const { return _records[0].size(); }

    /**
     *  \brief Returns an iterator to the first record
     *  @return  vector iterator
     */
    auto begin() const { return _records.begin(); }

    /**
     *  \brief Returns an iterator to the first record
     *  @return  vector iterator
     */
    auto end() const { return _records.end(); }

    /**
    *  \brief Returns a vector with all the values in the specified record number
    *  @param row [in] record number, staring at 0
    *  @return  a vector 
    *  @throw  out_of_range exception if there is no row with that number
    */
    const auto& operator[](size_t row) const { return _records[row]; }
  };

  using CSVFileReaderStr = CSVFileReader<std::string>;
    

  /**
    *  \brief Tokenizer Functor
    */
  class Tokenizer
  {
  public:
    Tokenizer() {};
    /**
      *  \brief operator() --> tokenizes a string according to the given separator character, and returns a vector of token strings
      *  @param tokens [out] reference to the vector of string to be used to return the tokens
      *  @param str [in] string to be tokenized
      *  @param sep [in] token separator character
      *  @return void
      */
    void operator() (std::vector<std::string> &tokens, const std::string &str, const char sep);
  };


  //*** DEFINITIONS ***********************************************************************************************************/
  //***************************************************************************************************************************/

  /// Private method _copyToTuple
  template<class... TYPES>
  template<size_t POS>
  void CSVFileReader<TYPES...>::_copyToTuple(std::tuple<TYPES...>& tuple, const std::vector<std::string>& strValues) {
    using VAL_TYPE = typename std::tuple_element<POS, std::tuple<TYPES...>>::type;
    static_assert(std::is_arithmetic<VAL_TYPE>::value || std::is_same<VAL_TYPE, std::string>::value, "Type not supported ");
    if constexpr (std::is_same<VAL_TYPE, std::string>::value) {
      std::get<POS>(tuple) = strValues[POS];
    }
    else {
      if constexpr (std::is_floating_point<VAL_TYPE>::value)
        std::get<POS>(tuple) = VAL_TYPE(atof(strValues[POS].c_str()));
      else if constexpr (sizeof(VAL_TYPE) > sizeof(int))
        std::get<POS>(tuple) = VAL_TYPE(atol(strValues[POS].c_str()));
      else
        std::get<POS>(tuple) = VAL_TYPE(atoi(strValues[POS].c_str()));
    }

    if constexpr ((POS + 1) < sizeof...(TYPES))
      this->_copyToTuple<POS + 1>(tuple, strValues);
  }

  /// CONSTRUCTOR 
  template<class... TYPES>
  CSVFileReader<TYPES...>::CSVFileReader(const std::string& fileName, char separator) {
    // Open file
    std::ifstream file(fileName, std::ios::in);
    if (!file.is_open())
      throw std::runtime_error("File cannot be opened: " + fileName);

    // Resize the vector for records
    _records.reserve(100);

    // Read each line, discarding empty ones or starting with '#' or '!'
    std::string line{ "" };
    std::vector<std::string> strValues(sizeof...(TYPES));


    Tokenizer tokenizer;
    while (!file.eof()) {
      getline(file, line);

      if (line.length() == 0 || line[0] == '#' || line[0] == '!') continue;

      tokenizer(strValues, line, separator);

      if (strValues.size() != sizeof...(TYPES)) {
        file.close();
        auto line{ _records.size() + 1 };
        _records.clear();
        throw std::range_error(std::string("Inconsistent CSV file. Line ") + std::to_string(line) + " contains " + std::to_string(strValues.size()) + " values. Expected " + std::to_string(sizeof...(TYPES)));
      }

      std::tuple<TYPES...> rec;
      _copyToTuple(rec, strValues);
        
      _records.push_back(rec);
    }

    file.close();

    _records.shrink_to_fit();
  }



  /// CONSTRUCTOR SPECIALIZED CLASS
  template<class TYPE>
  CSVFileReader<TYPE>::CSVFileReader(const std::string& fileName, char separator, size_t numValues) {
    // Open file
    std::ifstream file(fileName, std::ios::in);
    if (!file.is_open())
      throw std::runtime_error("File cannot be opened: " + fileName);

    // Resize the vector for records
    _records.reserve(100);

    // Read each line, discarding empty ones or starting with '#' or '!'
    std::string line{ "" };
    std::vector<std::string> values(numValues || 1);
    Tokenizer tokenizer;
    while (!file.eof()) {
      getline(file, line);

      if (line.length() == 0 || line[0] == '#' || line[0] == '!') continue;

      tokenizer(values, line, separator);

      // For the first record
      if (!_records.size() && !numValues)
        numValues = values.size();

      if (values.size() != numValues) {
        file.close();
        auto line{ _records.size() + 1 };
        _records.clear();
        throw std::range_error(std::string("Inconsistent CSV file. Line ") + std::to_string(line) + " contains " + std::to_string(values.size()) + " values. Expected " + std::to_string(numValues));
      }
      
      _records.push_back(values);
    }

    file.close();

    _records.shrink_to_fit();
  }


  /// TOKENIZER FUNCTOR ******************************************************
  void Tokenizer::operator() (std::vector<std::string>& tokens, const std::string& str, const char sep) {
    size_t init_pos{ 0 };
    size_t pos{ 0 };
    tokens.clear();
    while ((pos = str.find(sep, init_pos)) != std::string::npos) {
      tokens.push_back(str.substr(init_pos, pos - init_pos));
      init_pos = pos + 1;
    }
    // Add last token
    tokens.push_back(str.substr(init_pos, str.size() - init_pos));
    tokens.shrink_to_fit();
  }
}

#endif // CSV_FILE_READER_H
