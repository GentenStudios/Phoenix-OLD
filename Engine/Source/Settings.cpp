// Copyright 2019 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <Quartz2/Settings.hpp>

using namespace q2;

Setting::Setting(std::string name, std::string key, int defaultValue) :
    m_name(name), m_key(key), m_value(defaultValue), m_maxValue(SHRT_MAX), m_minValue(SHRT_MIN) 
    {}

bool Setting::set(int value) {
    if (m_maxValue >= value >= m_minValue) {
        m_value = value;
        return true;
    }
    return false;
}

void Setting::setMax(int value){
    m_maxValue = value;
}

void Setting::setMin(int value){
    m_minValue = value;
}

int Setting::value() {
    return m_value;
};

std::size_t Settings::add(const Setting *setting){
    m_setting.push_back(setting);
    return m_setting.size() - 1;
}

std::size_t Settings::add(std::string name, std::string key,
                          int defaultValue) {
    static Setting setting = Setting(name, key, defaultValue);
    add(&setting);
  
};

const Setting *Settings::getSetting(std::size_t key) {
  if (m_setting.size() > key) {
    return m_setting[key];
  }
  return nullptr;
};