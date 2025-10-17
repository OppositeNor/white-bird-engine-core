/* Copyright 2025 OppositeNor

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "core/logging/log_stream.hh"
#include "core/engine_core.hh"

namespace WhiteBirdEngine {
void LogStream::message(const std::string& p_str) {
    auto& channel_name = EngineCore::get_singleton()->label_manager->get_label_name(channel_id);
    *ostream << "[" << channel_name << "] <Message>: " << p_str << std::endl;
}

void LogStream::warning(const std::string& p_str) {
    auto& channel_name = EngineCore::get_singleton()->label_manager->get_label_name(channel_id);
    *ostream << "[" << channel_name << "] <Warning>: " << p_str << std::endl;
}

void LogStream::error(const std::string& p_str) {
    auto& channel_name = EngineCore::get_singleton()->label_manager->get_label_name(channel_id);
    *ostream << "[" << channel_name << "] <Error>: " << p_str << std::endl;
}

}
