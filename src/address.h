#ifndef favor_address_include
#define favor_address_include

#include "favor.h"
#include "reader.h"
#include "logger.h"

namespace favor {
    class Address {
    public:
        const string addr;
        const int count;
        const long contactId;
        const MessageType type;

        Address(string addr, int c, int contact, MessageType t) : addr(addr), count(c), contactId(contact), type(t){}

        bool belongsToContact(){
            return contactId > -1;
        }

        bool operator==(const Address& other) const{
            //logger::info(as_string(*this)+" compared to "+as_string(other));
            return (type == other.type && contactId == other.contactId &&
                count == other.count && addr == other.addr);
        }

        bool operator!=(const Address& other) const{
            return !(*this==other);
        }

//        //TODO: This won't compile for reasons unclear, but makes sense as a method here
//        bool inDatabase(){
//            return reader::addressExists(addr, type);
//        }
    };
}

#endif