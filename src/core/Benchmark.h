/* XMRig
 * Copyright 2018-2019 MoneroOcean <https://github.com/MoneroOcean>, <support@moneroocean.stream>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <set>
#include <map>
#include "net/interfaces/IJobResultListener.h"
#include "base/crypto/Algorithm.h"
#include "rapidjson/fwd.h"

#include <memory>

namespace xmrig {

class Controller;
class Miner;
class Job;

class MoBenchmark : public IJobResultListener {

        enum BenchAlgo : int {
            RX_0,          // "rx/0"             RandomX (Monero).
            RX_ARQ,        // "rx/arq"           RandomARQ (Arqma).
            CN_HEAVY_XHV,  // "cn-heavy/xhv"     CryptoNight-Heavy (modified, Haven Protocol only).
            MAX,
            MIN = 0,
            INVALID = -1,
        };

        const Algorithm::Id ba2a[BenchAlgo::MAX] = {
            Algorithm::RX_0,
            Algorithm::RX_ARQ,
            Algorithm::CN_HEAVY_XHV,
        };

        Job* m_bench_job[BenchAlgo::MAX];
        float m_bench_algo_perf[BenchAlgo::MAX];

        Controller* m_controller;          // to get access to config and network
        bool m_isNewBenchRun;              // true if benchmark is need to be executed or was executed
        MoBenchmark::BenchAlgo m_bench_algo; // current perf algo we benchmark
        uint64_t m_hash_count;             // number of hashes calculated for current perf algo
        uint64_t m_time_start;             // time of the first resultt for current perf algo (in ms)
        uint64_t m_bench_start;            // time of measurements start for current perf algo (in ms) after all backends are started
        unsigned m_enabled_backend_count;  // number of active miner backends
        std::set<uint32_t> m_backends_started; // id of backend started for benchmark

        uint64_t get_now() const;                      // get current time in ms
        float get_algo_perf(Algorithm::Id algo) const; // get algo perf based on m_bench_algo_perf
        void start(const MoBenchmark::BenchAlgo);        // start benchmark for specified perf algo
        void finish();                                 // end of benchmarks, switch to jobs from the pool (network), fill algo_perf
        void onJobResult(const JobResult&) override;   // onJobResult is called after each computed benchmark hash
        void run_next_bench_algo(BenchAlgo);           // run next bench algo or finish benchmark for the last one

    public:
        MoBenchmark();
        virtual ~MoBenchmark();

        void set_controller(Controller* controller) { m_controller = controller; }

        void start(); // start benchmarks

        bool isNewBenchRun() const { return m_isNewBenchRun; }
        mutable std::map<Algorithm::Id, float> algo_perf;

        rapidjson::Value toJSON(rapidjson::Document &doc) const;
        void read(const rapidjson::Value &value);
};

} // namespace xmrig
