#ifndef _CHASIS_CACULATION_H_
#define _CHASIS_CACULATION_H_
					
#ifdef __cplusplus
extern "C"
{
#endif

/*�ڴ˴������ⲿ�ļ���       begin*/	
#include <math.h>

/*�����ⲿ�ļ�end*/	


/*�ڴ˴���.c�ļ�����������       begin*/

/*��������end*/	
	
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*�ڴ˴����к궨�壺         begin*/	
#define PI 3.1415926	// �����
/*�궨��end*/	


/*�ڴ˴���ö�����Ͷ��壺         begin*/	

/*ö�ٶ���end*/	


/*�ڴ˴�������ͽṹ��Ķ��壺begin*/	

// ������̵���
class chasis{
	// �ӿڲ���
	public:
		float front_wheel_spe ,left_wheel_spe, right_wheel_spe;											// �������ӵ�Ŀ���ٶ�
		chasis(void){													// ���캯��,��������������ʼ��Ϊ0
			front_wheel_spe = 0;
			left_wheel_spe  = 0;
			right_wheel_spe = 0;
		}
		void robot_cal(float tgvx, float tgvy, float tgw);				// ����������ϵ�µĽ���
		void world_cal(float tgvx, float tgvy, float tgw, float angle_bias);		// ��������ϵ�µĽ���
	//�ǽӿڲ���
	private:
};

/*��ͽṹ�嶨��end*/	


/*�ڴ˴����к������壺       begin*/	


/*��������end*/	

#endif

#endif

