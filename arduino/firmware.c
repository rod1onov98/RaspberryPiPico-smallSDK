#include <Mouse.h>

void setup() {
  Mouse.begin();
  Serial.begin(115200);
  while(!Serial) delay(10);
  delay(2000);
  Serial.println("PICO_MOUSE_READY");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    Serial.print("recv: ");
    Serial.println(command);
    
    if (command.startsWith("MOVE|")) {
      int params[4];
      int paramIndex = 0;
      int lastIndex = 5;
      
      for (int i = 5; i < command.length(); i++) {
        if (command[i] == '|') {
          params[paramIndex++] = command.substring(lastIndex, i).toInt();
          lastIndex = i + 1;
        }
      }
      params[3] = command.substring(lastIndex).toInt();
      
      if (paramIndex == 3) {
        int total_time = params[0];
        int move_time = params[1];
        int target_x = params[2];
        int target_y = params[3];
        
        Serial.print(target_x);
        Serial.print(", ");
        Serial.print(target_y);
        Serial.print(" over ");
        Serial.print(move_time);
        Serial.println("ms");
        
        int steps = move_time / 10;
        if (steps < 1) steps = 1;
        
        int step_delay = move_time / steps;
        int current_x = 0, current_y = 0;
        
        for (int i = 1; i <= steps; i++) {
          int new_x = (target_x * i) / steps;
          int new_y = (target_y * i) / steps;
          
          int delta_x = new_x - current_x;
          int delta_y = new_y - current_y;
          
          if (delta_x != 0  delta_y != 0) {
            Mouse.move(delta_x, delta_y);
          }
          
          delay(step_delay);
          current_x = new_x;
          current_y = new_y;
        }
        
        int final_delta_x = target_x - current_x;
        int final_delta_y = target_y - current_y;
        
        if (final_delta_x != 0  final_delta_y != 0) {
          Mouse.move(final_delta_x, final_delta_y);
          delay(10);
        }
        
        Serial.println("movement finished");
      }
    }
  }
  delay(10);
}
