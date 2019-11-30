/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Interface of Data Bus Log
 * Author: xujingguo
 * Create: 2019-11-25
 */

#ifndef CRYPTO_COMMU_LOG_H
#define CRYPTO_COMMU_LOG_H

#ifndef __TITLE__
#define __TITLE__ __PRETTY_FUNCTION__
#endif

#include "hilog/log.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0
#define LOG_TAG "CryptoCommu"

#define DB_LOGE(fmt, args...) HILOG_WARN(LOG_CORE,"%s %d: " fmt, __TITLE__, __LINE__, ##args)
#define DB_LOGW(fmt, args...) HILOG_WARN(LOG_CORE,"%s %d: " fmt, __TITLE__, __LINE__, ##args)
#define DB_LOGI(fmt, args...) HILOG_INFO(LOG_CORE,"%s %d: " fmt, __TITLE__, __LINE__, ##args)
#define DB_LOGD(fmt, args...) HILOG_DEBUG(LOG_CORE,"%s %d: " fmt, __TITLE__, __LINE__, ##args)
#endif // CRYPTO_COMMU_LOG_H
