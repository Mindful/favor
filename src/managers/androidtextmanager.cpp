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



#include "androidtextmanager.h"

namespace favor{
    JavaVM* AndroidTextManager::vm = NULL;

    AndroidTextManager::AndroidTextManager(string accNm, string detailsJson) : AccountManager(accNm, TYPE_ANDROIDTEXT, detailsJson) {}

    void AndroidTextManager::setVM(JavaVM* inputVm){
        vm = inputVm;
    }

    void AndroidTextManager::updateJson(){
        //TODO:
    }

    void AndroidTextManager::consultJson(bool initial) {
        //TODO:
    }



    //TODO: next two methods should hit static methods for crawling text data. Remember there should only ever be one android text manager
    //http://stackoverflow.com/questions/12420463/jni-keeping-global-reference-to-the-environment

    void AndroidTextManager::fetchAddresses() {
        //TODO: this will be a (static) java method
    }

    void AndroidTextManager::fetchMessages() {
        //TODO: this will be a (static) java method
    }


}