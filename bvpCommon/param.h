#ifndef TPARAM_H
#define TPARAM_H

#include <stdint.h>
#include "debug.hpp"
#include "hardware.hpp"

namespace BVP {

// FIXME Эта версия должна быть в ПО для БВП!!!
// NOTE Версия ПО состоит из двух байт (major << 8) + (minor)
static const uint8_t versionMajor = 100;    ///< Версия ПО (major)
static const uint8_t versionMinor = 1;      ///< Версия ПО (minor)


enum param_t {
    PARAM_version = 0,      ///< Версия текущего устройства
    // Текущее состояние
    PARAM_error,            ///< Флаг наличия неисправности
    PARAM_warning,          ///< Флаг наличия предупреждения
    PARAM_defError,         ///< Неисправности Защиты
    PARAM_defWarning,       ///< Предупреждения Защиты
    PARAM_prmError,         ///< Неисправности Приемника
    PARAM_prmWarning,       ///< Педупреждения Приемника
    PARAM_prm2Error,        ///< Неисправности Приемника 2
    PARAM_prm2Warning,      ///> Педупреждения Приемника 2
    PARAM_prdError,         ///< Неисправности Передатчика
    PARAM_prdWarning,       ///< Педупреждения Передатчика
    PARAM_glbError,         ///< Неисправности Общие
    PARAM_glbWarning,       ///< Педупреждения Общие
    PARAM_defRemoteError,   ///< Неисправности Защиты удаленного аппарата
    PARAM_prmRemoteError,   ///< Неисправности Приемника удаленного аппарата
    PARAM_prdRemoteError,   ///< Неисправности Передатчика удаленного аппарата
    PARAM_glbRemoteError,   ///< Неисправности Общие удаленного аппарата
    // Дата и время
    PARAM_dateYear,         ///< Год
    PARAM_dateMonth,        ///< Месяц
    PARAM_dateDay,          ///< День
    PARAM_timeHour,         ///< Часы
    PARAM_timeMin,          ///< Минуты
    PARAM_timeSec,          ///< Секунды
    PARAM_timeMSec,         ///< Миллисекунды
    // Общие параметры
    PARAM_alarmResetMode,   ///< Режим сброса сигнализации
    // Внутренние параметры
    PARAM_extAlarm,         ///< Внешняя сигнализация
    PARAM_alarmRstCtrl,     ///< Управление "Сброс сигнализации"
    PARAM_control,          ///< Сигналы управления
    PARAM_debug1,           ///< Параметр для отладки ПО 1
    PARAM_debug2,           ///< Параметр для отладки ПО 2
    // Параметры панели виртуальных ключей
    PARAM_versionVp,        ///< Версия ПО панели ВК
    PARAM_dirControl,       ///< Управление ключами (SAC2)
    PARAM_blkComPrmAll,     ///< Блокировка всех выходов приемника (SAC1)
    PARAM_blkComPrmDir,     ///< Блокировка направлений выхода приемника (SAnn.x)
    PARAM_comPrmBlk08to01,  ///< Блокированные команды приемника с 8 по 1
    PARAM_comPrmBlk16to09,  ///< Блокированные команды приемника с 16 по 9
    PARAM_comPrmBlk24to17,  ///< Блокированные команды приемника с 24 по 17
    PARAM_comPrmBlk32to25,  ///< Блокированные команды приемника с 32 по 25
    PARAM_blkComPrm64to33,  ///< Блокированные команды приемника с 33 по 64
    PARAM_comPrdBlk08to01,  ///< Блокированные команды передатчика с 8 по 1
    PARAM_comPrdBlk16to09,  ///< Блокированные команды передатчика с 16 по 9
    PARAM_comPrdBlk24to17,  ///< Блокированные команды передатчика с 24 по 17
    PARAM_comPrdBlk32to25,  ///< Блокированные команды передатчика с 32 по 25
    PARAM_blkComPrd64to33,  ///< Блокированные команды передатчика с 33 по 64
    // Кнопки панели виртуальных ключей.
    PARAM_vpBtnSAnSbSac,    ///< Текущее положение кнопок SA.m, SB, SAC
    PARAM_vpBtnSA32to01,    ///< Текущее положение кнопок c SA32  по SA1
    PARAM_vpBtnSA64to33,    ///< Текущее положение кнопок c SA64  по SA33
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

/** Сигналы управления.
 *
 *  Команда перезагрузки аппарата должна стоять в конце!
 *  Чтобы перед ней были переданы все остальные.
 */
enum ctrl_t {
    CTRL_resetComInd = 0,   ///< Сброс индикации команд.
    CTRL_resetFault,        ///< Сброс неисправностей. Пока == CTRL_reset
    CTRL_resetSelf,         ///< Сброс аппарта. Желательно ставить в конец.
    //
    CTRL_MAX
};

/// Управление ключами
enum dirControl_t {
    DIR_CONTROL_local = 0,  ///< Местное
    DIR_CONTROL_remote,     ///< Дистанционное
    //
    DIR_CONTROL_MAX
};

/// Сброс сигнализации
enum alarmReset_t {
    ALARM_RESET_auto = 0,   ///< Автоматический
    ALARM_RESET_manual,     ///< Ручной
    //
    ALARM_RESET_MAX
};

/// Управление сбросом сигнализации
enum alarmRstCtrl_t {
    ALARM_RST_CTRL_no = 0,  ///< Нет.
    ALARM_RST_CTRL_pressed, ///< Нажат переключатель "Сброс сигнализации".
    //
    ALARM_RST_CTRL_MAX
};

/// Состояния параметра "Вывод ПРМ (SAC1)"
enum disablePrm_t {
    DISABLE_PRM_disable = 0,    ///< ПРМ выведен
    DISABLE_PRM_enable,         ///< ПРМ введен
    //
    DISABLE_PRM_MAX
};

///
enum switchOff_t {
    ON_OFF_off = 0, ///< Переключатель выключен
    ON_OFF_on,      ///< Переключатель включен
    //
    ON_OFF_MAX
};

///
enum vpBtnControl_t {
    VP_BTN_CONTROL_sac1 = 0,
    VP_BTN_CONTROL_sac2,
    VP_BTN_CONTROL_sb,
    VP_BTN_CONTROL_san1  = 8,
    VP_BTN_CONTROL_san2,
    VP_BTN_CONTROL_san3,
    VP_BTN_CONTROL_san4,
    VP_BTN_CONTROL_san5,
    VP_BTN_CONTROL_san6,
    VP_BTN_CONTROL_san7,
    VP_BTN_CONTROL_san8,
    //
    VP_BTN_CONTROL_MAX
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
        const param_t param;      ///< Параметр
        const uint32_t source;    ///< Источник установки данного значения (биты).
        bool isSet;         ///< Флаг наличия считанного значения.
        bool isModified;    ///< Флаг наличия нового знаения.
        uint32_t rValue;    ///< Считанное значение.
        uint32_t wValue;    ///< Значение для записи.
        ///< Доп. обработка при установке значения параметра.
        bool (* const set) (param_t, BVP::src_t, uint32_t&);
        ///< Доп. обработка при чтении значения параметра.
        bool (* const get) (param_t, BVP::src_t, uint32_t&);
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
    bool isSet(param_t param) const;

    /**
     * @brief isModified
     * @param param
     * @return
     */
    bool isModified(param_t param) const;

    /** Проверяет наличие доступа к параметру из указанного источника.
     *
     *  @param[in] param Параметр.
     *  @param[in] src Источник доступа.
     *  @return true если доступ разрешен, иначе false.
     */
    bool isAccess(param_t param, src_t src) const;

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
    uint32_t getValueR(param_t param) const;

    /** Возвраащет значение параметра для записи.
     *
     *  @param[in] param Параметр.
     *  @return  Значение параметра.
     */
    uint32_t getValueW(param_t param) const;

    /** Устанавливает значение параметра.
     *
     *  @param[in] param Параметр.
     *  @param[in] src Источник доступа.
     *  @param[in] value Значение параметра.
     *  @return true если установлено новое значение, иначе false.
     */
    bool setValue(param_t param, src_t src, uint32_t value);

    /** Сброс всех параметров в исходное состояние.
     *  После сброса будут установлены все внутренние параметры.
     */
    void reset();

private:
    /// Значения параметров.
    static paramFields_t params[PARAM_MAX];
    const src_t mSrc = SRC_int;

    friend bool setError(param_t param, src_t src, uint32_t &value);
    friend bool setWarning(param_t param, src_t src, uint32_t &value);

    /**
     * @brief Устанавлиает значение параметра.
     * Если источник доступа имеется в источниках установки параметра:
     * - то будет установлено новое значение rValue;
     * - иначе будет установлено значение для записи wValue.
     * @param[in] param Параметр.
     * @param[in] src Источник доступа.
     * @param[in] value Значение параметра
     */
    void setLocalValue(param_t param, src_t src, uint32_t value);
};

} // namespace BVP

#endif // TPARAM_H
