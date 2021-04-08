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

#ifndef PROPERTIES_FILE_READER_H
#define PROPERTIES_FILE_READER_H

#include <string>
#include <map>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <fstream>


namespace utils
{
  /**
    *  \brief JAVA-like properties file reader class
    *           Each property key is separated from the value by a '=' (by default).
    *           Property keys can be duplicated
    *           Commented lines (starting with '#' or '!') and invalid lines are discarded.
    *           Leading and trailing spaces are removed.
    *           Only ASCII characters are supported!
  */
  class PropertiesFileReader
  {
  protected:
    /**
      *  Multimap used to store in memory the content of the file
      */
    std::multimap<std::string, std::string> _properties;


  public:
    /**
      *  \brief Constructor
      *         Reads the file and initializes the map of key/value pairs
      *  @param fileName [in] Name of the properties file
      *  @param separator [in] Character used to separate key/values
      *  @throw  runtime_error exception containing the description of the issue (file not found)
      */
    PropertiesFileReader(const std::string& fileName, const char separator = '=');

    /**
      *  \brief Returns the different keys of the properties
      *  @return  a vector which contains the keys
      */
    std::vector<std::string> keys() const;

    /**
      *  \brief Returns the values of the properties with the specified key, converted to type TYPE
      *  @param key [in] key of the value(s) to be returned
      *  @return  a vector which contains the values with the specified key
      *  @throw  runtime_error if the value cannot be converted to the given type
      */
    template <class TYPE = std::string, typename = std::enable_if<std::is_arithmetic<TYPE>::value || std::is_same<TYPE, std::string>::value> >
    std::vector<TYPE> values(const std::string& key) const;

    /**
      *  \brief Returns the first value of the properties with the specified key, converted to type T
      *  @param key [in] key which value has to be returned
      *  @return  a TYPE which contains the first value for the properties with the specified key
      *  @throw  out_of_range exception if there is no property with the specified key
      */
    template <class TYPE = std::string, typename = std::enable_if<std::is_arithmetic<TYPE>::value || std::is_same<TYPE, std::string>::value> >
    TYPE value(const std::string& key) const;

    /**
     *  \brief Returns the first value of the properties with the specified key, as a string
     *  @param key [in] key which value has to be returned
     *  @return  a string which contains the first value for the properties with the specified key
     *  @throw  out_of_range exception if there is no property with the specified key
     */
    std::string operator[](const std::string& key) const { return this->value(key); }
  };


  /// CONSTRUCTOR
  PropertiesFileReader::PropertiesFileReader(const std::string& fileName, const char separator) {

    // Open file
    std::ifstream propFile(fileName, std::ios::in);
    if (!propFile.is_open())
      throw std::runtime_error("File cannot be opened: " + fileName);

    try {
      // Read all not commented or blank lines
      std::string line;
      while (!propFile.eof()) {
        getline(propFile, line);

        // Add key and value, trimming leading and trailing spaces, and skipping if the first character is '#'
        std::string key;
        size_t i{ 0 };
        while (line[i] != separator && i < line.length()) {
          if (line[i]<0 || !isspace(line[i]))
            key.push_back(line[i]);
          ++i;
        };
        if (key.length() == 0 || key[0] == '#' || key[0] == '!' || key.length() == line.length()) // Not a property
          continue;

        // Read value
        std::string value;
        // Trim leading spaces
        while (line[++i] < 0 || isspace(line[i]));
        value = line.substr(i);
        // And remove trailing spaces
        i = value.length();
        while (value[--i]<0 ||  isspace(value[i]));
        value.erase(++i);

        _properties.insert(std::make_pair(key, value));
      }
    }
    catch (std::exception& e) {
      throw std::runtime_error(e.what());
    }

    // Close file
    propFile.close();
  }

  std::vector<std::string> PropertiesFileReader::keys() const {
    std::vector<std::string> res;
    for (auto& kv : _properties) {
      if (!res.size() || res.back() != kv.first) res.push_back(kv.first);
    }
    return res;
  }


  template <class TYPE, typename >
  std::vector<TYPE> PropertiesFileReader::values(const std::string& key) const {
    auto match_iter{ _properties.equal_range(key) };

    std::vector<TYPE> values;
    try {
      if (match_iter.first != _properties.end()) {
        while (match_iter.first != match_iter.second) {
          if constexpr (std::is_same<TYPE, std::string>::value)
            values.push_back(match_iter.first->second);
          else {
            if constexpr (std::is_floating_point<TYPE>::value)
              values.push_back(TYPE(atof((match_iter.first->second).c_str())));
            else if constexpr (sizeof(TYPE) > sizeof(int))
              values.push_back(TYPE(atol((match_iter.first->second).c_str())));
            else
              values.push_back(TYPE(atoi((match_iter.first->second).c_str())));
          }
          ++match_iter.first;
        }
      }
    }
    catch (std::exception& e) {
      throw std::runtime_error(e.what());
    }

    return values;
  }


  template <class TYPE, typename >
  TYPE PropertiesFileReader::value(const std::string& key) const {
    auto match_iter = _properties.equal_range(key);
    if (match_iter.first != _properties.end() && match_iter.first != match_iter.second) {
      if constexpr (std::is_same<TYPE, std::string>::value)
        return match_iter.first->second;
      else {
        if constexpr (std::is_floating_point<TYPE>::value)
          return TYPE(atof((match_iter.first->second).c_str()));
        else if constexpr (sizeof(TYPE) > sizeof(int))
          return TYPE(atol((match_iter.first->second).c_str()));
        else
          return TYPE(atoi((match_iter.first->second).c_str()));
      }
    }
    else
      throw std::out_of_range("Property not found: " + key);
  }
  
}

#endif // PROPERTIES_FILE_READER_H
