// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DDLOG_H
#define DDLOG_H

#include <QDebug>
#include <QLoggingCategory>

// 自定义屏保运行单元的日志分类（独立进程，由 deepin-screensaver 宿主拉起），
// 便于 DConfig 调试模式（deepin-debug-config）按应用统一过滤
inline Q_LOGGING_CATEGORY(appLog, "org.deepin.customscreensaver")

#endif   // DDLOG_H
