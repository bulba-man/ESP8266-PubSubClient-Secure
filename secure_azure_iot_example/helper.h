#ifndef helper_h
#define helper_h

const char SSL_ERRORS[80][80] = {
  "Everything is fine",
  "Redo handshake before other things",
  "Handshake loop is complete",
  "An error occurred that cannot be further defined",
  "An error occurred while reading",
  "An error occurred in the provider. No further information is available",
  "A required library is missing",
  "A required library has no entry point?",
  "Initialization (of whatever was being initialized, library) failed",
  "There is no memory left for the application to use",
  "Can't locate your certificate.",
  "Your certificate isn't in a format readable by the provider",
  "You do not have permission to access the specified certificate",
  "The SSL package isn't there (SChannel specific)",
  "Can't work to the cipher strength required",
  "The context has expired or isn't properly initialized",
  "The buffer read isn't a valid SSL packet",
  "The buffer read isn't a valid socks 5 packet",
  "Your SSL packet has been modified illegally",
  "Your SSL packet is out of sequence",
  "The data received is not a complete packet",
  "The server response to socks hello is bad",
  "The server response to socks connect request is bad",
  "We do not support the given address type",
  "Send the given buffer, and terminate the communication",
  "Do socks 5 server side redirection before completing handshake",
  "Unable to open the specified keystore",
  "Unable to find the specified identity cert",
  "The socket given to a function is not of the right type",
  "The socks 5 handshake broke down in an unspecified manner",
  "The buffer supplied is not big enough for all the data",
  "The SDK context supplied is not valid for the function called",
  "The clients socks 5 hello is bad",
  "The clients connect request is bad",
  "The socks 5 command requested is not supported",
  "The socks 5 server refuses to redirect to the required destination",
  "The destination network requested is inaccessible",
  "The destination host requested is unreachable",
  "Connection to the destination host requested is refused",
  "The TTL on the packet sent the destination host requested expired",
  "The hostname could not be resolved",
  "A socket could not be created",
  "Connection to the host is refused",
  "A close notify alert was received",
  "An unexpected message alert was received",
  "A bad mac alert was received",
  "A decompression failure alert was received",
  "A handshake failure alert was received",
  "A no certificate alert was received",
  "A bad certificate alert was received",
  "An unsupported certificate alert was received",
  "A certificate revoked alert was received",
  "A certificate expired alert was received",
  "A certificate unknown (untrusted) alert was received",
  "An illegal parameter alert was received",
  "An unknown alert was received (probably TLS alert)",
  "Unable to set the CA certs verify path",
  "Unable to set identity certificate",
  "Unable to set private key",
  "The common name on the ID certificate is not what was expected",
  "a zero depth self signed cert was received",
  "a root cert to match the identity received could not be found locally",
  "a root cert to match the identity received could not be found at all",
  "a self signed cert was in the chain received",
  "unable to verify the signature on the leaf cert",
  "unable to decode the issuers public key",
  "unable to verify the signature on a cert",
  "the before field in the cert is corrupt",
  "the certificate is not yet valid",
  "the expiry field in the cert is corrupt",
  "the certificate has expired",
  "A method called is unimplemented",
  "The provider could not load any of the root certs in the keystore",
  "The provider could not load some of the root certs in the keystore",
  "Client authentication failed",
  "The connection timed-out",
  "A server certificate was revoked",
  "No CRL could not be retrieved for one of the certificates",
  "Revocation support is not available"
};

//const char MQTT_ERRORS[7][50][100] = {
//  {"Connection Accepted", "Connection Accepted"},
//  {"Connection Refused, unacceptable protocol version", "The Server does not support the level of the MQTT protocol requested by the Client"},
//  {"Connection Refused, identifier rejected", "The Client identifier is correct UTF-8 but not allowed by the Server"},
//  {"Connection Refused, Server unavailable", "The Network Connection has been made but the MQTT service is unavailable"},
//  {"Connection Refused, bad user name or password", "The data in the user name or password is malformed"},
//  {"Connection Refused, not authorized", "The Client is not authorized to connect"}
//};

const char MQTT_ERRORS[7][150] = {
  {"Connection Accepted"},
  {"Connection Refused, unacceptable protocol version. The Server does not support the level of the MQTT protocol requested by the Client"},
  {"Connection Refused, identifier rejected. The Client identifier is correct UTF-8 but not allowed by the Server"},
  {"Connection Refused, Server unavailable. The Network Connection has been made but the MQTT service is unavailable"},
  {"Connection Refused, bad user name or password. The data in the user name or password is malformed"},
  {"Connection Refused, not authorized. The Client is not authorized to connect"}
};

const char PUBSUB_ERRORS[6][30] = {
  "",
  "MQTT_DISCONNECTED",
  "MQTT_CONNECT_FAILED",
  "MQTT_CONNECTION_LOST",
  "MQTT_CONNECTION_TIMEOUT"
};


char* pub_sub_error (int error_code);

#endif
