#ifndef TPARAM_H
#define TPARAM_H

#include <stdint.h>
#include "debug.hpp"
#include "hardware.hpp"

namespace BVP {

enum param_t {
    //
    PARAM_control = 0,      ///< Сигналы управления.
    // Текущее состояние
    PARAM_error,            ///< Флаг наличия неисправности.
    PARAM_warning,          ///< Флаг наличия предупреждения.
    PARAM_defError,         ///< Неисправности Защиты.
    PARAM_defWarning,       ///< Предупреждения Защиты.
    PARAM_prmError,         ///< Неисправности Приемника.
    PARAM_prmWarning,       ///< Педупреждения Приемника.
    PARAM_prm2Error,        ///< Неисправности Приемника 2.
    PARAM_prm2Warning,      ///> Педупреждения Приемника 2.
    PARAM_prdError,         ///< Неисправности Передатчика.
    PARAM_prdWarning,       ///< Педупреждения Передатчика.
    PARAM_glbError,         ///< Неисправности Общие.
    PARAM_glbWarning,       ///< Педупреждения Общие.
    PARAM_defRemoteError,   ///< Неисправности Защиты удаленного аппарата.
    PARAM_prmRemoteError,   ///< Неисправности Приемника удаленного аппарата.
    PARAM_prdRemoteError,   ///< Неисправности Передатчика удаленного аппарата.
    PARAM_glbRemoteError,   ///< Неисправности Общие удаленного аппарата.
    // Дата и время
    PARAM_dateYear,         ///< Год.
    PARAM_dateMonth,        ///< Месяц.
    PARAM_dateDay,          ///< День.
    PARAM_timeHour,         ///< Часы.
    PARAM_timeMin,          ///< Минуты.
    PARAM_timeSec,          ///< Секунды.
    PARAM_timeMSec,         ///< Миллисекунды.
    // Общие параметры
    PARAM_alarmReset,       ///< Сброс сигнализации.
    // Внутренние параметры
    PARAM_extAlarm,         ///< Внешняя сигнализация.
    // Параметры панели виртуальных ключей
    PARAM_dirControl,       ///< Управление ключами (SAC2).
    PARAM_blkComPrmAll,     ///< Блокировка всех выходов приемника (SAC1).
    PARAM_blkComPrmDir,     ///< Блокировка направлений выхода приемника (SAnn.x)
    PARAM_blkComPrm32to01,  ///< Блокированные команды приемника с 1 по 32.
    PARAM_blkComPrm64to33,  ///< Блокированные команды приемника с 33 по 64.
    PARAM_blkComPrd32to01,  ///< Блокированные команды передатчика с 1 по 32.
    PARAM_blkComPrd64to33,  ///< Блокированные команды передатчика с 33 по 64.
    // Кнопки панели виртуальных ключей.
    PARAM_vpBtnSAnSbSac,
    PARAM_vpBtnSA32to01,
    PARAM_vpBtnSA64to33,
    //
    PARAM_MAX
};

/// Источник доступа к параметрам.
enum src_t {
    SRC_pi = 0, ///< БСП-ПИ
    SRC_pc,     ///< Конфигуратор ПК
    SRC_acs,    ///< АСУ
    SRC_vkey,   ///< Панель виртуальных ключей
    SRC_int,    ///< Внутренние алгоритмы программы
    //
    SRC_MAX
};

/// Сигналы управления.
enum ctrl_t {
    CTRL_MIN = 1,
    //
    CTRL_resetIndication = CTRL_MIN,
    CTRL_resetErrors,
    CTRL_reset,
    //
    CTRL_MAX

};

/// Управление ключами
enum dirControl_t {
    DIR_CONTROL_local = 0,
    DIR_CONTROL_remote,
    //
    DIR_CONTROL_MAX
};

/// Сброс сигнализации
enum alarmReset_t {
    ALARM_RESET_auto = 0,
    ALARM_RESET_manual,
    //
    ALARM_RESET_MAX
};

///
enum switchOff_t {
    ON_OFF_off = 0, ///<
    ON_OFF_on,
    //
    ON_OFF_MAX
};

///

/** Класс параметров.
 *
 *  Создан на подобие Одиночки (single-tone).конструктор спрятан в protected,
 *  а создание объекта new происходит при первом вызове getInstance).
 *  Но т.к. объект должен существовать всегда, его конструктор открыт и
 *  при вызове getInstance нет необходимости проверки первого запуска.
 *
 *  В каждом классе где используются параметры необходимо взять указатель
 *  на созданный объект (getInstance).
 *
 * Создавать объект где-либо нельзя, он создается сразу!!!
 */
class TParam {
    /// Параметры.
    static TParam mParam;

    /// Поля настроек параметра.
    struct paramFields_t {
        param_t param;    ///< Параметр
        bool isValue;     ///< Флаг наличия считанного значения.
        uint32_t rValue;  ///< Считанное значение.
        uint32_t wValue;  ///< Значение для записи.
        ///< Доп. обработка при установке значения параметра.
        bool (*set) (param_t, BVP::src_t, uint32_t&);
        ///< Доп. обработка при чтении значения параметра.
        bool (*get) (param_t, BVP::src_t, uint32_t&);
    };

    enum vpBtnControl_t {
        VP_BTN_CONTROL_sac1 = 0x00000001,
        VP_BTN_CONTROL_sac2 = 0x00000002,
        VP_BTN_CONTROL_sb   = 0x00000004,
        VP_BTN_CONTROL_san  = 0x0000FF00
    };

public:
    /// Конструктор.
    TParam();
    /// Конструктор копирования запрещен.
    TParam(TParam &other) = delete;
    /// Конструктор присваивания запрещен.
    void operator=(const TParam&) = delete;

    /** Возвращает экземпляр класса параметров.
   *
   *  @return Класс параметров.
   */
    static TParam* getInstance();

    /** Проверяет наличие установленного значения параметра.
   *
   *  @param[in] param Параметр.
   *  @return true если значение было установлено, иначе false.
   */
    bool isValueSet(param_t param) const;

    /** Проверяет возможность чтения параметра из указанного источника.
   *
   *  @param[in] param Параметр.
   *  @param[in] src Источник доступа.
   *  @return true если чтение разрешено, иначе false.
   */
    bool isAccessRead(param_t param, src_t src) const;

    /** Проверяет возможность установки параметра из указанного источника.
   *
   *  @param[in] param Параметр.
   *  @param[in] src Источник доступа.
   *  @return true если запись разрешена, иначе false.
   */
    bool isAccessSet(param_t param, src_t src) const;

    /** Возвращает значение параметра.
   *
   *  Значение не будет получено, если параметр еще не установлен или
   *  не достаточно прав доступа.
   *
   *  @param[in] param Параметр.
   *  @param[in] src Источник доступа.
   *  @param[out] ok true если значение считано, иначе false.
   *  @return Значение параметра.
   */
    uint32_t getValue(param_t param, src_t src, bool &ok);

    /** Возвращает считанное значение параметра.
   *
   *  @param[in] param Параметр.
   *  @return Значение параметра.
   */
    uint32_t getValueR(param_t param);

    /** Возвраащет значение параметра для записи.
   *
   *  @param[in] param Параметр.
   *  @return  Значение параметра.
   */
    uint32_t getValueW(param_t param);

    /** Устанавливает значение параметра.
   *
   *  @param[in] param Параметр.
   *  @param[in] src Источник доступа.
   *  @param[in] value Значение параметра.
   *  @return true если установлено новое значение, иначе false.
   */
    bool setValue(param_t param, src_t src, uint32_t value);

private:
    /// Значения параметров.
    static paramFields_t params[PARAM_MAX];

    friend bool getBlkComPrm(param_t param, src_t src, uint32_t &value);
    friend bool getControl(param_t param, src_t src, uint32_t &value);

    friend bool setBlkComPrmAll(param_t param, src_t src, uint32_t &value);
    friend bool setBlkComPrm(param_t param, src_t src, uint32_t &value);
    friend bool setBtnSA(param_t param, src_t src, uint32_t &value);
    friend bool setControl(param_t param, src_t src, uint32_t &value);
    friend bool setError(param_t param, src_t src, uint32_t &value);
    friend bool setDirControl(param_t param, src_t src, uint32_t &value);
    friend bool setVpBtnSAnSbSac(param_t param, src_t src, uint32_t &value);
    friend bool setWarning(param_t param, src_t src, uint32_t &value);


    void setLocalValue(param_t param, uint32_t value);
};

} // namespace BVP

#endif // TPARAM_H
