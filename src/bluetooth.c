#include "bluetooth.h"
#define BT_KEY 5
void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "INBOX!");
  Tuple *data = dict_find(iterator, BT_KEY);
  if (data) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Received '%s'", data->value->cstring);
  }
}
void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
  window_stack_pop_all(true);
}
void send_stool(int bristol){
  DictionaryIterator *iterator;
  app_message_outbox_begin(&iterator);
  dict_write_int(iterator, BT_KEY, &bristol, sizeof(int), true /* signed */);
  app_message_outbox_send();
}
