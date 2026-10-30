// SPDX-FileCopyrightText: 2017-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDateTime>

#include "screensaveradaptor.h"
#include "dbusscreensaver.h"
#include "customconfig.h"
#include "commandlinehelper.h"
#include "singlecustomsetting.h"
#include "logger.h"
#include "ddlog.h"

DWIDGET_USE_NAMESPACE

#define APPLICATION_XSTRING(s) APPLICATION_STRING(s)
#define APPLICATION_STRING(s) #s

#ifdef VERSION
static QString buildVersion(const QString &fallbackVersion)
{
    QString autoVersion = APPLICATION_XSTRING(VERSION);
    if (autoVersion.isEmpty()) {
        autoVersion = fallbackVersion;
    }
    return autoVersion;
}
#else
static QString buildVersion(const QString &fallbackVersion)
{
    return fallbackVersion;
}
#endif

int main(int argc, char *argv[])
{
    // 动态日志规则（DConfig log_rules）须在任何日志打印前初始化：
    // MLogger 内部会先 qunsetenv("QT_LOGGING_RULES") 再应用 DConfig 规则
    MLogger logger;
    Q_UNUSED(logger)

    auto envType = qEnvironmentVariable("XDG_SESSION_TYPE");
    if (envType.contains("wayland")) {
        qCInfo(appLog) << QDateTime::currentDateTime().toString() << "notes:change wayland to xcb for QT_QPA_PLATFORM.";
        qputenv("QT_QPA_PLATFORM", "xcb");
    }
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    DApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    DApplication app(argc, argv);

    app.setOrganizationName("deepin");
    app.setApplicationName("deepin-screensaver");
    app.setApplicationVersion(buildVersion(APP_VERSION));

    CommandLineHelper::instance()->process(app.arguments());

    if (CommandLineHelper::instance()->isSet(QStringList({ "c", "config" }))) {
        app.setQuitOnLastWindowClosed(true);
        SingleCustomSetting singleSetting;
        if (singleSetting.startCustomConfig())
            return app.exec();
        return 0;
    }

    bool doStart = !CommandLineHelper::instance()->isSet(QStringList({ "d", "dbus" }));

    // 注册DBus服务
    if (!QDBusConnection::sessionBus().isConnected()) {
        qCWarning(appLog, "Cannot connect to the D-Bus session bus.\n"
                          "Please check your system settings and try again.\n");

        if (!doStart)
            return -1;
    }

    const QString serviceName = "com.deepin.ScreenSaver";
    const QString objDbusPath = "/com/deepin/ScreenSaver";
    // add our D-Bus interface and connect to D-Bus
    if (!QDBusConnection::sessionBus().registerService(serviceName)) {
        qCWarning(appLog) << QString("Cannot register the \"%1\" service.\n").arg(serviceName);

        if (!doStart)
            return -1;
    }

    auto server = new DBusScreenSaver();
    Q_UNUSED(new ScreenSaverAdaptor(qobject_cast<DBusScreenSaver *>(server)))

    if (!QDBusConnection::sessionBus().registerObject(objDbusPath, server)) {
        qCWarning(appLog) << QString("Cannot register to the D-Bus object: \"%1\"\n").arg(objDbusPath);

        if (!doStart)
            return -1;
    }

    if (doStart) {
        if (argc > 1)
            server->Start(CommandLineHelper::instance()->positionalArguments().value(0));
        else
            server->Start();
    }

    return app.exec();
}
