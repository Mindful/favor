#include "favor.h"
#include "worker.h"
#include "reader.h"
#include "logger.h"
#include "message.h"

using namespace std;

namespace favor {
    const char *MessageTypeName[] = {"email", "androidtext", "line", "skype"};
    const char* dbPath = "."; //TODO: This can eventually be something else, but we'll need to ensure the directory exists in that case

    void initialize() {
        assert(sqlite3_config(SQLITE_CONFIG_MULTITHREAD)==SQLITE_OK);
        assert(sqlite3_initialize()==SQLITE_OK);
        assert(sqlite3_threadsafe()); //We just want this to not be 0, not for it to be ==SQLITE_OK
        assert(sqlite3_enable_shared_cache(1)==SQLITE_OK);
        //The worker must be initialized first, because it will create the database if there is none
        worker::initialize();
        reader::initialize();
    }

    void cleanup() {
        worker::cleanup();
        reader::cleanup();
        sqlite3_shutdown();
    }


    void sqlite3_validate(int result, sqlite3 *db, bool constraintFailureOK) {
        switch (result) {
            case SQLITE_OK:
                break;
            case SQLITE_ROW:
                break;
            case SQLITE_DONE:
                break;
            case SQLITE_CONSTRAINT:
                if (!constraintFailureOK){
                    logger::error("SQLite error #" + as_string(result) + ", db says \"" + sqlite3_errmsg(db) + "\"");
                    throw constraintViolationException();
                }
                else logger::warning("SQLite constraint failed, db says \"" + string(sqlite3_errmsg(db)) + "\"");
            default:
                logger::error("SQLite error #" + as_string(result) + ", db says \"" + sqlite3_errmsg(db) + "\"");
                throw sqliteException();
        }
    }

    bool compareAddress(const Address &lhs, const Address &rhs) {
        //Results in a largest first array
        if (lhs.contactId > -1){
            if (rhs.contactId > -1){
                lhs.count == rhs.count ? lhs.addr > rhs.addr : lhs.count > rhs.count;
            }
            else return true;
        }
        else if (rhs.contactId > -1) return false;
        else{
            return lhs.count == rhs.count ? lhs.addr > rhs.addr : lhs.count > rhs.count;
        }
    }

    //On Android, these methods will not behave well with numbers whose digit count exceeds the DIGIT_MAX
    string as_string(long int l) {
    #ifdef ANDROID
    #define LONG_DIGIT_MAX 20
    char result[LONG_DIGIT_MAX] = {0};
    sprintf(result, "%ld", l);
    return string(result);
    #else
        return to_string(l);
    #endif
    }

    string as_string(int i) {
    #ifdef ANDROID
    #define INT_DIGIT_MAX 10
    char result[INT_DIGIT_MAX] = {0};
    sprintf(result, "%d", i);
    return string(result);
    #else
        return to_string(i);
    #endif
    }

    string as_string(float f){
    #ifdef ANDROID
    //TODO: should be quick
    #else
        return to_string(f);
    #endif
    }

    string as_string(double d){
    #ifdef ANDROID
    //TODO: should be quick
    #else
        return to_string(d);
    #endif
    }

    string as_string(const rapidjson::Document &json) {
        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
        json.Accept(writer);
        return buff.GetString();
    }

    string as_string(const Message &m) {
        //TODO: update this to account for missing values (which there will be sometimes when we pull from the DB)
        string result = "[Message ID: "+as_string(m.id)+" | Sent? " + as_string(m.sent)+" | Date: "+m.prettyDate()+" | Address: "+m.address;
        result += " | Media? "+as_string(m.media()) + " | Body Length: "+as_string((long)m.charCount)+ "| Body: <<"+ m.body()+">>]";
        return result;
    }

    string as_string(const Address& a){
        return "[Address: "+a.addr+" | Count: " + as_string(a.count) + " | Contact ID: " + as_string(a.contactId)+"]";

    }

    string as_string(const Contact& c){
        string result = "[Contact Display Name: " +c.displayName + " | ID: "+as_string(c.id)+"| Addresses: (";
        for (int i = 0; i < c.getAddresses().size(); ++i){
            if (i == c.getAddresses().size() - 1) result += as_string(c.getAddresses()[i]) + ")";
            else result += as_string(c.getAddresses()[i]) + ", ";
        }
    }

    string as_string(const AccountManager& a){
        string result = "[Account Name: "+a.accountName+ "| Type: "+as_string((int)a.type)+" | JSON: \""+a.getJson()+"\"]";
        return result;
    }

    string lowercase(const string &s){
        string ret = s;
        transform(s.begin(), s.end(), ret.begin(), ::tolower); //Only compiles specifically with "::tolower"
        return ret;
    }


}