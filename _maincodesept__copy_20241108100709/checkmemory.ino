void checkMemory() {
  extern int __heap_start, *__brkval;
  int v;
  int free_memory = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__heap_start);
  Serial.print(F("Free memory: "));
  Serial.println(free_memory);
}
