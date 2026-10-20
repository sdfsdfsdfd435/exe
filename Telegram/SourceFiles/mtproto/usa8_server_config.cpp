/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "mtproto/usa8_server_config.h"

#include "logs.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#ifdef Q_OS_WIN
#include <windows.h>
#endif // Q_OS_WIN

#ifndef USA8_SERVER_IP
#define USA8_SERVER_IP "192.168.1.108"
#endif // USA8_SERVER_IP

#ifndef USA8_SERVER_PORT
#define USA8_SERVER_PORT 42000
#endif // USA8_SERVER_PORT

namespace MTP::Usa8 {
namespace {

// Fingerprint 0xe57178736b089355.
constexpr auto kDefaultPublicKey = "\
-----BEGIN RSA PUBLIC KEY-----\n\
MIIBCgKCAQEAuzqyuk2UvLltDOX5uoeFIryraHZwq2POBdk+RbehkA/QLc8Kv6UI\n\
fZkO9w0aC71B5tmI3pRPLv++O2DMs7OqjcBrurMMG7h8HM1c9Yg8iVMNfbThcVvU\n\
xFseZcpAYqyoGtDelHeQQU3t936jcsUJpG9fE5dBcNduLHKOhhENGO8Mm3CnAS5r\n\
I26rYXnohcdj6piCkK49ebTBvji/uhQ7cmC+8x1yZmtegZi5/YDN6QFfhhRIccwp\n\
QQgj8ej+6SkgrtAJcmOgPU7I1yUx1KohL928y5MWb2AWvhFzcku+SUqUJoPUT6+k\n\
Qk6CgH9cx3YIeBRX0gczziKYjN0dq9ZTkQIDAQAB\n\
-----END RSA PUBLIC KEY-----";

[[nodiscard]] QString ExecutableDir() {
#ifdef Q_OS_WIN
	WCHAR path[MAX_PATH * 2] = { 0 };
	const auto length = GetModuleFileNameW(nullptr, path, MAX_PATH * 2);
	if (length > 0 && length < MAX_PATH * 2) {
		return QFileInfo(QString::fromWCharArray(path, length)).absolutePath();
	}
#endif // Q_OS_WIN
	return QCoreApplication::instance()
		? QCoreApplication::applicationDirPath()
		: QDir::currentPath();
}

[[nodiscard]] ServerConfig Load() {
	auto result = ServerConfig{
		.ip = USA8_SERVER_IP,
		.port = USA8_SERVER_PORT,
		.publicKey = kDefaultPublicKey,
	};
	const auto dir = ExecutableDir();
	auto file = QFile(dir + u"/server.json"_q);
	if (!file.open(QIODevice::ReadOnly)) {
		LOG(("usa8: no server.json in '%1', using built-in %2:%3"
			).arg(dir
			).arg(QString::fromStdString(result.ip)
			).arg(result.port));
		return result;
	}
	auto error = QJsonParseError();
	const auto document = QJsonDocument::fromJson(file.readAll(), &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		LOG(("usa8: bad server.json (%1), using built-in server"
			).arg(error.errorString()));
		return result;
	}
	const auto object = document.object();
	const auto ip = object.value(u"ip"_q).toString().trimmed();
	if (!ip.isEmpty()) {
		result.ip = ip.toStdString();
	}
	const auto port = object.value(u"port"_q).toVariant().toInt();
	if (port > 0 && port < 65536) {
		result.port = port;
	}
	const auto keyFile = object.value(u"public_key_file"_q).toString();
	const auto key = object.value(u"public_key"_q).toString();
	if (!keyFile.isEmpty()) {
		auto f = QFile(QDir(dir).absoluteFilePath(keyFile));
		if (f.open(QIODevice::ReadOnly)) {
			result.publicKey = f.readAll().trimmed().toStdString();
		} else {
			LOG(("usa8: could not read public_key_file '%1'").arg(keyFile));
		}
	} else if (!key.isEmpty()) {
		result.publicKey = key.trimmed().toStdString();
	}
	LOG(("usa8: server.json -> %1:%2"
		).arg(QString::fromStdString(result.ip)
		).arg(result.port));
	return result;
}

} // namespace

const ServerConfig &Server() {
	static const auto result = Load();
	return result;
}

} // namespace MTP::Usa8
