#include "pwm.h"

#include "Compiler.h"
#include "logging.h"
#include "board.h"

typedef struct {
  unsigned int con1;
  unsigned int con2;
  unsigned int rs;
  unsigned int r;
  unsigned int tmr;
} OC_REGS;

// timer 1 is clocked @16MHz
// we use a 256x presclaer to achieve 62.5KHz
// used for low frequency PWM
static void Timer1Init() {
  T1CON = 0x0000;  // Timer off
  TMR1 = 0x0000;
  PR1 = 0xFFFF;
  T1CON = 0x8030;
}

#define OC_REG(num) (((volatile OC_REGS *) &OC1CON1) + num)

void PWMInit() {
  int i;
  // disable PWMs
  for (i = 0; i < NUM_PWMS; ++i) {
    SetPwmPeriod(i, 0, 0);
  }
  Timer1Init();  // constantly running, feeds low-speed PWM
}

void SetPwmDutyCycle(int pwm_num, int dc, int fraction) {
  volatile OC_REGS* regs;
  log_printf("SetPwmDutyCycle(%d, %d, %d)", pwm_num, dc, fraction);
  regs = OC_REG(pwm_num);
  regs->con2 &= ~0x0600;
  regs->con2 |= fraction << 9;
  regs->r = dc;
}

void SetPwmPeriod(int pwm_num, int period, int scale256) {
  volatile OC_REGS* regs;
  log_printf("SetPwmPeriod(%d, %d, %d)", pwm_num, period, scale256);
  regs = OC_REG(pwm_num);
  regs->con1 = 0x0000;
  if (period) {
    regs->r = 0;
    regs->rs = period;
    regs->con2 = 0x001F;
    regs->con1 = 0x0006 | (scale256 ? 0x1000 : 0x1C00);
  }
}
