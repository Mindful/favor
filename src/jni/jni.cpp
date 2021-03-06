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



#include <jni.h>

#include "jni_reader.h"
#include "jni_worker.h"
#include "jni_accountmanager.h"
#include "jni_processor.h"
#include "jni_globals.h"
#include "../logger.h"
#include "../managers/androidtextmanager.h"

extern "C" {

//Java, then package name, then class name, then method name, separated by underscores (also underscores per "." in the package name)
//Basically if you get an error calling this stuff, the solution is replacing every . in the error with an underscore

//TODO: might make way more sense to do the directory stuff in java.
JNIEXPORT void JNICALL init(JNIEnv* env, jobject obj, jstring path, jboolean first){
    favor::dbPath = env->GetStringUTFChars(path, NULL); //This is technically a memory leak, but we're holding onto the same amount of memory if we copy it anyway

    favor::initialize();
    if (first) favor::worker::buildDatabase();
    favor::reader::refreshAll();

}

JNIEXPORT void JNICALL cleanup(JNIEnv* env, jobject obj){
    favor::cleanup();
}

static JNINativeMethod coreMethodTable[] = {
        {"init", "(Ljava/lang/String;Z)V", (void*) init},
        {"cleanup", "()V", (void*) cleanup}
};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass core = env->FindClass(favor::jni::coreClassPath);
    jclass reader = env->FindClass(favor::jni::readerClassPath);
    jclass worker = env->FindClass(favor::jni::workerClassPath);
    jclass processor = env->FindClass(favor::jni::processorClassPath);

    favor::jni::account_manager = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass(favor::jni::accountManagerClassPath)));
    favor::jni::account_manager_constructor = env->GetMethodID(favor::jni::account_manager, "<init>", "(Ljava/lang/String;I)V");

    favor::jni::android_text_manager = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass(favor::jni::androidTextManagerClassPath)));
    favor::jni::android_text_manager_constructor = env->GetMethodID(favor::jni::android_text_manager, "<init>", "(Ljava/lang/String;)V");

    favor::jni::address = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass(favor::jni::addressClassPath)));
    favor::jni::address_constructor = env->GetMethodID(favor::jni::address, "<init>", "(Ljava/lang/String;JJI)V");

    favor::jni::contact = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass(favor::jni::contactClassPath)));
    favor::jni::contact_constructor = env->GetMethodID(favor::jni::contact, "<init>", "(JLjava/lang/String;)V");

    favor::jni::favor_exception = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/favor/library/FavorException")));
    favor::jni::java_string = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/String")));



    //It's weird this can happen before init, but init may end up called in this method eventually and this is crucial in any case
    favor::AndroidTextManager::setVM(vm);

    env->RegisterNatives(core, coreMethodTable, sizeof(coreMethodTable) / sizeof(coreMethodTable[0]));
    env->RegisterNatives(reader, favor::jni::readerMethodTable, sizeof(favor::jni::readerMethodTable) / sizeof (favor::jni::readerMethodTable[0]));
    env->RegisterNatives(favor::jni::account_manager, favor::jni::accountManagerMethodTable, sizeof(favor::jni::accountManagerMethodTable) / sizeof(favor::jni::accountManagerMethodTable[0]));
    env->RegisterNatives(worker, favor::jni::workerMethodTable, sizeof(favor::jni::workerMethodTable) / sizeof (favor::jni::workerMethodTable[0]));
    env->RegisterNatives(processor, favor::jni::processorMethodTable, sizeof(favor::jni::processorMethodTable) / sizeof (favor::jni::processorMethodTable[0]));

    return JNI_VERSION_1_6;
}

}
