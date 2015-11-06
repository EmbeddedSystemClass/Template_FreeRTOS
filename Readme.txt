****2015-11-06****

Feature
1. FreeRTOS is implmented.
2. The STM32Cube's i2c driver is used.
3. BSP driver is excluded

The issue need to be updated:
1. LED driver
2. Overwriting the systick setting. choose one function from HAL_init or xPortSysTickHandler

	