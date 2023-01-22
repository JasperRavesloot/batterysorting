double measuringVoltage() {
  int Sensorvalue = analogRead(A4);
  float voltage = Sensorvalue * (5.0 / 1023.0);
  return voltage;
}
