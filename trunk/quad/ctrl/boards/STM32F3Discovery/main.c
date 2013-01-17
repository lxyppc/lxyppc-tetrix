/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F3-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x

#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	      /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

/* Private variables ---------------------------------------------------------*/
  RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
__IO uint32_t UserButtonPressed = 0;
__IO float HeadingValue = 0.0f;  
float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};
uint8_t Xval, Yval = 0x00;

__IO uint8_t DataReady = 0;
extern __IO uint8_t PrevXferComplete;
__IO uint32_t USBConnectTimeOut = 100;

float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f, RollAng = 0.0f, PitchAng = 0.0f;
float fTiltedX,fTiltedY = 0.0f;
const uint8_t nrf_addr[] = RX_ADDR0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* callback functions */
void nrf_tx_done(uint8_t success)
{
}

void nrf_on_rx_data(const void* data, uint32_t len, uint8_t channel)
{
}

static uint8_t current_mode = DT_ATT;
void usb_get_data(const void* p, uint32_t len)
{
    const uint8_t* data = (const uint8_t*)p;
    if(data[0] == CMD_MODE){
        current_mode = data[1];
    }
}

static int32_t gyro200Hz[3];
static int32_t gyro200HzSum[3];
#define  GYRO_FREQ      200
static uint8_t gyro_flag = 0;
static uint32_t gyro_count = 0;
void gyro_ready(const int16_t* data, uint32_t freq)
{
    static uint32_t gyroSumCnt = 0;
    gyro200Hz[0] += data[0];
    gyro200Hz[1] += data[1];
    gyro200Hz[2] += data[2];
    gyroSumCnt++;
    if(gyroSumCnt >=  (freq + GYRO_FREQ-1)/GYRO_FREQ){
        gyro200HzSum[0] = gyro200Hz[0];
        gyro200HzSum[1] = gyro200Hz[1];
        gyro200HzSum[2] = gyro200Hz[2];
        gyroSumCnt = 0;
        gyro200Hz[0] = 0;
        gyro200Hz[1] = 0;
        gyro200Hz[2] = 0;
        gyro_count = gyroSumCnt;
        gyro_flag = 1;
    }
}

void l3gd20_int2_irq_handler(void)
{
    static uint32_t last_us = 0;
    uint32_t cur_us = current_us();
    static __IO uint32_t d_us;
    static __IO uint32_t d_us2;
    d_us = cur_us - last_us;
    last_us = cur_us;
    if(current_mode == DT_ATT)
    {
        int16_t gyro[3],acc[3],mag[3];
        read_raw_gyro(gyro);
        //read_raw_acc(acc);
        //read_raw_mag(mag);
    }
    d_us2 = current_us() - last_us;
    d_us2++;
}

extern uint8_t frame_100Hz;
extern uint8_t frame_200Hz;
extern uint8_t frame_1Hz;
void prepare_rc_data(uint8_t* data)
{
    data[0] = DT_RCDATA;
    data[1] = get_pwm_values((uint16_t*)(data+2),8);
}
/**
  * @brief  Main program.
  * @param  None 
  * @retval None
  */
int main(void)
{
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    setup_systick();
    enable_tick_count();
    setup_io_leds();
    setup_io_usb();
    
    GYRO_INIT();
    ACC_INIT();
    MAG_INIT();
    nrf_init();
    nrf_detect();
    nrf_rx_mode_dual(nrf_addr, 5, 40);
    {
        uint8_t status = nrf_read_reg(NRF_STATUS);
        nrf_write_reg(NRF_FLUSH_RX, 0xff);
        nrf_write_reg(NRF_FLUSH_TX, 0xff);
        nrf_write_reg(NRF_WRITE_REG|NRF_STATUS,status); // clear IRQ flags
    }
    pwm_input_init();
    /* Initialize LEDs and User Button available on STM32F3-Discovery board */
    //STM_EVAL_LEDInit(LED3);
    //STM_EVAL_LEDInit(LED4);
    //STM_EVAL_LEDInit(LED5);
    //STM_EVAL_LEDInit(LED6);
    //STM_EVAL_LEDInit(LED7);
    //STM_EVAL_LEDInit(LED8);
    //STM_EVAL_LEDInit(LED9);
    //STM_EVAL_LEDInit(LED10);
  
    //STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI); 
    
    USB_Init();
    
    // wait usb ready
    //while ((bDeviceState != CONFIGURED)&&(USBConnectTimeOut != 0))
    //{}
    
    // endless loop
    while(1)
    {
        if(frame_100Hz){
            uint8_t buf[64];
            frame_100Hz = 0;
            buf[0] = 0;
            if(current_mode == DT_RCDATA){
                prepare_rc_data(buf);
                usb_send_data(buf,64);
            }else if(current_mode == DT_SENSOR){
                int16_t gyro[3],acc[3],mag[3];
                read_raw_gyro(gyro);
                read_raw_acc(acc);
                read_raw_mag(mag);
                buf[0] = DT_SENSOR;
                memcpy(buf+1, gyro, 6);
                memcpy(buf+1+6, acc, 6);
                memcpy(buf+1+12, mag, 6);
                usb_send_data(buf,64);
            }else if(current_mode == DT_ATT){
                if(L3GD20_INT2){
                    int16_t gyro[3];
                    read_raw_gyro(gyro);
                }
            }
            if(buf[0]){
                usb_send_data(buf,64);
            }
        }
        
        if(gyro_flag){
            gyro_flag = 0;
        }
        if(frame_200Hz){
            frame_200Hz = 0;
        }
        if(frame_1Hz){
            frame_1Hz = 0;
            LED3_TOGGLE;
        }
    }
    
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
