//
//  Mouser.h
//  MRMouse
//
//  Created by Mark Rada on 12-03-17.
//  Copyright (c) 2012 Mark Rada. All rights reserved.
//

#include <ApplicationServices/ApplicationServices.h>

enum MouseMovementUnit {
  kMouseScrollByLine  = kCGScrollEventUnitLine,
  // TODO: might not be real pixels, might be Cocoa co-ords, need to investigate
  kMouseScrollByPixel = kCGScrollEventUnitPixel
};


CGPoint mouse_current_position();

void mouse_move_to(CGPoint point);
void mouse_move_to2(CGPoint point, double duration);

void mouse_drag_to(CGPoint point);
void mouse_drag_to2(CGPoint point, double duration);

void mouse_scroll(size_t amount);
void mouse_scroll2(size_t amount, enum MouseMovementUnit units);
void mouse_scroll3(size_t amount, enum MouseMovementUnit units, double duration);

void mouse_click_down();
void mouse_click_down2(CGPoint point);
void mouse_click_down3(CGPoint point, size_t sleep_quanta);

void mouse_click_up();
void mouse_click_up2(CGPoint point);

void mouse_click();
void mouse_click2(CGPoint point);

void mouse_secondary_click();
void mouse_secondary_click2(CGPoint point);
void mouse_secondary_click3(CGPoint point, size_t sleep_quanta);

void mouse_arbitrary_click(CGEventMouseSubtype button);
void mouse_arbitrary_click2(CGEventMouseSubtype button, CGPoint point);
void mouse_arbitrary_click3(CGEventMouseSubtype button, CGPoint point, size_t sleep_quanta);

void mouse_middle_click();
void mouse_middle_click2(CGPoint point);

void mouse_multi_click(size_t num_clicks);
void mouse_multi_click2(size_t num_clicks, CGPoint point);

void mouse_double_click();
void mouse_double_click2(CGPoint point);

void mouse_triple_click();
void mouse_triple_click2(CGPoint point);