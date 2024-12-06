/*
 *    Copyright 2023 The ChampSim Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <utility>
#include <vector>

#include "stats_printer.h"

void champsim::plain_printer::print(O3_CPU::stats_type stats) {
    constexpr std::array<std::pair<std::string_view, std::size_t>, 6> types{
        {std::pair{"BRANCH_DIRECT_JUMP", BRANCH_DIRECT_JUMP}, std::pair{"BRANCH_INDIRECT", BRANCH_INDIRECT}, std::pair{"BRANCH_CONDITIONAL", BRANCH_CONDITIONAL},
         std::pair{"BRANCH_DIRECT_CALL", BRANCH_DIRECT_CALL}, std::pair{"BRANCH_INDIRECT_CALL", BRANCH_INDIRECT_CALL},
         std::pair{"BRANCH_RETURN", BRANCH_RETURN}}};

    auto total_branch = std::ceil(
        std::accumulate(std::begin(types), std::end(types), 0ll, [tbt = stats.total_branch_types](auto acc, auto next) { return acc + tbt[next.second]; }));
    auto total_mispredictions = std::ceil(
        std::accumulate(std::begin(types), std::end(types), 0ll, [btm = stats.branch_type_misses](auto acc, auto next) { return acc + btm[next.second]; }));

    stream << std::endl;
    stream << stats.name << " cumulative IPC: " << std::ceil(stats.instrs()) / std::ceil(stats.cycles()) << " instructions: " << stats.instrs()
           << " cycles: " << stats.cycles() << std::endl;
    stream << stats.name << " Branch Prediction Accuracy: " << (100.0 * std::ceil(total_branch - total_mispredictions)) / total_branch
           << "% MPKI: " << (1000.0 * total_mispredictions) / std::ceil(stats.instrs());
    stream << " Average ROB Occupancy at Mispredict: " << std::ceil(stats.total_rob_occupancy_at_branch_mispredict) / total_mispredictions << std::endl;

    std::vector<double> mpkis;
    std::transform(std::begin(stats.branch_type_misses), std::end(stats.branch_type_misses), std::back_inserter(mpkis),
                   [instrs = stats.instrs()](auto x) { return 1000.0 * std::ceil(x) / std::ceil(instrs); });

    stream << "Branch type MPKI" << std::endl;
    for (auto [str, idx] : types)
        stream << str << ": " << mpkis[idx] << std::endl;
    stream << std::endl;
}

void champsim::plain_printer::print(CACHE::stats_type stats) {
    constexpr std::array<std::pair<std::string_view, std::size_t>, 5> types{
        {std::pair{"LOAD", LOAD}, std::pair{"RFO", RFO}, std::pair{"PREFETCH", PREFETCH}, std::pair{"WRITE", WRITE}, std::pair{"TRANSLATION", TRANSLATION}}};

    if (stats.name == "LLC") {
        for (std::size_t cpu = 0; cpu < NUM_CPUS; ++cpu) {
            uint64_t TOTAL_HIT = 0, TOTAL_MISS = 0;
            for (const auto& type : types) {
                TOTAL_HIT += stats.hits.at(type.second).at(cpu);
                TOTAL_MISS += stats.misses.at(type.second).at(cpu);
            }

            stream << stats.name << " TOTAL       ";
            stream << "ACCESS: " << std::setw(10) << TOTAL_HIT + TOTAL_MISS << "  ";
            stream << "HIT: " << std::setw(10) << TOTAL_HIT << "  ";
            stream << "MISS: " << std::setw(10) << TOTAL_MISS << std::endl;

            for (const auto& type : types) {
                std::ostringstream name;
                name << std::left << std::setw(12) << type.first;
                stream << stats.name << " " << name.str() << " ";
                stream << "ACCESS: " << std::setw(10) << stats.hits[type.second][cpu] + stats.misses[type.second][cpu] << "  ";
                stream << "HIT: " << std::setw(10) << stats.hits[type.second][cpu] << "  ";
                stream << "MISS: " << std::setw(10) << stats.misses[type.second][cpu] << std::endl;
            }

            stream << stats.name << " ";
            stream << "PREFETCH REQUESTED: " << std::setw(10) << stats.pf_requested << " ";
            stream << "PREFETCH ISSUED: " << std::setw(10) << stats.pf_issued << " ";
            stream << "PREFETCH FILLED: " << std::setw(10) << stats.pf_fill << " ";
            stream << "PREFETCH USEFUL: " << std::setw(10) << stats.pf_useful << " ";
            stream << "PREFETCH USELESS: " << std::setw(10) << stats.pf_useless << " ";
            stream << "PREFETCH LATE: " << std::setw(10) << stats.pf_late << " ";
            
            stream << stats.name << " ";
            stream << "MSHR_FULL: " << std::setw(10) << stats.mshr_full << " ";
            stream << "USEFUL_L2_L1: " << std::setw(10) << stats.pf_useful_at_l2_from_l1 << " ";
            stream << "USELESS_L2_L1: " << std::setw(10) << stats.pf_useless_at_l2_from_l1 << " ";
            stream << "TO_L1: " << std::setw(10) << stats.l2_pf_to_l1 << " ";
            stream << std::endl;

            // stream << stats.name << " IPCP STREAM pref_filled: " << stats.pref_filled[1] << " "
            //        << "IPCP STREAM pref_useful: " << stats.pref_useful[1] << " "
            //        << "IPCP STREAM pref_late: " << stats.pref_late[1] << "; ";
            // stream << "IPCP CS pref_filled: " << stats.pref_filled[2] << " "
            //        << "IPCP CS pref_useful: " << stats.pref_useful[2] << " "
            //        << "IPCP CS pref_late: " << stats.pref_late[2] << "; ";
            // stream << "IPCP CPLX pref_filled: " << stats.pref_filled[3] << " "
            //        << "IPCP CPLX pref_useful: " << stats.pref_useful[3] << " "
            //        << "IPCP CPLX pref_late: " << stats.pref_late[3] << " ";
            // stream << "IPCP NL pref_filled: " << stats.pref_filled[4] << " "
            //        << "IPCP NL pref_useful: " << stats.pref_useful[4] << "; "
            //        << "IPCP NL pref_late: " << stats.pref_late[4] << std::endl;

            stream
                << stats.name << " AVERAGE MISS LATENCY: " << std::ceil(stats.total_miss_latency) / std::ceil(TOTAL_MISS) << " cycles" << std::endl;
            // stream << " AVERAGE MISS LATENCY: " << (stats.total_miss_latency)/TOTAL_MISS << " cycles " << stats.total_miss_latency << "/" << TOTAL_MISS<< std::endl;
        }
    } else {
        uint64_t TOTAL_HIT = 0, TOTAL_MISS = 0;

        for (const auto& type : types) {
            for (std::size_t cpu = 0; cpu < NUM_CPUS; ++cpu) {
                TOTAL_HIT += stats.hits.at(type.second).at(cpu);
                TOTAL_MISS += stats.misses.at(type.second).at(cpu);
            }
        }
        stream << stats.name << " TOTAL       ";
        stream << "ACCESS: " << std::setw(10) << TOTAL_HIT + TOTAL_MISS << "  ";
        stream << "HIT: " << std::setw(10) << TOTAL_HIT << "  ";
        stream << "MISS: " << std::setw(10) << TOTAL_MISS << std::endl;

        for (const auto& type : types) {
            uint64_t ACCESS = 0, HIT = 0, MISS = 0;
            std::ostringstream name;
            name << std::left << std::setw(12) << type.first;
            stream << stats.name << " " << name.str() << " ";
            for (std::size_t cpu = 0; cpu < NUM_CPUS; ++cpu) {
                ACCESS += (stats.hits[type.second][cpu] + stats.misses[type.second][cpu]);
            }
            stream << "ACCESS: " << std::setw(10) << ACCESS << "  ";
            for (std::size_t cpu = 0; cpu < NUM_CPUS; ++cpu) {
                HIT += stats.hits[type.second][cpu];
            }
            stream << "HIT: " << std::setw(10) << ACCESS << "  ";
            for (std::size_t cpu = 0; cpu < NUM_CPUS; ++cpu) {
                MISS += stats.misses[type.second][cpu];
            }
            stream << "MISS: " << std::setw(10) << ACCESS << "  ";
        }

        stream << stats.name << " ";
        stream << "PREFETCH REQUESTED: " << std::setw(10) << stats.pf_requested << " ";
        stream << "PREFETCH ISSUED: " << std::setw(10) << stats.pf_issued << " ";
        stream << "PREFETCH FILLED: " << std::setw(10) << stats.pf_fill << " ";
        stream << "PREFETCH USEFUL: " << std::setw(10) << stats.pf_useful << " ";
        stream << "PREFETCH USELESS: " << std::setw(10) << stats.pf_useless << " ";
        stream << "PREFETCH LATE: " << std::setw(10) << stats.pf_late << " ";

        stream << stats.name << " ";
        stream << "MSHR_FULL: " << std::setw(10) << stats.mshr_full << " ";
        stream << "USEFUL_L2_L1: " << std::setw(10) << stats.pf_useful_at_l2_from_l1 << " ";
        stream << "USELESS_L2_L1: " << std::setw(10) << stats.pf_useless_at_l2_from_l1 << " ";
        stream << "TO_L1: " << std::setw(10) << stats.l2_pf_to_l1 << " ";
        stream << std::endl;

        stream
            << stats.name << " AVERAGE MISS LATENCY: " << std::ceil(stats.total_miss_latency) / std::ceil(TOTAL_MISS) << " cycles" << std::endl;
    }
}

void champsim::plain_printer::print(CACHE::NonTranslatingQueues::stats_type stats) {
    stream << "Queue Stats ";
    stream << " PQ_ACCESS: " << std::setw(10) << stats.PQ_ACCESS;
    stream << " PQ_FULL: " << std::setw(10) << stats.PQ_FULL;
    stream << " PQ_TO_CACHE: " << std::setw(10) << stats.PQ_TO_CACHE;
    stream << " PQ_MERGED: " << std::setw(10) << stats.PQ_MERGED << std::endl;
}

void champsim::plain_printer::print(DRAM_CHANNEL::stats_type stats) {
    stream << stats.name << std::endl;
    stream << " RQ ROW_BUFFER_HIT: " << std::setw(10) << stats.RQ_ROW_BUFFER_HIT << std::endl;
    stream << "  ROW_BUFFER_MISS: " << std::setw(10) << stats.RQ_ROW_BUFFER_MISS << std::endl;
    stream << " AVG DBUS CONGESTED CYCLE: ";
    if (stats.dbus_count_congested > 0)
        stream << std::setw(10) << std::ceil(stats.dbus_cycle_congested) / std::ceil(stats.dbus_count_congested);
    else
        stream << "-";
    stream << std::endl;
    stream << " WQ ROW_BUFFER_HIT: " << std::setw(10) << stats.WQ_ROW_BUFFER_HIT << std::endl;
    stream << "  ROW_BUFFER_MISS: " << std::setw(10) << stats.WQ_ROW_BUFFER_MISS;
    stream << "  FULL: " << std::setw(10) << stats.WQ_FULL << std::endl;
    stream << std::endl;
}

void champsim::plain_printer::print(champsim::phase_stats& stats) {
    stream << "=== " << stats.name << " ===" << std::endl;

    int i = 0;
    for (auto tn : stats.trace_names)
        stream << "CPU " << i++ << " runs " << tn << std::endl;

    if (NUM_CPUS > 1) {
        stream << std::endl;
        stream << "Total Simulation Statistics (not including warmup)" << std::endl;

        std::cout << stats.sim_cpu_stats.size() << std::endl;
        for (const auto& stat : stats.sim_cpu_stats)
            print(stat);

        std::cout << stats.sim_cache_stats.size() << std::endl;
        for (const auto& stat : stats.sim_cache_stats)
            print(stat);

        std::cout << stats.sim_cache_queue_stats.size() << std::endl;
        for (const auto& stat : stats.sim_cache_queue_stats)
            print(stat);
    }

    stream << std::endl;
    stream << "Region of Interest Statistics" << std::endl;

    for (const auto& stat : stats.roi_cpu_stats)
        print(stat);

    auto it_cache_stat = stats.roi_cache_stats.cbegin();
    auto it_cache_queue_stat = stats.roi_cache_queue_stats.cbegin();
    for (; it_cache_stat != stats.roi_cache_stats.cend();) {
        print(*it_cache_stat);
        print(*it_cache_queue_stat);
        it_cache_stat++;
        it_cache_queue_stat++;
    }

    stream << std::endl;
    stream << "DRAM Statistics" << std::endl;
    for (const auto& stat : stats.roi_dram_stats)
        print(stat);
}

void champsim::plain_printer::print(std::vector<phase_stats>& stats) {
    for (auto p : stats)
        print(p);
}