#include <pebble.h>
//App Config
const char *scale[7] = {
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
int bristol = 1;
//UI Vars
static Window *main_window;
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
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(bristol<6){bristol = bristol+1;}
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Bristol now %d : %s", bristol, scale[bristol]);
  static char desc_buffer[50];
  strcpy(desc_buffer, scale[bristol]);
  text_layer_set_text(desc_layer, desc_buffer);
  GSize cont_size = text_layer_get_content_size(desc_layer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", cont_size.h);
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(bristol>0){bristol = bristol-1;}
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Bristol now %d : %s", bristol, scale[bristol]);
  static char desc_buffer[50];
  strcpy(desc_buffer, scale[bristol]);
  text_layer_set_text(desc_layer, desc_buffer);
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(desc_layer, "Select pressed!");
}
static void click_config_provider(void *context) {
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
  GRect bounds = layer_get_bounds(window_layer);
  //Description layer
  desc_layer = text_layer_create(GRect(10, 60, bounds.size.w-sidebar_width-20, bounds.size.h));
  text_layer_set_font(desc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(desc_layer, "Tell me about your poop.");
  text_layer_set_overflow_mode(desc_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(desc_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(desc_layer));
  //Sidebar
  sidebar_layer = layer_create(GRect(bounds.size.w-sidebar_width, sidebar_margin, sidebar_width, bounds.size.h-2*sidebar_margin));
  layer_set_update_proc(sidebar_layer, draw_sidebar);
  layer_add_child(window_layer, sidebar_layer);
  //Rating layer
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(desc_layer);
  text_layer_destroy(rating_layer);
  layer_destroy(sidebar_layer);
}

static void init() {
  // Create main Window
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_click_config_provider(main_window, click_config_provider);
  window_stack_push(main_window, true);
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
