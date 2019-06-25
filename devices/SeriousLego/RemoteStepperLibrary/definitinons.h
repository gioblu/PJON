#ifndef definitions_h
#define definition_h
#endif

enum {
  // direct
  pinLow,
  pinHigh,

  // handlePin
  timedPin,
  timedPinSequence,
  pwmPin,

  // motor
  setCurrentPosition,
  setSpeed,
  setAcceleration,
  moveTo,
  move,
  home,
  reportFinished,

  // sensor
  blockingReadDigital,
  blockingReadAnalog,
  asynchReadDigital,
  asynchReadAnalog,
  continualRead,

  // meta
  configure,       
  initialize,
  sendState,
  sleep,
  wakeUp,
  testSerialComm,
  resync,
  error
};

