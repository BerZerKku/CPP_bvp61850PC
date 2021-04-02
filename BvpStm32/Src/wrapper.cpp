/*
 * wrapper.c
 *
 *  Created on: 10 авг. 2020 г.
 *      Author: Shcheblykin
 */


#include "wrapper.hpp"
#include "debug.h"
#include "main.h"
#include "usbd_cdc_if.h"

#include <cassert>
#include <stdio.h>

#include "bvpCommon/param.h"
#include "bvpCommon/bvpCommon.hpp"
#include "bvpCommon/serial/modbusVp.h"
#include "bvpCommon/serial/avantpi.h"
#include "bvpCommon/serial/avantpc.h"
#include "bvpCommon/extAlarm.hpp"
#include "bvpCommon/clock.hpp"

using namespace BVP;

extern I2C_HandleTypeDef hi2c2;
extern IWDG_HandleTypeDef hiwdg;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

static void i2cReset(I2C_HandleTypeDef *hi2c);
static void i2cActionStart(I2C_HandleTypeDef *hi2c, uint32_t size);
static void i2cActionStop();
static void i2cProcessing();
static void i2cWatchDogReset();

static void powerWatchDogOff();
static void powerWatchDog();

static void rpiWatchDog();
static void rpiWatchDogReset();
static void rpiReset();

static void alarmLoop();
static void protocolPoll();

#define I2C_ACTION_TIME_MIN_MS 20

#define I2C_TIME_RESET_MS 100
#define I2C_MAX_ERROR_COUNTER 5

#define POWER_OFF_TIME_MS 100

#define RPI_REBOOT_TIME_MS 30000
#define RPI_RESET_NO_CONNECT_MS 1000

//
TParam params;
TModbusVp modbusVp(TSerialProtocol::REGIME_master);
TAvantPi avantPi(TSerialProtocol::REGIME_master);
TAvantPc avantPc(TSerialProtocol::REGIME_slave);

TExtAlarm mAlarm;

enum ePort_t {
  PORT_PI = 0,  ///< Связь с БСП-ПИ (UART1)
  PORT_LN,      ///< Локальная сеть (UART6)
  PORT_DR,      ///< Цифровой переприем (UART3 + EnDr)
  PORT_PC,      ///< Связь с ПК (Virtual Port Com)
  PORT_RPi,     ///< Связь с RPi (I2C2 или UART2)
  //
  PORT_MAX
};

enum ePortType_t {
  PORT_TYPE_uart = 0,
  PORT_TYPE_usb,
  PORT_TYPE_i2c
};

template <size_t size, typename type_t>
struct port_t {
  ePortType_t type;
  type_t rxByte;
  type_t txByte;
  type_t buf[size];
  BVP::TSerialProtocol *protocol = nullptr;
  UART_HandleTypeDef *huart = nullptr;
};

port_t<256, uint8_t> port[PORT_MAX];

//
enum i2cState_t {
  I2C_STATE_no = 0,
  I2C_STATE_readWait,
  I2C_STATE_readOk,
  I2C_STATE_write,
  I2C_STATE_writeWait,
  I2C_STATE_writeOk,
  //
  I2C_STATE_MAX
};

uint16_t address = 0;
uint8_t direction = 0;
uint8_t state = 0;
uint8_t buf[36] = { 0 };

volatile bool i2cAction = false;
uint32_t i2cActionTime = 0;
uint32_t i2cErrorCounter = 0;
uint32_t i2cErrorCounterAf = 0;
uint32_t i2cWdTimer = I2C_TIME_RESET_MS;
uint64_t data = 0;

bool power = true;
uint32_t powerOffTime = POWER_OFF_TIME_MS;

bool rpiConnection = false;
uint32_t rpiTimeToReset = RPI_REBOOT_TIME_MS;

bool printDebug = true;
uint32_t debug = 0;

i2cState_t i2cState = I2C_STATE_no; /// Текущее состояние интерфейса

BvpPkg bvpPkg(BvpPkg::MODE_slave);

/**
 *
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
    if (port[i].type == PORT_TYPE_uart) {
      if (huart == port[i].huart) {
        TSerialProtocol *protocol = port[i].protocol;

        if (protocol != nullptr) {
          protocol->sendFinished();
          HAL_UART_Receive_IT(huart, &port[i].rxByte, 1);
        }

        break;
      }
    }
  }
}

/**
 *
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
    if (port[i].type == PORT_TYPE_uart) {
      if (huart == port[i].huart) {
        TSerialProtocol *protocol = port[i].protocol;

        if (protocol != nullptr) {
          protocol->push(port[i].rxByte);
          HAL_UART_Receive_IT(huart, &port[i].rxByte, 1);
        }

        break;
      }
    }
  }
}

/**
 *
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  // TODO Проверить работает ли данный обработчик
  for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
    if (port[i].type == PORT_TYPE_uart) {
      if (huart == port[i].huart) {
        TSerialProtocol *protocol = port[i].protocol;

        if (protocol != nullptr) {
          protocol->readError();
          HAL_UART_Receive_IT(huart, &port[i].rxByte, 1);
        }

        break;
      }
    }
  }
}

/**
 *
 */
void CdcTransmitCpltFsCallback() {
  for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
    if (port[i].type == PORT_TYPE_usb) {
      TSerialProtocol *protocol = port[i].protocol;

      if (protocol != nullptr) {
        protocol->sendFinished();
      }

      break;
    }
  }
}

/**
 *
 */
void CdcReceiveFsCallback(uint8_t *buf, uint16_t len) {
  for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
    if (port[i].type == PORT_TYPE_usb) {
      TSerialProtocol *protocol = port[i].protocol;

      if (protocol != nullptr) {
        for(uint16_t i = 0; i < len; i++) {
          protocol->push(buf[i]);
        }
      }

      break;
    }
  }
}

/**
 * @brief  Period elapsed callback in non-blocking mode
 * @param  htim TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  static uint16_t cnt = 0;

  if (htim == &htim6) {
    // 1 мс
    TClock::tick();

    if (++cnt >= 1000) {
      if (rpiConnection) {
        HAL_GPIO_WritePin(LED1_VD7_GPIO_Port, LED1_VD7_Pin, GPIO_PIN_RESET);
      } else {
        HAL_GPIO_TogglePin(LED1_VD7_GPIO_Port, LED1_VD7_Pin);
      }
      Debug::proc();
      cnt = 0;
    }

    if (i2cWdTimer > 0) {
      i2cWdTimer--;
    }

    if (i2cWdTimer == 0) {
      Debug::addMsg(Debug::MSG_i2cWdTimerReset);
      i2cReset(&hi2c2);
    }

    if ((i2cActionTime > 0) && i2cAction) {
      if (--i2cActionTime == 0) {
        Debug::addMsg(Debug::MSG_i2cActionTimeReset);
        i2cReset(&hi2c2);
      }
    }

    if (powerOffTime > 0) {
      if (--powerOffTime == 0) {
        powerWatchDogOff();
      }
    }

    if (!rpiConnection) {
      Debug::addMsg(Debug::MSG_rpiConnectionNo);
    }

    powerWatchDog();
    rpiWatchDog();

    Debug::send();
  } else if (htim == &htim7) {
    // 0,1мс

    for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
      TSerialProtocol *p = port[i].protocol;
      if ((p!= nullptr) && p->isEnable()) {
        p->tick();
      }
    }
  }
}


/**
 * @brief  Slave Rx Transfer completed callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c == &hi2c2) {
    i2cActionStop();

    i2cState = (i2cState == I2C_STATE_readWait) ?
        I2C_STATE_readOk : I2C_STATE_no;

    rpiWatchDogReset();
    i2cWatchDogReset();
  }
}


/**
 * @brief  Slave Tx Transfer completed callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c == &hi2c2) {
    i2cActionStop();

    i2cState = (i2cState == I2C_STATE_writeWait) ?
        I2C_STATE_writeOk : I2C_STATE_no;

    rpiWatchDogReset();
    i2cWatchDogReset();
  }
}


/**
 * @brief  I2C error callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c == &hi2c2) {
    i2cActionStop();

    uint32_t error = HAL_I2C_GetError(hi2c);

    Debug::addMsg(Debug::MSG_i2cErrorCallback);
    if (error & HAL_I2C_ERROR_AF) {
      Debug::addMsg(Debug::MSG_i2cErrorCounterAf);
    }

    i2cState = I2C_STATE_no;
    i2cErrorCounter++;
  }
}


/**
 * @brief  I2C abort callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c == &hi2c2) {
    i2cActionStop();

    Debug::addMsg(Debug::MSG_HAL_I2C_AbortCpltCallback);

    i2cState = I2C_STATE_no;
    i2cErrorCounter++;
  }
}


// Вызов после инициализации всей периферии в main().
void wrapperMainInit() {

  i2cWatchDogReset();
  HAL_Delay(10);
  printf("Hello STM32\n");

  TClock::setTickInMs(1);

  ePort_t index = PORT_PI;
  port[index].type = PORT_TYPE_uart;
  port[index].huart = &huart1;
  port[index].protocol = &avantPi;
  port[index].protocol->setNetAddress(0x01);
  port[index].protocol->setID(SRC_pi);
  port[index].protocol->setBuffer(port[index].buf,
      sizeof(port[index].buf) / sizeof(port[index].buf[0]));
  port[index].protocol->setTimeTick(100);
  port[index].protocol->setup(port[index].huart->Init.BaudRate,
      (port[index].huart->Init.Parity != UART_PARITY_NONE),
      (port[index].huart->Init.StopBits == UART_STOPBITS_2) ? 2 : 1);
  port[index].protocol->setEnable(true);

  index = PORT_DR;
  port[index].type = PORT_TYPE_uart;
  port[index].huart = &huart3;
  port[index].protocol = &modbusVp;
  port[index].protocol->setNetAddress(0x0A);
  port[index].protocol->setID(SRC_vkey);
  port[index].protocol->setBuffer(port[index].buf,
      sizeof(port[index].buf) / sizeof(port[index].buf[0]));
  port[index].protocol->setTimeTick(100);
  port[index].protocol->setup(port[index].huart->Init.BaudRate,
      (port[index].huart->Init.Parity != UART_PARITY_NONE),
      (port[index].huart->Init.StopBits == UART_STOPBITS_2) ? 2 : 1);
  port[index].protocol->setEnable(true);

  index = PORT_PC;
  port[index].type = PORT_TYPE_usb;
  port[index].protocol = &avantPc;
  port[index].protocol->setNetAddress(0x01);
  port[index].protocol->setID(SRC_pi);
  port[index].protocol->setBuffer(port[index].buf,
      sizeof(port[index].buf) / sizeof(port[index].buf[0]));
  port[index].protocol->setTimeTick(100);
  port[index].protocol->setup(port[index].huart->Init.BaudRate,
      (port[index].huart->Init.Parity != UART_PARITY_NONE),
      (port[index].huart->Init.StopBits == UART_STOPBITS_2) ? 2 : 1);
  port[index].protocol->setEnable(true);
}


// Вызов из бесконечного цикла main().
void wrapperMainLoop() {
  HAL_IWDG_Refresh(&hiwdg);

  i2cProcessing();

  protocolPoll();
  alarmLoop();
}

// FIXME ПОлучается лишнее либо params, либо mParam
TParam *mParam = TParam::getInstance();

//
bool getExtAlarmSignals(extAlarm_t signal) {
  bool state = false;
  const GPIO_PinState defval = GPIO_PIN_SET;

  switch(signal) {
    case EXT_ALARM_channelFault: {
      state = HAL_GPIO_ReadPin(Sout5_GPIO_Port, Sout5_Pin) != defval;
    } break;

    case EXT_ALARM_warning: {
      state = HAL_GPIO_ReadPin(Sout6_GPIO_Port, Sout6_Pin) != defval;
    } break;

    case EXT_ALARM_fault: {
      state = (HAL_GPIO_ReadPin(Sout7_GPIO_Port, Sout7_Pin) == defval);
//      state |= !rpiConnection;
    } break;

    case EXT_ALARM_comPrd: {
      state = HAL_GPIO_ReadPin(Sout2_GPIO_Port, Sout2_Pin) != defval;
    } break;

    case EXT_ALARM_comPrm: {
      state = HAL_GPIO_ReadPin(Sout1_GPIO_Port, Sout1_Pin) != defval;
    } break;

    case EXT_ALARM_disablePrm: break;
    case EXT_ALARM_model61850: break;
    case EXT_ALARM_test61850: break;
    case EXT_ALARM_MAX: break;
  }

  return state;
}

//
void setExtAlarmSignal(extAlarm_t signal, bool value) {
  const GPIO_PinState defval = GPIO_PIN_SET;
  const GPIO_PinState actval = GPIO_PIN_RESET;

  GPIO_PinState state = value ? actval : defval;

  switch(signal) {
    case EXT_ALARM_channelFault: {
      HAL_GPIO_WritePin(HF_FAULT_GPIO_Port, HF_FAULT_Pin, state);
    } break;

    case EXT_ALARM_warning: {
      HAL_GPIO_WritePin(WARNING_GPIO_Port, WARNING_Pin, state);
    } break;

    case EXT_ALARM_fault: {
      state = (state == defval) ? actval : defval;
      HAL_GPIO_WritePin(ALARM_GPIO_Port, ALARM_Pin, state);
    } break;

    case EXT_ALARM_comPrd: {
      HAL_GPIO_WritePin(COM_TR_GPIO_Port, COM_TR_Pin, state);
    } break;

    case EXT_ALARM_comPrm: {
      HAL_GPIO_WritePin(COM_RC_GPIO_Port, COM_RC_Pin, state);
    } break;

    case EXT_ALARM_disablePrm: {
      HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, state);
    } break;

    case EXT_ALARM_model61850: {
      HAL_GPIO_WritePin(LED2_VD8_GPIO_Port, LED2_VD8_Pin, state);
    } break;

    case EXT_ALARM_test61850: {
      HAL_GPIO_WritePin(TEST_GOOSE_GPIO_Port, TEST_GOOSE_Pin, state);
    } break;

    case EXT_ALARM_MAX: break;
  }
}

void alarmResetLoop()
{
  static clockPoint_t last = TClock::getClockPoint();
  const src_t src = SRC_int;
  bool ok = true;
  uint32_t uval32;

  // Обработка нажатия кнопки сброса

  uval32 = mParam->getValue(PARAM_alarmRstCtrl, src, ok);
  if ((ok) && (uval32 != ALARM_RST_CTRL_no)) {
    if (uval32 == ALARM_RST_CTRL_pressed) {
      uval32 = mParam->getValue(PARAM_control, src, ok);
      if (!ok) {
        uval32 = 0;
      }

      uval32 |= (1 << CTRL_resetComInd);
      if (mAlarm.isSignalForDeviceReset()) {
        uval32 |= (1 << CTRL_resetFault);
      }

      mParam->setValue(PARAM_control, src, uval32);

      // Сброс сигнализации и начало отсчета времени
      mAlarm.reset(true);
      last = TClock::getClockPoint();
    }
    uval32 = ALARM_RST_CTRL_no;
  }
  mParam->setValue(PARAM_alarmRstCtrl, src, uval32);

  // Отключение сброса сигнализации, если прошло заданное время.
  if (mAlarm.isReset() && (TClock::getDurationS(last) >= 2)) {
    mAlarm.reset(false);
  }
}

//
void alarmLoop()
{
  const src_t src = SRC_int;
  bool ok;
  uint32_t uval32;

  alarmResetLoop();

  // Обработка входных и установка выходных сигналов

  uval32 = mParam->getValue(PARAM_alarmResetMode, src, ok);
  if (ok) {
    mAlarm.setAlarmReset(alarmReset_t(uval32));
  }

  for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
    bool value;
    extAlarm_t signal = static_cast<extAlarm_t> (i);

    if (signal == EXT_ALARM_disablePrm) {
      uval32 = mParam->getValue(PARAM_blkComPrmAll,
          SRC_int, ok);
      if (!ok) {
          uval32 = DISABLE_PRM_enable;
      }

      value = (uval32 == DISABLE_PRM_disable);
    } else {
      value = getExtAlarmSignals(signal);
    }

    mAlarm.setAlarmInputSignal(signal, value);
  }

  for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
    extAlarm_t signal = static_cast<extAlarm_t> (i);

    bool value = mAlarm.getAlarmOutputSignal(signal);

    setExtAlarmSignal(signal, value);
  }
}

// Сброс интерфейса I2Cю
void i2cReset(I2C_HandleTypeDef *hi2c) {
  i2cWatchDogReset();

  i2cState = I2C_STATE_no;
  HAL_I2C_DeInit(hi2c);
  HAL_I2C_Init(hi2c);
}

//
void i2cWatchDogReset() {
  i2cWdTimer = I2C_TIME_RESET_MS;

  i2cAction = false;
  i2cActionTime = 100;
}

//
void i2cActionStart(I2C_HandleTypeDef *hi2c, uint32_t size) {
  // Минимальное время на действие.
  // TODO Время на передачу между чтением и записью отличается! Нужно учесть в минимальном времени.
  i2cActionTime = I2C_ACTION_TIME_MIN_MS;
  // 9 = 8 data bits + 1 ack/nack, старт и стоп биты по одному на всю посылку.
  i2cActionTime += (size * 9 * 1000)/(hi2c->Init.ClockSpeed);
  i2cAction = true;
}

//
void i2cActionStop() {
  i2cAction = false;
  i2cActionTime = 100;
}

/// Сторожевой таймер для Raspberry.
void rpiWatchDog() {
  if (rpiTimeToReset > 0) {
    rpiTimeToReset--;
  }

  /// Сигнал сброса подается в течении одного такта таймера.
  if (rpiTimeToReset == 0) {
    rpiReset();
    rpiTimeToReset = RPI_REBOOT_TIME_MS;
  } else {
    HAL_GPIO_WritePin(RASP_RESET_GPIO_Port, RASP_RESET_Pin, GPIO_PIN_SET);
  }
}

//
void rpiWatchDogReset() {
  // FIXME сделать сброс только для корректно принятых пакетов!
  rpiConnection = true;
  rpiTimeToReset = RPI_RESET_NO_CONNECT_MS;
}

//
void rpiReset() {
#ifdef NDEBUG
  HAL_GPIO_WritePin(RASP_RESET_GPIO_Port, RASP_RESET_Pin, GPIO_PIN_RESET);
#endif
  rpiConnection = false;
  Debug::addMsg(Debug::MSG_rpiReset);
}

// TODO переделать на прерывание ?!
void powerWatchDog() {
  GPIO_PinState pinstate = GPIO_PIN_RESET;

  pinstate = HAL_GPIO_ReadPin(EXT_PWR_DOWN_GPIO_Port, EXT_PWR_DOWN_Pin);
  if (pinstate == GPIO_PIN_SET) {
    powerOffTime = POWER_OFF_TIME_MS;
  } else {
    Debug::addMsg(Debug::MSG_powerExtPwrDownIsLow);
  }
}

//
void powerWatchDogOff() {
  // При выключении необходимо удерживать распберии в сбросе.
#ifdef NDEBUG
  HAL_GPIO_WritePin(BACKUP_EN_GPIO_Port, BACKUP_EN_Pin, GPIO_PIN_SET);
#endif
  Debug::addMsg(Debug::MSG_powerTimeReset);
}

void i2cProcessing() {
  uint8_t *buf = nullptr;
  uint16_t len = 0;
  uint8_t data[DATA_LEN];


  if (i2cState > I2C_STATE_MAX) {
    i2cState = I2C_STATE_MAX;
  }

  switch(i2cState) {
    case I2C_STATE_no: {
      buf = bvpPkg.getRxPkg(len);
      if (HAL_I2C_Slave_Receive_IT(&hi2c2, buf, len) != HAL_BUSY) {
        i2cState = I2C_STATE_readWait;
        i2cActionStart(&hi2c2, len);
      }
    } break;

    case I2C_STATE_readWait: {
    } break;

    case I2C_STATE_readOk: {
      len = 1;
      if (bvpPkg.getDataFromPkg(data, len)) {
        // FIXME Обратно передается полученный пакет
        // TODO Добавить обработку принятого пакета.
        // TODO  Добавить формирование пакета на передачу.
        if (bvpPkg.addDataToPkg(data, len)) {
          i2cState = I2C_STATE_write;
        } else {
          i2cState = I2C_STATE_no;
        }
      } else {
        i2cState = I2C_STATE_no;
      }
    } break;

    case I2C_STATE_write: {
      buf = bvpPkg.getRxPkg(len);
      if (HAL_I2C_Slave_Transmit_IT(&hi2c2, buf, len) != HAL_BUSY) {
        i2cState = I2C_STATE_writeWait;
        i2cActionStart(&hi2c2, len);
      }
    } break;

    case  I2C_STATE_writeWait: {
    } break;

    case  I2C_STATE_writeOk: {
      i2cState = I2C_STATE_no;
    } break;

    case I2C_STATE_MAX: {
      assert(false);
    } break;
  }

  HAL_GPIO_WritePin(TP2_GPIO_Port, TP2_Pin, GPIO_PIN_RESET);
}

void protocolPoll() {
  for(uint8_t i = 0; i < (sizeof(port) / sizeof(port[0])); i++) {
    TSerialProtocol *p = port[i].protocol;

    if ((p != nullptr) && p->isEnable()) {
      p->read();

      if (p->write()) {
        uint8_t *data = nullptr;
        uint16_t len = p->pop(&data);

        Q_ASSERT(data != nullptr);

        if (len > 0) {
          switch(port[i].type) {
            case PORT_TYPE_uart: {
              if (port[i].huart != nullptr) {
                HAL_UART_Transmit_IT(port[i].huart, data, len);
              }
            } break;

            case PORT_TYPE_usb: {
              uint8_t state = CDC_Transmit_FS(data, len);
              //              printf("send data to usb: %d\n", state);
            } break;

            case PORT_TYPE_i2c: {

            } break;
          }
        }
      }
    }
  }

  // XXX There is no net "EnDR" on KVP_05v0.
  if (port[PORT_DR].protocol != nullptr) {
    HAL_GPIO_WritePin(En_DR_GPIO_Port, En_DR_Pin,
        port[PORT_DR].protocol->isEnable() ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

