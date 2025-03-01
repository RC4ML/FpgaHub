/*   SPDX-License-Identifier: BSD-3-Clause
 *   Copyright (C) 2016 Intel Corporation.
 *   All rights reserved.
 */

#include <QDMAController.hpp>

#include <algorithm>
#include <thread>
#include <tuple>
#include <vector>

#include "threadPool.h"

#include "spdk/stdinc.h"

#include "spdk/env.h"
#include "spdk/log.h"
#include "spdk/nvme.h"
#include "spdk/nvme_zns.h"
#include "spdk/string.h"
#include "spdk/vmd.h"

struct ctrlr_entry {
    struct spdk_nvme_ctrlr* ctrlr;
    char name[1024];
};

struct ns_entry {
    int32_t id;
    struct spdk_nvme_ctrlr* ctrlr;
    struct spdk_nvme_ns* ns;
    struct spdk_nvme_qpair* qpair;
};

static const int32_t max_dev_num = 12;

static std::vector<ctrlr_entry*> g_controllers;
static std::vector<ns_entry*> g_namespaces;
static struct spdk_nvme_transport_id g_trid = {};

static int64_t total_pending = 0;
static std::vector<int64_t> pending_io_per_dev;

static const int64_t lba_size = 512;
static const int64_t embed_entry_width = 4096;
static const int64_t embed_entry_lba = embed_entry_width / lba_size;

// std::chrono::high_resolution_clock::time_point global_start_time;
// std::chrono::high_resolution_clock::duration global_duration;

// <dev_id, lba_addr>
static inline std::pair<int64_t, int64_t> getEmbedAddr(int32_t embed_id) {
    int64_t dev_id = embed_id % g_namespaces.size();
    int64_t lba_addr = embed_id / g_namespaces.size() * embed_entry_lba;

    return std::make_pair(dev_id, lba_addr);
}

static void
register_ns(struct spdk_nvme_ctrlr* ctrlr, struct spdk_nvme_ns* ns) {
    struct ns_entry* entry;

    if (!spdk_nvme_ns_is_active(ns)) {
        return;
    }

    entry = (struct ns_entry*)malloc(sizeof(struct ns_entry));
    if (entry == NULL) {
        perror("ns_entry malloc");
        exit(1);
    }

    entry->ctrlr = ctrlr;
    entry->ns = ns;
    entry->id = g_namespaces.size();
    g_namespaces.push_back(entry);
    pending_io_per_dev.push_back(0);

    printf("  Namespace ID: %d size: %juGB\n", spdk_nvme_ns_get_id(ns),
           spdk_nvme_ns_get_size(ns) / 1000000000);
}

static void
read_complete(void* arg, const struct spdk_nvme_cpl* completion) {
    struct ns_entry* ns_entry = (struct ns_entry*)arg;

    /* See if an error occurred. If so, display information
     * about it, and set completion value so that I/O
     * caller is aware that an error occurred.
     */
    if (spdk_nvme_cpl_is_error(completion)) {
        spdk_nvme_qpair_print_completion(ns_entry->qpair, (struct spdk_nvme_cpl*)completion);
        fprintf(stderr, "I/O error status: %s\n", spdk_nvme_cpl_get_status_string(&completion->status));
        fprintf(stderr, "Read I/O failed, aborting run\n");
        exit(1);
    }

    --total_pending;
    --pending_io_per_dev[ns_entry->id];
}

static void
write_complete(void* arg, const struct spdk_nvme_cpl* completion) {
    struct ns_entry* ns_entry = (struct ns_entry*)arg;

    /* See if an error occurred. If so, display information
     * about it, and set completion value so that I/O
     * caller is aware that an error occurred.
     */
    if (spdk_nvme_cpl_is_error(completion)) {
        spdk_nvme_qpair_print_completion(ns_entry->qpair, (struct spdk_nvme_cpl*)completion);
        fprintf(stderr, "I/O error status: %s\n", spdk_nvme_cpl_get_status_string(&completion->status));
        fprintf(stderr, "Write I/O failed, aborting run\n");
        exit(1);
    }

    --total_pending;
    --pending_io_per_dev[ns_entry->id];
}

const bool is_write = true;

std::vector<std::future<int>> wait_flag;

static void task_submit(int64_t embed_num, int32_t* embed_id, uintptr_t* dev_addr) {
    auto time_start = std::chrono::high_resolution_clock::now();

    for (int64_t i = 0; i < embed_num; i++) {
        auto [dev_id, lba_addr] = getEmbedAddr(embed_id[i]);
        struct ns_entry* ns_entry = g_namespaces[dev_id];

        ++total_pending;
        ++pending_io_per_dev[dev_id];

        if (is_write == true) {
            auto rc = spdk_nvme_ns_cmd_write(ns_entry->ns, ns_entry->qpair, (void*)dev_addr[i],
                                             lba_addr,        /* LBA start */
                                             embed_entry_lba, /* number of LBAs */
                                             write_complete, (void*)ns_entry, 0);

            if (rc != 0) {
                fprintf(stderr, "Starting write I/O failed at index %ld\n", i);
                exit(1);
            }
        } else {
            auto rc = spdk_nvme_ns_cmd_read(ns_entry->ns, ns_entry->qpair, (void*)dev_addr[i],
                                            lba_addr,        /* LBA start */
                                            embed_entry_lba, /* number of LBAs */
                                            read_complete, (void*)ns_entry, 0);

            if (rc != 0) {
                fprintf(stderr, "Starting read I/O failed at index %ld\n", i);
                exit(1);
            }
        }
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(time_end - time_start);
    printf("Submit CMD Time: %f\n", time_span.count());

    time_start = std::chrono::high_resolution_clock::now();

    while (total_pending > 0) {
        for (int64_t i = 0; i < (int64_t)g_namespaces.size(); i++) {
            struct ns_entry* ns_entry = g_namespaces[i];
            spdk_nvme_qpair_process_completions(ns_entry->qpair, 0);
        }
    }

    time_end = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(time_end - time_start);
    printf("Wait CMD Time: %f\n", time_span.count());
}

const int64_t dev_num = 10;

const int64_t embed_num = 65536 * 4 * dev_num;
const int64_t page_size = 2 * 1024 * 1024;
const int64_t buffer_page_num = embed_num * embed_entry_width / page_size + 1;

char* buffer = nullptr;

int32_t embed_id[embed_num];
uintptr_t dev_addr[embed_num];

static void run_task() {
    buffer = (char*)spdk_dma_malloc(buffer_page_num * page_size, page_size, nullptr);

    for (int64_t i = 0; i < embed_num; i++) {
        embed_id[i] = i;
        dev_addr[i] = (uintptr_t)(buffer + i * embed_entry_width);
    }

    std::random_shuffle(embed_id, embed_id + embed_num);
    std::random_shuffle(dev_addr, dev_addr + embed_num);

    g_namespaces.resize(dev_num);

    printf("Start to submit task\n");

    auto time_start = std::chrono::high_resolution_clock::now();
    // global_start_time = time_start;

    task_submit(embed_num, embed_id, dev_addr);

    auto time_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(time_end - time_start);
    printf("Time: %f\n", time_span.count());

    int64_t total_data_size = embed_num * embed_entry_width;
    double total_data_size_gb = total_data_size / 1024.0 / 1024.0 / 1024.0;
    double bandwidth = total_data_size_gb / time_span.count();

    printf("Bandwidth: %.3f GB/s\n", bandwidth);
    printf("Bandwidth per device: %.3f GB/s\n", bandwidth / g_namespaces.size());

    spdk_dma_free(buffer);
}

static void alloc_qpair() {
    for (auto ns_entry : g_namespaces) {
        /*
         * Allocate an I/O qpair that we can use to submit read/write requests
         *  to namespaces on the controller.  NVMe controllers typically support
         *  many qpairs per controller.  Any I/O qpair allocated for a controller
         *  can submit I/O to any namespace on that controller.
         *
         * The SPDK NVMe driver provides no synchronization for qpair accesses -
         *  the application must ensure only a single thread submits I/O to a
         *  qpair, and that same thread must also check for completions on that
         *  qpair.  This enables extremely efficient I/O processing by making all
         *  I/O operations completely lockless.
         */
        spdk_nvme_io_qpair_opts opts = {};
        spdk_nvme_ctrlr_get_default_io_qpair_opts(ns_entry->ctrlr, &opts, sizeof(spdk_nvme_io_qpair_opts));
        opts.io_queue_requests = 1048576;
        // printf("opts.sq.buffer_size: %ld\n", opts.sq.buffer_size);
        // printf("opts.cq.buffer_size: %ld\n", opts.cq.buffer_size);
        ns_entry->qpair = spdk_nvme_ctrlr_alloc_io_qpair(ns_entry->ctrlr, &opts, sizeof(spdk_nvme_io_qpair_opts));
        if (ns_entry->qpair == NULL) {
            printf("ERROR: spdk_nvme_ctrlr_alloc_io_qpair() failed\n");
            return;
        }
    }
}

static void release_qpair() {
    /*
     * Free the I/O qpair.  This typically is done when an application exits.
     *  But SPDK does support freeing and then reallocating qpairs during
     *  operation.  It is the responsibility of the caller to ensure all
     *  pending I/O are completed before trying to free the qpair.
     */
    for (auto ns_entry : g_namespaces) {
        spdk_nvme_ctrlr_free_io_qpair(ns_entry->qpair);
    }
}

static bool
probe_cb(void* cb_ctx, const struct spdk_nvme_transport_id* trid, struct spdk_nvme_ctrlr_opts* opts) {
    printf("Attaching to %s\n", trid->traddr);

    return true;
}

static void
attach_cb(void* cb_ctx, const struct spdk_nvme_transport_id* trid, struct spdk_nvme_ctrlr* ctrlr, const struct spdk_nvme_ctrlr_opts* opts) {
    int nsid;
    struct ctrlr_entry* entry;
    struct spdk_nvme_ns* ns;
    const struct spdk_nvme_ctrlr_data* cdata;

    entry = (struct ctrlr_entry*)malloc(sizeof(struct ctrlr_entry));
    if (entry == NULL) {
        perror("ctrlr_entry malloc");
        exit(1);
    }

    printf("Attached to %s\n", trid->traddr);

    /*
     * spdk_nvme_ctrlr is the logical abstraction in SPDK for an NVMe
     *  controller.  During initialization, the IDENTIFY data for the
     *  controller is read using an NVMe admin command, and that data
     *  can be retrieved using spdk_nvme_ctrlr_get_data() to get
     *  detailed information on the controller.  Refer to the NVMe
     *  specification for more details on IDENTIFY for NVMe controllers.
     */
    cdata = spdk_nvme_ctrlr_get_data(ctrlr);

    snprintf(entry->name, sizeof(entry->name), "%-20.20s (%-20.20s)", cdata->mn, cdata->sn);

    entry->ctrlr = ctrlr;
    g_controllers.push_back(entry);

    /*
     * Each controller has one or more namespaces.  An NVMe namespace is basically
     *  equivalent to a SCSI LUN.  The controller's IDENTIFY data tells us how
     *  many namespaces exist on the controller.  For Intel(R) P3X00 controllers,
     *  it will just be one namespace.
     *
     * Note that in NVMe, namespace IDs start at 1, not 0.
     */
    for (nsid = spdk_nvme_ctrlr_get_first_active_ns(ctrlr); nsid != 0;
         nsid = spdk_nvme_ctrlr_get_next_active_ns(ctrlr, nsid)) {
        ns = spdk_nvme_ctrlr_get_ns(ctrlr, nsid);
        if (ns == NULL) {
            continue;
        }
        register_ns(ctrlr, ns);
    }
}

static void
rc4ml_spdk_cleanup(void) {
    struct spdk_nvme_detach_ctx* detach_ctx = NULL;

    release_qpair();

    for (auto ns_entry : g_namespaces) {
        free(ns_entry);
    }
    g_namespaces.clear();

    for (auto ctrlr_entry : g_controllers) {
        spdk_nvme_detach_async(ctrlr_entry->ctrlr, &detach_ctx);
        free(ctrlr_entry);
    }
    g_controllers.clear();

    if (detach_ctx) {
        spdk_nvme_detach_poll(detach_ctx);
    }

    spdk_env_fini();
}

int rc4ml_spdk_init() {
    struct spdk_env_opts opts;

    /*
     * SPDK relies on an abstraction around the local environment
     * named env that handles memory allocation and PCI device operations.
     * This library must be initialized first.
     *
     */

    spdk_nvme_trid_populate_transport(&g_trid, SPDK_NVME_TRANSPORT_PCIE);
    snprintf(g_trid.subnqn, sizeof(g_trid.subnqn), "%s", SPDK_NVMF_DISCOVERY_NQN);

    spdk_env_opts_init(&opts);

    opts.name = "gpussd_baseline";
    if (spdk_env_init(&opts) < 0) {
        fprintf(stderr, "Unable to initialize SPDK env\n");
        return 1;
    }

    printf("Initializing NVMe Controllers\n");

    /*
     * Start the SPDK NVMe enumeration process.  probe_cb will be called
     *  for each NVMe controller found, giving our application a choice on
     *  whether to attach to each controller.  attach_cb will then be
     *  called for each controller after the SPDK NVMe driver has completed
     *  initializing the controller we chose to attach.
     */

    if (spdk_nvme_probe(&g_trid, NULL, probe_cb, attach_cb, NULL) != 0) {
        fprintf(stderr, "spdk_nvme_probe() failed\n");
        return 1;
    }

    if (g_controllers.empty()) {
        fprintf(stderr, "no NVMe controllers found\n");
        return 1;
    }

    printf("Initialization complete.\n");

    alloc_qpair();

    return 0;
}

int main(int argc, char** argv) {
    int rc;

    rc = rc4ml_spdk_init();
    if (rc != 0) {
        fprintf(stderr, "rc4ml_spdk_init() failed\n");
        rc = 1;
        goto exit;
    }

    run_task();

exit:
    fflush(stdout);
    rc4ml_spdk_cleanup();

    return rc;
}
