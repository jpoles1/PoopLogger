#include "SendWindow.h"
#include "bluetooth.h"
static Window *send_window;
static TextLayer *s_label_layer;
static BitmapLayer *s_icon_layer;
static GBitmap *s_icon_bitmap, *s_cross_bitmap, *s_tick_bitmap;
static ActionBarLayer *s_action_bar_layer;
static int bristol;
static void send_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "TEST");
  send_stool(bristol+1);
}
static void send_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true);
}
static void send_click(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, send_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, send_down_click_handler);
}
static void send_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CONFIRM);

  const GEdgeInsets icon_insets = {.top = 7, .right = 40, .bottom = 56, .left = 5};
  s_icon_layer = bitmap_layer_create(grect_inset(bounds, icon_insets));
  bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

  const GEdgeInsets label_insets = {.top = 105, .right = ACTION_BAR_WIDTH+20, .left = ACTION_BAR_WIDTH / 2};
  s_label_layer = text_layer_create(grect_inset(bounds, label_insets));
  text_layer_set_text(s_label_layer, "Send your Poop Log?");
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  s_tick_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TICK);
  s_cross_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CROSS);

  s_action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_tick_bitmap);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_DOWN, s_cross_bitmap);
  action_bar_layer_add_to_window(s_action_bar_layer, window);
}
static void send_window_unload(Window *window){
  text_layer_destroy(s_label_layer);
    action_bar_layer_destroy(s_action_bar_layer);
    bitmap_layer_destroy(s_icon_layer);

    gbitmap_destroy(s_icon_bitmap);
    gbitmap_destroy(s_tick_bitmap);
    gbitmap_destroy(s_cross_bitmap);

    window_destroy(window);
    //s_main_window = NULL;
}
void pushSend(int val){
  bristol = val;
  send_window = window_create();
  window_set_window_handlers(send_window, (WindowHandlers) {
    .load = send_window_load,
    .unload = send_window_unload
  });
  window_stack_push(send_window, true);
  window_set_click_config_provider(send_window, send_click);
}
