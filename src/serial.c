#ifndef NO_NRFSDK
  #include "sdk_config.h"
  #include "boards.h"
  #include "app_util_platform.h"
  #include "nrf_gpio.h"
  #include "nrf_delay.h"
  #include "app_uart.h"
  #if defined(UART_PRESENT)
    #include "nrf_uart.h"
  #endif
  #if defined(UARTE_PRESENT)
    #include "nrf_uarte.h"
  #endif
  #include "nrf_log.h"
#endif /* NO_NRFSDK */

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/array.h"
#include "mruby/data.h"
#include <string.h>
#include <stdbool.h>

#ifndef NO_NRFSDK
/* UART TX buffer size */
#ifndef UART_TX_BUF_SIZE
  #define UART_TX_BUF_SIZE 256
#endif
/* UART RX buffer size */
#ifndef UART_RX_BUF_SIZE
  #define UART_RX_BUF_SIZE 256      /**< UART RX buffer size. */
#endif

/* IO pins */
#ifndef PORT_UART_TX
  #define PORT_UART_TX    NRF_GPIO_PIN_MAP(0, 22)
#endif
#ifndef PORT_UART_RX
  #define PORT_UART_RX    NRF_GPIO_PIN_MAP(0, 20)
#endif

static app_uart_comm_params_t comm_params = {
  PORT_UART_RX,
  PORT_UART_TX,
  0xff,                           /* RTS not use */
  0xff,                           /* CTS not use */
  APP_UART_FLOW_CONTROL_DISABLED, /* disable flow control */
  false,                          /* No parity */
// #if defined (UART_PRESENT)
  NRF_UART_BAUDRATE_9600
// #else
//   NRF_UARTE_BAUDRATE_9600
// #endif
};

static void
uart_error_handle(app_uart_evt_t * p_event)
{
  if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR) {
    NRF_LOG_WARNING("APP_UART_COMMUNICATION_ERROR");  /* Ignore UART communication error */
  }
  else if (p_event->evt_type == APP_UART_FIFO_ERROR) {
    APP_ERROR_HANDLER(p_event->data.error_code);
  }
}
#endif /* NO_NRFSDK */

static bool _uart_enabled = false;

static mrb_value
mrb_serial_init(mrb_state *mrb, mrb_value self)
{
  mrb_int baud;
  mrb_int dbits=8, start=1, stop=1;
  mrb_sym sympari = mrb_intern_lit(mrb, "none");
  bool parity = false;
#ifndef NO_NRFSDK
  uint32_t err_code;
#endif /* NO_NRFSDK */

  mrb_get_args(mrb, "i|iiin", &baud, &dbits, &start, &stop, &sympari);

  parity = (bool)(sympari == mrb_intern_lit(mrb, "even"));

#ifndef NO_NRFSDK
  switch (baud) {
    case 1200:    comm_params.baud_rate = NRF_UART_BAUDRATE_1200;   break;
    case 2400:    comm_params.baud_rate = NRF_UART_BAUDRATE_2400;   break;
    case 4800:    comm_params.baud_rate = NRF_UART_BAUDRATE_4800;   break;
    case 9600:    comm_params.baud_rate = NRF_UART_BAUDRATE_9600;   break;
    case 14400:   comm_params.baud_rate = NRF_UART_BAUDRATE_14400;  break;
    case 19200:   comm_params.baud_rate = NRF_UART_BAUDRATE_19200;  break;
    case 28800:   comm_params.baud_rate = NRF_UART_BAUDRATE_28800;  break;
    case 38400:   comm_params.baud_rate = NRF_UART_BAUDRATE_38400;  break;
    case 57600:   comm_params.baud_rate = NRF_UART_BAUDRATE_57600;  break;
    case 76800:   comm_params.baud_rate = NRF_UART_BAUDRATE_76800;  break;
    case 115200:  comm_params.baud_rate = NRF_UART_BAUDRATE_115200; break;
    case 230400:  comm_params.baud_rate = NRF_UART_BAUDRATE_230400; break;
    case 250000:  comm_params.baud_rate = NRF_UART_BAUDRATE_250000; break;
    case 460800:  comm_params.baud_rate = NRF_UART_BAUDRATE_460800; break;
    case 921600:  comm_params.baud_rate = NRF_UART_BAUDRATE_921600; break;
    case 1000000: comm_params.baud_rate = NRF_UART_BAUDRATE_1000000;break;
    default:
      mrb_raisef(mrb, E_ARGUMENT_ERROR, "wrong baud rate %S.", mrb_fixnum_value(baud));
  }

  comm_params.use_parity = parity;

  APP_UART_FIFO_INIT(
    &comm_params,
    UART_RX_BUF_SIZE,
    UART_TX_BUF_SIZE,
    uart_error_handle,
    APP_IRQ_PRIORITY_LOWEST,
    err_code);
  APP_ERROR_CHECK(err_code);

  _uart_enabled = true;
#endif /* NO_NRFSDK */

  return self;
}

static mrb_value
mrb_serial_read(mrb_state *mrb, mrb_value self)
{
#ifndef NO_NRFSDK
  uint8_t c;
  ret_code_t err_code;
#endif /* NO_NRFSDK */
  mrb_int val = -1;

  if (!_uart_enabled) mrb_raise(mrb, E_RUNTIME_ERROR, "Serial port not ready.");

#ifndef NO_NRFSDK
  /* read data from Serial */
  err_code = app_uart_get(&c);
  if (err_code == NRF_SUCCESS) val = c;
#endif /* NO_NRFSDK */

  return mrb_fixnum_value(val);
}

static mrb_value
mrb_serial_write(mrb_state *mrb, mrb_value self)
{
  mrb_int val;

  if (!_uart_enabled) mrb_raise(mrb, E_RUNTIME_ERROR, "Serial port not ready.");

  mrb_get_args(mrb, "i", &val);

#ifndef NO_NRFSDK
  /* write data to Serial */
  app_uart_put((uint8_t)val);
#endif /* NO_NRFSDK */

  return mrb_nil_value();
}

static mrb_value
mrb_serial_available(mrb_state *mrb, mrb_value self)
{
  mrb_int len = 0;

  if (!_uart_enabled) mrb_raise(mrb, E_RUNTIME_ERROR, "Serial port not ready.");
  // TODO

  return mrb_fixnum_value(len);
}

static mrb_value
mrb_serial_flush(mrb_state *mrb, mrb_value self)
{
  if (!_uart_enabled) mrb_raise(mrb, E_RUNTIME_ERROR, "Serial port not ready.");
#ifndef NO_NRFSDK
  app_uart_flush();
#endif /* NO_NRFSDK */

  return mrb_nil_value();
}

static mrb_value
mrb_serial_close(mrb_state *mrb, mrb_value self)
{
  if (_uart_enabled) {

#ifndef NO_NRFSDK
    app_uart_close();
#endif /* NO_NRFSDK */

    _uart_enabled = false;
  }
  return mrb_nil_value();
}

void
mrb_mruby_plato_serial_nrf52_gem_init(mrb_state *mrb)
{
  struct RClass *mod = mrb_define_module(mrb, "PlatoNRF52");
  struct RClass *ser = mrb_define_class_under(mrb, mod, "Serial", mrb->object_class);

  mrb_define_method(mrb, ser, "initialize", mrb_serial_init,      MRB_ARGS_ARG(1, 4));
  mrb_define_method(mrb, ser, "_read",      mrb_serial_read,      MRB_ARGS_NONE());
  mrb_define_method(mrb, ser, "_write",     mrb_serial_write,     MRB_ARGS_REQ(1));
  mrb_define_method(mrb, ser, "available",  mrb_serial_available, MRB_ARGS_NONE());
  mrb_define_method(mrb, ser, "flush",      mrb_serial_flush,     MRB_ARGS_NONE());
  mrb_define_method(mrb, ser, "close",      mrb_serial_close,     MRB_ARGS_NONE());
}

void
mrb_mruby_plato_serial_nrf52_gem_final(mrb_state *mrb)
{
}
