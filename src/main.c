
#include <fcntl.h>
#include <termios.h>

#include <crc16.h>
#include <gpio.h>
#include <uart_defs.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <bme280.h>
#include <lcd_control.h>
#include <gpio.h>
#include <pid.h>
#include <thermometer.h>
#include <export.h>

struct bme280_dev bme_connection;
int term = 0;
int fs_uart;
int key_gpio = 1;

void clear_screen()
{
  system("clear");
}

void get_uart(int filestream, unsigned char msgn)
{

  unsigned char matricula[7] = {0x01, 0x23, msgn, 0x04, 0x09, 0x01, 0x01};
  short crc = calcula_CRC(matricula, 7);
  unsigned char msgnn[9];
  memcpy(msgnn, &matricula, 7);

  memcpy(&msgnn[7], &crc, 2);

  int cnt = write(filestream, &msgnn[0], 9);

  if (cnt < 0)
  {
    printf("Erro na comunicação com o UART\n");
  }
  sleep(1);
}

Number_type uard_r(int filestream, unsigned char msgn)
{
  unsigned char buffer[20];
  Number_type number = {-1, -1.0};
  int cnt = read(filestream, buffer, 20);
  if (!cnt)
  {
    printf("Dado não recebido!\n");
  }
  else if (cnt < 0)
  {
    printf("Erro!\n");
  }
  else
  {
    buffer[cnt] = '\0';
    if (msgn == 0xC3)
      memcpy(&number.int_value, &buffer[3], sizeof(int));
    else
      memcpy(&number.float_value, &buffer[3], sizeof(float));
    return number;
  }
  return number;
}

int init_uart()
{
  int filestream = -1;

  char uart_file[] = "/dev/serial0";

  filestream = open(uart_file, O_RDWR | O_NOCTTY | O_NDELAY);

  if (filestream == -1)
  {
    printf("Não foi possível conectar a UART!\n");
  }

  else
  {
    printf("UART iniciada!\n");
  }

  struct termios options;

  tcgetattr(filestream, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;

  tcflush(filestream, TCIFLUSH);
  tcsetattr(filestream, TCSANOW, &options);
  return filestream;
}

void encerra_sistema()
{
  clear_screen();

  printf("Programa encerrado!\n");

  turn_resistance_off();

  turn_fan_off();

  uart_finish(fs_uart);

  exit(0);
}

void uard_r_s(int filestream, int sign)
{
  unsigned char matricula[7] = {0x01, 0x16, SEND_SIGNAL, 0x04, 0x09, 0x01, 0x01};
  unsigned char msgn[13];
  memcpy(msgn, &matricula, 7);
  memcpy(&msgn[7], &sign, 4);

  short crc = calcula_CRC(msgn, 11);
  memcpy(&msgn[11], &crc, 2);

  int cnt = write(filestream, &msgn[0], 13);
  if (cnt < 0)
  {
    printf("Erro na comunicação com o UART!\n");
  }
  sleep(1);
}

void uart_finish(int filestream)
{
  printf("Conexão UART encerrada!\n");
  close(filestream);
}

void switch_routine()
{
  if (term)
  {
    if (key_gpio == 1)
    {
      key_gpio = 0;
    }
    else
      key_gpio = 1;
  }
  else
    printf("\n");
    
  printf("Instrução inválida! Reinicie o sistema.\n");
}

void r_on_off(int key)
{
  clear_screen();

  int value_ts = 0;
  float histerese;
  float TI;
  float TR;
  float TE;

  printf("+------------- ON/OFF -------------+\n");

  printf("\n");
  printf("Insira o valor do Histerese: ");
  scanf("%f", &histerese);

  do
  {
    get_uart(fs_uart, GET_INTERNAL_TEMP);
    TI = uard_r(fs_uart, GET_INTERNAL_TEMP).float_value;
    get_uart(fs_uart, GET_POTENTIOMETER);
    TR = uard_r(fs_uart, GET_POTENTIOMETER).float_value;
    TE = get_current_temperature(&bme_connection);
    
    printf("+-------------------------------+\n");
    printf("+          TI: %.2f⁰C          +\n", TI);
    printf("+          TR: %.2f⁰C          +\n", TR);
    printf("+          TE: %.2f⁰C          +\n", TE);
    printf("+-------------------------------+\n");

    print_d(TI, TR, TE);

    add_file(TI, TR, TE);

    if (TR + histerese <= TI)
    {
      turn_fan_on(100);
      turn_resistance_off();
      value_ts = -100;
    }
    else if (TR - histerese >= TI)
    {
      turn_resistance_on(100);
      turn_fan_off();
      value_ts = 100;
    }

    if (!term)
    {
      get_uart(fs_uart, GET_KEY_VALUE);
      key_gpio = uard_r(fs_uart, GET_KEY_VALUE).int_value;
    }

    uard_r_s(fs_uart, value_ts);
  }
  while (key_gpio == key);
  printf("+--------------------------------------\n");
  r_pid(key_gpio);
}

void r_pid(int key)
{
  clear_screen();
  float histerese, TI, TR, TE;
  int value_ts = 0;
  printf("+------------- PID -------------+\n");
  pid_configure_consts(5, 1, 5);
  do
  {
    get_uart(fs_uart, GET_INTERNAL_TEMP);
   
    TI = uard_r(fs_uart, GET_INTERNAL_TEMP).float_value;

    double value_ts = pid_control(TI);

    pwm_control(value_ts);

    get_uart(fs_uart, GET_POTENTIOMETER);
   
    TR = uard_r(fs_uart, GET_POTENTIOMETER).float_value;

    pid_update_reference(TR);

    TE = get_current_temperature(&bme_connection);
    printf("+-------------------------------+\n");
    printf("+          TI: %.2f⁰C          +\n", TI);
    printf("+          TR: %.2f⁰C          +\n", TR);
    printf("+          TE: %.2f⁰C          +\n", TE);
    printf("+-------------------------------+\n");
    print_d(TI, TR, TE);
    add_file(TI, TR, TE);

    if (!term)
    {
      get_uart(fs_uart, GET_KEY_VALUE);
      key_gpio = uard_r(fs_uart, GET_KEY_VALUE).int_value;
    }

    uard_r_s(fs_uart, value_ts);
  } while (key_gpio == key);
  printf("+-------------------------------+\n");
  r_on_off(key_gpio);
}

void init()
{
  wiringPiSetup();
  turn_resistance_off();
  turn_fan_off();
  init_d();
  bme_connection = connect_bme();
  fs_uart = init_uart();
  clear_screen();
}

void init_menu()
{
  int opt, key;
  printf("+-------------------------------+\n");
  printf("+           PROJETO 1           +\n");
  printf("+-------------------------------+\n");
  printf("Escolha opção de entrada\n\n");
  printf("1) Chave\n2) Terminal\n");
  scanf("%d", &opt);
  switch (opt)
  {
  case 1:
    getchar();
    clear_screen();
    get_uart(fs_uart, GET_KEY_VALUE);
    key = uard_r(fs_uart, GET_KEY_VALUE).int_value;
    printf("+-------------------------------+\n");
    printf("+          CHAVE: %d             +\n", key);
    printf("+-------------------------------+\n\n");
    printf("0) Rotina ON/OFF\n");
    printf("1) Rotina PID\n");
    printf("ENTER para continuar\n");
    char enter = 0;
    while (enter != '\r' && enter != '\n')
    {
      enter = getchar();
    }
    break;
  default:
    clear_screen();
    printf("Opção invalida!\n");
    init_menu();
    break;
  case 2:
    clear_screen();
    printf("+---------------------------------+\n");
    printf("+           1) On/Off             +\n");
    printf("+           2) PID                +\n");
    printf("+---------------------------------+\n");
    scanf("%d", &key);
    key--;
    term = 1;
    break;
  }
  if (key == 0)
    r_on_off(key);
  else
    r_pid(key);
}

int main()
{
  init();
  create_csv();
  signal(SIGINT, encerra_sistema);
  signal(SIGQUIT, switch_routine);
  init_menu();
  return 0;
}
