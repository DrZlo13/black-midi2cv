#include "fusb302.h"
#include <cstring>
#include <hal/hal.h>
#include "debug.h"

namespace FUSB302 {

// Register 0x01: Device ID
typedef struct {
    uint8_t REVISION_ID : 2;
    uint8_t PRODUCT_ID : 2;
    uint8_t VERSION_ID : 4;
} DEVICE_ID;

// Register 0x02: Switches0
typedef struct {
    uint8_t PDWN1 : 1;
    uint8_t PDWN2 : 1;
    uint8_t MEAS_CC1 : 1;
    uint8_t MEAS_CC2 : 1;
    uint8_t VCONN_CC1 : 1;
    uint8_t VCONN_CC2 : 1;
    uint8_t PU_EN1 : 1;
    uint8_t PU_EN2 : 1;
} SWITCHES0;

// Register 0x03: Switches1
typedef struct {
    uint8_t TXCC1 : 1;
    uint8_t TXCC2 : 1;
    uint8_t AUTO_CRC : 1;
    uint8_t RESERVED : 1;
    uint8_t DATA_ROLE : 1;
    uint8_t SPEC_REV : 2;
    uint8_t POWER_ROLE : 1;
} SWITCHES1;

// Register 0x04: Measure
typedef struct {
    uint8_t MDAC : 6;
    uint8_t MEAS_VBUS : 1;
    uint8_t RESERVED : 1;
} MEASURE;

// Register 0x05: Slice
typedef struct {
    uint8_t SDAC : 6;
    uint8_t SDAC_HYS : 2;
} SLICE;

// Register 0x06: Control0
typedef struct {
    uint8_t TX_START : 1;
    uint8_t AUTO_PRE : 1;
    uint8_t HOST_CUR : 2;
    uint8_t RESERVED : 1;
    uint8_t INT_MASK : 1;
    uint8_t TX_FLUSH : 1;
    uint8_t RESERVED_2 : 1;
} CONTROL0;

// Register 0x07: Control1
typedef struct {
    uint8_t ENSOP1 : 1;
    uint8_t ENSOP2 : 1;
    uint8_t RX_FLUSH : 1;
    uint8_t BIST_MODE2 : 1;
    uint8_t ENSOP1DB : 1;
    uint8_t ENSOP2DB : 1;
    uint8_t RESERVED : 2;
} CONTROL1;

// Register 0x08: Control2
typedef struct {
    uint8_t TOGGLE : 1;
    uint8_t MODE : 2;
    uint8_t WAKE_EN : 1;
    uint8_t RESERVED : 1;
    uint8_t TOG_RD_ONLY : 1;
    uint8_t TOG_SAVE_PWR : 2;
} CONTROL2;

// Register 0x09: Control3
typedef struct {
    uint8_t AUTO_RETRY : 1;
    uint8_t N_RETRIES : 2;
    uint8_t AUTO_SOFTRESET : 1;
    uint8_t AUTO_HARDRESET : 1;
    uint8_t BIST_TMODE : 1;
    uint8_t SEND_HARD_RESET : 1;
    uint8_t RESERVED : 1;
} CONTROL3;

// Register 0x0A: Mask
typedef struct {
    uint8_t M_BC_LVL : 1;
    uint8_t M_COLLISION : 1;
    uint8_t M_WAKE : 1;
    uint8_t M_ALERT : 1;
    uint8_t M_CRC_CHK : 1;
    uint8_t M_COMP_CHNG : 1;
    uint8_t M_ACTIVITY : 1;
    uint8_t M_VBUSOK : 1;
} MASK;

// Register 0x0B: Power
typedef struct {
    uint8_t PWR_ : 4;
    uint8_t RESERVED : 4;
} POWER;

// Register 0x0C: Reset
typedef struct {
    uint8_t SW_RES : 1;
    uint8_t PD_RESET : 1;
    uint8_t RESERVED : 6;
} RESET;

// Register 0x0D: OCPreg
typedef struct {
    uint8_t OCP_CUR : 3;
    uint8_t OCP_RANGE : 1;
    uint8_t RESERVED : 4;
} OCPREG;

// Register 0x0E: Maska
typedef struct {
    uint8_t M_HARDRST : 1;
    uint8_t M_SOFTRST : 1;
    uint8_t M_TXSENT : 1;
    uint8_t M_HARDSENT : 1;
    uint8_t M_RETRYFAIL : 1;
    uint8_t M_SOFTFAIL : 1;
    uint8_t M_TOGDONE : 1;
    uint8_t M_OCP_TEMP : 1;
} MASKA;

// Register 0x0F: Maskb
typedef struct {
    uint8_t M_GCRCSENT : 1;
    uint8_t RESERVED : 7;
} MASKB;

// Register 0x10: Control4
typedef struct {
    uint8_t TOG_EXIT_AUD : 1;
    uint8_t RESERVED : 7;
} CONTROL4;

// Register 0x3C: Status0a
typedef struct {
    uint8_t HARDRST : 1;
    uint8_t SOFTRST : 1;
    uint8_t POWER : 2;
    uint8_t RETRYFAIL : 1;
    uint8_t SOFTFAIL : 1;
    uint8_t RESERVED : 2;
} STATUS0A;

// Register 0x3D: Status1a
typedef struct {
    uint8_t RXSOP : 1;
    uint8_t RXSOP1DB : 1;
    uint8_t RXSOP2DB : 1;
    uint8_t TOGSS : 3;
    uint8_t RESERVED : 2;
} STATUS1A;

// Register 0x3E: Interrupta
typedef struct {
    uint8_t I_HARDRST : 1;
    uint8_t I_SOFTRST : 1;
    uint8_t I_TXSENT : 1;
    uint8_t I_HARDSENT : 1;
    uint8_t I_RETRYFAIL : 1;
    uint8_t I_SOFTFAIL : 1;
    uint8_t I_TOGDONE : 1;
    uint8_t I_OCP_TEMP : 1;
} INTERRUPTA;

// Register 0x3F: Interruptb
typedef struct {
    uint8_t I_GCRCSENT : 1;
    uint8_t RESERVED : 7;
} INTERRUPTB;

// Register 0x40: Status0
typedef struct {
    uint8_t BC_LVL : 2;
    uint8_t WAKE : 1;
    uint8_t ALERT : 1;
    uint8_t CRC_CHK : 1;
    uint8_t COMP : 1;
    uint8_t ACTIVITY : 1;
    uint8_t VBUSOK : 1;
} STATUS0;

// Register 0x41: Status1
typedef struct {
    uint8_t OCP : 1;
    uint8_t OVRTEMP : 1;
    uint8_t TX_FULL : 1;
    uint8_t TX_EMPTY : 1;
    uint8_t RX_FULL : 1;
    uint8_t RX_EMPTY : 1;
    uint8_t RXSOP1 : 1;
    uint8_t RXSOP2 : 1;
} STATUS1;

// Register 0x42: Interrupt
typedef struct {
    uint8_t I_BC_LVL : 1;
    uint8_t I_COLLISION : 1;
    uint8_t I_WAKE : 1;
    uint8_t I_ALERT : 1;
    uint8_t I_CRC_CHK : 1;
    uint8_t I_COMP_CHNG : 1;
    uint8_t I_ACTIVITY : 1;
    uint8_t I_VBUSOK : 1;
} INTERRUPT;

static EventCallback _callback;
static void* _context;

static void int_cb(void* context) {
    INTERRUPT interrupt;
    if(!HalI2C::read_mem(
           0x22, 0x42, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&interrupt), 1), 200000)) {
        Debug::error("I2C", "FUSB302B interrupt read failed");
        return;
    } else {
        Debug::info(
            "I2C",
            "FUSB302B interrupt: I_BC_LVL: %d, I_COLLISION: %d, I_WAKE: %d, I_ALERT: %d, "
            "I_CRC_CHK: %d, I_COMP_CHNG: %d, I_ACTIVITY: %d, I_VBUSOK: %d",
            interrupt.I_BC_LVL,
            interrupt.I_COLLISION,
            interrupt.I_WAKE,
            interrupt.I_ALERT,
            interrupt.I_CRC_CHK,
            interrupt.I_COMP_CHNG,
            interrupt.I_ACTIVITY,
            interrupt.I_VBUSOK);
    }

    INTERRUPTA interrupta;
    if(!HalI2C::read_mem(
           0x22, 0x3E, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&interrupta), 1), 200000)) {
        Debug::error("I2C", "FUSB302B interrupta read failed");
        return;
    } else {
        Debug::info(
            "I2C",
            "FUSB302B interrupta: I_HARDRST: %d, I_SOFTRST: %d, I_TXSENT: %d, I_HARDSENT: %d, "
            "I_RETRYFAIL: %d, I_SOFTFAIL: %d, I_TOGDONE: %d, I_OCP_TEMP: %d",
            interrupta.I_HARDRST,
            interrupta.I_SOFTRST,
            interrupta.I_TXSENT,
            interrupta.I_HARDSENT,
            interrupta.I_RETRYFAIL,
            interrupta.I_SOFTFAIL,
            interrupta.I_TOGDONE,
            interrupta.I_OCP_TEMP);
    }

    INTERRUPTB interruptb;
    if(!HalI2C::read_mem(
           0x22, 0x3F, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&interruptb), 1), 200000)) {
        Debug::error("I2C", "FUSB302B interruptb read failed");
        return;
    } else {
        Debug::info("I2C", "FUSB302B interruptb: I_GCRCSENT: %d", interruptb.I_GCRCSENT);
    }

    STATUS0 status0;
    if(!HalI2C::read_mem(
           0x22, 0x40, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&status0), 1), 200000)) {
        Debug::error("I2C", "FUSB302B status0 read failed");
        return;
    } else {
        Debug::info(
            "I2C",
            "FUSB302B status0: BC_LVL: %d, WAKE: %d, ALERT: %d, CRC_CHK: %d, COMP: %d, "
            "ACTIVITY: %d, VBUSOK: %d",
            status0.BC_LVL,
            status0.WAKE,
            status0.ALERT,
            status0.CRC_CHK,
            status0.COMP,
            status0.ACTIVITY,
            status0.VBUSOK);
    }
}

bool init(HalGpio& int_gpio, EventCallback callback, void* context) {
    FUSB302::_callback = callback;
    FUSB302::_context = context;

    DEVICE_ID device_id = {0};

    if(HalI2C::read_mem(
           0x22, 0x01, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&device_id), 1), 200000)) {
        Debug::info(
            "I2C",
            "FUSB302B found, REG 0x01: REVISION_ID: %d, PRODUCT_ID: %d, VERSION_ID: %d",
            device_id.REVISION_ID,
            device_id.PRODUCT_ID,
            device_id.VERSION_ID);
    } else {
        Debug::error("I2C", "FUSB302B not found");
        return false;
    }

    int_gpio.config(HalGpio::Mode::InterruptFall, HalGpio::Pull::Up);
    int_gpio.set_interrupt_callback(int_cb, NULL);

    CONTROL0 control0;
    memset(&control0, 0x24, sizeof(control0));
    control0.HOST_CUR = 0b01;
    control0.INT_MASK = 0;
    if(!HalI2C::write_mem(
           0x22, 0x06, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&control0), 1), 200000)) {
        Debug::error("I2C", "FUSB302B control0 write failed");
        return false;
    } else {
        uint8_t reg = 0;
        if(!HalI2C::read_mem(0x22, 0x06, &reg, 200000)) {
            Debug::error("I2C", "FUSB302B control0 read failed");
            return false;
        } else {
            Debug::info("I2C", "FUSB302B control0: 0x%02X", reg);
        }
    }

    SWITCHES0 switches0;
    memset(&switches0, 0x03, sizeof(switches0));
    switches0.VCONN_CC1 = 0;
    switches0.VCONN_CC2 = 0;
    if(!HalI2C::write_mem(0x22, 0x02, reinterpret_cast<uint8_t*>(&switches0), 200000)) {
        Debug::error("I2C", "FUSB302B switches0 write failed");
        return false;
    } else {
        uint8_t reg = 0;
        if(!HalI2C::read_mem(0x22, 0x02, &reg, 200000)) {
            Debug::error("I2C", "FUSB302B switches0 read failed");
            return false;
        } else {
            Debug::info("I2C", "FUSB302B switches0: 0x%02X", reg);
        }
    }

    MASK mask;
    memset(&mask, 0x00, sizeof(mask));
    if(!HalI2C::write_mem(
           0x22, 0x0A, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&mask), 1), 200000)) {
        Debug::error("I2C", "FUSB302B mask write failed");
        return false;
    }

    MASKA maska;
    memset(&maska, 0x00, sizeof(maska));
    if(!HalI2C::write_mem(
           0x22, 0x0E, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&maska), 1), 200000)) {
        Debug::error("I2C", "FUSB302B maska write failed");
        return false;
    }

    MASKB maskb;
    memset(&maskb, 0x00, sizeof(maskb));
    if(!HalI2C::write_mem(
           0x22, 0x0F, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&maskb), 1), 200000)) {
        Debug::error("I2C", "FUSB302B maskb write failed");
        return false;
    }

    POWER power;
    memset(&power, 0x0F, sizeof(power));
    if(!HalI2C::write_mem(
           0x22, 0x0B, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&power), 1), 200000)) {
        Debug::error("I2C", "FUSB302B power write failed");
        return false;
    }

    {
        INTERRUPT interrupt;
        if(!HalI2C::read_mem(
               0x22, 0x42, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&interrupt), 1), 200000)) {
            Debug::error("I2C", "FUSB302B interrupt read failed");
            return false;
        } else {
            Debug::info(
                "I2C",
                "FUSB302B interrupt: I_BC_LVL: %d, I_COLLISION: %d, I_WAKE: %d, I_ALERT: %d, "
                "I_CRC_CHK: %d, I_COMP_CHNG: %d, I_ACTIVITY: %d, I_VBUSOK: %d",
                interrupt.I_BC_LVL,
                interrupt.I_COLLISION,
                interrupt.I_WAKE,
                interrupt.I_ALERT,
                interrupt.I_CRC_CHK,
                interrupt.I_COMP_CHNG,
                interrupt.I_ACTIVITY,
                interrupt.I_VBUSOK);
        }

        INTERRUPTA interrupta;
        if(!HalI2C::read_mem(
               0x22,
               0x3E,
               std::span<uint8_t>(reinterpret_cast<uint8_t*>(&interrupta), 1),
               200000)) {
            Debug::error("I2C", "FUSB302B interrupta read failed");
            return false;
        } else {
            Debug::info(
                "I2C",
                "FUSB302B interrupta: I_HARDRST: %d, I_SOFTRST: %d, I_TXSENT: %d, I_HARDSENT: %d, "
                "I_RETRYFAIL: %d, I_SOFTFAIL: %d, I_TOGDONE: %d, I_OCP_TEMP: %d",
                interrupta.I_HARDRST,
                interrupta.I_SOFTRST,
                interrupta.I_TXSENT,
                interrupta.I_HARDSENT,
                interrupta.I_RETRYFAIL,
                interrupta.I_SOFTFAIL,
                interrupta.I_TOGDONE,
                interrupta.I_OCP_TEMP);
        }

        INTERRUPTB interruptb;
        if(!HalI2C::read_mem(
               0x22,
               0x3F,
               std::span<uint8_t>(reinterpret_cast<uint8_t*>(&interruptb), 1),
               200000)) {
            Debug::error("I2C", "FUSB302B interruptb read failed");
            return false;
        } else {
            Debug::info("I2C", "FUSB302B interruptb: I_GCRCSENT: %d", interruptb.I_GCRCSENT);
        }
    }

    CONTROL2 control2;
    memset(&control2, 0x02, sizeof(control2));
    control2.TOGGLE = 1;
    control2.TOG_SAVE_PWR = 0b00;

    if(!HalI2C::write_mem(
           0x22, 0x10, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&control2), 1), 200000)) {
        Debug::error("I2C", "FUSB302B control2 write failed");
        return false;
    } else {
        uint8_t reg = 0;
        if(!HalI2C::read_mem(0x22, 0x10, &reg, 200000)) {
            Debug::error("I2C", "FUSB302B control2 read failed");
            return false;
        } else {
            Debug::info("I2C", "FUSB302B control2: 0x%02X", reg);
        }
    }

    return true;
}

}; // namespace FUSB302