/*
 * ConfigFile.h
 *
 *  Created on: 8 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_CONFIGFILE_H_
#define INCLUDE_ODCORE_CONFIGFILE_H_

#include <string>
#include <unordered_map>
#include <istream>
#include <sstream>

#include <odCore/Exception.h>

namespace od
{

    /**
     * @brief Simple INI-style config file parser.
     *
     * Supports sections, key=value or key:value style, non-associative arguments and typed access via templates.
     */
    class ConfigFile
    {
    public:

        void parse(std::istream &in);

        /**
         * @brief Accesses a value, trying to parse it as a certain type.
         *
         * Keys that appear in the file before any section statement use an empty string as section name.
         *
         * If the key can not be found or not be parsed, this method will throw.
         *
         * A specialization for T=std::string is provided, which gives direct access to the key's value without
         * parsing anything.
         */
        template <typename T>
        T get(const std::string &section, const std::string &key);

        /**
         * @brief Accesses a value, trying to parse it as a certain type or return a default value.
         *
         * This is identical to the method without a defaultValue parameter, except that this method will
         * not throw, but return defaultValue instead.
         */
        template <typename T>
        T get(const std::string &section, const std::string &key, const T &defaultValue);

        const std::vector<std::string> &getRawValuesInSection(const std::string &section);


    private:

        struct Section
        {
            std::unordered_map<std::string, std::string> keyValueMap;
            std::vector<std::string> rawValueList;
        };

        std::unordered_map<std::string, Section> mSections;

    };

    template <typename T>
    T ConfigFile::get(const std::string &section, const std::string &key)
    {
        std::istringstream iss(get<std::string>(section, key));

        T value;
        iss >> value;

        if(iss.bad())
        {
            throw od::Exception("Failed to parse key in config file: " + key);
        }

        return value;
    }

    template <typename T>
    T ConfigFile::get(const std::string &section, const std::string &key, const T &defaultValue)
    {
        auto sectionIt = mSections.find(section);
        if(sectionIt == mSections.end())
        {
            return defaultValue;
        }

        auto keyIt = sectionIt->second.keyValueMap.find(key);
        if(keyIt == sectionIt->second.keyValueMap.end())
        {
            return defaultValue;
        }

        std::istringstream iss(keyIt->second);

        T value;
        iss >> value;

        if(iss.bad())
        {
            return defaultValue;
        }

        return value;
    }

    template <>
    std::string ConfigFile::get(const std::string &section, const std::string &key);

    template <>
    std::string ConfigFile::get(const std::string &section, const std::string &key, const std::string &defaultValue);

}

#endif /* INCLUDE_ODCORE_CONFIGFILE_H_ */
