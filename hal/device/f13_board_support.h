#ifndef F13_BOARD_SUPPORT_H
#define F13_BOARD_SUPPORT_H

/* QB-R5F10BMG-TB clock profile: X1 4 MHz -> PLL/CPU 32 MHz.
 * ClockPrepare runs before C RAM initialization. ClockStart uses bounded
 * oscillator/PLL waits and preserves an inherited valid Boot clock.
 */
#define F13_BOARD_CPU_MHZ (32U)
#define F13_BOARD_CLOCK_OK (0U)
#define F13_BOARD_CLOCK_CONFIG_ERROR (1U)
#define F13_BOARD_CLOCK_X1_TIMEOUT (2U)
#define F13_BOARD_CLOCK_MAIN_TIMEOUT (3U)
#define F13_BOARD_CLOCK_PLL_TIMEOUT (4U)
#define F13_BOARD_CLOCK_SELECT_TIMEOUT (5U)
#define F13_BOARD_CLOCK_FALLBACK_ERROR (6U)

#ifndef F13_BOARD_CLOCK_POLL_LIMIT
#define F13_BOARD_CLOCK_POLL_LIMIT (2000000UL)
#endif

void F13_BoardClockPrepare(void);
unsigned char F13_BoardClockStart(void);
unsigned char F13_BoardClockReady(void);

#endif
