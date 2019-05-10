/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include <string.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

//OS_MEM  mem;                    //�����ڴ�������
//uint8_t ucArray [ 3 ] [ 20 ];   //�����ڴ������С


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;    //������ƿ�

static  OS_TCB   AppTaskLed1TCB;
static  OS_TCB   AppTaskLed2TCB;
static  OS_TCB   AppTaskLed3TCB;




/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];
static  CPU_STK  AppTaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];
static  CPU_STK  AppTaskLed3Stk [ APP_TASK_LED3_STK_SIZE ];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //����������

static  void  AppTaskLed1  ( void * p_arg );
static  void  AppTaskLed2  ( void * p_arg );
static  void  AppTaskLed3  ( void * p_arg );


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
	OS_TICK			start;

int  main (void)
{
    OS_ERR  err;

	
    OSInit(&err);                                                           //��ʼ�� uC/OS-III

	  /* ������ʼ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",                            //��������
                 (OS_TASK_PTR ) AppTaskStart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

    OSStart(&err);                                                          //�����������������uC/OS-III���ƣ�

}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


    (void)p_arg;

    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������
    BSP_Init();                                                 //�弶��ʼ��

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u                                    //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif                                                          //���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU 
                                                                //ʹ����ʹ�ã���
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��
#endif

    
//    /* ����ʱ��Ƭ��ת���� */		
//    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //ʹ��ʱ��Ƭ��ת����
//		                     (OS_TICK       )0,                    //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
//												 (OS_ERR       *)&err );               //���ش�������


		/* ���� LED1 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led1",                             //��������
                 (OS_TASK_PTR ) AppTaskLed1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
		
		/* ���� LED2 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskLed2TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led2",                             //��������
                 (OS_TASK_PTR ) AppTaskLed2,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed2Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������								 

		/* ���� LED3 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskLed3TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led3",                             //��������
                 (OS_TASK_PTR ) AppTaskLed3,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED3_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed3Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
								 
	
		OSTaskDel ( 0, & err );                     //ɾ����ʼ������������������

		
}


/*
*********************************************************************************************************
*                                          LED1 TASK
*********************************************************************************************************
*/

static  void  AppTaskLed1 ( void * p_arg )
{
	
	OS_ERR      err;
	OS_TICK			begain=0;
	OS_TICK			finish=0;
	OS_TICK			i=0;
	OS_TICK			dly=0;
	OS_REG      value;	
	(void)p_arg;
	
	start = OSTimeGet( &err );			//ϵͳ�ϵ��ʼִ���û��������ʵʱ��
	
	while(1){
	begain = OSTimeGet(&err);													//��¼��������ʱ��
	value  = OSTaskRegGet ( 0, 0, & err );	
		while ( value<1 )				//value��¼���������������α�ʾ����һ�����ڣ�һ������Ϊ100��tick
		{
			LED1_ON ; 
			OSTaskRegSet(0,0,++value,&err);
			OSIntEnter();							//��ֹ������ȣ���֤���´���Ϊһ��ʱ�䵥λ
			while(i<=14999999)							//ģ������ִ������ʱ��Ƭ
				i++;
				i=0;
			
			LED1_OFF ;
			while(i<=14999999)							//ģ������ִ������ʱ��Ƭ
				i++;
				i=0;
			OSIntExit();

		}	
		
		OSTaskRegSet ( 0, 0, 0, & err );
		finish=OSTimeGet(&err);	
			if (400!=(finish-start))
				dly=400-(finish-start)%400;
			else
				dly=0;
		OSTimeDly(dly, OS_OPT_TIME_DLY, & err );	//400-(finish-begain)
		
}
}

/*
*********************************************************************************************************
*                                          LED2 TASK
*********************************************************************************************************
*/

static  void  AppTaskLed2 ( void * p_arg )
{
	OS_ERR      err;
	OS_TICK			begain=0;
	OS_TICK			finish=0;
	OS_TICK			i=0;
	OS_TICK			dly;
	OS_REG      value;	
	(void)p_arg;
	begain=OSTimeGet(&err);
	while(1){										
	
		value = OSTaskRegGet ( 0, 0, & err );	
		while ( value<2 )				//value��¼����ʱ��Ƭ��
		{
			OSTaskRegSet(0,0,++value,&err);
			OSIntEnter();
			LED2_ON ; 
			while(i<=14999999)							//ģ������ִ������ʱ��Ƭ
				i++;
				i=0;
			
			LED2_OFF ; 
			while(i<=14999999)							//ģ������ִ������ʱ��Ƭ
				i++;
				i=0;
			OSIntExit();
		}
		
			OSTaskRegSet ( 0, 0, 0, & err );
			finish=OSTimeGet(&err);
			if (600!=(finish-start))
				dly=600-(finish-start)%600;
			else
				dly=0;
			OSTimeDly(dly, OS_OPT_TIME_DLY, & err );		                        
		
}	
}


/*
*********************************************************************************************************
*                                          LED3 TASK
*********************************************************************************************************
*/

static  void  AppTaskLed3 ( void * p_arg )
{
	OS_ERR      err;
	OS_TICK			begain;
	OS_TICK			finish;
	OS_TICK			i=0;
	OS_TICK			dly;
	OS_REG      value;	
	(void)p_arg;
	
	while(1){
	begain=OSTimeGet(&err);													//��¼��������ʱ��

		value = OSTaskRegGet ( 0, 0, & err );			
		while ( value<3 )				//value��¼����ʱ��Ƭ��
		{
			//OS_ENTER_CRITICAL();
			OSIntEnter();
			LED3_ON ;
			while(i<=14999999)							//ģ������ִ������ʱ��Ƭ
				i++;
				i=0;
			
			LED3_OFF ;
			while(i<=14999999)							//ģ������ִ������ʱ��Ƭ
				i++;
				i=0;
			OSIntExit();
			//OS_EXIT_CRITICAL();
			OSTaskRegSet(0,0,++value,&err);		//valueֵ��1
		}

			OSTaskRegSet ( 0, 0, 0, & err );
			finish=OSTimeGet(&err);	
			dly=finish-start;
	/**���������������������ĩβִ���꣬��dly��ֵΪһ���������� ���˴���һ���ж� **/
			if (dly!=1000)		
				dly=1000-(finish-start)%1000;
			else
				dly=0;
			OSTimeDly(dly, OS_OPT_TIME_DLY, & err );
			                        	
}
}
	


