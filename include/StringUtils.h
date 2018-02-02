/*
 * std::string.h
 *
 *  Created on: 26.08.2015
 *      Author: Zalasus
 */

#ifndef INCLUDE_STRINGUTILS_H_
#define INCLUDE_STRINGUTILS_H_

#include <string>
#include <vector>

namespace od
{

	class StringUtils
	{
	public:

		static uint32_t split(std::string s, const std::string &delim, std::vector<std::string> &elems);

		static std::string toLower(const std::string &s);
		static std::string toUpper(const std::string &s);
		static bool compareIgnoringCase(const std::string &a, const std::string &b);

		static std::string &ltrim(std::string &sw);
		static std::string &rtrim(std::string &sw);

		static std::string trim(const std::string &s);

		static bool startsWith(const std::string &s, const std::string &begin);
		static bool endsWith(const std::string &s, const std::string &end);

		static int32_t indexOf(const std::string &s, char find);
        static int32_t indexOf(const std::string &s, char find, int32_t startIndex);
        static int32_t indexOf(const std::string &s, const std::string &find);
        static int32_t indexOf(const std::string &s, const std::string &find, int32_t startIndex);

	};

}

#endif /* INCLUDE_STRING_H_ */
