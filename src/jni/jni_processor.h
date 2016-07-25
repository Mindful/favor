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



#ifndef favor_jni_processor_include
#define favor_jni_processor_include
#include <jni.h>
#include <processor.h>
#include "jni_globals.h"
#include "jni_exceptions.h"
#include "jni_reader.h"
#include "jni_string.h"

namespace favor{
    namespace jni{
        typedef long(*LongQuery)(AccountManager* account, const Contact* c, time_t fromDate, time_t untilDate, bool sent);
        typedef double(*DoubleQuery)(AccountManager* account, const Contact* c, time_t fromDate, time_t untilDate, bool sent);
        //NintiethPercentile, totalCharcount, totalMessageCount
        LongQuery longQueries[] = {&::favor::processor::responseTimeNintiethPercentile, &::favor::processor::totalCharcount, &::favor::processor::totalMessagecount};
        //averageCharcount, conversationalResponsetime
        DoubleQuery doubleQueries[] = {&::favor::processor::averageCharcount, &::favor::processor::conversationalResponsetime};
        /*
                double averageCharcount(AccountManager* account, const Contact& c, time_t fromDate, time_t untilDate, bool sent);
        double conversationalResponsetime(AccountManager* account, const Contact& c, time_t fromDate, time_t untilDate, bool sent);
        long responseTimeNintiethPercentile(AccountManager* account, const Contact& c, time_t fromDate, time_t untilDate, bool sent);

        long totalCharcount(AccountManager* account, const Contact& c, time_t fromDate, time_t untilDate, bool sent);
        long totalMessagecount(AccountManager* account, const Contact& c, time_t fromDate, time_t untilDate, bool sent);
         */

        JNIEXPORT jlong JNICALL longQuery(JNIEnv* env, jobject callingObj, jint query, jstring account, jint type, jlong contactId, jlong fromDate, jlong untilDate, jboolean sent){
            JNIString accountName(env, account);
            AccountManager* accountManager = findAccountManager(accountName, (MessageType) type);
            Contact contact = findContact((long) contactId);
            long result = longQueries[(int)query](accountManager, &contact, (time_t)fromDate, (time_t)untilDate, (bool)sent);
            return (jlong)result;

        }

        JNIEXPORT jdouble JNICALL doubleQuery(JNIEnv* env, jobject callingObj, jint query, jstring account, jint type, jlong contactId, jlong fromDate, jlong untilDate, jboolean sent){
            JNIString accountName(env, account);
            AccountManager* accountManager = findAccountManager(accountName, (MessageType) type);
            Contact contact = findContact((long) contactId);
            double result = doubleQueries[(int)query](accountManager, &contact, (time_t)fromDate, (time_t)untilDate, (bool)sent);
            return (jdouble)result;
        }


        JNIEXPORT jlongArray JNICALL longMultiQuery(JNIEnv* env, jobject callingObj, jint query, jstring account,
                    jint type, jlongArray contactIds, jlong fromDate, jlong untilDate, jboolean sent){
            JNIString accountName(env, account);
            AccountManager* accountManager = findAccountManager(accountName, (MessageType) type);
            jlong* contactIdArray = env->GetLongArrayElements(contactIds, NULL);
            size_t length = (size_t) env->GetArrayLength(contactIds);

            jlongArray result = env->NewLongArray(length);
            if (env->ExceptionOccurred() || result == NULL){
                //Something went wrong, we failed to make our array
                env->DeleteLocalRef(result);
                logger::error("Unable to requisition array for longMultiQuery");
                env->ExceptionClear();
                return NULL;
            }

            std::vector<jlong> buffer(length); //We use a vector instead of an array here only because we can determine vector size at runtime
            for (size_t i = 0; i < length; ++i){
                Contact contact = findContact((long) contactIdArray[i]);
                buffer[i] = (jlong)(longQueries[(int)query](accountManager, &contact, (time_t)fromDate, (time_t)untilDate, (bool)sent));
            }
            env->SetLongArrayRegion(result, 0, length, &buffer[0]);
            env->ReleaseLongArrayElements(contactIds, contactIdArray, JNI_ABORT);
            return result;
        }

        JNIEXPORT jlongArray JNICALL messageCountLastTwoWeeks(JNIEnv* env, jobject callingObj, jstring account, jint type,
                     jlong contactId, jlong fromDate, jlong untilDate, jboolean sent){

            JNIString accountName(env, account);
            AccountManager* accountManager = findAccountManager(accountName, (MessageType) type);
            Contact contact = findContact((long) contactId);
            long endDate = processor::lastContactDate(accountManager, &contact);
            auto messagesPerDay = processor::messagesPerDay(accountManager, &contact, endDate - (WEEK_IN_SECONDS * 2), endDate, sent);

            jlongArray result = env->NewLongArray(messagesPerDay->size());
            if (env->ExceptionOccurred() || result == NULL){
                //Something went wrong, we failed to make our array
                env->DeleteLocalRef(result);
                logger::error("Unable to requisition array for longMultiQuery");
                env->ExceptionClear();
                return NULL;
            }
            std::vector<jlong> buffer(messagesPerDay->size());
            for (size_t i = 0; i < messagesPerDay->size(); ++i){
                buffer[i] = (jlong)(*messagesPerDay)[i];
            }


            env->SetLongArrayRegion(result, 0, messagesPerDay->size(), &buffer[0]);
            return result;

        }

        //TODO: double version untested, but should work fine just like long
        JNIEXPORT jdoubleArray JNICALL doubleMultiQuery(JNIEnv* env, jobject callingObj, jint query, jstring account, jint type, jlongArray contactIds, jlong fromDate, jlong untilDate, jboolean sent){
            JNIString accountName(env, account);
            AccountManager* accountManager = findAccountManager(accountName, (MessageType) type);
            jlong* contactIdArray = env->GetLongArrayElements(contactIds, NULL);
            size_t length = (size_t) env->GetArrayLength(contactIds);

            jdoubleArray result = env->NewDoubleArray(length);
            if (env->ExceptionOccurred() || result == NULL){
                //Something went wrong, we failed to make our array
                env->DeleteLocalRef(result);
                logger::error("Unable to requisition array for longMultiQuery");
                env->ExceptionClear();
                return NULL;
            }

            std::vector<jdouble> buffer(length); //We use a vector instead of an array here only because we can determine vector size at runtime
            for (size_t i = 0; i < length; ++i){
                Contact contact = findContact((long) contactIdArray[i]);
                buffer[i] = (jdouble)(doubleQueries[(int)query](accountManager, &contact, (time_t)fromDate, (time_t)untilDate, (bool)sent));
            }
            env->SetDoubleArrayRegion(result, 0, length, &buffer[0]);
            env->ReleaseLongArrayElements(contactIds, contactIdArray, JNI_ABORT);
            return result;
        }

        JNIEXPORT void JNICALL clearCache(JNIEnv* env, jobject callingObj){
            processor::clearCache();
        }



        static JNINativeMethod processorMethodTable[] = {
                {"longQuery", "(ILjava/lang/String;IJJJZ)J", (void*) longQuery},
                {"doubleQuery", "(ILjava/lang/String;IJJJZ)D", (void*) doubleQuery},
                {"longMultiQuery", "(ILjava/lang/String;I[JJJZ)[J", (void*) longMultiQuery},
                {"doubleMultiQuery", "(ILjava/lang/String;I[JJJZ)[D", (void*) doubleMultiQuery},
                {"messageCountLastTwoWeeks", "(Ljava/lang/String;IJJJZ)[J", (void*) messageCountLastTwoWeeks},
                {"clearCache", "()V", (void*) clearCache}
        };
    }
}

#endif