/*
 * String.cpp
 *
 *  Created on: 27.08.2015
 *      Author: Zalasus
 */

#include <odCore/StringUtils.h>

#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace od
{

    std::string &StringUtils::ltrim(std::string &sw)
    {
        sw.erase(sw.begin(), std::find_if(sw.begin(), sw.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));

        return sw;
    }

    std::string &StringUtils::rtrim(std::string &sw)
    {
        sw.erase(std::find_if(sw.rbegin(), sw.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), sw.end());

        return sw;
    }

    std::string StringUtils::trim(const std::string &s)
    {
        std::string sw(s);

        ltrim(rtrim(sw));

        return sw;
    }

	uint32_t StringUtils::split(std::string s, const std::string &delim, std::vector<std::string> &elems)
	{

		uint32_t i = 0;
		size_t pos = s.find(delim);
		std::string token;
		while((pos = s.find(delim)) != std::string::npos)
		{
			token = s.substr(0, pos);

			elems.push_back(token);

			s.erase(0, pos + delim.length());

			i++;
		}

		//append stuff after last delimiter
		if(s.length() > 0)
		{
			elems.push_back(s);
			i++;
		}

		return i;
	}

	bool StringUtils::compareIgnoringCase(const std::string &a, const std::string &b)
	{
		auto ciPred = [](char a, char b){ return std::toupper(a) == std::toupper(b); };

		return std::equal(a.begin(), a.end(), b.begin(), b.end(), ciPred);
	}

	bool StringUtils::startsWith(const std::string &s, const std::string &begin)
    {
        if(begin.length() > s.length() || begin.length() == 0 || s.length() == 0)
        {
            return false;
        }

        for(uint32_t i = 0; i < begin.length(); i++)
        {
            if(s[i] != begin[i])
            {
                return false;
            }
        }

        return true;
    }

    bool StringUtils::endsWith(const std::string &s, const std::string &end)
    {
        if(end.length() > s.length() || end.length() == 0 || s.length() == 0)
        {
            return false;
        }

        for(uint32_t i = 0; i < end.length(); i++)
        {
            if(s[i + (s.length() - end.length())] != end[i])
            {
                return false;
            }
        }

        return true;
    }

    int32_t StringUtils::indexOf(const std::string &s, char find)
    {
        return indexOf(s, find, 0);
    }

    int32_t StringUtils::indexOf(const std::string &s, char find, int32_t startIndex)
    {
        for(uint32_t i = startIndex; i<s.length(); i++)
        {
            if(s[i] == find)
            {
                return i;
            }
        }

        return -1;
    }

    int32_t StringUtils::indexOf(const std::string &s, const std::string &find)
    {
        return indexOf(s, find, 0);
    }

    int32_t StringUtils::indexOf(const std::string &s, const std::string &find, int32_t startIndex)
    {
        if((find.length() + startIndex) > s.length() || find.length() == 0)
        {
            return -1;
        }

        for(uint32_t i = startIndex ; i < (s.length() - find.length()); i++)
        {
            bool found = true;

            for(uint32_t j = 0 ; j < s.length(); j++)
            {
                if(s[i + j] != find[j])
                {
                    found = false;
                    break;
                }
            }

            if(found)
            {
                return i;
            }
        }

        return -1;
    }

}

