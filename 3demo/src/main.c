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
#include "pressure.h"
#include "quadrature_encoder.h"

#include "ili/hw_ili9341.h"
#include "ili/UTFT.h"

#ifdef PCD_DISPLAY
#include "pcd/pcd8544.h"
#include "pcd/hw_pcd8544.h"
#endif//PCD_DISPLAY

#include "ad9958_drv.h"

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
#define PI                         (float)     3.14159265f

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

/* Private variables ---------------------------------------------------------*/
  RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
__IO uint32_t UserButtonPressed = 0;
__IO float HeadingValue = 0.0f;  
static float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};
uint8_t Xval, Yval = 0x00;

__IO uint8_t DataReady = 0;
__IO uint32_t USBConnectTimeOut = 100;

float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f, RollAng = 0.0f, PitchAng = 0.0f;
float fTiltedX,fTiltedY = 0.0f;

void mainPcd8544();
void mainOriginal();
void mainIli9341();
void main9958();

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None 
  * @retval None
  */

int main(void)
{  
  // SysTick end of count event each 10ms
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  //mainOriginal();
  //mainPcd8544();
  mainIli9341();
  //main9958();
}

extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

void main9958()
{
  // SysTick end of count event each 1ms
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);

  STM_EVAL_LEDOn(LED3);
  AD9958_Init();
  STM_EVAL_LEDOn(LED4);
  Delay(5);

  if(AD9958_Test())
    STM_EVAL_LEDOn(LED5);
  else
    STM_EVAL_LEDOn(LED7);

  AD9958_Set_Frequency(DDS_MAIN, 1000000);
  AD9958_Set_Frequency(DDS_LO, 1000000);
  while(1)
  {
  }
}


void mainIli9341()
{
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);

  HwLcdInit();

  HwLcdPinLed(1);
  STM_EVAL_LEDOn(LED3);

  UTFT_InitLCD(UTFT_LANDSCAPE);

  //HwLcdChangeSpiPrescaler(SPI_BaudRatePrescaler_64);
  HwLcdChangeSpiPrescaler(SPI_BaudRatePrescaler_2);

  UTFT_clrScr();
  UTFT_setColor(0, 255, 0);
  UTFT_setBackColor(0, 0, 0);

  UTFT_drawRect(10,10, 20, 20);

  UTFT_setFont(BigFont);
  UTFT_print(" !\"#$%&'()*+,-./", UTFT_CENTER, 0, 0);
  UTFT_print("0123456789:;<=>?", UTFT_CENTER, 16, 0);
  UTFT_print("@ABCDEFGHIJKLMNO", UTFT_CENTER, 32, 0);
  UTFT_print("PQRSTUVWXYZ[\\]^_", UTFT_CENTER, 48, 0);
  UTFT_print("`abcdefghijklmno", UTFT_CENTER, 64, 0);
  UTFT_print("pqrstuvwxyz{|}~ ", UTFT_CENTER, 80, 0);

  UTFT_setColor(255, 0, 0);
  UTFT_setFont(SmallFont);
  UTFT_print(" !\"#$%&'()*+,-./0123456789:;<=>?", UTFT_CENTER, 120, 0);
  UTFT_print("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", UTFT_CENTER, 132, 0);
  UTFT_print("`abcdefghijklmnopqrstuvwxyz{|}~ ", UTFT_CENTER, 144, 0);

  UTFT_setColor(0, 0, 255);
  UTFT_setFont(SevenSegNumFont);
  UTFT_print("0123456789", UTFT_CENTER, 190, 0);
  STM_EVAL_LEDOn(LED4);

  //int i = 0;
  while(1)
  {
    STM_EVAL_LEDToggle(LED8);

    //char ch[] = "0";
    //ch[0] = '0'+((++i)%10);

    //UTFT_print(ch, UTFT_CENTER, 190, 0);
    Delay(100);
  }
}
  
#ifdef PCD_DISPLAY
void mainPcd8544()
{
  //STM_EVAL_LEDInit(LED3);
  //STM_EVAL_LEDToggle(LED3);
  //QuadEncInit();
  
  LcdInit();

  LcdClear();
  LcdGotoXYFont ( 2, 2 );
  LcdStr( FONT_1X, "1234" );
  LcdUpdate();
  
/*
  HwLcdInit();

  while (1)
  {
    HwLcdPinRst(1);
    HwLcdPinDC(1);
    HwLcdPinCE(1);
    HwLcdSend(0xEEEE);
    Delay(2);
    HwLcdPinRst(0);
    HwLcdPinDC(0);
    HwLcdPinCE(0);
    Delay(2);
  }
*/
  for(int i=0; 1; i++)
  {
    LcdClear();
    LcdGotoXYFont ( 1, 1 );
    printInt(i, FONT_1X);
    LcdUpdate();
    Delay(100);
  }

  while (1)
  {
    STM_EVAL_LEDToggle(LED3);
    LcdClear();

    if(QuadEncButton())
    {
      LcdGotoXYFont ( 1, 1 );
      LcdStr(FONT_1X, "PRESSED!");
    }
    
    LcdGotoXYFont ( 1, 3 );
    LcdStr(FONT_1X, "Q=");
    printInt(QuadEncValue(), FONT_2X);
    
    LcdUpdate();

    Delay(20);
  }
/*
  while (1)
  {

    //uint16_t temp_lo = PressureRead(0x20);
    //uint16_t temp_hi = PressureRead(0x0F);
    STM_EVAL_LEDToggle(LED3);
    LcdClear();
    LcdGotoXYFont ( 1, 3 );
    LcdStr(FONT_1X, "P=");
    printInt(PressureReadPressure(), FONT_1X);
    LcdGotoXYFont ( 1, 5 );
    LcdStr(FONT_1X, "T=");
    printIntFixed(PressureReadTemp(), FONT_1X, 3, 1);
    LcdUpdate();

    Delay(200);
  }
*/

}
#endif//PCD_DISPLAY

void mainOriginal()
{
  uint8_t i = 0;
  /* Initialize LEDs and User Button available on STM32F3-Discovery board */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);
  
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI); 
  
  /* Reset UserButton_Pressed variable */
  UserButtonPressed = 0x00; 
   
  /* Infinite loop */
  while (1)
  {   
    /* LEDs Off */
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED10);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED5);
    
    /* Waiting User Button is pressed */
    while (UserButtonPressed == 0x00)
    {
      /* Toggle LD3 */
      STM_EVAL_LEDToggle(LED3);
      /* Insert 50 ms delay */
      Delay(5);
      /* Toggle LD5 */
      STM_EVAL_LEDToggle(LED5);
      /* Insert 50 ms delay */
      Delay(5);
      /* Toggle LD7 */
      STM_EVAL_LEDToggle(LED7);
      /* Insert 50 ms delay */
      Delay(5);
      /* Toggle LD9 */
      STM_EVAL_LEDToggle(LED9);
      /* Insert 50 ms delay */
      Delay(5);
      /* Toggle LD10 */
      STM_EVAL_LEDToggle(LED10);
      /* Insert 50 ms delay */
      Delay(5);
      /* Toggle LD8 */
      STM_EVAL_LEDToggle(LED8);
      /* Insert 50 ms delay */
      Delay(5); 
      /* Toggle LD6 */
      STM_EVAL_LEDToggle(LED6);
      /* Insert 50 ms delay */
      Delay(5);
      /* Toggle LD4 */
      STM_EVAL_LEDToggle(LED4);
      /* Insert 50 ms delay */
      Delay(5);
    }
    
    DataReady = 0x00;
    
    /* All LEDs Off */
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED10);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED5); 
    
    /* Demo Gyroscope */
    Demo_GyroConfig();

    /* Waiting User Button is pressed */
    while (UserButtonPressed == 0x01)
    {
      /* Wait for data ready */
      while(DataReady != 0x05)
      {}
      DataReady = 0x00;
      
      /* LEDs Off */
      STM_EVAL_LEDOff(LED3);
      STM_EVAL_LEDOff(LED6);
      STM_EVAL_LEDOff(LED7);
      STM_EVAL_LEDOff(LED4);
      STM_EVAL_LEDOff(LED10);
      STM_EVAL_LEDOff(LED8);
      STM_EVAL_LEDOff(LED9);
      STM_EVAL_LEDOff(LED5);
      
      /* Read Gyro Angular data */
      Demo_GyroReadAngRate(Buffer);
         
      /* Update autoreload and capture compare registers value*/
      Xval = ABS((int8_t)(Buffer[0]));
      Yval = ABS((int8_t)(Buffer[1])); 
      
      if ( Xval>Yval)
      {
        if ((int8_t)Buffer[0] > 5.0f)
        { 
          /* LD10 On */
          STM_EVAL_LEDOn(LED10);
        }
        if ((int8_t)Buffer[0] < -5.0f)
        { 
          /* LD3 On */
          STM_EVAL_LEDOn(LED3);
        }
      }
      else
      {
        if ((int8_t)Buffer[1] < -5.0f)
        {
          /* LD6 on */
          STM_EVAL_LEDOn(LED6);
        }
        if ((int8_t)Buffer[1] > 5.0f)
        {
          /* LD7 On */
          STM_EVAL_LEDOn(LED7);
        } 
      }
    }
        
    DataReady = 0x00;
    
    /* LEDs Off */
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED10);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED5);
    
    /* Demo Compass */
    Demo_CompassConfig();
    
    /* Waiting User Button is pressed */
    while (UserButtonPressed == 0x02)
    {
      /* Wait for data ready */
      while(DataReady !=0x05)
      {}
      DataReady = 0x00;
      
      /* Read Compass data */
      Demo_CompassReadMag(MagBuffer);
      Demo_CompassReadAcc(AccBuffer);
      
      for(i=0;i<3;i++)
        AccBuffer[i] /= 100.0f;
      
      fNormAcc = sqrt((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));
      
      fSinRoll = -AccBuffer[1]/fNormAcc;
      fCosRoll = sqrt(1.0-(fSinRoll * fSinRoll));
      fSinPitch = AccBuffer[0]/fNormAcc;
      fCosPitch = sqrt(1.0-(fSinPitch * fSinPitch));
     if ( fSinRoll >0)
     {
       if (fCosRoll>0)
       {
         RollAng = acos(fCosRoll)*180/PI;
       }
       else
       {
         RollAng = acos(fCosRoll)*180/PI + 180;
       }
     }
     else
     {
       if (fCosRoll>0)
       {
         RollAng = acos(fCosRoll)*180/PI + 360;
       }
       else
       {
         RollAng = acos(fCosRoll)*180/PI + 180;
       }
     }
     
      if ( fSinPitch >0)
     {
       if (fCosPitch>0)
       {
            PitchAng = acos(fCosPitch)*180/PI;
       }
       else
       {
          PitchAng = acos(fCosPitch)*180/PI + 180;
       }
     }
     else
     {
       if (fCosPitch>0)
       {
            PitchAng = acos(fCosPitch)*180/PI + 360;
       }
       else
       {
          PitchAng = acos(fCosPitch)*180/PI + 180;
       }
     }

      if (RollAng >=360)
      {
        RollAng = RollAng - 360;
      }
      
      if (PitchAng >=360)
      {
        PitchAng = PitchAng - 360;
      }
      
      fTiltedX = MagBuffer[0]*fCosPitch+MagBuffer[2]*fSinPitch;
      fTiltedY = MagBuffer[0]*fSinRoll*fSinPitch+MagBuffer[1]*fCosRoll-MagBuffer[1]*fSinRoll*fCosPitch;
      
      HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/PI;
 
      if (HeadingValue < 0)
      {
        HeadingValue = HeadingValue + 360;    
      }
      
      if ((RollAng <= 40.0f) && (PitchAng <= 40.0f))
      {
        if (((HeadingValue < 25.0f)&&(HeadingValue >= 0.0f))||((HeadingValue >=340.0f)&&(HeadingValue <= 360.0f)))
        {
          STM_EVAL_LEDOn(LED10);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED7);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED5);
        }
        else  if ((HeadingValue <70.0f)&&(HeadingValue >= 25.0f))
        {
          STM_EVAL_LEDOn(LED9);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED7);
        } 
        else  if ((HeadingValue < 115.0f)&&(HeadingValue >= 70.0f))
        {
          STM_EVAL_LEDOn(LED7);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED5);
        }
        else  if ((HeadingValue <160.0f)&&(HeadingValue >= 115.0f))
        {
          STM_EVAL_LEDOn(LED5);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED7);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED3);
        } 
        else  if ((HeadingValue <205.0f)&&(HeadingValue >= 160.0f))
        {
          STM_EVAL_LEDOn(LED3);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED7);
        } 
        else  if ((HeadingValue <250.0f)&&(HeadingValue >= 205.0f))
        {
          STM_EVAL_LEDOn(LED4);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED7);
        } 
        else  if ((HeadingValue < 295.0f)&&(HeadingValue >= 250.0f))
        {
          STM_EVAL_LEDOn(LED6);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED7);
        }        
        else  if ((HeadingValue < 340.0f)&&(HeadingValue >= 295.0f))
        {
          STM_EVAL_LEDOn(LED8);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED7);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED5);
        }
      }
      else
      {
        /* Toggle All LEDs */
        STM_EVAL_LEDToggle(LED7);
        STM_EVAL_LEDToggle(LED6);
        STM_EVAL_LEDToggle(LED10);
        STM_EVAL_LEDToggle(LED8);
        STM_EVAL_LEDToggle(LED9);
        STM_EVAL_LEDToggle(LED3);
        STM_EVAL_LEDToggle(LED4);
        STM_EVAL_LEDToggle(LED5);
        /* Delay 50ms */
        Delay(5);
      }
    }
  }
}


/**
  * @brief  Configure the Mems to gyroscope application.
  * @param  None
  * @retval None
  */
void Demo_GyroConfig(void)
{
  L3GD20_InitTypeDef L3GD20_InitStructure;
  L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
  
  /* Configure Mems L3GD20 */
  L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
  L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
  L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
  L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
  L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
  L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
  L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500; 
  L3GD20_Init(&L3GD20_InitStructure);
   
  L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
  L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
  L3GD20_FilterConfig(&L3GD20_FilterStructure) ;
  
  L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}

/**
  * @brief  Calculate the angular Data rate Gyroscope.
  * @param  pfData : Data out pointer
  * @retval None
  */
void Demo_GyroReadAngRate (float* pfData)
{
  uint8_t tmpbuffer[6] ={0};
  int16_t RawData[3] = {0};
  uint8_t tmpreg = 0;
  float sensitivity = 0;
  int i =0;

  L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);
  
  L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);
  
  /* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
  if(!(tmpreg & 0x40))
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i] << 8) + tmpbuffer[2*i+1]);
    }
  }
  
  /* Switch the sensitivity value set in the CRTL4 */
  switch(tmpreg & 0x30)
  {
  case 0x00:
    sensitivity=L3G_Sensitivity_250dps;
    break;
    
  case 0x10:
    sensitivity=L3G_Sensitivity_500dps;
    break;
    
  case 0x20:
    sensitivity=L3G_Sensitivity_2000dps;
    break;
  }
  /* divide by sensitivity */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)RawData[i]/sensitivity;
  }
}

/**
  * @brief  Configure the Mems to compass application.
  * @param  None
  * @retval None
  */
void Demo_CompassConfig(void)
{
  LSM303DLHCMag_InitTypeDef LSM303DLHC_InitStructure;
  LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
  LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;
  
  /* Configure MEMS magnetometer main parameters: temp, working mode, full Scale and Data rate */
  LSM303DLHC_InitStructure.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
  LSM303DLHC_InitStructure.MagOutput_DataRate =LSM303DLHC_ODR_30_HZ ;
  LSM303DLHC_InitStructure.MagFull_Scale = LSM303DLHC_FS_8_1_GA;
  LSM303DLHC_InitStructure.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;
  LSM303DLHC_MagInit(&LSM303DLHC_InitStructure);
  
   /* Fill the accelerometer structure */
  LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
  LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_50_HZ;
  LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
  LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;
  LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
  LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_LSB;
  LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
  /* Configure the accelerometer main parameters */
  LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);
  
  /* Fill the accelerometer LPF structure */
  LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

  /* Configure the accelerometer LPF main parameters */
  LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

/**
* @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
* @param pnData: pointer to float buffer where to store data
* @retval None
*/
void Demo_CompassReadAcc(float* pfData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx[2];
  uint8_t buffer[6], cDivider;
  uint8_t i = 0;
  float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
  
  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);
   
  if(ctrlx[1]&0x40)
    cDivider=64;
  else
    cDivider=16;

  /* check in the control register4 the data alignment*/
  if(!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
  {
    for(i=0; i<3; i++)
    {
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i])/cDivider;
    }
  }
  else /* Big Endian Mode */
  {
    for(i=0; i<3; i++)
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])/cDivider;
  }
  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);


  if(ctrlx[1]&0x40)
  {
    /* FIFO mode */
    LSM_Acc_Sensitivity = 0.25;
  }
  else
  {
    /* normal mode */
    /* switch the sensitivity value set in the CRTL4*/
    switch(ctrlx[0] & 0x30)
    {
    case LSM303DLHC_FULLSCALE_2G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
      break;
    case LSM303DLHC_FULLSCALE_4G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
      break;
    case LSM303DLHC_FULLSCALE_8G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
      break;
    case LSM303DLHC_FULLSCALE_16G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
      break;
    }
  }

  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)pnRawData[i]/LSM_Acc_Sensitivity;
  }

}

/**
  * @brief  calculate the magnetic field Magn.
* @param  pfData: pointer to the data out
  * @retval None
  */
void Demo_CompassReadMag (float* pfData)
{
  static uint8_t buffer[6] = {0};
  uint8_t CTRLB = 0;
  uint16_t Magn_Sensitivity_XY = 0, Magn_Sensitivity_Z = 0;
  uint8_t i =0;
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_CRB_REG_M, &CTRLB, 1);
  
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M, buffer, 1);
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_L_M, buffer+1, 1);
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_M, buffer+2, 1);
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_L_M, buffer+3, 1);
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_M, buffer+4, 1);
  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_L_M, buffer+5, 1);
  /* Switch the sensitivity set in the CRTLB*/
  switch(CTRLB & 0xE0)
  {
  case LSM303DLHC_FS_1_3_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_3Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_3Ga;
    break;
  case LSM303DLHC_FS_1_9_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_9Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_9Ga;
    break;
  case LSM303DLHC_FS_2_5_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_2_5Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_2_5Ga;
    break;
  case LSM303DLHC_FS_4_0_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4Ga;
    break;
  case LSM303DLHC_FS_4_7_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4_7Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4_7Ga;
    break;
  case LSM303DLHC_FS_5_6_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_5_6Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_5_6Ga;
    break;
  case LSM303DLHC_FS_8_1_GA:
    Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_8_1Ga;
    Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_8_1Ga;
    break;
  }
  
  for(i=0; i<2; i++)
  {
    pfData[i]=(float)((int16_t)(((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])*1000)/Magn_Sensitivity_XY;
  }
  pfData[2]=(float)((int16_t)(((uint16_t)buffer[4] << 8) + buffer[5])*1000)/Magn_Sensitivity_Z;
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
  return 0;
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
  return 0;
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
