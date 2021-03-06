/*
Copyright (C) 2015  Joshua Tanner (mindful.jt@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef favor_core_include
#define favor_core_include

#include <string>
#include <vector>
#include <list>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <memory>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <set>
#include <regex>

//External libraries
#include "sqlite/sqlite3.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "pugixml/pugixml.hpp"
#include "utf8cpp/utf8.h"
#ifndef ANDROID
#include "iconvpp/iconv.hpp"
#endif

//Favor core headers
#include "definitions.h"
#include "exceptions.h"
#include "types.h"
#include "conversation_data.h"

namespace favor {
    class Message;
    class Address;
    class Contact;

    //A custom pair class, purely for clarity, because this comes up a lot
    template <typename T>
    class SentRec {
    public:
        T sent;
        T received;

        SentRec(T s, T r){
            sent = s;
            received = r;
        }

        SentRec(const SentRec &other){
            sent = other.sent;
            received = other.received;
        }

        SentRec(){}

    };

    extern const char* dbPath;
    extern const char* dbName;

    //Basic
    void initialize();

    void cleanup();

    void rebuildDatabase();

    //Utility
    void sqlite3_validate(int result, sqlite3 *db);

    void refresh_utc_offset();

    time_t to_time_t_utc( struct tm* timeptr );

    void sqlite3_bind_regexp_function(sqlite3* db);

    string sqlite3_get_string(sqlite3_stmt* stmt, int col);

    bool compareAddress(const Address& lhs, const Address& rhs);

    double round(double d);

    string lowercase(const string& s);

#ifndef ANDROID
    string to_utf8(const string& s, const string& inputEncoding);
#endif

    string as_string(int i);

    string as_string(long l);

    string as_string(float f);

    string as_string(double d);

    string as_string(const Message& m);

    string as_string(const Address& a);

    string as_string(const Contact& c);

    string as_string(const AccountManager& a);


    string as_string(const rapidjson::Document &json);

    string as_string(const ConversationData c);
}

#endif