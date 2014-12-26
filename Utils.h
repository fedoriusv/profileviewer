#ifndef _PV_UTILS_H_
#define _PV_UTILS_H_

#include "common.h"

namespace pv
{
    class Codec
    {
    public:

        //according to RFC 3986 http://www.ietf.org/rfc/rfc3986.txt
        //; HTTP
        //
        //httpurl        = "http://" hostport [ "/" hpath [ "?" search ]]
        //hpath          = hsegment *[ "/" hsegment ]
        //hsegment       = *[ uchar | ";" | ":" | "@" | "&" | "=" ]
        //search         = *[ uchar | ";" | ":" | "@" | "&" | "=" ]
        //
        //; Miscellaneous definitions
        //
        //lowalpha       = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" |
        //                 "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" |
        //                 "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" |
        //                 "y" | "z"
        //hialpha        = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" |
        //                 "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" |
        //                 "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
        //
        //alpha          = lowalpha | hialpha
        //digit          = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" |
        //                 "8" | "9"
        //safe           = "$" | "-" | "_" | "." | "+"
        //extra          = "!" | "*" | "'" | "(" | ")" | ","
        //national       = "{" | "}" | "|" | "\" | "^" | "~" | "[" | "]" | "`"
        //punctuation    = "<" | ">" | "#" | "%" | <">
        //
        //
        //reserved       = ";" | "/" | "?" | ":" | "@" | "&" | "="
        //hex            = digit | "A" | "B" | "C" | "D" | "E" | "F" |
        //                 "a" | "b" | "c" | "d" | "e" | "f"
        //escape         = "%" hex hex
        //
        //unreserved     = alpha | digit | safe | extra
        //uchar          = unreserved | escape

        //! Escape any character that is not unreserved according to RFC1738
        static bool EncodeUrlRFC3986(const std::string& source, std::string& dest);

        static void EncUrlGetKeysFromChar(char nChar, char& hex1, char& hex2);
    };
};

#endif //_PV_UTILS_H_