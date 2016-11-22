/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
#include "../_can_dbc/generated_can.h"
#include "string.h"
#include "stdio.h"
#include "motor.hpp"
#include "lpc_pwm.hpp"


#define FW                           6.4
#define DC_STOP                      7.0

/**
 * todo: avoid global variables.
 */
int pwm_input = 6.4;

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_DISPATCHER_TASK_STACK_SIZE_BYTES = (512 * 3);


bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
	can_msg_t can_msg = { 0 };
	can_msg.msg_id= mid;
	can_msg.frame_fields.data_len = dlc;
	memcpy(can_msg.data.bytes, bytes, dlc);

	// printf("sending\n");
	return CAN_tx(can1, &can_msg, 0);

}
/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
	int c=0;
	while(c < 20)
	{
		MotorControl.setDC(DC_STOP);
		delay_ms(50);
		c++;
	}

	delay_ms(20);
	CAN_init(can1, 100, 5, 5, 0, 0);
	CAN_reset_bus(can1);

	//RX PART
	CAN_bypass_filter_accept_all_msgs();

	CAN_reset_bus(can1);
	//	printf("can bus initialized\n");
	return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
	// Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
	return true; // Must return true upon success
}


/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */

void period_1Hz(uint32_t count)
{

	if(CAN_is_bus_off(can1))
	{
		puts(" Bus OFF ");
		CAN_reset_bus(can1);
	}

	//	if(SW.getSwitch(1))
	//	{
	//	//	if(count%2)
	//		//{
	//			dc_accelerate(pwm_input);
	//		//	printf("%d\n",pwm_input);
	//			//if(pwm_input>5.7)
	//				//pwm_input-=0.1;
	//		//}
	//
	//	}
	//	else if(SW.getSwitch(2))
	//		{
	//		dc_stop();
	//		}
	//	MOTORIO_HEARTBEAT_t motorio_heartbeat={0};
	//motorio_heartbeat.MOTORIO_HEARTBEAT_data=1;
	//	dbc_encode_and_send_MOTORIO_HEARTBEAT(&motorio_heartbeat);
}

void period_10Hz(uint32_t count)
{
	//drive_car();

	if(SW.getSwitch(1))
	{
		dc_accelerate(6.4); // todo: avoid magic numbers. You already defined this constant above.
	}
	else if(SW.getSwitch(2))
	{
		dc_stop();
	}


	//	if(dbc_handle_mia_RESET(&reset_cmd_msg, 1))
	//
	//	{
	//		LE.on(1);
	//		reset_cmd_msg.RESET_data=RESET__MIA_MSG.RESET_data;
	//
	//		LE.on(1);
	//
	//	}
	//	if(dbc_handle_mia_MOTORIO_DIRECTION(&mDirection_cmd_msg, 1))
	//
	//	{
	//		LE.on(1);
	//		mDirection_cmd_msg.MOTORIO_DIRECTION_direction=MOTORIO_DIRECTION__MIA_MSG.MOTORIO_DIRECTION_direction;
	//
	//
	//		LE.on(1);
	//		LD.setNumber(41);
	//
	//
	//	}
	//	LE.toggle(2);


}

void period_100Hz(uint32_t count)
{
	//LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
	//LE.toggle(4);
}
