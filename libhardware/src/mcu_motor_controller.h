#ifndef __mcu_motor_controller_h__
#define __mcu_motor_controller_h__

#define MCUMTR_NUM_MOTORS 3
#define MCUMTR_MAX_MOTOR_SPEED 7

class McuMotorController
{
   public:
      McuMotorController();
      ~McuMotorController();
      void PowerOn();
      void PowerOff();
      void MotorSpeed(int motor, int speed);
};

#endif