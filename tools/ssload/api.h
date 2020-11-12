/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef API_H_
#define API_H_

#define API_CMD_FILE    (0xFB)
#define API_CMD_SIZE    (0xFC)
#define API_CMD_LOG     (0xFD)
#define API_CMD_QUIT    (0xFE)
#define API_CMD_INVALID (0xFF)

#define API_RET_ERROR   (0xFF)

#define API_RETRIES_COUNT       (1000)
#define API_USLEEP              (16666)

bool api_variable_read(const struct device_driver *, void *, uint32_t);
bool api_byte_read(const struct device_driver *, uint8_t *);
bool api_long_read(const struct device_driver *, uint32_t *);

#endif /* !API_H_ */
