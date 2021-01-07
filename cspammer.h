#pragma once

#include <pthread.h>
#include <stdint.h>

#include "cclient/api/core/core_api.h"
#include "cclient/api/extended/extended_api.h"
#include "common/model/transaction.h"

#include "cspammer_logger.h"

#define IOTA_CLIENT_DEBUG 1
#define DEFAULT_SECURITY_LEVEL 2

typedef int32_t cspammer_err_t;

#define DEFAULT_TAG "CSPAMMER9999999999999999999"
#define DEFAULT_MSG "CSpammer be spammin'!"

#define CSPAMMER_OK 0
#define CSPAMMER_ERR_UNKNOWN -1
#define CSPAMMER_ERR_PARARMS 1           // invalid paramiters
#define CSPAMMER_ERR_OOM 1 << 1         // out of memory
#define CSPAMMER_ERR_CLIENT_ERR 1 << 2  // cclient error
#define CSPAMMER_ERR_FLEX_TRITS 1 << 3  // flex trits error

/**
 * @brief CSpammer context
 *
 */
typedef struct {
    uint8_t mwm;                          /*!< Minimum Weight Magnitude for doing Proof-of-Work */
    uint8_t security;                     /*!< security level of addresses, value could be 1,2,3. */
    uint32_t depth;                       /*!< number of bundles to go back to determine the transactions for approval. */
    uint64_t unused_idx;                  /*!< recent unused index of the address. */
    flex_trit_t seed[FLEX_TRIT_SIZE_243]; /*!< seed */
    iota_client_service_t *iota_client;   /*!< iota client service */
} cspammer_ctx_t;

/**
 * @brief Create a cspammer instance
 *
 * @param node_url the url of a node
 * @param port_number the port number of a node
 * @param ca_pem a root ca in pem format, pass NULL for HTTP only.
 * @param node_depth the depth of the connected node
 * @param node_mwm the MWM of the connected node
 * @param seed a seed for this cspammer
 * @return cspammer_ctx_t* return NULL on errors
 */
cspammer_ctx_t *cspammer_create(char const *const node_url, uint16_t port_number, char const *const ca_pem,
                            uint32_t node_depth, uint8_t node_mwm, uint8_t node_sec, char const *const seed);

/**
 * @brief Clean up and free a cspammer instance
 *
 * @param cspammer cspammer context
 */
void cspammer_destroy(cspammer_ctx_t **cspammer);

/**
 * @brief Send out a valued or policy transaction to the node.
 *
 * @param ctx cspammer context
 * @param receiver the receiver address
 * @param balance token value
 * @param msg message in the transaction
 * @param bundle_hash a bundle hash with char[81] is returned by client API.
 * @return cspammer_err_t
 */
cspammer_err_t cspammer_send(cspammer_ctx_t const *const ctx, char const *const receiver, uint64_t balance,
                         char const *const msg, char *const bundle_hash);