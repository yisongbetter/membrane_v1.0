#include "detector.h"
#include "MvCameraControl.h"
 



typedef struct thread_param
{
	void* handle;
	char key;
}th_param;




/*typedef struct flaw_image
{
	int flag=0;
	char flaw_type[256];
	Mat flawImage;
}fl_image;*/

float fGammaStep=0.1;
float fGainStep=0.5;
void* Key_process(void *ptr);








//int test_num=1;
/////1111111111111111111111111111111111111111111
void __stdcall ImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO* pFrameInfo, void* pUser)
{
    if (pFrameInfo)
    {

	int cols=pFrameInfo->nWidth;
	int rows=pFrameInfo->nHeight;
	Mat Image(rows,cols,CV_8UC3,pData);
	Mat Image2;
	resize(Image,Image2,Size(2448>>2,2048>>2));
	

	cvtColor(Image2,Image2,COLOR_RGB2BGR);
	imshow("mainWin",Image2);


	

	//1.run algo

	my_detector *p_results=(my_detector *)pUser;

	my_detector results;



	/*pthread_t threadAlgo;
	th_al_param algo_param;
	algo_param.src=Image;
	algo_param.nRet=0;
	pthread_create(&threadAlgo,NULL, algo_process, (void *)&algo_param);
	*/



#if 1
	int nRet=defects_detector(Image,&results);
	#else
	char path[256]={0};
	sprintf(path,"./A/%d.bmp",(test_num++)%289);
	Mat src=imread(path,1);
	int nRet=defects_detector(src,&results);
	#endif

	if(1==nRet)
	{
		p_results->my_flag=1;
		//results.defects.copyTo(p_results->defects);
		p_results->defects=results.defects;
		sprintf(p_results->my_label,"%s",results.my_label);
	}



		if(p_results->my_flag!=0)
		{
			
			imshow(p_results->my_label,p_results->defects);
			
		}
		waitKey(1);




	//results->p_results;
	//2. if first has flaw, set pim_s->flag=1; 
	//3. all has flaw, set pim_s->flawImage & flaw_type 
    //4. has no flaw, nothing;



	
       // 输出时加上当前系统时间
      /*   char   szInfo[128] = {0};
        SYSTEMTIME sys; 
        GetLocalTime( &sys );
        sprintf_s(szInfo, 128, "[%d-%02d-%02d %02d:%02d:%02d:%04d] : GetOneFrame succeed, width[%d], height[%d]", sys.wYear, sys.wMonth, 
                  sys.wDay, sys.wHour, sys.wMinute, sys.wSecond,     sys.wMilliseconds, pFrameInfo->nWidth, pFrameInfo->nHeight);

        printf("%s\n", szInfo);*/


    }
}





int main()
{
    int nRet = -1;
    void* m_handle = NULL;

    //枚举子网内指定的传输协议对应的所有设备
     unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
    MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
    nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
    if (MV_OK != nRet)
    {
        printf("error: EnumDevices fail [%x]\n", nRet);
        return -1;
    }

    int i = 0;
    if (m_stDevList.nDeviceNum == 0)
    {
        printf("no camera found!\n");
        return -2;
    }

    //选择查找到的第一台在线设备，创建设备句柄
    int nDeviceIndex = 0;

    MV_CC_DEVICE_INFO m_stDevInfo = {0};
    memcpy(&m_stDevInfo, m_stDevList.pDeviceInfo[nDeviceIndex], sizeof(MV_CC_DEVICE_INFO));

    nRet = MV_CC_CreateHandle(&m_handle, &m_stDevInfo);

    if (MV_OK != nRet)
    {
        printf("error: CreateHandle fail [%x]\n", nRet);
        return -3;
    }

    //连接设备
     unsigned int nAccessMode = MV_ACCESS_Exclusive;
    unsigned short nSwitchoverKey = 0;
           
    nRet = MV_CC_OpenDevice(m_handle, nAccessMode, nSwitchoverKey);
    if (MV_OK != nRet)
    {
        printf("error: OpenDevice fail [%x]\n", nRet);
        return -4;
    }
    //...其他处理 setup other param 
////2222222222222222222222222222222222222222222222222
    nRet = MV_CC_SetExposureTime(m_handle, 80000);
     unsigned int enValue = PixelType_Gvsp_RGB8_Packed ;
    nRet = MV_CC_SetPixelFormat(m_handle, enValue);
    if (MV_OK != nRet)
    {
        printf("error: SetPixelFormat fail [%x]\n", nRet);
        return -3;
    }

 
	

	my_detector im_s;	
	im_s.my_flag=0;

    nRet = MV_CC_RegisterImageCallBack(m_handle, ImageCallBack, (void*) &im_s);
    if (MV_OK != nRet)
    {
        printf("error: RegisterImageCallBack fail [%x]\n", nRet);
        return -5;
    }

   	 //开始采集图像
    	 nRet = MV_CC_StartGrabbing(m_handle);
   	 if (MV_OK != nRet)
   	 {
        	 printf("error: StartGrabbing fail [%x]\n", nRet);
       		 return -6;
   	 }


	pthread_t threadKey;
	th_param param1;
	param1.handle=m_handle;
	param1.key='0';
	pthread_create(&threadKey,NULL, Key_process, (void *)&param1);


///333333333333333333333333333333333333333
	while(param1.key!='q')
	{	

		/*if(im_s.my_flag!=0)
		{
			printf("main->detects show\n");
			imshow(im_s.my_label,im_s.defects);
			waitKey(10);
//show flaw image with box;
		}*/
		sleep(1);
	}

    //...其他处理 

    //停止采集图像 
     nRet = MV_CC_StopGrabbing(m_handle);
    if (MV_OK != nRet)
    {
        printf("Operation: Stop Grabbing......failed!");
        return -7;
    }

     //关闭设备，释放资源
     nRet = MV_CC_CloseDevice(m_handle);
    if (MV_OK != nRet)
    {
        printf("error: CloseDevice fail [%x]\n", nRet);
        return -8;
    }

    //销毁句柄，释放资源
     nRet = MV_CC_DestroyHandle(m_handle);
    if (MV_OK != nRet)
    {
        printf("error: DestroyHandle fail [%x]\n", nRet);
        return -9;
    }
} 






void* Key_process(void *ptr)
{
	th_param* param1=(th_param*)ptr;
	
printf("\n\n*********************\nUsages:\n");
	printf("'w': Increase Gain \n");
	printf("'s': Decrease Gain \n");
	printf("'d': Increase Gamma \n");
	printf("'a': Decrease Gamma \n");
	printf("'h': Display this 'Usages' panel \n");
	printf("'q': Exit \n*********************\n");

	while(1)
	{


		scanf("%c",&(param1->key));


		switch((param1->key))
		{
		case 'w':
			{
				void*m_handle=param1->handle;
        			MVCC_FLOATVALUE struFloatValue = {0};
				int nRet = MV_CC_GetGain(m_handle, &struFloatValue);
				printf("GainV:%4.1lf in (%4.1lf %4.1lf)\n",struFloatValue.fCurValue,struFloatValue.fMin,struFloatValue.fMax);
				if (MV_OK != nRet)
				        printf("error: GetGainMode fail [%x]\n", nRet);
				else if(struFloatValue.fCurValue!=struFloatValue.fMax)
				{
					float vl=struFloatValue.fCurValue+fGainStep;
					vl=vl<struFloatValue.fMax?vl:struFloatValue.fMax;
					 nRet = MV_CC_SetGain(m_handle, vl);
					if (MV_OK != nRet)
					{
				        	printf("error: SetGain fail [%x]\n", nRet);
					}
				}
				
				break;
			}
		case 'a':
{
				void*m_handle=param1->handle;
        			MVCC_FLOATVALUE struFloatValue = {0}; 

			       int nRet = MV_CC_GetGamma(m_handle, &struFloatValue);
				printf("GammaV:%4.1lf in (%4.1lf %4.1lf)\n",struFloatValue.fCurValue,struFloatValue.fMin,struFloatValue.fMax);
				if (MV_OK != nRet)
				        printf("error: GetGainMode fail [%x]\n", nRet);
				else if(struFloatValue.fCurValue!=struFloatValue.fMax)
				{
					float vl=struFloatValue.fCurValue+fGammaStep;
					vl=vl<struFloatValue.fMax?vl:struFloatValue.fMax;
					  nRet = MV_CC_SetGamma(m_handle, vl);
					if (MV_OK != nRet)
					{
				        	printf("error: SetGain fail [%x]\n", nRet);
					}
				}
				
				break;
			}





		case 's':
{
				void*m_handle=param1->handle;
        			MVCC_FLOATVALUE struFloatValue = {0};
				int nRet = MV_CC_GetGain(m_handle, &struFloatValue);
printf("GainV:%4.1lf in (%4.1lf %4.1lf)\n",struFloatValue.fCurValue,struFloatValue.fMin,struFloatValue.fMax);
				if (MV_OK != nRet)
				        printf("error: GetGain fail [%x]\n", nRet);
				else if(struFloatValue.fCurValue!=struFloatValue.fMin)
				{
					float vl=struFloatValue.fCurValue-fGainStep;
					vl=vl>struFloatValue.fMin?vl:struFloatValue.fMin;
					nRet = MV_CC_SetGain(m_handle, vl);
				    if (MV_OK != nRet)
				    {
				        printf("error: SetGain fail [%x]\n", nRet);
				    }


				}
				
				break;
			}


		case 'd':
{
				void*m_handle=param1->handle;
        			MVCC_FLOATVALUE struFloatValue = {0}; 

			       int nRet = MV_CC_GetGamma(m_handle, &struFloatValue);
				printf("GammaV:%4.1lf in (%4.1lf %4.1lf)\n",struFloatValue.fCurValue,struFloatValue.fMin,struFloatValue.fMax);
				if (MV_OK != nRet)
				        printf("error: GetGamma fail [%x]\n", nRet);
				else if(struFloatValue.fCurValue!=struFloatValue.fMin)
				{
					float vl=struFloatValue.fCurValue-fGammaStep;
					vl=vl>struFloatValue.fMin?vl:struFloatValue.fMin;
					  nRet = MV_CC_SetGamma(m_handle, vl);
					if (MV_OK != nRet)
					{
				        	printf("error: SetGamma fail [%x]\n", nRet);
					}
				}
				
				break;
			}

case 'h':
{
printf("\n\n*********************\nUsages:\n");
	printf("'w': Increase Gain \n");
	printf("'s': Decrease Gain \n");
	printf("'d': Increase Gamma \n");
	printf("'a': Decrease Gamma \n");
	printf("'q': Exit \n*********************\n");
break;
}

		case 'q':return NULL;

		default:break;
		}
	}
}



