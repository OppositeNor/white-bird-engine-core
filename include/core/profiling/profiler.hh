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
#ifndef __WBE_PROFILLER_HH__
#define __WBE_PROFILLER_HH__

#include "core/engine_core.hh"
#include "core/profiling/profiling_manager.hh"
namespace WhiteBirdEngine {

#define WBE_START_PROFILE(CHANNEL, p_message) auto profiler_##__LINE__ = WhiteBirdEngine::Profiler(CHANNEL, p_message, std::string(__FILE__), __LINE__)

/**
 * @brief The profiler class.
 * This initiates the profiling right after it is constructed, and ends and push data to the manager
 * right after it is destructed.
 */
class Profiler {
public:
    using ProfileData = ProfilingManager::ProfileData;
    Profiler(ChannelID p_channel, const std::string& p_message, const std::string& p_file, uint32_t p_line) {
        profile_data = { .channel = p_channel, .message = p_message, .file = p_file, .line = p_line };
        profile_data.start_time = EngineCore::get_singleton()->global_clock->get_duration();
    }

    ~Profiler() {
        profile_data.delta = EngineCore::get_singleton()->global_clock->get_duration() - profile_data.start_time;
        EngineCore::get_singleton()->profiling_manager->push_profiling_data(std::move(profile_data));
    }

    Profiler(const Profiler&) = delete;
    Profiler(Profiler&&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler& operator=(Profiler&&) = delete;

private:
    ProfileData profile_data;
};

}

#endif
