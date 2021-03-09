#ifndef AVANT_PI_H
#define AVANT_PI_H

#include "ringArray.hpp"
#include "protocolAvant.h"

namespace BVP {

class TAvantPi : public TProtocolAvant {

  /// Значение байта данных для команды COM_AVANT_control "Управление"
  enum comControlBytes_t {
    COM_CONTROL_BYTES_selfReset = 1 //<
  };


 public:
  TAvantPi(regime_t regime);

  bool vWriteAvant() override;
  bool vReadAvant() override;

 private:
  TRingArray<comAvant_t, 3, COM_AVANT_getError> ringComArray;


  comAvant_t mComCycle[]; ///< Массив команд опрашиваемых по кругу.

  /** Формирование команды управления.
   *
   *  @return true если команда сформирована, иначе false.
   */
  bool fillComControl(uint32_t value);

  bool comGetError(); ///< Обработчик команды чтения неисправностей и предупр.
  bool comGetTime();  ///< Обработчик команды чтения времени.


};

} // namespace BVP

#endif // AVANT_PI_H
