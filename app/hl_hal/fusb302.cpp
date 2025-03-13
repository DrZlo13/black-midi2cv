#include "fusb302.h"
#include <cstring>
#include <hal/hal.h>
#include "debug.h"

namespace FUSB302 {

const uint8_t I2C_ADDR = 0x22;

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

template <typename T> class Reg {
public:
    const uint8_t _address;
    T value = {0};

    Reg(uint8_t address)
        : _address(address) {
    }

    bool read(void) {
        bool result = HalI2C::read_mem(
            I2C_ADDR, _address, std::span(reinterpret_cast<uint8_t*>(&value), sizeof(T)), 200000);
        if(!result) {
            Debug::error("I2C", "FUSB302B read %02X failed", _address);
        }
        return result;
    }

    bool write(void) {
        bool result = HalI2C::write_mem(
            I2C_ADDR, _address, std::span(reinterpret_cast<uint8_t*>(&value), sizeof(T)), 200000);
        if(!result) {
            Debug::error("I2C", "FUSB302B write %02X failed", _address);
        }
        return result;
    }
};

// static bool manual_toggle(void) {
//     Reg<RESET> reset_reg(0x0C);
//     reset_reg.value.SW_RES = 1;
//     reset_reg.write();

//     HalCortex::delay_us(1000);

//     Reg<DEVICE_ID> device_id_reg(0x01);
//     device_id_reg.read();
//     Debug::info(
//         "I2C",
//         "FUSB302B found, REG 0x01: REVISION_ID: %d, PRODUCT_ID: %d, VERSION_ID: %d",
//         device_id_reg.value.REVISION_ID,
//         device_id_reg.value.PRODUCT_ID,
//         device_id_reg.value.VERSION_ID);

//     Reg<POWER> power_reg(0x0B);
//     power_reg.value.PWR_ = 0x0F;
//     power_reg.write();

//     Reg<CONTROL0> control0_reg(0x06);
//     control0_reg.read();
//     control0_reg.value.HOST_CUR = 0b01;
//     control0_reg.write();

//     while(true) {
//         Reg<SWITCHES0> switches0_backup_reg(0x02);
//         switches0_backup_reg.read();

//         // Measure CC1
//         {
//             Reg<SWITCHES0> switches0_reg(0x02);
//             switches0_reg.value.PU_EN1 = 1;
//             switches0_reg.value.PU_EN2 = 1;
//             switches0_reg.value.PDWN1 = 0;
//             switches0_reg.value.PDWN2 = 0;
//             switches0_reg.value.MEAS_CC1 = 1;
//             switches0_reg.write();

//             HalCortex::delay_us(10000);

//             Reg<STATUS0> status0_reg(0x40);
//             status0_reg.read();
//             Debug::info("I2C", "FUSB302B status0: BC_LVL: %d", status0_reg.value.BC_LVL);

//             switches0_reg.value.MEAS_CC1 = 0;
//             switches0_reg.write();
//         }

//         // Measure CC2
//         {
//             Reg<SWITCHES0> switches0_reg(0x02);
//             switches0_reg.value.PU_EN1 = 1;
//             switches0_reg.value.PU_EN2 = 1;
//             switches0_reg.value.PDWN1 = 0;
//             switches0_reg.value.PDWN2 = 0;
//             switches0_reg.value.MEAS_CC2 = 1;
//             switches0_reg.write();

//             HalCortex::delay_us(10000);

//             Reg<STATUS0> status0_reg(0x40);
//             status0_reg.read();
//             Debug::info("I2C", "FUSB302B status0: BC_LVL: %d", status0_reg.value.BC_LVL);

//             switches0_reg.value.MEAS_CC2 = 0;
//             switches0_reg.write();
//         }

//         switches0_backup_reg.write();

//         HalCortex::delay_us(1000000);

//         // Measure VBUS
//         {
//             Reg<SWITCHES0> switches0_reg(0x02);
//             switches0_reg.value.PU_EN1 = 0;
//             switches0_reg.value.PU_EN2 = 0;
//             switches0_reg.value.PDWN1 = 1;
//             switches0_reg.value.PDWN2 = 1;
//             switches0_reg.write();

//             Reg<MEASURE> measure_reg(0x04);
//             measure_reg.read();
//             measure_reg.value.MEAS_VBUS = 1;
//             measure_reg.write();

//             HalCortex::delay_us(1000000);

//             Reg<STATUS0> status0_reg(0x40);
//             status0_reg.read();
//             Debug::info(
//                 "I2C",
//                 "FUSB302B status0: BC_LVL: %d, WAKE: %d, ALERT: %d, CRC_CHK: %d, COMP: %d, "
//                 "ACTIVITY: %d, VBUSOK: %d",
//                 status0_reg.value.BC_LVL,
//                 status0_reg.value.WAKE,
//                 status0_reg.value.ALERT,
//                 status0_reg.value.CRC_CHK,
//                 status0_reg.value.COMP,
//                 status0_reg.value.ACTIVITY,
//                 status0_reg.value.VBUSOK);

//             measure_reg.value.MEAS_VBUS = 0;
//             measure_reg.write();
//         }

//         HalCortex::delay_us(1000000);
//     }

//     return true;
// }

enum State {
    MeasureCC1Start,
    MeasureCC1End,
    MeasureCC2Start,
    MeasureCC2End,
    MeasureVBUSStart,
    MeasureVBUSMid,
    MeasureVBUSEnd,
};

static State state = MeasureCC1Start;
static Reg<SWITCHES0> switches0_backup_reg(0x02);

static void measure_cc1_start(void) {
    switches0_backup_reg.read();

    Reg<SWITCHES0> switches0_reg(0x02);
    switches0_reg.value.PU_EN1 = 1;
    switches0_reg.value.PU_EN2 = 1;
    switches0_reg.value.PDWN1 = 0;
    switches0_reg.value.PDWN2 = 0;
    switches0_reg.value.MEAS_CC1 = 1;
    switches0_reg.write();
}

static uint8_t measure_cc1_end(void) {
    Reg<STATUS0> status0_reg(0x40);
    status0_reg.read();

    switches0_backup_reg.write();
    return status0_reg.value.BC_LVL;
}

static void measure_cc2_start(void) {
    switches0_backup_reg.read();

    Reg<SWITCHES0> switches0_reg(0x02);
    switches0_reg.value.PU_EN1 = 1;
    switches0_reg.value.PU_EN2 = 1;
    switches0_reg.value.PDWN1 = 0;
    switches0_reg.value.PDWN2 = 0;
    switches0_reg.value.MEAS_CC2 = 1;
    switches0_reg.write();
}

static uint8_t measure_cc2_end(void) {
    Reg<STATUS0> status0_reg(0x40);
    status0_reg.read();

    switches0_backup_reg.write();
    return status0_reg.value.BC_LVL;
}

static void measure_vbus_start(void) {
    Reg<SWITCHES0> switches0_reg(0x02);
    switches0_reg.value.PU_EN1 = 0;
    switches0_reg.value.PU_EN2 = 0;
    switches0_reg.value.PDWN1 = 1;
    switches0_reg.value.PDWN2 = 1;
    switches0_reg.write();
}

static void measure_vbus_mid(void) {
    Reg<MEASURE> measure_reg(0x04);
    measure_reg.read();
    measure_reg.value.MEAS_VBUS = 1;
    measure_reg.write();
}

static uint8_t measure_vbus_end(void) {
    Reg<STATUS0> status0_reg(0x40);
    status0_reg.read();

    Reg<MEASURE> measure_reg(0x04);
    measure_reg.read();
    measure_reg.value.MEAS_VBUS = 0;
    measure_reg.write();

    return status0_reg.value.VBUSOK;
}

uint8_t cc1_level = 0;
uint8_t cc2_level = 0;
uint8_t vbus_valid = 0;

EventType poll(void) {
    switch(state) {
    case MeasureCC1Start:
        state = MeasureCC1End;
        measure_cc1_start();
        break;
    case MeasureCC1End:
        state = MeasureCC2Start;
        cc1_level = measure_cc1_end();
        break;
    case MeasureCC2Start:
        state = MeasureCC2End;
        measure_cc2_start();
        break;
    case MeasureCC2End:
        state = MeasureVBUSStart;
        cc2_level = measure_cc2_end();
        break;
    case MeasureVBUSStart:
        state = MeasureVBUSMid;
        measure_vbus_start();
        break;
    case MeasureVBUSMid:
        state = MeasureVBUSEnd;
        measure_vbus_mid();
        break;
    case MeasureVBUSEnd:
        state = MeasureCC1Start;
        vbus_valid = measure_vbus_end();

        if(vbus_valid == 1) {
            return EventType::ConnectedHost;
        } else if(cc1_level == 1 || cc2_level == 1) {
            return EventType::ConnectedDevice;
        }
        break;
    }

    return EventType::None;
}

bool init(void) {
    Reg<RESET> reset_reg(0x0C);
    reset_reg.value.SW_RES = 1;
    reset_reg.write();

    HalCortex::delay_us(1000);

    Reg<DEVICE_ID> device_id_reg(0x01);
    if(!device_id_reg.read()) {
        Debug::error("I2C", "FUSB302B not found");
        return false;
    }

    Debug::info(
        "I2C",
        "FUSB302B found, REG 0x01: REVISION_ID: %d, PRODUCT_ID: %d, VERSION_ID: %d",
        device_id_reg.value.REVISION_ID,
        device_id_reg.value.PRODUCT_ID,
        device_id_reg.value.VERSION_ID);

    Reg<POWER> power_reg(0x0B);
    power_reg.value.PWR_ = 0x0F;
    power_reg.write();

    Reg<CONTROL0> control0_reg(0x06);
    control0_reg.read();
    control0_reg.value.HOST_CUR = 0b01;
    control0_reg.write();

    return true;
}

}; // namespace FUSB302