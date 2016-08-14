#include "simple_analog.h"

#include "pebble.h"
#include <ctype.h>

static Window *s_window;
static Layer *s_simple_bg_layer, *s_ticks_layer, *s_hands_layer, *s_date_layer, *s_bluetooth_layer;

static GPath *s_bt_icon_gpath;

static BitmapLayer *s_ticks_layers[12];
static GBitmap *s_bt_icon_bitmap, *s_tick_sprite_bitmap, *s_tick_bitmap[12];

static TextLayer *s_day_label, *s_num_label;
static char s_num_buffer[4], s_day_buffer[6];

bool first_run = 1;
int vibrate = 1;
int second = 1;
int date = 0;
int ticks = 1;
int color = 0;
int bluetooth = 0;

char *upcase(char *str) {
    char *s = str;

    while (*s) {
        *s++ = toupper((int)*s);
    }

    return str;
}

static void create_ticks(Layer *layer)
{ 
  GRect bounds = layer_get_bounds(layer);
#if defined(PBL_RECT)
  if ( color == 0 )
  {
    s_tick_sprite_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TICK20_INV_SPRITE);
  } else {
    s_tick_sprite_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TICK20_SPRITE);
  }
#elif defined(PBL_ROUND) 
  if ( color == 0 )
  {
    s_tick_sprite_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TICK_SPRITE_24_WOB);
  } else {
    s_tick_sprite_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TICK_SPRITE_24_BOW);
  }
#endif
    
  layer_remove_child_layers(layer);
  
  if ( ticks == 1 )
  {
  
    for (int i = 0; i < 12; ++i) {
    
      int16_t angle = i * 30;

#if defined(PBL_RECT)      
      GPoint point = gpoint_from_polar(GRect(1, 0, bounds.size.w - 19, bounds.size.h - 38), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angle));
      s_tick_bitmap[i] = gbitmap_create_as_sub_bitmap(s_tick_sprite_bitmap, TICK20_SPRITE_DEF[i]);
      s_ticks_layers[i] = bitmap_layer_create(GRect(point.x-5, point.y+5, 30, 30));
#elif defined(PBL_ROUND)
      GPoint point = gpoint_from_polar(GRect(0, 0, bounds.size.w - 29, bounds.size.h - 38), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angle));
      s_tick_bitmap[i] = gbitmap_create_as_sub_bitmap(s_tick_sprite_bitmap, TICK_SPRITE_24_DEF[i]);
      s_ticks_layers[i] = bitmap_layer_create(GRect(point.x+1, point.y+5, 30, 30));
#endif
    
      bitmap_layer_set_bitmap(s_ticks_layers[i], s_tick_bitmap[i]);
      layer_insert_below_sibling(bitmap_layer_get_layer(s_ticks_layers[i]), layer);
    }  
  } else {
    for (int i = 0; i < 12; ++i) {
      if ( s_ticks_layers[i] != NULL )
      {
        bitmap_layer_destroy(s_ticks_layers[i]);
        s_ticks_layers[i] = NULL;
      }
    }
  }
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *vibrate_key = dict_find(iter, MESSAGE_KEY_KEY_VIBRATE);
  Tuple *second_key = dict_find(iter, MESSAGE_KEY_KEY_SECOND);
  Tuple *date_key = dict_find(iter, MESSAGE_KEY_KEY_DATE);
  Tuple *ticks_key = dict_find(iter, MESSAGE_KEY_KEY_TICKS);
  Tuple *color_key = dict_find(iter, MESSAGE_KEY_KEY_COLOR);
  Tuple *bluetooth_key = dict_find(iter, MESSAGE_KEY_KEY_BLUETOOTH);
  
  vibrate = vibrate_key->value->int32;
  second = second_key->value->int32;
  date = date_key->value->int32;
  ticks = ticks_key->value->int32;
  color = color_key->value->int32;
  bluetooth = bluetooth_key->value->uint8 - 48;

  persist_write_int(MESSAGE_KEY_KEY_VIBRATE, vibrate_key->value->int32);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "vibrate = %d", vibrate);
  
  persist_write_int(MESSAGE_KEY_KEY_SECOND, second_key->value->int32);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "second = %d", second);
  
  persist_write_int(MESSAGE_KEY_KEY_DATE, date_key->value->int32);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "date = %d", date);
  
  persist_write_int(MESSAGE_KEY_KEY_TICKS, ticks_key->value->int32);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ticks = %d", ticks);
  
  persist_write_int(MESSAGE_KEY_KEY_COLOR, color_key->value->int32);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "color = %d", color);
  
  persist_write_int(MESSAGE_KEY_KEY_BLUETOOTH, bluetooth);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "bluetooth = %d", bluetooth);
  
  if ( second == 1 ) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);  
  }
  
  if ( date == 1 ) {
    layer_set_hidden(s_date_layer, false);
  } else {
    layer_set_hidden(s_date_layer, true);
  }
  
  create_ticks(s_ticks_layer);
  
  if ( bluetooth == 2 ) {
    layer_set_hidden(s_bluetooth_layer, true);
  } else {
    layer_set_hidden(s_bluetooth_layer, false);
  }
  layer_mark_dirty(s_bluetooth_layer);
}

static void bluetooth_callback(bool connected) {
  
  if ( first_run == 0 && vibrate == 1)
    vibes_double_pulse();
  
  if ( first_run == 1 )
    first_run = 0;
  
  layer_mark_dirty(s_bluetooth_layer);
}

static void bluetooth_update_proc(Layer *layer, GContext *ctx)
{
  if ( bluetooth != 2 ) {    
    bool connected = connection_service_peek_pebble_app_connection();
    
    if(connected)
    {
      if ( bluetooth == 1 ) {
        if ( color == 0 )
        {
          graphics_context_set_stroke_color(ctx, GColorWhite);
        } else {
          graphics_context_set_stroke_color(ctx, GColorBlack);  
        }
        gpath_draw_outline_open(ctx, s_bt_icon_gpath);
      }
    } else {
      graphics_context_set_stroke_color(ctx, GColorRed);
      gpath_draw_outline_open(ctx, s_bt_icon_gpath);
    }
  }
}

static void bg_update_proc(Layer *layer, GContext *ctx) {

  if ( color == 0 )
  {
    graphics_context_set_fill_color(ctx, GColorBlack);
  } else {
    graphics_context_set_fill_color(ctx, GColorWhite);
  }
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  const int16_t second_hand_length = PBL_IF_ROUND_ELSE((bounds.size.w / 2) - 12, bounds.size.w / 2 - 3);
  const int16_t second_hand_length_back = PBL_IF_ROUND_ELSE(24, 20);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  GColor8 bg_color = GColorBlack;
  GColor8 fg_color = GColorWhite;
  if ( color > 0 ) 
  {
    bg_color = GColorWhite;
    fg_color = GColorBlack;
  }
  
  // for screenshot purposes (10:09.30)
#if defined(SCREENSHOT)
  t->tm_sec = 30;
  t->tm_min = 9; 
  t->tm_hour = 10;
#endif

  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  GPoint second_hand_start = {
    .x = center.x - (int16_t)(sin_lookup(second_angle) *  (int32_t)second_hand_length_back / TRIG_MAX_RATIO),
    .y = center.y - (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length_back / TRIG_MAX_RATIO),
  };
  
  // minute hand
  const int16_t minute_hand_length = PBL_IF_ROUND_ELSE((bounds.size.w / 2) - 12, bounds.size.w / 2 - 5);
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.y,
  };  
  
  graphics_context_set_stroke_color(ctx, fg_color);
  graphics_context_set_stroke_width(ctx, 4);
  graphics_draw_line(ctx, minute_hand, center);
  
  // hour hand
  const int16_t hour_hand_length = PBL_IF_ROUND_ELSE((bounds.size.w / 2) - 42, bounds.size.w / 2 - 32);
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.y,
  };  

  graphics_context_set_stroke_color(ctx, fg_color);
  graphics_context_set_stroke_width(ctx, 4);

  graphics_draw_line(ctx, hour_hand, center);
  
  // dot in the middle
  graphics_context_set_fill_color(ctx, fg_color);
  graphics_fill_circle(ctx, grect_center_point(&bounds), 5);
  if ( second == 1 ) {
    graphics_context_set_fill_color(ctx, GColorRed);
  } else {
    graphics_context_set_fill_color(ctx, bg_color);
  }
  graphics_fill_circle(ctx, grect_center_point(&bounds), 3);

  // second hand
  if ( second == 1 ) {
    graphics_context_set_stroke_color(ctx, GColorOrange);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, second_hand, second_hand_start);
  }
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  // for screenshot purposes (Tue Sep 9)
#if defined(SCREENSHOT)
  t->tm_mday = 29;
  t->tm_wday = 5;
#endif
  
  // TESTING SETUP
  //t->tm_mday = 1;
  //t->tm_wday = 5;
  
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  upcase(s_day_buffer);
  text_layer_set_text(s_day_label, s_day_buffer);
  
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer+(('0' == s_num_buffer[0])?1:0));
  
  if ( color == 0 )
  {
    text_layer_set_text_color(s_num_label, GColorWhite);
  } else {
    text_layer_set_text_color(s_num_label, GColorBlack);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);
 
  s_ticks_layer = layer_create(bounds);
  layer_add_child(window_layer, s_ticks_layer);
  create_ticks(s_ticks_layer);
  
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  if ( date == 0 ) {
    layer_set_hidden(s_date_layer, true);
  }
  layer_add_child(window_layer, s_date_layer);
  
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

#if defined(PBL_RECT)
  int x = bounds.size.w / 2;
  int y = bounds.size.h / 10 * 6; // 60% of height
  s_num_label = text_layer_create(GRect(120, 67, 25, 28));
#elif defined(PBL_ROUND)
  int x = bounds.size.w / 2;
  int y = bounds.size.h / 10 * 5.8; // 60% of height
  s_num_label = text_layer_create(GRect(x + 9, y, 50, y+20));
  s_day_label = text_layer_create(GRect(x - 45, y, 50, y+20));
#endif
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorClear);
  text_layer_set_text_alignment(s_num_label, GTextAlignmentLeft);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label)); 

  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_text_color(s_day_label, GColorOrange);
  text_layer_set_background_color(s_day_label, GColorClear);
  text_layer_set_text_alignment(s_day_label, GTextAlignmentRight);
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label)); 
  
  // BLUETOOTH LAYER
  x = bounds.size.w / 2 - 4;
  y = bounds.size.h / 4;
    
  s_bt_icon_gpath = gpath_create(&BT_ICON_PATH);
  gpath_move_to(s_bt_icon_gpath, GPoint(x,y));
  
  s_bluetooth_layer = layer_create(bounds);
  if ( bluetooth == 2 ) {
    layer_set_hidden(s_bluetooth_layer, true);
  } else {
    layer_set_hidden(s_bluetooth_layer, false);
  }
  layer_set_update_proc(s_bluetooth_layer, bluetooth_update_proc);
  layer_add_child(window_layer, s_bluetooth_layer);
  
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);

  layer_destroy(s_hands_layer);
}

static void init() {
  
  if ( persist_exists(MESSAGE_KEY_KEY_VIBRATE) )
    vibrate = persist_read_int(MESSAGE_KEY_KEY_VIBRATE);
  if ( persist_exists(MESSAGE_KEY_KEY_SECOND) )
    second = persist_read_int(MESSAGE_KEY_KEY_SECOND);
  if ( persist_exists(MESSAGE_KEY_KEY_DATE) )
    date = persist_read_int(MESSAGE_KEY_KEY_DATE);
  if ( persist_exists(MESSAGE_KEY_KEY_TICKS) )
    ticks = persist_read_int(MESSAGE_KEY_KEY_TICKS);
  if ( persist_exists(MESSAGE_KEY_KEY_COLOR) )
    color = persist_read_int(MESSAGE_KEY_KEY_COLOR);
  if ( persist_exists(MESSAGE_KEY_KEY_BLUETOOTH) )
    bluetooth = persist_read_int(MESSAGE_KEY_KEY_BLUETOOTH);
  
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  
  if ( second == 1 ) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);  
  }
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
