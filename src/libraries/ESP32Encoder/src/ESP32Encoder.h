#pragma once
#include <driver/gpio.h>
#if __has_include(<esp_idf_version.h>)
#include <esp_idf_version.h>
#endif
#include <soc/soc_caps.h>

#if defined(ESP_IDF_VERSION) && defined(ESP_IDF_VERSION_VAL) && ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#define ESP32ENCODER_USE_NEW_PCNT 1
#include <driver/pulse_cnt.h>
#else
#define ESP32ENCODER_USE_NEW_PCNT 0
#include <driver/pcnt.h>
#endif
#ifndef ARDUINO
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portable.h>
#include <freertos/semphr.h>
#endif

#if ESP32ENCODER_USE_NEW_PCNT
#define MAX_ESP32_ENCODERS SOC_PCNT_UNITS_PER_GROUP
#else
#define MAX_ESP32_ENCODERS PCNT_UNIT_MAX
#endif
#define 	_INT16_MAX 32766
#define  	_INT16_MIN -32766
#define ISR_CORE_USE_DEFAULT (0xffffffff)

enum class encType {
	single,
	half,
	full
};

enum class puType {
	up,
	down,
	none
};

class ESP32Encoder;

typedef void (*enc_isr_cb_t)(void*);

class ESP32Encoder {
public:
	/**
	 * @brief Construct a new ESP32Encoder object
	 *
	 * @param always_interrupt set to true to enable interrupt on every encoder pulse, otherwise false
	 * @param enc_isr_cb callback executed on every encoder ISR, gets a pointer to
	 * 	the ESP32Encoder instance as an argument, no effect if always_interrupt is
	 * 	false
	 */
	ESP32Encoder(bool always_interrupt=false, enc_isr_cb_t enc_isr_cb=nullptr, void* enc_isr_cb_data=nullptr);
	~ESP32Encoder();
	void attachHalfQuad(int aPintNumber, int bPinNumber);
	void attachFullQuad(int aPintNumber, int bPinNumber);
	void attachSingleEdge(int aPintNumber, int bPinNumber);
	int64_t getCount();
	int64_t clearCount();
	int64_t pauseCount();
	int64_t resumeCount();
	void detach();
	[[deprecated("Replaced by detach")]] void detatch();
	bool isAttached(){return attached;}
	void setCount(int64_t value);
	void setFilter(uint16_t value);
	static ESP32Encoder *encoders[MAX_ESP32_ENCODERS];
	bool always_interrupt;
	gpio_num_t aPinNumber;
	gpio_num_t bPinNumber;
#if ESP32ENCODER_USE_NEW_PCNT
	int unit;
	pcnt_unit_config_t r_enc_config;
#else
	pcnt_unit_t unit;
	pcnt_config_t r_enc_config;
#endif
	int countsMode = 2;
	volatile int64_t count=0;
	static puType useInternalWeakPullResistors;
	static uint32_t isrServiceCpuCore;
	enc_isr_cb_t _enc_isr_cb;
	void* _enc_isr_cb_data;

private:
	static bool attachedInterrupt;
	void attach(int aPintNumber, int bPinNumber, encType et);
	int64_t getCountRaw();
#if ESP32ENCODER_USE_NEW_PCNT
	static bool pcntCallback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx);
	pcnt_unit_handle_t unitHandle = nullptr;
	pcnt_channel_handle_t channelHandles[2] = {nullptr, nullptr};
#endif
	bool attached;
  bool direction;
  bool working;
};

//Added by Sloeber
#pragma once
