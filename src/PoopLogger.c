#include <pebble.h>
#include "bluetooth.h"
//App Config
const char *bristol_scale[7] = {
  "Separate hard lumps, like nuts (hard to pass)",
  "Sausage-shaped, but lumpy",
  "Like a sausage but with cracks on its surface",
  "Like a sausage or snake, smooth and soft",
  "Soft blobs with clear cut edges (passed easily)",
  "Fluffy pieces with ragged edges, a mushy stool",
  "Watery, no solid pieces, entirely liquid"
};
const int sidebar_width = 30;
const int sidebar_margin = 10;
int bristol = 3;
//UI Vars
static Window *main_window, *send_window;
static void pushSend();
static TextLayer *desc_layer, *rating_layer;
static Layer *sidebar_layer;
static GPath *up_arrow, *down_arrow;
static const GPathInfo up_path = {
  .num_points = 3,
  .points = (GPoint []) {{0, 15}, {15, 15}, {15/2+.5, 0}}
};
static const GPathInfo down_path = {
  .num_points = 3,
  .points = (GPoint []) {{0, 0}, {15, 0}, {15/2+.5, 15}}
};
static void verticalAlignTextLayer(TextLayer *layer) {
  GRect frame = layer_get_frame(text_layer_get_layer(layer));
  GSize content = text_layer_get_content_size(layer);
  layer_set_frame(text_layer_get_layer(layer),
         GRect(frame.origin.x, frame.origin.y + (frame.size.h - content.h) / 2,
         frame.size.w, content.h));
}
static void change_rating(){
  static char desc_buffer[60];
  strcpy(desc_buffer, bristol_scale[bristol]);
  text_layer_set_text(desc_layer, desc_buffer);
  static char rating_buffer[20];
  snprintf(rating_buffer, sizeof(rating_buffer), "%d", bristol+1);
  text_layer_set_text(rating_layer, rating_buffer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Bristol now %d : %s", bristol+1, bristol_scale[bristol]);
  verticalAlignTextLayer(desc_layer);
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(bristol<6){
    bristol = bristol+1;
    change_rating();
  }
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(bristol>0){
    bristol = bristol-1;
    change_rating();
  }
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  pushSend();
}
static void main_click(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}
static void draw_sidebar(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 5, GCornersLeft);
  graphics_context_set_fill_color(ctx, GColorWhite);
  up_arrow = gpath_create(&up_path);
  gpath_move_to(up_arrow, (GPoint) {8,10});
  gpath_draw_filled(ctx, up_arrow);
  down_arrow = gpath_create(&down_path);
  gpath_move_to(down_arrow, (GPoint) {8, bounds.size.h-25});
  gpath_draw_filled(ctx, down_arrow);
}
static void main_window_load(Window *window) {
  //Main Window
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, GColorBulgarianRose);
  GRect bounds = layer_get_bounds(window_layer);
  //Description layer
  desc_layer = text_layer_create(GRect(10, 0, bounds.size.w-sidebar_width-20, bounds.size.h));
  text_layer_set_font(desc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(desc_layer, "                                                                                                    ");
  text_layer_set_overflow_mode(desc_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(desc_layer, GTextAlignmentCenter);
  text_layer_set_background_color(desc_layer, GColorBulgarianRose);
  text_layer_set_text_color(desc_layer, GColorWhite);
  verticalAlignTextLayer(desc_layer);
  layer_add_child(window_layer, text_layer_get_layer(desc_layer));
  //Sidebar
  sidebar_layer = layer_create(GRect(bounds.size.w-sidebar_width, sidebar_margin, sidebar_width, bounds.size.h-2*sidebar_margin));
  layer_set_update_proc(sidebar_layer, draw_sidebar);
  layer_add_child(window_layer, sidebar_layer);
  //Rating layer
  rating_layer = text_layer_create(GRect(bounds.size.w-25, 75, 25, 25));
  text_layer_set_font(rating_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(rating_layer, GColorBlack);
  text_layer_set_text_color(rating_layer, GColorWhite);
  text_layer_set_text(rating_layer, "#");
  text_layer_set_overflow_mode(rating_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(rating_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(rating_layer));
  change_rating();
}
static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(desc_layer);
  text_layer_destroy(rating_layer);
  layer_destroy(sidebar_layer);
}
static TextLayer *s_body_layer, *s_title_layer, *s_label_layer;
static BitmapLayer *s_icon_layer;
static GBitmap *s_icon_bitmap, *s_cross_bitmap, *s_tick_bitmap;
static ActionBarLayer *s_action_bar_layer;
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

  const GEdgeInsets icon_insets = {.top = 7, .right = 20, .bottom = 56, .left = 14};
  s_icon_layer = bitmap_layer_create(grect_inset(bounds, icon_insets));
  bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

  const GEdgeInsets label_insets = {.top = 112, .right = ACTION_BAR_WIDTH, .left = ACTION_BAR_WIDTH / 2};
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
static void pushSend(){
  send_window = window_create();
  window_set_window_handlers(send_window, (WindowHandlers) {
    .load = send_window_load,
    .unload = send_window_unload
  });
  window_stack_push(send_window, true);
  window_set_click_config_provider(send_window, send_click);
}
static void init() {
  // Create main Window
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_click_config_provider(main_window, main_click);
  window_stack_push(main_window, true);
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(128, 64);
}

static void deinit() {
  // Destroy main Window
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
