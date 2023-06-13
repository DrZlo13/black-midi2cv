#include <stdint.h>
#include <cstddef>

namespace UsbMidi {
/**
 * @brief Callback function for receiving MIDI data.
 */
typedef void (*Callback)(const uint8_t* data, size_t size);

/**
 * @brief Send RAW USB-MIDI data.
 * @param data Pointer to the data to send.
 * @param size Size of the data to send.
 */
void send(const uint8_t* data, size_t size);

/**
 * @brief Send a MIDI system exclusive message.
 * @param cable_id Cable ID.
 * @param data Pointer to the data to send.
 * @param size Size of the data to send.
 */
void send_sysex(uint8_t cable_id, const uint8_t* data, size_t size);

/**
 * @brief Set the callback function for receiving MIDI data.
 * @param callback Callback function.
 */
void set_receive_callback(Callback callback);

}
