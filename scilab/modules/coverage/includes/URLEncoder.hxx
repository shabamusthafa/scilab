/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2015 - Scilab Enterprises - Calixte DENIZET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

#ifndef __URLENCODER_HXX__
#define __URLENCODER_HXX__

#include <sstream>
#include <set>
#include <string>

namespace coverage
{
class URLEncoder
{
    struct __Pair
    {
        const char c;
        const std::string rep;

        __Pair(const char _c, const std::string _rep) : c(_c), rep(_rep) { }
        __Pair(const char _c) : c(_c) { }

        inline bool operator<(const __Pair & R) const
        {
            return c < R.c;
        }
    };
    static std::set<__Pair> pairs;

public:

    inline static void replace(std::ostringstream & out, const char c)
    {
        auto i = pairs.find(__Pair(c));
        if (i == pairs.end())
        {
            out << c;
        }
        else
        {
            out << i->rep;
        }
    }

private:

    inline static std::set<__Pair> init()
    {
        std::set<__Pair> pairs;
        pairs.emplace('%', "%25");
        pairs.emplace('!', "%21");
        pairs.emplace('\"', "%22");
        pairs.emplace('#', "%23");
        pairs.emplace('$', "%24");
        pairs.emplace('&', "%26");
        pairs.emplace('\'', "%27");
        pairs.emplace('(', "%28");
        pairs.emplace(')', "%29");
        pairs.emplace('*', "%2A");
        pairs.emplace('+', "%2B");
        pairs.emplace(',', "%2C");
        pairs.emplace('/', "%2F");
        pairs.emplace(':', "%3A");
        pairs.emplace(';', "%3B");
        pairs.emplace('=', "%3D");
        pairs.emplace('?', "%3F");
        pairs.emplace('@', "%40");
        pairs.emplace('[', "%5B");
        pairs.emplace(']', "%5D");
        pairs.emplace(' ', "%20");
        pairs.emplace('-', "%2D");
        pairs.emplace('<', "%3C");
        pairs.emplace('>', "%3E");
        pairs.emplace('{', "%7B");
        pairs.emplace('}', "%7D");
        pairs.emplace('_', "%5F");
        pairs.emplace('~', "%7E");

        return pairs;
    }
};

} // namespace coverage

#endif // __URLENCODER_HXX__