/*PGR-GNU*****************************************************************

File: trsp_core.cpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/


#include "drivers/trsp/trsp_driver.h"
#include <utility>
#include <vector>
#include <cstdint>
#include <sstream>
#include "trsp/pgr_trspHandler.h"
#include "cpp_common/pgr_assert.h"
#include "cpp_common/pgr_alloc.hpp"



int trsp_node_wrapper(
        pgr_edge_t *edges,
        size_t total_edges,

        restrict_t *restricts,
        size_t restrict_count,

        int64_t start_vertex,
        int64_t end_vertex,

        bool directed,
        bool has_reverse_cost,

        path_element_tt **path,
        size_t *path_count,
        char **err_msg) {
    std::ostringstream err;
    try {
        pgassert(*path == NULL);
        pgassert(*path_count == 0);


#if 1
        // defining min and max vertex id

        int64_t v_max_id = 0;
        int64_t v_min_id = INT64_MAX;
        size_t z;
        for (z = 0; z < total_edges; z++) {
            if (edges[z].source < v_min_id)
                v_min_id = edges[z].source;

            if (edges[z].source > v_max_id)
                v_max_id = edges[z].source;

            if (edges[z].target < v_min_id)
                v_min_id = edges[z].target;

            if (edges[z].target > v_max_id)
                v_max_id = edges[z].target;
        }

        // ::::::::::::::::::::::::::::::::::::
        // :: reducing vertex id (renumbering)
        // ::::::::::::::::::::::::::::::::::::
        size_t s_count = 0;
        size_t t_count = 0;
        auto start_id = start_vertex;
        auto end_id = end_vertex;
        for (z = 0; z < total_edges; z++) {
            if (edges[z].source == start_id || edges[z].target == start_vertex)
                ++s_count;
            if (edges[z].source == end_id || edges[z].target == end_vertex)
                ++t_count;
            edges[z].source -= v_min_id;
            edges[z].target -= v_min_id;
            edges[z].cost = edges[z].cost;
        }


        if (s_count == 0) {
            err << "Start id was not found.";
            *err_msg = pgr_msg(err.str().c_str());
            return -1;
        }

        if (t_count == 0) {
            err << "Start id was not found.";
            *err_msg = pgr_msg(err.str().c_str());
            return -1;
        }

        start_id -= v_min_id;
        end_id   -= v_min_id;
#endif


        std::vector<pgrouting::trsp::PDVI> ruleTable;
        size_t MAX_RULE_LENGTH = 5;

        size_t i, j;
        ruleTable.clear();
        for (i = 0; i < restrict_count; i++) {
            std::vector<int64_t> seq;
            seq.clear();
            seq.push_back(restricts[i].target_id);
            for (j = 0; j < MAX_RULE_LENGTH && restricts[i].via[j] > -1; j++) {
                seq.push_back(restricts[i].via[j]);
            }
            ruleTable.push_back(make_pair(restricts[i].to_cost, seq));
        }

        pgrouting::trsp::Pgr_trspHandler gdef;
        int res = gdef.initializeAndProcess(
                edges, total_edges,
                ruleTable,
                start_id, end_id,
                directed, has_reverse_cost,
                path, path_count, err_msg);

#if 1
        // ::::::::::::::::::::::::::::::::
        // :: restoring original vertex id
        // ::::::::::::::::::::::::::::::::
        for (z = 0; z < *path_count; z++) {
            if (z || (*path)[z].vertex_id != -1)
                (*path)[z].vertex_id += v_min_id;
        }
#endif

        if (res < 0)
            return res;
        else
            return EXIT_SUCCESS;
    }
    catch(std::exception& e) {
        *err_msg = (char *) e.what();
        return -1;
    }
    catch(...) {
        *err_msg = (char *) "Caught unknown exception!";
        return -1;
    }
}

