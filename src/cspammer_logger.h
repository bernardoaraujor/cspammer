#ifndef CSPAMMER_LOGGER_H
#define CSPAMMER_LOGGER_H

#include "utils/logger_helper.h"

/**
 * @brief logger ID
 *
 */
extern logger_id_t cspammer_logger_id;

/**
 * @brief init cspammer logger
 *
 * @param[in] level A level of the logger
 *
 */
void logger_init_cspammer(logger_level_t level);

/**
 * @brief cleanup Network logger
 *
 */
void logger_destroy_cspammer();

#endif  // CSPAMMER_LOGGER_H
