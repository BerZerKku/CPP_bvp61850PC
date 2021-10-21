#ifndef __AVANT_RPI_H__
#define __AVANT_RPI_H__

#include "protocolAvant.h"
#include "ringArray.hpp"

namespace BVP
{

class TAvantRPi : public TProtocolAvant
{

public:
    TAvantRPi(regime_t regime);

private:
    bool mIsComRx = false;  ///< Флаг наличия принятого сообщения.

    /**
     * @brief Формирование новой команды для передачи.
     * @return true если есть команда для передачи, иначе false.
     */
    bool vWriteAvant() override;

    /**
     * @brief Обработка принятой команды.
     * @return true если комада обработана, иначе false.
     */
    bool vReadAvant() override;
};

}  // namespace BVP

#endif  // __AVANT_RPI_H__
