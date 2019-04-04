# ESP8266-PubSubClient-Secure

On the Internet, there are many examples of implementing secure connections for MQTT, but they are all outdated.

I took the liberty of writing a few examples for modern libraries.

- **[ESP8266 Arduino core](https://github.com/esp8266/Arduino "ESP8266 Arduino core")** *ver* 2.5.0
- **[PubSubClient](https://github.com/knolleary/pubsubclient "PubSubClient")** *ver* 2.7
- **[ArduinoJson](https://github.com/bblanchon/ArduinoJson "ArduinoJson")** *ver* 6.10.0

## Generate Certificates

I suggest to generate certificates by myself using the [openssl](https://www.openssl.org/) utility. You can also use other certificates.

First of all, we need to generate a self-signed CA certificate.
##### Generate Key for CA:
```bash
openssl genrsa -out rootCA.key 2048
```
##### Generate self-signed CA Certificate:
```bash
openssl req -x509 -new -nodes -key rootCA.key -sha256 -days 1024 -out rootCA.pem
```

Now we generate a certificate for the client, i.e. of our device.

> **Note**: for each device you need to generate a separate key-certificate pair and sign them with a CA certificate

##### Generate private key for the client:
```bash
openssl genrsa -out client.key 2048
```
##### Request for Certificate Signing Request (CSR) to generate client certificate:
```bash
openssl req -new -key client.key -out client.csr
```
##### Generate Client Certificate:
```bash
openssl x509 -req -in client.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out client.pem -days 500 -sha256
```




