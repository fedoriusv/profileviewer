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


        //! Encode a block of data in a base64 string
        /*!
        \param src Block of data to encode
        \param dataSize Size of the block of data to encode
        \param dest Reference to append the encoded string to
        \param maxLineLength Maximum encoded line length. A line feed will be inserted every maxLineLength
        \return True if the data was encoded successfully
        */
        static bool EncodeBase64(const unsigned char* src, size_t dataSize, std::string& dest, size_t maxLineLength);

        //! Decode a base64 encoded string into a block of data
        /*!
        \param base64Data Base64 c-string to decode
        \param dataSize Size of the string to decode
        \param outData Pointer to the buffer to write to <b>(must be already allocated)</b>
        \param strict If true, all character not in the alphabet for Base64, will be ignored.
        \return Size of the decoded data
        */
        static size_t DecodeBase64(const char* base64Data, size_t dataSize, void* outData, bool strict = false);

        //! Decode a base64 encoded string into a block of data
        /*!
        \param base64Data Base64 string to decode
        \param outData Pointer to the buffer to write to <b>(must be already allocated)</b>
        \param strict If true, all character not in the alphabet for Base64, will be ignored.
        \return Size of the decoded data
        */
        static size_t DecodeBase64(const std::string& base64Data, void* outData, bool strict = false);

        //! Test whether data is in the base encoding alhpabet.
        /*!
        \param data Base64 char to decode.
        \return True if  data is in the base encoding alhpabet, otherwise false.
        */
        static bool IsInBase64Alphabet(char data);

        //! Get the decoded size of a base64 string
        /*!
        \param base64Data Base64 string to decode
        \param strict If true, all character not in the alphabet for Base64, will be ignored.
        \return Size of the decoded data
        */
        static size_t GetDecodedBase64DataSize(const std::string& base64Data, bool strict = false);
    };
};

#endif //_PV_UTILS_H_