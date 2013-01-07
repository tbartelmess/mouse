//
//  Mouser.c
//  MRMouse
//
//  Created by Mark Rada on 12-03-17.
//  Copyright (c) 2012 Mark Rada. All rights reserved.
//

#include <ApplicationServices/ApplicationServices.h>
#include "mouser.h"
#include "CGEventAdditions.h"

static const uint_t FPS     = 240;
static const uint_t QUANTUM = 1000000 / 240; // should be FPS, but GCC sucks
static const double DEFAULT_DURATION      = 0.2; // seconds
static const double DEFAULT_MAGNIFICATION = 2.0; // factor

#define NEW_EVENT(type,point,button) CGEventCreateMouseEvent(nil,type,point,button)
#define POST(event) CGEventPost(kCGHIDEventTap, event)
#define CHANGE(event,type) CGEventSetType(event, type)

#define CLOSE_ENOUGH(a,b) ((abs(a.x - b.x) < 1.0) && (abs(a.y - b.y) < 1.0))
#define NOW (CFDateCreate(nil,CFAbsoluteTimeGetCurrent()))

#ifdef NOT_MACRUBY
#define RELEASE(x) CFRelease(x)
#else
#define RELEASE(x) CFMakeCollectable(x)
#endif

#define POSTRELEASE(x) do {			\
    CGEventRef _event = x;  	 		\
    POST(_event);				\
    RELEASE(_event);				\
  } while(false);


static
void
mouse_sleep(uint_t quanta)
{
  usleep(quanta * QUANTUM);
}

CGPoint
mouse_current_position()
{
  CGEventRef event = CGEventCreate(nil);
  CGPoint point = CGEventGetLocation(event);
  RELEASE(event);
  return point;
}

// Executes a linear mouse movement animation. It can be a simple cursor
// move or a drag depending on what is passed to `type`.
static
void
mouse_animate(
	      CGEventType type,
	      CGMouseButton button,
	      CGPoint start_point,
	      CGPoint end_point,
	      double duration
	      )
{
  CFDateRef current_time = NULL;
  CGPoint current_point  = start_point;
  double     xstep = (end_point.x - start_point.x) / (duration * FPS);
  double     ystep = (end_point.y - start_point.y) / (duration * FPS);
  CFDateRef  start = NOW;
  double remaining = 0.0;

  while (!CLOSE_ENOUGH(current_point, end_point)) {
    remaining  = end_point.x - current_point.x;
    current_point.x += abs(xstep) > abs(remaining) ? remaining : xstep;

    remaining  = end_point.y - current_point.y;
    current_point.y += abs(ystep) > abs(remaining) ? remaining : ystep;

    POSTRELEASE(NEW_EVENT(type, current_point, button));

    mouse_sleep(1);
    current_time = NOW;

    // this is a safety
    if (CFDateGetTimeIntervalSinceDate(NOW, start) > (duration + 1))
      break;

    RELEASE(current_time);
    current_time = NULL;

    current_point = mouse_current_position();
  }

  RELEASE(start);
  if (current_time)
    RELEASE(current_time);
}


void
mouse_move_to2(CGPoint point, double duration)
{
  mouse_animate(
		kCGEventMouseMoved,
		kCGMouseButtonLeft,
		mouse_current_position(),
		point,
		duration
		);
}

void
mouse_move_to(CGPoint point)
{
  mouse_move_to2(point, DEFAULT_DURATION);
}


void
mouse_drag_to2(CGPoint point, double duration)
{
  POSTRELEASE(NEW_EVENT(
			kCGEventLeftMouseDown,
			mouse_current_position(),
			kCGMouseButtonLeft
			));


  mouse_animate(
		kCGEventLeftMouseDragged,
		kCGMouseButtonLeft,
		mouse_current_position(),
		point,
		duration
		);

  POSTRELEASE(NEW_EVENT(
			kCGEventLeftMouseUp,
			mouse_current_position(),
			kCGMouseButtonLeft
			));
}

void
mouse_drag_to(CGPoint point)
{
  mouse_drag_to2(point, DEFAULT_DURATION);
}


void
mouse_scroll3(int amount, CGScrollEventUnit units, double duration)
{
  size_t     steps = round(FPS * duration);
  double   current = 0.0;
  double      done = 0.0;
  int32_t   scroll = 0;

  for (size_t step = 0; step < steps; step++) {
    done   = (double)(step+1) / (double)steps;
    scroll = round((done - current) * amount);
    POSTRELEASE(CGEventCreateScrollWheelEvent(nil, units, 1, scroll));
    mouse_sleep(2);
    current += (double)scroll / (double)amount;
  }
}

void
mouse_scroll2(int amount, CGScrollEventUnit units)
{
  mouse_scroll3(amount, units, DEFAULT_DURATION);
}

void
mouse_scroll(int amount)
{
  mouse_scroll2(amount, kCGScrollEventUnitLine);
}


void
mouse_click_down3(CGPoint point, uint_t sleep_quanta)
{
  POSTRELEASE(NEW_EVENT(kCGEventLeftMouseDown, point, kCGMouseButtonLeft));
  mouse_sleep(sleep_quanta);
}

void
mouse_click_down2(CGPoint point)
{
  mouse_click_down3(point, FPS / 10);
}

void
mouse_click_down()
{
  mouse_click_down2(mouse_current_position());
}


void
mouse_click_up2(CGPoint point)
{
  POSTRELEASE(NEW_EVENT(kCGEventLeftMouseUp, point, kCGMouseButtonLeft));
}

void
mouse_click_up()
{
  mouse_click_up2(mouse_current_position());
}


void
mouse_click2(CGPoint point)
{
  mouse_click_down2(point);
  mouse_click_up2(point);
}

void
mouse_click()
{
  mouse_click2(mouse_current_position());
}


void
mouse_secondary_click_down3(CGPoint point, uint_t sleep_quanta)
{
  CGEventRef base_event = NEW_EVENT(
				    kCGEventRightMouseDown,
				    point,
				    kCGMouseButtonRight
				    );
  POSTRELEASE(base_event);
  mouse_sleep(sleep_quanta);
}

void
mouse_secondary_click_down2(CGPoint point)
{
  mouse_secondary_click_down3(point, FPS / 10);
}

void
mouse_secondary_click_down()
{
  mouse_secondary_click_down2(mouse_current_position());
}


void
mouse_secondary_click_up2(CGPoint point)
{
  CGEventRef base_event = NEW_EVENT(
				    kCGEventRightMouseUp,
				    point,
				    kCGMouseButtonRight
				    );
  POSTRELEASE(base_event);
}

void
mouse_secondary_click_up()
{
  mouse_secondary_click_up2(mouse_current_position());
}


void
mouse_secondary_click3(CGPoint point, uint_t sleep_quanta)
{
  mouse_secondary_click_down3(point, sleep_quanta);
  mouse_secondary_click_up2(point);
}

void
mouse_secondary_click2(CGPoint point)
{
  mouse_secondary_click_down2(point);
  mouse_secondary_click_up2(point);
}

void
mouse_secondary_click()
{
  mouse_secondary_click_down();
  mouse_secondary_click_up();
}


void
mouse_arbitrary_click_down3(
			    CGEventMouseSubtype button,
			    CGPoint point,
			    uint_t sleep_quanta
			    )
{
  CGEventRef base_event = NEW_EVENT(
				    kCGEventOtherMouseDown,
				    point,
				    button
				    );
  POSTRELEASE(base_event);
  mouse_sleep(sleep_quanta);
}

void
mouse_arbitrary_click_down2(CGEventMouseSubtype button, CGPoint point)
{
  mouse_arbitrary_click_down3(button, point, FPS / 10);
}

void
mouse_arbitrary_click_down(CGEventMouseSubtype button)
{
  mouse_arbitrary_click_down2(button, mouse_current_position());
}


void mouse_arbitrary_click_up2(CGEventMouseSubtype button, CGPoint point)
{
  CGEventRef base_event = NEW_EVENT(
				    kCGEventOtherMouseUp,
				    point,
				    button
				    );
  POSTRELEASE(base_event);
}

void mouse_arbitrary_click_up(CGEventMouseSubtype button)
{
  mouse_arbitrary_click_up2(button, mouse_current_position());
}


void
mouse_arbitrary_click3(CGEventMouseSubtype button, CGPoint point, uint_t sleep_quanta)
{
  mouse_arbitrary_click_down3(button, point, sleep_quanta);
  mouse_arbitrary_click_up2(button, point);
}

void
mouse_arbitrary_click2(CGEventMouseSubtype button, CGPoint point)
{
  mouse_arbitrary_click_down2(button, point);
  mouse_arbitrary_click_up2(button, point);
}

void
mouse_arbitrary_click(CGEventMouseSubtype button)
{
  mouse_arbitrary_click_down(button);
  mouse_arbitrary_click_up(button);
}


void
mouse_middle_click2(CGPoint point)
{
  mouse_arbitrary_click2(kCGMouseButtonCenter, point);
}

void
mouse_middle_click()
{
  mouse_middle_click2(mouse_current_position());
}


void
mouse_multi_click2(size_t num_clicks, CGPoint point)
{
  CGEventRef base_event = NEW_EVENT(
				    kCGEventLeftMouseDown,
				    point,
				    kCGMouseButtonLeft
				    );
  CGEventSetIntegerValueField(base_event, kCGMouseEventClickState, num_clicks);

  CHANGE(base_event, kCGEventLeftMouseDown);
  POST(base_event);

  CHANGE(base_event, kCGEventLeftMouseUp);
  POSTRELEASE(base_event);
}

void
mouse_multi_click(size_t num_clicks)
{
  mouse_multi_click2(num_clicks, mouse_current_position());
}


void
mouse_double_click2(CGPoint point)
{
  // some apps still expect to receive the single click event first
  // and then the double click event
  mouse_multi_click2(1, point);
  mouse_multi_click2(2, point);
}

void
mouse_double_click()
{
  mouse_double_click2(mouse_current_position());
}


void
mouse_triple_click2(CGPoint point)
{
  // some apps still expect to receive the single click event first
  // and then the double and triple click events
  mouse_double_click2(point);
  mouse_multi_click2(3, point);
}

void
mouse_triple_click()
{
  mouse_triple_click2(mouse_current_position());
}


static
void
mouse_gesture(CGPoint point, uint_t sleep_quanta, void (^gesture_block)(void))
{
  POSTRELEASE(NEW_EVENT(kCGEventMouseMoved, point, kCGMouseButtonLeft));

  CGEventRef gesture = CGEventCreate(nil);
  CHANGE(gesture, kCGEventGesture);
  CGEventSetIntegerValueField(gesture, kCGEventGestureType, kCGGestureTypeGestureStarted);
  POST(gesture);

  gesture_block();

  CGEventSetIntegerValueField(gesture, kCGEventGestureType, kCGGestureTypeGestureEnded);
  POSTRELEASE(gesture);

  mouse_sleep(sleep_quanta);
}

void
mouse_smart_magnify3(CGPoint point, uint_t sleep_quanta)
{
  mouse_gesture(point, sleep_quanta, ^(void) {
      CGEventRef event = CGEventCreate(nil);
      CHANGE(event, kCGEventGesture);
      CGEventSetIntegerValueField(event, kCGEventGestureType, kCGGestureTypeSmartMagnify);
      POSTRELEASE(event);
    });
}

void
mouse_smart_magnify2(CGPoint point)
{
  mouse_smart_magnify3(point, FPS / 2);
}

void
mouse_smart_magnify()
{
  mouse_smart_magnify2(mouse_current_position());
}

void
mouse_swipe2(int direction, double duration)
{
}

void
mouse_swipe(int direction)
{
  mouse_swipe2(direction, DEFAULT_DURATION);
}

void
mouse_pinch3(int direction, double magnification, double duration)
{
}

void
mouse_pinch2(int direction, double magnification)
{
 mouse_pinch3(direction, magnification, DEFAULT_DURATION);
}

void
mouse_pinch(int direction)
{
  mouse_pinch2(direction, DEFAULT_MAGNIFICATION);
}

void
mouse_rotate2(int direction, double angle, double duration)
{
}

void
mouse_rotate(int direction, double angle)
{
  mouse_rotate2(direction, angle, DEFAULT_DURATION);
}
