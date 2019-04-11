#include "helper.h"
#include <stdio.h>

char* pub_sub_error (int error_code) {
  if (error_code >= 0) {
    return (char*)MQTT_ERRORS[error_code];
  } else {
    int c = 0 - error_code;
    return (char*)PUBSUB_ERRORS[c];
  }
}
