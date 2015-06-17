#include <pebble.h>

#define BOX_SIZE 20
#define ANIM_DURATION 500
#define ANIM_DELAY 300

static Window *s_main_window;
static Layer *s_box_layer;
static PropertyAnimation *s_box_animation;
static int s_current_stage = 0;

static void next_animation();

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
#ifdef PBL_PLATFORM_APLITE
  property_animation_destroy(s_box_animation);
#endif

  if (finished) {
    next_animation();
  }
}

static void next_animation() {
  GRect start, finish;
  switch (s_current_stage) {
  case 0:
    start = GRect(0, 0, BOX_SIZE, BOX_SIZE);
    finish = GRect(144 - BOX_SIZE, 0, BOX_SIZE, BOX_SIZE);
    break;
  case 1:
    start = GRect(144 - BOX_SIZE, 0, BOX_SIZE, BOX_SIZE);
    finish = GRect(144 - BOX_SIZE, 168 - BOX_SIZE, BOX_SIZE, BOX_SIZE);
    break;
  case 2:
    start = GRect(144 - BOX_SIZE, 168 - BOX_SIZE, BOX_SIZE, BOX_SIZE);
    finish = GRect(0, 168 - BOX_SIZE, BOX_SIZE, BOX_SIZE);
    break;
  case 3:
    start = GRect(0, 168 - BOX_SIZE, BOX_SIZE, BOX_SIZE);
    finish = GRect(0, 0, BOX_SIZE, BOX_SIZE);
    break;
  default:
    start = GRect(0, 0, BOX_SIZE, BOX_SIZE);
    finish = GRect(0, 0, BOX_SIZE, BOX_SIZE);
  }

  s_box_animation = property_animation_create_layer_frame(s_box_layer, &start, &finish);
  animation_set_duration((Animation*)s_box_animation, ANIM_DURATION);
  animation_set_delay((Animation*)s_box_animation, ANIM_DELAY);
  animation_set_curve((Animation*)s_box_animation, AnimationCurveEaseInOut);
  animation_set_handlers((Animation*)s_box_animation, (AnimationHandlers) {
      .stopped = anim_stopped_handler
  }, NULL);
  animation_schedule((Animation*)s_box_animation);

  s_current_stage = (s_current_stage + 1) % 4;
}

static void update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 4, GCornersAll);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_box_layer = layer_create(GRect(0, 0, BOX_SIZE, BOX_SIZE));
  layer_set_update_proc(s_box_layer, update_proc);
  layer_add_child(window_layer, s_box_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_box_layer);
}

static void init(void) {
  s_main_window = window_create();
#ifdef PBL_SDK_2
  window_set_fullscreen(s_main_window, true);
#endif
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  next_animation();
}

static void deinit(void) {
  animation_unschedule_all();

  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
