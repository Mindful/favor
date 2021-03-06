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



#ifndef favor_jni_reader_include
#define favor_jni_reader_include
#include <jni.h>
#include <reader.h>
#include "jni_exceptions.h"
#include "jni_string.h"
#include "../worker.h"

namespace favor{
    namespace jni{

        JNIEXPORT jlong JNICALL _createContact(JNIEnv* env, jobject callingObj, jstring address, jint type, jstring displayName, jboolean addressExistsHint){
            //TODO: this isn't finding/associating with the address properly when we pass it one that exists
            JNIString addressString(env, address);
            JNIString displaynameString(env, displayName);
            logger::info("Create contact with address: "+addressString.getString()+" and name "+displaynameString.getString());
            //Short circuiting saves us the method call if we know the address exists
            jlong contactId;
            if (addressExistsHint || reader::addressExists(addressString, static_cast<MessageType>(type))){
                //We believe our reader or hint, make an address object and pass it in like it was from the database
                jniExcept(
                    Address addrObj(addressString, -1, -1, static_cast<MessageType>(type));
                        logger::info("Address exists, creating from address with name "+addrObj.addr);
                    contactId = worker::createContactFromAddress(addrObj, displaynameString);
                )
            } else {
                //There's no address already, so we make one
                logger::info("No address found, creating with address");
                jniExcept(
                   contactId = worker::createContactWithAddress(addressString, static_cast<MessageType>(type), displaynameString);
                )
            }
            return contactId;
        }

        JNIEXPORT void JNICALL _createAddress(JNIEnv* env, jobject callingObj, jstring address, jint type,
                jlong inputCount, jlong inputContactId){
            JNIString addressString(env, address);
            long contactId = (long)inputContactId;
            long count = (long)inputCount;
            logger::info("Create address "+addressString.getString()+" with contact id "+as_string(contactId)+" and count "+as_string(count));
            jniExcept(
                    worker::createAddress(addressString.getString(), count, contactId, static_cast<MessageType>(type));
            )
        }

        JNIEXPORT void JNICALL _updateAddressContactId(JNIEnv* env, jobject callingObj, jstring address, jint type, jlong inputContactId){
            JNIString addressString(env, address);
            long contactId = (long)inputContactId;
            logger::info("Update address "+addressString.getString()+" to contact id "+as_string(contactId));
            jniExcept(
                    worker::updateAddressContactId(addressString.getString(), static_cast<MessageType>(type), contactId);
            )
        }

        static JNINativeMethod workerMethodTable[] = {
                {"_createContact", "(Ljava/lang/String;ILjava/lang/String;Z)J", (void*) _createContact},
                {"_createAddress", "(Ljava/lang/String;IJJ)V", (void*) _createAddress},
                {"_updateAddressContactId", "(Ljava/lang/String;IJ)V", (void*) _updateAddressContactId}
        };

    }
}



#endif