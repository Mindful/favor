#ifndef favor_message_include
#define favor_message_include

#include "favor.h"

namespace favor {
    class Message {
    public:

        const MessageType type;
        const bool sent;
        const long id;
        const std::time_t date;
        const string address;
        const bool media;
        const string body;
        const size_t charCount;

        Message(MessageType t, bool s, long int i, std::time_t d, string a, bool m, const string& b, size_t cc);

        string prettyDate() const;

        bool failure() const;

    };
}

#endif