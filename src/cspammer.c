#include <stdio.h>

#include <inttypes.h>

#include "utils/input_validators.h"
#include "cspammer.h"
#include "cspammer_logger.h"

cspammer_ctx_t *cspammer_create(char const *const node_url, uint16_t port_number, char const *const ca_pem,
                            uint32_t node_depth, uint8_t node_mwm, uint8_t node_sec, char const *const seed) {
    cspammer_ctx_t *ctx = NULL;

    // check parameters
    if (!is_seed((tryte_t *)seed)) {
        printf("Err: Invalid seed! \n");
        return NULL;
    }

    if ((ctx = (cspammer_ctx_t *)malloc(sizeof(cspammer_ctx_t))) != NULL) {
        if ((ctx->iota_client = iota_client_core_init(node_url, port_number, ca_pem)) == NULL) {
            free(ctx);
            log_error(cspammer_logger_id, "[%s:%d] Init iota client failed.\n", __func__, __LINE__);

            printf("Err: \n");
            return NULL;
        }
        ctx->depth = node_depth;
        ctx->mwm = node_mwm;
        ctx->security = node_sec;
        ctx->unused_idx = 0;
        if (flex_trits_from_trytes(ctx->seed, NUM_TRITS_HASH, seed, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
            log_error(cspammer_logger_id, "[%s:%d] Converting seed to flex_trit failed.\n", __func__, __LINE__);
            cspammer_destroy(&ctx);
            return NULL;
        }

        logger_helper_init(LOGGER_DEBUG);
        logger_init_client_core(LOGGER_DEBUG);
        logger_init_client_extended(LOGGER_DEBUG);
        logger_init_json_serializer(LOGGER_ERR);
        logger_init_cspammer(LOGGER_DEBUG);

        char *int2str = malloc(8);
        log_debug(cspammer_logger_id, "[%s:%d] cspammer initialized.\n", __func__, __LINE__);
        snprintf(int2str, 3, "%d", node_depth);
        log_debug(cspammer_logger_id, "[%s:%d] cspammer depth: %s\n", __func__, __LINE__, int2str);
        snprintf(int2str, 3, "%d", node_mwm);
        log_debug(cspammer_logger_id, "[%s:%d] cspammer mwm: %s\n", __func__, __LINE__, int2str);
        snprintf(int2str, 8, "%d", port_number);
        log_debug(cspammer_logger_id, "[%s:%d] cspammer node url: %s:%s\n", __func__, __LINE__, node_url, int2str);

        free(int2str);

        return ctx;
    }

    return NULL;
}

void cspammer_destroy(cspammer_ctx_t **cspammer) {
    if (cspammer && *cspammer) {
        if ((*cspammer)->iota_client) {
            iota_client_core_destroy(&(*cspammer)->iota_client);
#ifdef IOTA_CLIENT_DEBUG
            logger_destroy_client_core();
            logger_destroy_client_extended();
            logger_destroy_json_serializer();
            logger_destroy_cspammer();
            logger_helper_destroy();
#endif
        }
        free(*cspammer);
        *cspammer = NULL;
    }
}

cspammer_err_t cspammer_send(cspammer_ctx_t const *const ctx, char const *const receiver, uint64_t balance,
                         char const *const msg, char *const bundle_hash) {
    retcode_t client_ret = RC_ERROR;
    cspammer_err_t ret = CSPAMMER_ERR_UNKNOWN;
    // check parameters
    if (!is_address((tryte_t *)receiver)) {
        log_error(cspammer_logger_id, "[%s:%d] Invalid receiver address.\n", __func__, __LINE__);
        return CSPAMMER_ERR_PARARMS;
    }

    bundle_transactions_t *bundle = NULL;
    bundle_transactions_new(&bundle);
    transfer_array_t *transfers = transfer_array_new();

    /* transfer setup */
    transfer_t tf = {};

    // receiver
    if (flex_trits_from_trytes(tf.address, NUM_TRITS_ADDRESS, (tryte_t const *)receiver, NUM_TRYTES_ADDRESS,
                               NUM_TRYTES_ADDRESS) == 0) {
        log_error(cspammer_logger_id, "[%s:%d] Address flex_trits convertion failed.\n", __func__, __LINE__);
        ret = CSPAMMER_ERR_FLEX_TRITS;
        goto done;
    }

    // tag
    if (flex_trits_from_trytes(tf.tag, NUM_TRITS_TAG, (tryte_t const *)DEFAULT_TAG, NUM_TRYTES_TAG, NUM_TRYTES_TAG) ==
        0) {
        log_error(cspammer_logger_id, "[%s:%d] Tag flex_trits convertion failed.\n", __func__, __LINE__);
        ret = CSPAMMER_ERR_FLEX_TRITS;
        goto done;
    }

    // value
    tf.value = balance;

    // message (optional)
    transfer_message_set_string(&tf, msg);

    transfer_array_add(transfers, &tf);

    client_ret = iota_client_send_transfer(ctx->iota_client, ctx->seed, ctx->security, ctx->depth, ctx->mwm, false,
                                           transfers, NULL, NULL, NULL, bundle);

    log_info(cspammer_logger_id, "[%s:%d] Send transaction: %s.\n", __func__, __LINE__, error_2_string(client_ret));
    if (client_ret == RC_OK) {
        flex_trit_t const *hash = bundle_transactions_bundle_hash(bundle);
        flex_trits_to_trytes((tryte_t *)bundle_hash, NUM_TRYTES_BUNDLE, hash, NUM_TRITS_BUNDLE, NUM_TRITS_BUNDLE);

        ret = CSPAMMER_OK;
    } else {
        log_error(cspammer_logger_id, "[%s:%d] Send transaction failed.\n", __func__, __LINE__);
        ret = CSPAMMER_ERR_CLIENT_ERR;
    }

    done:
    bundle_transactions_free(&bundle);
    transfer_message_free(&tf);
    transfer_array_free(transfers);
    return ret;
}