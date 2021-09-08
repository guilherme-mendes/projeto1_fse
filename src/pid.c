/*
 * pid.c
 *
 * Author: Renato Coral Sampaio
 */

#include "pid.h"
#include <stdio.h>

double measure_out, control_signal;
double reference = 0.0;
double Kp = 0.0; // Proportional Gain
double Ki = 0.0; // Integral Gain
double Kd = 0.0; // Derivated Gain
int T = 1.0;     // Sample Period (ms)
unsigned long last_time;
double total_error, erro_anterior = 0.0;
int control_signal_MAX = 100.0;
int control_signal_MIN = -100.0;

void pid_configure_consts(double Kp_, double Ki_, double Kd_)
{
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;
}

void pid_update_reference(float reference_)
{
  reference = (double)reference_;
}

double pid_control(double measure_out)
{

  double erro = reference - measure_out;

  total_error += erro; // Accumulate o erro (Integral Term)

  if (total_error >= control_signal_MAX)
  {
    total_error = control_signal_MAX;
  }
  else if (total_error <= control_signal_MIN)
  {
    total_error = control_signal_MIN;
  }

  double delta_error = erro - erro_anterior; // Difference between error (Derivated Term)

  control_signal = Kp * erro + (Ki * T) * total_error + (Kd / T) * delta_error; // PID calculate singal de control

  if (control_signal >= control_signal_MAX)
  {
    control_signal = control_signal_MAX;
  }
  else if (control_signal <= control_signal_MIN)
  {
    control_signal = control_signal_MIN;
  }

  erro_anterior = erro;

  return control_signal;
}
