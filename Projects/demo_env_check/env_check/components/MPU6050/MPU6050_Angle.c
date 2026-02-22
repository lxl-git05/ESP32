#include "MPU6050_Angle.h"

// 全局变量
ImuOffset_Typedef  	 MPU_Offset;			// 误差纠正参数
ImuCali_Typedef		 MPU_Cali	 ;			// 纠正后的数据
ImuReal_Typedef 	 MPU_Real  ;			// 最终的确定角度
int isMPU_Still_Flag ;						// MPU6050静止状态检测

// 积分后的角度
float gyroAngleX = 0.0f ;
float gyroAngleY = 0.0f ;

// 自动纠正误差(可配合按键使用)
void MPU6050_Data_Error_Check(int Sample_Cnt)
{
    if (Sample_Cnt < 200 || Sample_Cnt > 2000)
        Sample_Cnt = 1000;  // 默认1000次，更稳定

    float sum_ax = 0.0f, sum_ay = 0.0f, sum_az = 0.0f;
    float sum_gx = 0.0f, sum_gy = 0.0f, sum_gz = 0.0f;

    for (int i = 0; i < Sample_Cnt; i++)
    {
        MPU6050_Update_Data();

        sum_ax += MPU_Raw_Data.AX;
        sum_ay += MPU_Raw_Data.AY;
        sum_az += MPU_Raw_Data.AZ;
        sum_gx += MPU_Raw_Data.GX;
        sum_gy += MPU_Raw_Data.GY;
        sum_gz += MPU_Raw_Data.GZ;
    }

    // 原始数据零偏（g 和 °/s）
    MPU_Offset.AccErrorX = sum_ax / Sample_Cnt;
    MPU_Offset.AccErrorY = sum_ay / Sample_Cnt;
    // 可选：计算AZ相对于1g的偏移（小偏移校正）
    MPU_Offset.AccErrorZ = sum_az / Sample_Cnt - 1.0f;  // 只减小偏差，不减重力

    MPU_Offset.GyroErrorX = sum_gx / Sample_Cnt;
    MPU_Offset.GyroErrorY = sum_gy / Sample_Cnt;
    MPU_Offset.GyroErrorZ = sum_gz / Sample_Cnt;

    // 初始化姿态
    MPU_Real.roll  = 0.0f;
    MPU_Real.pitch = 0.0f;
    MPU_Real.yaw   = 0.0f;

    MPU_Real.AccX = 0.0f;
    MPU_Real.AccY = 0.0f;
    MPU_Real.AccZ = 1.0f;  // 重点
}
// 检测静止状态函数 18us , 检测静止使用了历史数据，待优化为随环境而变
void MPU_Still_Check(void)
{
	// 函数顺序执行，确保得到小车真实的状态
	static int still_times_cnt = 0;	// 静止状态次数统计,超过100次视为静止
	
	float MPU_Temp_Cali_Data[6] = {MPU_Raw_Data.AX - MPU6050_AX_Offset , MPU_Raw_Data.AY - MPU6050_AY_Offset , MPU_Raw_Data.AZ - MPU6050_AZ_Offset,
													MPU_Raw_Data.GX - MPU6050_GX_Offset , MPU_Raw_Data.GY - MPU6050_GY_Offset , MPU_Raw_Data.GZ - MPU6050_GZ_Offset};
	// 先检测ACC,综合大于0.05g即为运动,千万记得AZ - 1
	if ( MPU_Temp_Cali_Data[0] * MPU_Temp_Cali_Data[0] + MPU_Temp_Cali_Data[1] * MPU_Temp_Cali_Data[1] + (MPU_Temp_Cali_Data[2] - 1.0f) * (MPU_Temp_Cali_Data[2] - 1.0f) > STILL_ACCEL_THRES_BASE_SQ )
	{
		still_times_cnt = 0 ;
		isMPU_Still_Flag = 0 ;	// 在运动
		return ;
	}
	// 综合角速度大于3度为运动
	if ( MPU_Temp_Cali_Data[3] * MPU_Temp_Cali_Data[3] + MPU_Temp_Cali_Data[4] * MPU_Temp_Cali_Data[4] + MPU_Temp_Cali_Data[5] * MPU_Temp_Cali_Data[5]> STILL_GYRO_THRES_BASE_SQ)
	{
		still_times_cnt = 0 ;
		isMPU_Still_Flag = 0 ;	// 在运动
		return ;
	}
	// 静止
	still_times_cnt ++ ;
	// 确定静止
	if (still_times_cnt == STILL_REQUIRED_CNT)
	{
		still_times_cnt = 0 ;
		isMPU_Still_Flag = 1 ;
	}
}

// 静止检测后自动纠正零漂 30us左右
void MPU6050_Data_Error_Check_Auto(void)
{
	if (isMPU_Still_Flag == 1)  // 确认静止
	{
		// 加速度（X/Y趋0，Z趋1g）
		MPU_Offset.AccErrorX += (MPU_Raw_Data.AX - MPU_Offset.AccErrorX) * OFFSET_LEARNING_RATE;
		MPU_Offset.AccErrorY += (MPU_Raw_Data.AY - MPU_Offset.AccErrorY) * OFFSET_LEARNING_RATE;
		MPU_Offset.AccErrorZ += ( (MPU_Raw_Data.AZ - 1.0f) - MPU_Offset.AccErrorZ) * OFFSET_LEARNING_RATE;
		
		// 陀螺仪零偏缓慢向当前静止值收敛
		MPU_Offset.GyroErrorX += (MPU_Raw_Data.GX - MPU_Offset.GyroErrorX) * OFFSET_LEARNING_RATE;
		MPU_Offset.GyroErrorY += (MPU_Raw_Data.GY - MPU_Offset.GyroErrorY) * OFFSET_LEARNING_RATE;
		MPU_Offset.GyroErrorZ += (MPU_Raw_Data.GZ - MPU_Offset.GyroErrorZ) * OFFSET_LEARNING_RATE;
	}
}

// 减去误差后的归零数据
void MPU6050_Raw_Error_Update(void)
{	
	// 得到归零数据
	MPU_Cali.AX = MPU_Raw_Data.AX - MPU_Offset.AccErrorX ;
	MPU_Cali.AY = MPU_Raw_Data.AY - MPU_Offset.AccErrorY ;
	MPU_Cali.AZ = MPU_Raw_Data.AZ - MPU_Offset.AccErrorZ ;
	
	MPU_Cali.GX = MPU_Raw_Data.GX - MPU_Offset.GyroErrorX ;
	MPU_Cali.GY = MPU_Raw_Data.GY - MPU_Offset.GyroErrorY ;
	MPU_Cali.GZ = MPU_Raw_Data.GZ - MPU_Offset.GyroErrorZ ;
}

// 数据深度处理
void MPU6050_Raw_Deal(int Deal_dt_ms)
{
	// 根据加速度得到静态角度
	// 计算翻滚角（绕 X 轴）— 使用 Y 和 Z 轴的加速度值
	float accAngleX = atan2(MPU_Cali.AY, MPU_Cali.AZ)  * 180 / 3.14159265358 ;
	// 计算俯仰角（绕 Y 轴）— 使用 X 和 Z 轴的加速度值
	float accAngleY = atan2(-MPU_Cali.AX, MPU_Cali.AZ) * 180 / 3.14159265358 ;
	
	// 得到粗略积分角度
	gyroAngleX += MPU_Cali.GX * Deal_dt_ms * 1.0 / 1000;
	gyroAngleY += MPU_Cali.GY * Deal_dt_ms * 1.0 / 1000;
	
	// 得到偏航角yaw
	if (MPU_Cali.GZ < 0)
	{
		MPU_Real.yaw += MPU_Cali.GZ * Deal_dt_ms * 1.0  / 1000;
	}
	else
	{
		MPU_Real.yaw += MPU_Cali.GZ * Deal_dt_ms * 1.08 / 1000;		// 标定的误差参数
	}
	
	// 得到互补滤波角度
	gyroAngleX = 0.98 * gyroAngleX + 0.02 * accAngleX;
	gyroAngleY = 0.98 * gyroAngleY + 0.02 * accAngleY;
	
	// 得到Roll和Pitch
	MPU_Real.roll  = gyroAngleX ;
	MPU_Real.pitch = gyroAngleY;
	// Ax Ay Az
	MPU_Real.AccX = MPU_Cali.AX ;
	MPU_Real.AccY = MPU_Cali.AY ;
	MPU_Real.AccZ = MPU_Cali.AZ ;
}

// =========== Task: MPU6050数据读取(25ms)s ============ 
void Task_MPU(void *param)
{
    // setup
    // 存在互斥锁,所以在全局里面顺序进行初始化
    MPU_Offset.AccErrorX  = 0.0927990749f ;
    MPU_Offset.AccErrorY  = -0.0469766855f;
    MPU_Offset.AccErrorZ  = 0.00974702835f;
    MPU_Offset.GyroErrorX = -2.07370448f  ;
    MPU_Offset.GyroErrorY = 1.26626182f   ;
    MPU_Offset.GyroErrorZ = 0.0725877061f ;
    #define MPU_Delay_time_ms 25    // MPU6050采样时间(ms)
    // loop
    while (1)
    {
        if (xSemaphoreTake(I2C_Mutex , portMAX_DELAY))
        {
            // 三项总共550us
            MPU6050_Update_Data() ; // 470 us

            MPU6050_Raw_Error_Update() ;

            MPU6050_Raw_Deal(MPU_Delay_time_ms) ;
            if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
            {
                Sensor_Data.AccX = MPU_Real.AccX ;
                Sensor_Data.AccY = MPU_Real.AccY ;
                Sensor_Data.AccZ = MPU_Real.AccZ ;
                Sensor_Data.roll = MPU_Real.roll ;
                Sensor_Data.pitch= MPU_Real.pitch;
                Sensor_Data.yaw  = MPU_Real.yaw  ;
                xSemaphoreGive(data_Mutex) ;
            }
            // 尾处理
            xSemaphoreGive(I2C_Mutex) ;
        }
        vTaskDelay(pdMS_TO_TICKS(MPU_Delay_time_ms)) ;
    } 
}
