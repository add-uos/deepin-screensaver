// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CUSTOMSCREENSAVER_LOGGER_H
#define CUSTOMSCREENSAVER_LOGGER_H

#include <QObject>
#include <dtkcore_global.h>

#ifdef DTKCORE_CLASS_DConfigFile
DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE
#endif

// 动态日志规则管理器（参考 deepin-reader 的 MLogger 实现）：
// 启动时读取 DConfig 的 log_rules（appId=org.deepin.screensaver，
// resource=org.deepin.screensaver，与宿主 deepin-screensaver 共用一个规则键，
// 规则值内含本进程 org.deepin.customscreensaver* 分类），并监听其变更，
// 实现运行中动态调整日志级别（deepin-debug-config 调试模式）。
class MLogger : public QObject
{
    Q_OBJECT
public:
    explicit MLogger(QObject *parent = nullptr);
    ~MLogger() override;

    inline QString rules() const { return m_rules; }
    void setRules(const QString &rules);

private:
    void appendRules(const QString &rules);

private:
    QString m_rules;
#ifdef DTKCORE_CLASS_DConfigFile
    Dtk::Core::DConfig *m_config;
#endif
};

#endif   // CUSTOMSCREENSAVER_LOGGER_H
