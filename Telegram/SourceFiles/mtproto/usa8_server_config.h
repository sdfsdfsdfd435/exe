/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <string>

namespace MTP::Usa8 {

// usa8: the single MTProto server every DC (prod and test) is routed to.
//
// Read once from "server.json" next to the executable:
//   {
//     "ip": "141.98.7.179",
//     "port": 42000,
//     "public_key_file": "pubkey.asc",  // optional, relative to the exe
//     "public_key": "-----BEGIN RSA PUBLIC KEY-----\n..." // optional
//   }
// Missing file or fields fall back to the compile-time defaults
// (USA8_SERVER_IP / USA8_SERVER_PORT, see cmake/td_mtproto.cmake).
struct ServerConfig {
	std::string ip;
	int port = 0;
	std::string publicKey; // PEM, "-----BEGIN RSA PUBLIC KEY-----".
};

[[nodiscard]] const ServerConfig &Server();

} // namespace MTP::Usa8
