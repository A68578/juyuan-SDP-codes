/*****************************************************************************************************************************************
 *Copyright(C),unauthorized use, reproduction and dissemination are not allowed
 *FileName:bootmanagerfbl.cpp
 *Date: 2025.xxx
 *Description:This file is the implementation of the bootmanager module
 *****************************************************************************************************************************************/
 /*****************************************************************************************************************************************
  *   Revision History:
  *
  *   Version     Date          Author        Descriptions
  *   ---------   ----------    ------------  ---------------
  *   01.00.00    2025.xxx      /      Original
  *****************************************************************************************************************************************/

#include "bootmanager.h"
#include "AbstractForOtherModules.h"

#include "Windows.h"


  /*global macros*/
#define SIZEARRAY(list) sizeof(list)/sizeof(list[0])

#define DEBUG 1 
#if DEBUG 
#define APPENTRY(addr)  do { \
        void (*jump_func)(void) = (void (*)(void))addr; \
        jump_func(); \
    } while (0)

#endif


/*global definitions*/
fblManagerContext g_fblchange;
static unsigned int app_check_flag;

#if OFF_BOARD
fblManagerBackup g_fblmanager_backup;
#endif

/*static functions to debug*/
/**
 * @brief ���Դ�����ֹ����
 *
 * �ú��������ڵ��Թ����д�ӡ������Ϣ����ͨ������ exit(1) ǿ����ֹ����
 * ͨ�����ڴ����ɻָ��Ĵ�������ʱ�Ķ���ʧ�ܡ�
 *
 * @note �˺���Ϊ��̬���������ڵ�ǰ�ļ��ڿɼ���
 */
static void DET_debug(void)
{
	printf("error debug\n");
	exit(1);
}

/**
 * @brief ��ӡӦ�ó���ĵ�����Ϣ��
 *
 * �ú��������ڵ���ģʽ�����Ӧ�ó���ĵ�����Ϣ��ͨ�����ڿ����׶ε���־��¼��
 * ���ô˺������ڱ�׼����д�ӡ "app debug" �ַ�����
 */
static void APP_debug(void)
{
	printf("app debug\n");
}

/**
 * @brief ���Ŀ���־λ��״̬��
 *
 * �ú���������ָ֤���ı�־λ�Ƿ���Ԥ��ֵƥ�䡣���ȸ�������ı�־λID��ȡ������Ϣ��
 * Ȼ���л���EA�ڴ����ͣ���ȡĿ���־λ��ֵ����Ԥ��ֵ���бȽϡ�
 *
 * @param readflagId ��־λ��ID����Ч��ΧΪ0��2��
 * @return StdReturnType ���ؼ������
 *   - Boot_Check_SUCCESS: ��־λֵ��Ԥ��ֵƥ�䡣
 *   - Boot_Check_FAIL: ��־λֵ��Ԥ��ֵ��ƥ����ȡʧ�ܡ�
 */
StdReturnType FBL_CheckTargetFlag(unsigned int readflagId)
{
	unsigned int checkFlagData = 0;
	unsigned int checkFlagDataTemp = 0;
	unsigned int checkFlagSize = 0;
	unsigned int checkFlagAddr = 0;
	StdReturnType result = Boot_Check_FAIL;

	if (readflagId < 3)
	{
		/* Extract the target Flag information */
		checkFlagSize = IMemIfConfigList[readflagId].IMemIfConfig_Len;
		checkFlagData = IMemIfConfigList[readflagId].IMemIfConfig_Flag;
		checkFlagAddr = IMemIfConfigList[readflagId].IMemIfConfig_Addr;
	}
	/* Select EA as the memory type */
	MemIf_SwtichToEaType();
	result = MemIf_Read(checkFlagAddr, checkFlagSize, &checkFlagDataTemp, readflagId);
	/* Check the read result of the target flag */
	if ((result == Boot_Check_SUCCESS) && (checkFlagData == checkFlagDataTemp))
	{
		result = Boot_Check_SUCCESS;
	}
	else
	{
		result = Boot_Check_FAIL;
	}

	return result;
}

/**
 * @brief ���ָ���ı�־λ��
 *
 * �ú�����������ڴ���ָ���ı�־λ��ͨ�������ı�־ID��������������б�����ȡ��Ӧ�ĵ�ַ�����Ⱥͱ�־ֵ��
 * �������ڴ�ӿڵĲ��������������־λ��
 *
 * @param readflagId ��־ID������ָ����Ҫ����ı�־λ����Ч��ΧΪ0��2��
 * @return StdReturnType ���ز��������
 *   - Boot_Check_FAIL: ����ʧ�ܣ���־ID��Ч�����ʧ�ܣ���
 *   - ����ֵ: �����ɹ������巵��ֵ��MemIf_Erase������������
 *
 * @note �����ڲ����л���EA�ڴ����ͣ�������MemIf_Erase����ִ��ʵ�ʵĲ���������
 */
StdReturnType FBL_ClearFlag(unsigned int readflagId)
{

	StdReturnType result = Boot_Check_FAIL;
	unsigned int data;
	unsigned int addr;
	unsigned int len;

	/* Check for matching Flag ID */
	if (readflagId < 3)
	{
		/* Extracts the Flag information to be cleared */
		len = IMemIfConfigList[readflagId].IMemIfConfig_Len;
		data = IMemIfConfigList[readflagId].IMemIfConfig_Flag;
		addr = IMemIfConfigList[readflagId].IMemIfConfig_Addr;
		/* Select EA as the memory type */
		MemIf_SwtichToEaType();
		/* Call the erase function to erase the flag information in memory */
		result = MemIf_Erase(addr, len, &data);
	}

	return result;
}

/**
 * @brief ��鲢�������±���¼���
 *
 * �ú������ڼ���Ƿ���Ҫ���±�̣������ݼ����ִ����Ӧ�Ĳ�����
 * ������ͨ�����������־λ���������±����Ӧ�����򣬱��Ϊʧ�ܡ�
 *
 * @param fblmanagercontext ָ�� FBL �����������ĵ�ָ�룬���ڴ洢����״̬��
 * @return StdReturnType ���ز��������
 *   - Boot_Check_SUCCESS: �����ɹ���
 *   - Boot_Check_FAIL: ����ʧ�ܡ�
 *
 * @note ��������������ָ��Ϊ NULL���ᴥ�����������
 */
StdReturnType FBL_CheckReprog_Event(fblManagerContext* fblmanagercontext)
{
	printf("check reprog\n");
	StdReturnType checkresult = Boot_Check_FAIL;
	unsigned int flag = 0xFF;
	StdReturnType ClearFlag_result = Boot_Check_FAIL;
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			checkresult = FBL_CheckTargetFlag(IMemIf_FLAGS_REPROGRAM_ID);
			if (checkresult == Boot_Check_SUCCESS)
			{
				flag = VALID_REQUEST_FLAG;
			}
			else
			{
				flag = INVALID_REQUEST_FLAG;
			}
			if (flag == VALID_REQUEST_FLAG)
			{
				ClearFlag_result = FBL_ClearFlag(IMemIf_FLAGS_REPROGRAM_ID);
				if (ClearFlag_result == Boot_Check_SUCCESS)
				{
					/* Send a Response */
					DCM_10DelayCancel();
					SendReprogramRespond();
					SetReprogramState();
					result = Boot_Check_SUCCESS;
					fblmanagercontext->success_flag = Boot_Check_SUCCESS;
				}
				else
				{
					fblmanagercontext->success_flag = Boot_Check_FAIL;
				}
			}
			else
			{
				fblmanagercontext->success_flag = Boot_Check_FAIL;
				result = Boot_Check_FAIL;
			}
		}
	}
	return result;
}

/**
 * @brief ��鲢����λ�¼���
 *
 * �ú������ڼ�鸴λ��־λ�������ݱ�־λ��״̬�����Ƿ�ִ��MCU��λ������
 * �����λ��־λ��Ч��������ñ�־λ������MCU��λ�����򣬺�������ʧ��״̬��
 *
 * @param fblmanagercontext ָ��FBL�����������ĵ�ָ�롣���ΪNULL���ᴥ�����������
 * @return StdReturnType ���ز��������
 *   - Boot_Check_SUCCESS: ��λ�����ɹ�ִ�С�
 *   - Boot_Check_FAIL: ��λ����δִ�л�ִ��ʧ�ܡ�
 */
StdReturnType FBL_CheckReset_Event(fblManagerContext* fblmanagercontext)
{
	printf("check reset\n");
	StdReturnType result = Boot_Check_FAIL;
	StdReturnType checkresult = Boot_Check_FAIL;
	StdReturnType ClearFlag_result = Boot_Check_FAIL;
	unsigned int flag = 0xFF;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		checkresult = FBL_CheckTargetFlag(IMemIf_FLAGS_RESET_ID);
		if (checkresult == Boot_Check_SUCCESS)
		{
			flag = VALID_RESET_FLAG;
		}
		else
		{
			flag = INVALID_RESET_FLAG;
		}
		if (flag == VALID_RESET_FLAG)
		{
			ClearFlag_result = FBL_ClearFlag(IMemIf_FLAGS_RESET_ID);
			if (ClearFlag_result == Boot_Check_SUCCESS)
			{
				/* Reset MCU */
				SendResponseMCUReset();
				result = Boot_Check_SUCCESS;
				exit(0);
			}
			else
			{
				;
			}
		}
	}
	return result;
}

/**
 * @brief ��ʼ�� MCAL��Microcontroller Abstraction Layer�����ģ����¼���������
 *
 * �ú��������ʼ�� MCAL �еĶ��ģ�飬����ʱ�ӡ��˿ڡ�CAN����ʱ����EA�����Ź����жϵȡ�
 * �������������Ķ����ѱ��Ϊ�ɹ�����ֱ�ӷ��سɹ�״̬������ִ�г�ʼ�����̡�
 *
 * @param fblmanagercontext ָ�� FBL �����������ĵ�ָ�룬���ڴ���״̬�Ϳ�����Ϣ��
 *                         ���Ϊ NULL���ᴥ�����������
 *
 * @return StdReturnType ���س�ʼ�������
 *                       - Boot_Check_SUCCESS: ��ʼ���ɹ���
 *                       - Boot_Check_FAIL: ��ʼ��ʧ�ܻ������Ч��
 *
 * @note ��������Ķ���� success_flag �ѱ��Ϊ�ɹ�����������ʼ�����̡�
 *       ��ʼ�������л����ε��ø�ģ��ĳ�ʼ��������
 */
StdReturnType FBL_Mcal_Init_Event(fblManagerContext* fblmanagercontext)
{
	printf("mcal init\n");
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			/* initializes the clock module */
			MCU_ClkSet();
			/* initializes the Can Port module */
			Port_Init();
			/* initializes the Can module */
			Can_Init();
			/* initializes the Timer module */
			TIM_Init();
			/* initializes the Ea module */
			Ea_Init();
			/* initializes the Watch Dog module */
			WDog_Init();
			/*MCU interrupt initialization*/
			MCU_ISR_Init();
			fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			result = Boot_Check_SUCCESS;
		}
	}
	return result;
}
/**
 * @brief ��ʼ������¼���
 *
 * �ú������ڳ�ʼ������¼�����鴫��� `fblmanagercontext` �Ƿ���Ч����������״ִ̬����Ӧ�Ĳ�����
 * ��� `fblmanagercontext` Ϊ NULL������õ��Ժ��� `DET_debug`��
 * ��� `fblmanagercontext->success_flag` Ϊ `Boot_Check_SUCCESS`����ֱ�ӷ��سɹ�״̬��
 * ���򣬳�ʼ�� CAN ����� (`CanTp_Init`) �����ͨ�Ź����� (`Dcm_Init`)������ `success_flag` ����Ϊ�ɹ�״̬��
 *
 * @param fblmanagercontext ָ�� `fblManagerContext` �ṹ���ָ�룬������������״̬��־��
 * @return StdReturnType ���ز��������
 *   - `Boot_Check_SUCCESS`�������ɹ���
 *   - `Boot_Check_FAIL`������ʧ�ܣ����� `fblmanagercontext` Ϊ NULL ʱ���أ���
 */
StdReturnType FBL_Diagnose_Init_Event(fblManagerContext* fblmanagercontext)
{
	printf("diagnose init\n");
	StdReturnType result = Boot_Check_FAIL;
	unsigned int readflag = 0;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			CanTp_Init();
			Dcm_Init();
			fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			result = Boot_Check_SUCCESS;
		}
	}
	return result;
}

/**
 * @brief FBL���������¼���������
 *
 * �ú���������FBL��Flash Bootloader�������¼�ѭ���������������Ź��������Ե�ִ��CANͨ�ŵķ��ͺͽ��գ�
 * �Լ���������ģ������������С�
 *
 * @param fblmanagercontext ָ��FBL�����������ĵ�ָ�롣���ΪNULL���ᴥ�����Դ���
 *
 * @return StdReturnType ���ز���״̬���ɹ�ʱ����Boot_Check_SUCCESS��
 *
 * @note �����ڲ�����һ������ѭ�������������Ե�ִ�����²�����
 *       1. ����Can_MainFunction_Write��Can_MainFunction_Read����CANͨ�š�
 *       2. ����CanTp_MainFunction��Dcm_MainFunction��������ģ������С�
 *
 * @warning ��������������ָ��ΪNULL���ᴥ��DET_debug()���Դ���
 */
StdReturnType FBL_MainFunction_Event(fblManagerContext* fblmanagercontext)
{
	printf("fbl_bsw_mainfunction\n");
	unsigned int readflag = 0;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		/* start watchdog */
		WdgM_Start();
		while (1)
		{
			/* Can sends and receives packets periodically */
			Can_MainFunction_Write();
			Can_MainFunction_Read();
			/* Modules main run-time scheduling */
			CanTp_MainFunction();
			Dcm_MainFunction();
		}

	}
	return Boot_Check_SUCCESS;
}
/**
 * @brief ���Ӧ�ó������Ч�Բ�����״̬��
 *
 * �ú���������֤Ӧ�ó������Ч�ԣ������ݼ�������´���������Ľṹ���еĳɹ���־��
 * ��������Ľṹ��Ϊ�գ��ᴥ�����������
 *
 * @param fblmanagercontext ָ��FBL�����������ĵ�ָ�룬���ڴ洢�������
 * @return StdReturnType ���ؼ������
 *   - Boot_Check_SUCCESS: Ӧ�ó�����Ч��
 *   - Boot_Check_FAIL: Ӧ�ó�����Ч����ʧ�ܡ�
 */
StdReturnType FBL_CheckApp_Event(fblManagerContext* fblmanagercontext)
{
	printf("check app\n");
	StdReturnType checkresult = Boot_Check_FAIL;
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			result = FBL_CheckTargetFlag(IMemIf_FLAGS_APPLICATION_ID);
			if (result == Boot_Check_SUCCESS)
			{
				app_check_flag = VALID_APP_FLAG;
				fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			}
			else
			{
				app_check_flag = INVALID_APP_FLAG;
				fblmanagercontext->success_flag = Boot_Check_FAIL;
			}
		}
	}
	return result;
}
/**
 * @brief ������ת��Ӧ�ó�����¼���
 *
 * �ú��������������������д�����ת��Ӧ�ó��������������Ӧ�ó����־�Ƿ���Ч��
 * �����Ч����ת��Ӧ�ó�����ڵ㣬���򷵻�ʧ��״̬��
 *
 * @param fblmanagercontext ָ�����������������ĵ�ָ�롣���ΪNULL���ᴥ�����������
 * @return StdReturnType ���ز��������
 *                      - Boot_Check_SUCCESS: �ɹ���ת��Ӧ�ó���
 *                      - Boot_Check_FAIL: ��תʧ�ܻ������Ч��
 */
StdReturnType FBL_GotoApp_Event(fblManagerContext* fblmanagercontext)
{
	printf("go to app\n");
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			if (app_check_flag == VALID_APP_FLAG)
			{
				APPENTRY(APP_debug);
				result = Boot_Check_SUCCESS;
				fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			}
		}
	}
	return result;
}

static fblManagerEvent fblchange_channel_select[] =
{
	{FBL_CheckReprog,FBL_CheckReprog_Event},
	{FBL_CheckReset,FBL_CheckReset_Event},
	{FBL_Mcal_Init,FBL_Mcal_Init_Event},
	{FBL_Diagnose_Init,FBL_Diagnose_Init_Event},
	{FBL_MainFunction,FBL_MainFunction_Event},
	{FBL_CheckApp,FBL_CheckApp_Event},
	{FBL_GotoApp,FBL_GotoApp_Event}
};
static FuncRouterType FuncRouter[] =
{
	{FBL_CheckReprog_Event,{FBL_Mcal_Init,FBL_CheckApp}},
	{FBL_CheckApp_Event,{FBL_GotoApp,FBL_Mcal_Init}},
	{FBL_Mcal_Init_Event,{FBL_Diagnose_Init,FBL_MainFunction}},
	{FBL_Diagnose_Init_Event,{FBL_MainFunction,FBL_MainFunction}},
	{FBL_MainFunction_Event,{FBL_MainFunction,FBL_MainFunction}},
	{FBL_GotoApp_Event,{FBL_MainFunction,FBL_MainFunction}}
};


static EventHandler FuncConfig[1] = { NULL };

#if 0 //OFF_BOARD
/*back up data*/
/**
 * @brief ���� fblManagerContext �ṹ�����ݡ�
 *
 * �ú������ڽ������ fblManagerContext �ṹ�����ݱ��ݵ�ȫ�ֱ��� g_fblmanager_backup �С�
 * ע�⣺�����ڲ�ʹ���˶�̬�ڴ���䣬��������ȷ�������ָ����Ч��
 *
 * @param fblmanagercontext ָ����Ҫ���ݵ� fblManagerContext �ṹ���ָ�롣
 */
static void fbl_manager_backup(fblManagerContext* fblmanagercontext)
{
	g_fblmanager_backup.fblmanagerContextBackup = (fblManagerContext*)malloc(sizeof(fblManagerContext));
	g_fblmanager_backup.fblmanagerContextBackup = fblmanagercontext;
}
/*free data*/
/**
 * �ͷ� fblManagerContext �ṹ����ڴ档
 * �ú������������� fblManagerContext ռ�õ��ڴ���Դ��
 *
 * @param fblmanagercontext ָ����Ҫ�ͷŵ� fblManagerContext �ṹ���ָ�롣
 */
static void fbl_free_data(fblManagerContext* fblmanagercontext)
{
	free(fblmanagercontext);
}
/*get data from back up*/
/**
 * @brief ��ȡ���ݵ�FBL�����������ġ�
 *
 * �˺������ڷ���ȫ�ֱ��ݵ�FBL������������ָ�롣
 * ͨ�����ڻָ����鱸�ݵ�FBL������״̬��
 *
 * @return fblManagerContext* ָ�򱸷ݵ�FBL�����������ĵ�ָ�롣
 */
static fblManagerContext* get_fbl_manager_backup(void)
{
	return g_fblmanager_backup.fblmanagerContextBackup;
}
#endif

/**
 * @brief ����FBL�����״̬ת����
 *
 * ���ݵ�ǰ״̬���ö�Ӧ�Ĵ�������ִ��״̬ת���߼���
 *
 * @param fblmanagercontext ָ��FBL�����������ĵ�ָ�룬������ǰ״̬�����������Ϣ��
 * @return StdReturnType ����ִ�н�����ɹ���ʧ�ܡ�
 */
static StdReturnType FBltaskhandler(fblManagerContext* fblmanagercontext)
{
	StdReturnType result = Boot_Check_FAIL;
	result = fblchange_channel_select[fblmanagercontext->currentstate].handle(fblmanagercontext);
	return result;
}

/**
 * @brief ������������ĺ�����
 *
 * �ú������ݵ�ǰ״̬��FBL_CheckApp��ִ���ض����������߼���
 * �����ǰ״̬Ϊ FBL_CheckApp�������л�Ϊ FBL_CheckReset ����������������
 * ���������������ʧ�ܣ���ָ�״̬Ϊ FBL_CheckApp ���ٴε�������������
 * �����ǰ״̬���� FBL_CheckApp����ֱ�ӷ��سɹ�״̬��
 *
 * @param fblmanagercontext ָ�� FBL �����������ĵ�ָ�룬������ǰ״̬����Ϣ��
 * @return StdReturnType ����������Ľ����Boot_Check_SUCCESS �� Boot_Check_FAIL����
 */
static StdReturnType forspecial_task(fblManagerContext* fblmanagercontext)
{
	StdReturnType result = Boot_Check_FAIL;

	if (fblmanagercontext->currentstate == FBL_CheckApp)
	{
		fblmanagercontext->currentstate = FBL_CheckReset;
		result = FBltaskhandler(fblmanagercontext);
		if (result == Boot_Check_FAIL)
		{
			fblmanagercontext->currentstate = FBL_CheckApp;
			result = FBltaskhandler(fblmanagercontext);
		}
	}
	else
	{
		result = Boot_Check_SUCCESS;
	}
	return result;
}

/*for task cycle.*/
/**
 * @brief ִ��FBL��Flash Boot Loader��������
 *
 * �ú���������FBL����Ҫ�߼���������
 * 1. ���ó�ʼ���ú�����FuncConfig[0]�����г�ʼ����
 * 2. ��������·�ɱ�FuncRouter�������ݵ�ǰ״̬�ͽ��ѡ��ִ�в�ͬ�Ĺ��ܷ�֧��
 * 3. ����ִ�н������FBL��������״̬��currentstate���ͳɹ���־��success_flag����
 * 4. ����FBltaskhandler����������񣬲������䷵��ֵ�����Ƿ���ֹѭ����
 * 5. ��ѭ�������󣬸���FuncConfig[0]��ƥ�䵱ǰ״̬��
 *
 * @param fblmanager ָ��FBL�����������ĵ�ָ�룬������ǰ״̬��������Ϣ��
 * @note ���������OFF_BOARD�꣬���������fbl_manager_backup�������ݱ��ݣ���ǰ��ע�͵�����
 */
void fblmaintask(fblManagerContext* fblmanager)
{
	StdReturnType result = Boot_Check_FAIL;
	result = FuncConfig[0](fblmanager);
	for (int i = 0; i < SIZEARRAY(FuncRouter); i++)
	{
		result = forspecial_task(fblmanager);
		if (FuncConfig[0] == FuncRouter[i].originfunc)
		{
			if (result == Boot_Check_SUCCESS)
			{
				fblmanager->currentstate = FuncRouter[i].FuncRouterSelect.selectfunc_true;
				fblmanager->success_flag = Boot_Check_FAIL;
				result = FBltaskhandler(fblmanager);
				if (result == Boot_Check_SUCCESS)
				{
					break;
				}
				else
				{
					;
				}

			}
			else
			{
				fblmanager->currentstate = FuncRouter[i].FuncRouterSelect.selectfunc_false;
				fblmanager->success_flag = Boot_Check_FAIL;
				result = FBltaskhandler(fblmanager);
				if (result == Boot_Check_SUCCESS)
				{
					break;
				}
				else
				{
					;
				}
			}
		}
		else
		{
			;
		}
	}
	for (int j = 0; j < SIZEARRAY(FuncRouter); j++)
	{
		if (fblchange_channel_select[fblmanager->currentstate].handle == FuncRouter[j].originfunc)
		{
			FuncConfig[0] = fblchange_channel_select[fblmanager->currentstate].handle;
		}
	}
#if 0 //OFF_BOARD
	/*back up data*/
	fbl_manager_backup(fblmanager);
#endif
}

/*initial all the state-machine task*/
/**
 * @brief ��ʼ��FBL������������
 *
 * �ú������ڳ�ʼ��FBL�����������񣬰�������״̬�������ͳɹ���־��
 * �û���Ҫ�����ʼ�����ţ����������������Ч�ԣ�����������ѡ���Ӧ�Ĵ�������
 * ���������Ч������ʾ����Ҫ���������롣
 *
 * @note ����������ű�������Ч��Χ�ڣ�0��6�����������ʾ����
 * @warning ʹ����goto���ʵ��ѭ�����룬��ע�����Ŀɶ��Ժ�ά���ԡ�
 */
void initfblmanager_task(void)
{
	g_fblchange.stateCount = SIZEARRAY(fblchange_channel_select);
	g_fblchange.success_flag = Boot_Check_FAIL;
loop:
	printf("please input your initial task:");
	scanf("%d", &g_fblchange.currentstate);
	if ((g_fblchange.currentstate < 0) || (g_fblchange.currentstate > 6))
	{
		printf("error input!\n");
		system("cls");
		goto loop;
	}
	else
	{
		FuncConfig[0] = fblchange_channel_select[g_fblchange.currentstate].handle;
	}

#if 0 //OFF_BOARD
	g_fblmanager_backup.fblmanagerContextBackup = { 0 };
#endif
}

/**
 * @brief �������������ʼ��������FBL��Flash Boot Loader������������
 *
 * �ú�������Ҫ�������£�
 * 1. ��ʼ��FBL����������initfblmanager_task����
 * 2. ������ѭ������������FBL����������fblmaintask����ֱ����ǰ״̬��ΪFBL_NULL��
 *
 * ע�⣺
 * - ��ѭ��ͨ�����ȫ�ֱ���g_fblchange.currentstate��״̬�������Ƿ�������С�
 * - �����а�����һ�α�ע�͵����߼���OFF_BOARD�����������������ʱ�ͷű������ݡ�
 */
int main()
{
#if 1
	initfblmanager_task();
	/*when it is not null state,it will continue.*/
	while (g_fblchange.currentstate != FBL_NULL)
	{
		fblmaintask(&g_fblchange);
	}
#if 0//OFF_BOARD
	/*finish the task,free the data back up.*/
	if (g_fblchange.currentstate == FBL_NULL)
	{
		fbl_free_data(g_fblmanager_backup.fblmanagerContextBackup);
	}
#endif
#endif
}