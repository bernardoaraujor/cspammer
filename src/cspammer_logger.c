#include "cspammer_logger.h"

#define CSPAMMER_LOGGER_ID "cspammer"

logger_id_t cspammer_logger_id;

void logger_init_cspammer(logger_level_t level) {
    cspammer_logger_id = logger_helper_enable(CSPAMMER_LOGGER_ID, level, true);
    log_info(cspammer_logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__, CSPAMMER_LOGGER_ID);
}

void logger_destroy_cspammer() {
    log_info(cspammer_logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__, CSPAMMER_LOGGER_ID);
    logger_helper_release(cspammer_logger_id);
}
