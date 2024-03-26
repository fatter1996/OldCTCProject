#include "TrainAndTest.h"
#include "MyDoc/mydoc.h"
CTrainAndTest::CTrainAndTest()
{
    type = 0;
    //pDoc = &MyDoc::GetInstance();
}

void CTrainAndTest::CheckUdpData(unsigned char *Rec, int len)
{
    int Framelen = 0;
    for(int i=0;i<len;i++)
    {
        if(Rec[i]==0xEF && Rec[i+1]==0xEF && Rec[i+2]==0xEF && Rec[i+3]==0xEF)
        {
            memcpy(&Framelen,Rec+i+4,2);
            if (Rec[i+Framelen-1]==0xFE && Rec[i+Framelen-2]==0xFE && Rec[i+Framelen-3]==0xFE && Rec[i+Framelen-4]==0xFE)
            {
                AnalyUdpData(Rec+8,Framelen);
                if(Rec[9] == 0x13)// 调度命令接收  20200806
                {
//					pDoc->DDMLAnalysis(Rec);
                }
                break;
            }
        }
    }
}
void CTrainAndTest::AnalyUdpData(unsigned char *Rec, int len)
{
    MyStation* pStation = pDoc->getMyStationByIndexInStaArray(pDoc->currStaIndex);

    OrderStr orderStr;
    int cmdLen = 0;
    unsigned char TXT[500] = { 0 };
    QString ParaStr;
    int index;
    index = 1;
    type = Rec[0];
    if(Rec[index] == 0x11)//场景设置
    {
        index++;
        cmdLen = Rec[++index];
        memcpy(TXT, &Rec[++index],cmdLen);
        orderStr.FunStr = uncharToQString(TXT,cmdLen);


        index = index + cmdLen;
        cmdLen = Rec[index];

        memset(TXT,0,sizeof(TXT));
        memcpy(TXT, &Rec[++index],cmdLen);
        ParaStr = uncharToQString(TXT,cmdLen);

        orderStr.ParaStr = ParaStr;

        QStringList StrArray;
        StringSplit(ParaStr,",",StrArray);
        if(StrArray.size()==1)
        {
            orderStr.DevName = StrArray[0];
        }
        else if(StrArray.size()==2)
        {
            orderStr.SubFunStr = StrArray[0];
            orderStr.DevName = StrArray[1];
        }
        else if(StrArray.size()==3)
        {
            orderStr.SubFunStr = StrArray[0];
            orderStr.TipShowStr = StrArray[1];
            orderStr.DevName = StrArray[2];
        }
        v_OrderArray.push_back(orderStr);
        ManageSpecialOrder();
    }
    else if(Rec[index] == 0x12)//阶段计划接收
    {
//        pDoc->JDJHAnalysis(Rec+2);
    }
    else if(Rec[index] == 0x66)//清除场景
    {
        CloseMessageBox();
        this->v_OrderArray.clear();

        pStation->ResetStationInfo();
        emit pDoc->TrainRouteOrderUpData();
        emit pDoc->ResetStationSignal();
    }
    else if(Rec[index] == 0xaa)//清除站场演示信息  返回到CTC主页面
    {
        CloseMessageBox();
        this->v_OrderArray.clear();

        pStation->ResetStationInfo();
        emit pDoc->TrainRouteOrderUpData();
        emit pDoc->ResetStationSignal();
    }
}
void CTrainAndTest::CloseMessageBox()
{
//	CMainFrame *pFrame=(CMainFrame *)AfxGetApp()->m_pMainWnd;
//	HWND  hWnd = NULL;

//	hWnd = ::FindWindow(NULL,SYS_TITLE); //查找消息框的标题
//	if(hWnd)
//	{
//		HWND hForeWnd = hWnd;
//		DWORD dwForeID;
//		DWORD dwCurID;

//		hForeWnd =  GetForegroundWindow();
//		dwCurID =  GetCurrentThreadId();
//		dwForeID =  GetWindowThreadProcessId( hForeWnd, NULL );
//		AttachThreadInput( dwCurID, dwForeID, TRUE);
//		SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );
//        SetWindowPos( hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );
//		SetForegroundWindow(hWnd);
//		AttachThreadInput( dwCurID, dwForeID, FALSE);
//	}

//	CStringArray TitleStr;
//	TitleStr.Add(_T("请输入车次号"));
//	TitleStr.Add(_T("进路预计占用时间"));
//	TitleStr.Add(_T("请输入密码"));
//	TitleStr.Add(_T("阶段计划签收"));
//	TitleStr.Add(_T("添加车次号"));
//	TitleStr.Add(_T("车次号确认或更改"));
//	TitleStr.Add(_T("车次号删除"));
//	TitleStr.Add(_T("模式转换"));

//	hWnd = ::FindWindow(_T("CDlgEnsure"),NULL); //查找消息框的标题
//	if (hWnd)
//	{
//		::PostMessage((HWND)hWnd,WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), NULL);
//	}

//	if(TitleStr.GetSize()>0)
//	{
//		for(int i =0;i<TitleStr.GetSize();i++)
//		{
//			HWND  hWnd = ::FindWindow(NULL,TitleStr.GetAt(i)); //查找消息框的标题
//			if (hWnd)
//			{
//				::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//			}
//		}
//	}
//	pDoc->SetYDZSButtonTwinkle(0x33);
//	pDoc->SetMSZHButtonTwinkle(0x22);
}
void CTrainAndTest::HandleTrainSence()
{
    //int size = v_OrderArray.size();
    //for(int i=0;i<size;i++)
    //{
    //	v_OrderArray[i].SetFlag = TRUE;
    //}
    //for(int i=0;i<v_OrderArray.size();i++)
    //{
    //	if(v_OrderArray[i].SetFlag)
    //		v_OrderArray.erase(v_OrderArray.begin()+i);
    //}
}
void CTrainAndTest::DrawSence(QPainter *painter)
{
//	if(DDMLCount>0)
//		DDMLCount--;
//	if(DDMLCount == 0)
//	{
//		HWND  hWnd = ::FindWindow(NULL,_T("签收调度命令")); //查找消息框的标题
//		if (hWnd)
//		{
//			::PostMessage((HWND)hWnd,WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), NULL);
//		}
//		hWnd = ::FindWindow(NULL,_T("请选择命令类型")); //查找消息框的标题
//		if (hWnd)
//		{
//			::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//		}
//		DDMLCount--;
//	}
//	if(XCPZCount>0)
//		XCPZCount--;
//	if(XCPZCount == 0)
//	{
//		CStringArray TitleStr;
//		TitleStr.Add(_T("新建路票"));
//		TitleStr.Add(_T("新建绿色许可证"));
//		TitleStr.Add(_T("新建红色许可证"));

//		if(TitleStr.GetSize()>0)
//		{
//			for(int i =0;i<TitleStr.GetSize();i++)
//			{
//				HWND  hWnd = ::FindWindow(NULL,TitleStr.GetAt(i)); //查找消息框的标题
//				if(hWnd)
//					::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//			}
//		}
//		XCPZCount--;
//	}

//	int count = v_OrderArray.size();
//	for(int i = 0; i < v_OrderArray.size(); i++)
//	{
//		DrawSence(pDC,v_OrderArray[i],i);
//	}
}
void CTrainAndTest::ManageSpecialOrder()
{
//	CMainFrame *pFrame=(CMainFrame *)AfxGetApp()->m_pMainWnd;
//	int size = v_OrderArray.size();
//	for(int i = 0; i < v_OrderArray.size(); i++)
//	{
//		if(v_OrderArray[i].FunStr.Find(_T("JDJHML-MEMU"))!=-1)//发送一包阶段计划命令
//		{
//			OrderStr ExapleStr = v_OrderArray[i];
//			v_OrderArray.erase(v_OrderArray.begin()+i);
//			if(ExapleStr.DevName == "QS")
//			{
//				pFrame->m_wndDispatchOrderBar.OnBnClickedBtnStageplan();
//			}
//			else
//			{
//				//unsigned char JDJH[60]={
//				//	0xEF, 0xEF, 0xEF, 0xEF, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x33, 0xE9, 0x03, 0x54, 0x31, 0x32, 0x33, 0x34, 0x00, 0x54, 0x31,
//				//	0x32, 0x33, 0x34, 0x00, 0x11, 0x51, 0x00, 0xE4, 0x07, 0x05, 0x1B, 0x11, 0x1A, 0x00, 0x51, 0x00, 0xE4, 0x07, 0x05, 0x1B,
//				//	0x11, 0x1A, 0x00, 0xcc, 0x19, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0xFE
//				//};
//				unsigned int GDCode = getDevNodeOfName(ExapleStr.TipShowStr);
//				unsigned int XHCode = getDevNodeOfName(ExapleStr.DevName);
//				CTime m_currTime, m_Time1, m_Time2;
//				m_currTime = CTime::GetCurrentTime();
//				m_Time1 = m_currTime + CTimeSpan(0,0,2,0);//加2分钟//CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs);
//				m_Time2 = m_currTime + CTimeSpan(0,0,4,0);//加3分钟

//				//NEW.2021.8.28.LWM
//				int nLength = 0;
//				int nCount = 9;
//				unsigned char JDJH[500];
//				memset(JDJH, 0x00, 500);

//				//命令类型-阶段计划
//				JDJH[nCount] = 0x33;
//				nCount += 1;

//				PlanCode ++;
//				//memcpy(JDJH+10,&PlanCode,2);//计划号2B
//				memcpy(&JDJH[nCount],&PlanCode,4);//计划号4B
//				nCount += 4;

//				//到达车次
//				Num ++;
//				unsigned char ch[6]={0x54, 0x31, 0x32, 0x33, 0x34,0x00};
//				ch[4] = Num;
//				//memcpy(&JDJH[12],ch,6);
//				JDJH[nCount] = 0x05;
//				nCount += 1;
//				memcpy(&JDJH[nCount],ch,5);
//				nCount += 5;

//				//出发车次
//				Num++;
//				ch[4] = Num;
//				//memcpy(&JDJH[18],ch,6);
//				JDJH[nCount] = 0x05;
//				nCount += 1;
//				memcpy(&JDJH[nCount],ch,5);
//				nCount += 5;

//				//类型：添加还是删除
//				JDJH[nCount] = 0x11;//添加
//				nCount += 1;

//				if(GDCode!=0xffff && XHCode!=0xffff)
//				{
//					int  Year_DC = m_Time1.GetYear();
//					int  Month_DC = m_Time1.GetMonth();
//					int  Day_DC = m_Time1.GetDay();
//					int  Hour_DC = m_Time1.GetHour();
//					int  Minute_DC = m_Time1.GetMinute();
//					//接车股道
//					//memcpy(JDJH+25,&GDCode,2);
//					memcpy(&JDJH[nCount],&GDCode,2);
//					nCount += 2;
//					///到达时间
//					//memcpy(JDJH+27,&Year_DC,2);
//					//memcpy(JDJH+29,&Month_DC,1);
//					//memcpy(JDJH+30,&Day_DC,1);
//					//memcpy(JDJH+31,&Hour_DC,1);
//					//memcpy(JDJH+32,&Minute_DC,1);
//					memcpy(&JDJH[nCount],&Year_DC,2);
//					memcpy(&JDJH[nCount+2],&Month_DC,1);
//					memcpy(&JDJH[nCount+3],&Day_DC,1);
//					memcpy(&JDJH[nCount+4],&Hour_DC,1);
//					memcpy(&JDJH[nCount+5],&Minute_DC,1);
//					JDJH[nCount+6] = 0x00;//秒
//					nCount += 7;

//					Year_DC = m_Time2.GetYear();
//					Month_DC = m_Time2.GetMonth();
//					Day_DC = m_Time2.GetDay();
//					Hour_DC = m_Time2.GetHour();
//					Minute_DC = m_Time2.GetMinute();
//					//发车股道
//					//memcpy(JDJH+34,&GDCode,2);
//					memcpy(&JDJH[nCount],&GDCode,2);
//					nCount += 2;
//					//出发时间
//					//memcpy(JDJH+36,&Year_DC,2);
//					//memcpy(JDJH+38,&Month_DC,1);
//					//memcpy(JDJH+39,&Day_DC,1);
//					//memcpy(JDJH+40,&Hour_DC,1);
//					//memcpy(JDJH+41,&Minute_DC,1);
//					memcpy(&JDJH[nCount],&Year_DC,2);
//					memcpy(&JDJH[nCount+2],&Month_DC,1);
//					memcpy(&JDJH[nCount+3],&Day_DC,1);
//					memcpy(&JDJH[nCount+4],&Hour_DC,1);
//					memcpy(&JDJH[nCount+5],&Minute_DC,1);
//					JDJH[nCount+6] = 0x00;//秒
//					nCount += 7;

//					//始发终到标志
//					if(ExapleStr.SubFunStr == "ZD")
//					{
//						//JDJH[43]=0xcc;
//						JDJH[nCount]=0xcc;
//					}
//					else if(ExapleStr.SubFunStr == "SF")
//					{
//						//JDJH[43]=0xbb;
//						JDJH[nCount]=0xbb;
//					}
//					else
//					{
//						return;
//					}
//					nCount += 1;

//					//接车口/进站口
//					//memcpy(JDJH+44,&XHCode,2);
//					memcpy(&JDJH[nCount],&XHCode,2);
//					nCount += 2;

//					//发车口/出站口
//					//memcpy(JDJH+46,&XHCode,2);
//					memcpy(&JDJH[nCount],&XHCode,2);
//					nCount += 2;

//					//电力
//					JDJH[nCount] = 0x11;
//					nCount += 1;
//					//超限
//					JDJH[nCount] = 0x00;
//					nCount += 1;
//					//列车客货类型
//					JDJH[nCount] = LCTYPE_KC;
//					nCount += 1;
//					//列车类型序号
//					JDJH[nCount] = 0x00;
//					nCount += 1;
//					//运行类型序号
//					JDJH[nCount] = 0x00;
//					nCount += 1;

//					//memcpy(pDoc->m_chArrayRecvDataBuffToJSJ[0],JDJH,60);
//					//pDoc->RecvStagePlanData();
//					nLength=nCount+4;
//					memcpy(&JDJH[4], &nLength, 2);//帧长度
//					for(int i = 0; i < 4; i++)
//					{
//						JDJH[i] = 0xEF;
//						JDJH[nLength - i -1] = 0xFE;
//					}
//					pDoc->RecvStagePlanData(JDJH, nLength);
//				}
//			}
//		}
//		else if(v_OrderArray[i].FunStr.Find(_T("DDMLJS-MEMU"))!=-1)//调度命令接收
//		{
//			if(v_OrderArray[i].DevName == "OK")
//			{

//				HWND  hWnd = ::FindWindow(NULL,_T("签收调度命令")); //查找消息框的标题
//				if (hWnd)
//				{
//					::PostMessage((HWND)hWnd,WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), NULL);
//				}
//			}
//			else
//			{
//				DDMLCount = 4;
//				pFrame->m_wndDispatchOrderBar.OnBnClickedDispatchButton();
//			}
//			v_OrderArray.erase(v_OrderArray.begin()+i);
//		}
//		else if(v_OrderArray[i].FunStr.Find(_T("DDMLCX-MEMU"))!=-1)//调度命令查询
//		{
//			pFrame->OnDispatchorderMenu();
//			if(v_OrderArray[i].DevName == _T("CX"))
//				pFrame->m_pDispatchOrderDlg->DDMLCheck();
//			/*else if(v_OrderArray[i].DevName == _T("XS"))
//				((CDispatchFormView *)(pFrame->m_pDispatchFormView))->m_page3.ShowDisOrder(0);*/
//			v_OrderArray.erase(v_OrderArray.begin()+i);
//		}
//		else if(v_OrderArray[i].FunStr.Find(_T("XCPZBJ-MEMU"))!=-1)//行车凭证页面
//		{
//			pFrame->OnDispatchorderMenu();
//			if(v_OrderArray[i].DevName == "MAIN")//主页面
//			{
//				pFrame->m_pDispatchOrderDlg->TurnXCPZPage();
//			}
//			else if(v_OrderArray[i].DevName == "NEW")//新建行车凭证
//			{
//				DDMLCount = 4;
//				CDlgSelectDisType dlgSelectDisType; //机车调度命令类型选择
//				dlgSelectDisType.DoModal();
//				//HWND  hWnd = ::FindWindow(NULL,_T("调度命令")); //查找消息框的标题
//				//if (hWnd)
//				//{
//				//	::PostMessage((HWND)hWnd,WM_COMMAND, MAKEWPARAM(IDC_BTN_NEWTRAINDIS, BN_CLICKED), NULL);
//				//}
//			}
//			else if(v_OrderArray[i].DevName == "路票")//路票演示
//			{
//				XCPZCount = 4;
//				CDlgNewTrainDis dlgNewTrainDis(NULL,2);	//新建机车调度命令
//			    dlgNewTrainDis.DoModal();
//			}
//			else if(v_OrderArray[i].DevName == "绿色许可证")//绿色许可证演示
//			{
//				XCPZCount = 4;
//				CDlgNewTrainDis dlgNewTrainDis(NULL,3);	//新建机车调度命令
//			    dlgNewTrainDis.DoModal();
//			}
//			else if(v_OrderArray[i].DevName == "红色许可证")//红色许可证演示
//			{
//				XCPZCount = 4;
//				CDlgNewTrainDis dlgNewTrainDis(NULL,4);	//新建机车调度命令
//			    dlgNewTrainDis.DoModal();
//			}
//			v_OrderArray.erase(v_OrderArray.begin()+i);
//		}
//		else if(v_OrderArray[i].FunStr.Find(_T("FSFCQQ-MEMU"))!=-1)//发车请求
//		{
//			pDoc->SetXCRZTrainMessage(0,0x11);
//			v_OrderArray.erase(v_OrderArray.begin()+i);
//		}
//		else if(v_OrderArray[i].FunStr.Find(_T("TYLZFC-MEMU"))!=-1)//同意邻站发车
//		{
//			pDoc->SetXCRZTrainMessage(0,0x33);
//			v_OrderArray.erase(v_OrderArray.begin()+i);
//		}
//	}
}
void CTrainAndTest::DrawSence(QPainter *painter,OrderStr ExapleStr,int index)
{
//	CMainFrame *pFrame=(CMainFrame *)AfxGetApp()->m_pMainWnd;
//	CString Getstring;
//	if(ExapleStr.FunStr.Find(_T("DDML-MEMU"))!=-1)
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		//unsigned char DDML[500]={
//		//	0xEF, 0xEF ,0xEF, 0xEF, 0xF4, 0x01, 0x05, 0x01, 0xBB, 0x99, 0x31, 0x00 ,0x00, 0x00, 0x00, 0x00, 0xE4, 0x07, 0x05 ,0x14,
//		//	0x0B, 0x08, 0x00, 0x44 ,0x45, 0x46 ,0x41 ,0x55 ,0x4C, 0x54 ,0x00 ,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0x00, 0x00 ,
//		//	0x00, 0x00, 0x00, 0xBD, 0xD3, 0xB4, 0xA5, 0xCD, 0xF8, 0xCD, 0xA3, 0xA1, 0xA2, 0xCB, 0xCD, 0xB5, 0xE7, 0xBC, 0xB0, 0xCE,
//		//	0xAC, 0xD0, 0xDE, 0xD7, 0xF7, 0xD2, 0xB5, 0x00, 0x00, 0x00, 0x00, 0x00 ,0x00, 0x00 ,0x00 ,0x00 ,0x00, 0x00, 0x00, 0x00 ,
//		//	0x00, 0x00, 0x00 ,0x31, 0x2E, 0xBD, 0xD3 ,0xB4 ,0xA5 ,0xCD, 0xF8, 0xD3, 0xD0, 0xBC, 0xC6, 0xB0, 0xAE, 0xCD, 0xA3, 0xA1 ,
//		//	0xA2, 0xCB, 0xCD, 0xB5, 0xE7, 0xBC, 0xB0, 0xCE, 0xAC, 0xD0, 0xDE, 0xD7, 0xF7, 0xD2, 0xB5, 0x0D, 0x0A, 0x20, 0x20, 0x20,
//		//	0xB8, 0xF9, 0xBE, 0xDD, 0xB9, 0xA9, 0xB5, 0xE7, 0xB5, 0xF7, 0xB6 ,0xC8, 0x20, 0x20, 0x20, 0x20, 0xBA, 0xC5, 0xC9, 0xEA ,
//		//	0xC7, 0xEB, 0xA3, 0xAC, 0xD7, 0xD4, 0xBD, 0xD3, 0xC1, 0xEE, 0xCA, 0xB1, 0x28, 0xCA ,0xB1, 0x20, 0x20, 0x20 ,0x20, 0xB7 ,
//		//	0xD6, 0x29, 0x20, 0x20, 0x20, 0xB4, 0xCE, 0xC1, 0xD0, 0xB3, 0xB5, 0xB5, 0xBD, 0xB4 ,0xEF, 0x20, 0x20, 0x20 ,0xD5, 0xBE ,
//		//	0xA3, 0xA9, 0xC6, 0xF0, 0xD6, 0xC1, 0x20, 0x20, 0xCA, 0xB1, 0x20, 0x20, 0xB7, 0xD6, 0xD6, 0xB9, 0xA3, 0xAC ,0x0D, 0x0A ,
//		//	0xD7, 0xBC, 0xD0, 0xED, 0x20, 0x20, 0x20, 0xD5, 0xBE, 0xD6, 0xC1, 0x20, 0x20, 0x20, 0xD5, 0xBE ,0xBC, 0xE4 ,0x20, 0x20 ,
//		//	0x20, 0x20, 0xD0, 0xD0, 0xCF, 0xDF, 0xBD, 0xD3, 0xB4, 0xA5, 0xCD, 0xF8, 0xCD, 0xA3, 0xB5, 0xE7 ,0xA3, 0xAC ,0xD3 ,0xD0,
//		//	0xB9, 0xD8, 0xB0, 0xB2, 0xC8, 0xAB, 0xB4, 0xEB, 0xCA, 0xA9, 0xBC, 0xB0, 0xCD, 0xA3, 0xB5, 0xE7 ,0xB7, 0xB6 ,0xCE ,0xA7,
//		//	0xB0, 0xB4, 0x20, 0x20, 0xB9, 0xA9, 0xB5, 0xE7, 0xB6, 0xCE, 0x20, 0x20, 0x20, 0xB5, 0xA5, 0xD4 ,0xAA, 0xD6, 0xB4 ,0xD0,
//		//	0xD0, 0xA1, 0xA3, 0x0D, 0x0A, 0x20, 0xCD, 0xAC, 0xCA, 0xB1, 0xD7, 0xBC, 0xD0, 0xED, 0xBD, 0xF8 ,0xD0, 0xD0 ,0xCE ,0xAC,
//		//	0xD0, 0xDE, 0x28, 0xCA, 0xA9, 0xB9, 0xA4, 0x29, 0xD7, 0xF7, 0xD2, 0xB5, 0x2C, 0xB2, 0xA2, 0xB7, 0xE2, 0xCB ,0xF8 ,0x20,
//		//	0x20, 0xD5, 0xBE, 0xD6, 0xC1, 0x20, 0x20, 0x20, 0xD5, 0xBE, 0xBC, 0xE4, 0x20, 0x20, 0xD0, 0xD0, 0xCF, 0xDF, 0xA1 ,0xA3,
//		//	0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xA3, 0xA8, 0x31, 0xA3, 0xA9, 0xD7, 0xBC ,0xD0 ,0xED, 0xB9 ,0xA4, 0xCE,
//		//	0xF1, 0xB2, 0xBF, 0xC3, 0xC5, 0xD4, 0xDA, 0x20, 0x20, 0x20, 0x6B, 0x6D, 0x20, 0x20, 0x20 ,0x6D ,0xD6, 0xC1 ,0x20, 0x20,
//		//	0x20, 0x6B, 0x6D, 0x20, 0x20, 0x20, 0x6D, 0xB4, 0xA6, 0xCA, 0xA9, 0xB9, 0xA4, 0xA1, 0xA3, 0x0D, 0x0A, 0x20, 0x20, 0x20,
//		//	0x20, 0x20, 0xA3, 0xA8, 0x32, 0xA3, 0xA9, 0xD7, 0xBC, 0xD0, 0xED, 0xB9, 0xA9, 0xB5, 0xE7, 0xB2 ,0xBF, 0xC3, 0xC5, 0xD4,
//		//	0xDA, 0x20, 0x20, 0x20, 0x6B, 0x6D, 0x20, 0x20, 0x20, 0x6D, 0xD6, 0xC1, 0x20, 0x20, 0x20, 0x6B, 0x6D, 0x20, 0x20, 0x20,
//		//	0x20, 0x6D, 0xB4, 0xA6, 0xCA, 0xA9, 0xB9, 0xA4, 0xA1, 0xA3, 0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0xA3, 0xA8, 0x33,
//		//	0xA3, 0xA9, 0xD7, 0xBC, 0xD0, 0xED, 0xA1, 0xA1, 0xA1, 0xA1, 0xB2, 0xBF, 0xC3, 0xC5, 0xD4, 0xDA, 0x20, 0x20, 0x20, 0x6B,
//		//	0x6D, 0x20, 0x20, 0x20, 0x6D, 0xD6, 0xC1, 0x20, 0x20, 0x20, 0x6B, 0x6D ,0x20 ,0x00, 0x00 ,0x00, 0xFE, 0xFE, 0xFE, 0xFE
//		//};
//		////memcpy(pDoc->m_chArrayRecvDataBuffToJSJ[0],DDML,500);
//		////pDoc->RecvStagePlanData();
//		//pDoc->RecvStagePlanData(DDML, 500);
//		return;
//	}

//	//行车日志相关培训
//	if(ExapleStr.FunStr.Find(_T("XCRZ-MEMU"))!=-1 || ExapleStr.FunStr.Find(_T("FSFCQQ-MEMU"))!=-1 || ExapleStr.FunStr.Find(_T("QXFCQQ-MEMU"))!=-1
//		||ExapleStr.FunStr.Find(_T("TYLZFC-MEMU"))!=-1)//行车日志相关
//	{
//		CPoint setPoint;
//		setPoint.SetPoint(0,0);
//		//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//		pFrame->pSingStaView->ClientToScreen(&setPoint);
//		pFrame->StaConfigInfo.SecondScreenXOffset = setPoint.x;
//		pFrame->OnXCRZFormView();
//	}
//	if(ExapleStr.FunStr.Find(_T("XCRZ-MEMU"))!=-1)//行车日志
//	{
//		CCTC10View* pView = (CCTC10View*)pFrame->GetActiveView();
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//	}
//	else if(ExapleStr.FunStr.Find(_T("YXTJM-MEMU"))!=-1)//运行图界面
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CPoint setPoint;
//		setPoint.SetPoint(0,0);
//		//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//		pFrame->pSingStaView->ClientToScreen(&setPoint);
//		pFrame->StaConfigInfo.SecondScreenXOffset = setPoint.x;
//		pFrame->OnTrainDiagramView();
//		return;
//	}

//	CCTC10View* pView = (CCTC10View*)pFrame->GetActiveView();
//	/*pView->m_pDlgYXT->ShowWindow(SW_HIDE);
//	pView->m_pDlgViews->ShowWindow(SW_HIDE);*/

//	if(ExapleStr.FunStr.Find(_T("ARROW")) != -1 )
//	{
//		if(ExapleStr.SubFunStr == "CPOINT")//根据坐标显示箭头
//		{
//			CPoint ShowPt;
//			Getstring = ExapleStr.DevName.Mid(0,ExapleStr.DevName.Find(_T("|"),0)-0);
//			ShowPt.x  = StringToInt(Getstring);
//			Getstring = ExapleStr.DevName.Mid(ExapleStr.DevName.Find(_T("|"),0)+1,ExapleStr.DevName.GetLength()-ExapleStr.DevName.Find(_T("|"),0));
//			ShowPt.y = StringToInt(Getstring);
//			DrawArrow(pDC,ShowPt,ExapleStr.TipShowStr);
//		}
//		else
//		{
//			ArrowDrawByDev(pDC,ExapleStr.SubFunStr,ExapleStr.DevName,ExapleStr.TipShowStr);
//		}
//		return;
//	}
//	int temp = 0;
//	for( temp=0;temp<this->v_TrainTips.size();temp++)
//	{
//		if(ExapleStr.FunStr.Find(v_TrainTips[temp].TipsName)!=-1)
//		{
//			v_OrderArray.erase(v_OrderArray.begin()+index);
//			::MessageBox(NULL, v_TrainTips[temp].TipsContent + _T("：")+ExapleStr.ParaStr+_T("，确认此操作吗？"), _T("提示"), MB_YESNO);
//			break;
//		}
//	}
//	if(temp < v_TrainTips.size())
//		return;


//	if(ExapleStr.FunStr.Find(_T("MLXDYJ-MEMU"))!=-1)//命令下达右键菜单
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//        CMenu menu;
//		menu.LoadMenu(IDR_CTCVIEW_ORDER);
//        CPoint myPoint;
//		myPoint.SetPoint(860,440);
//		//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&myPoint);
//		pFrame->pSingStaView->ClientToScreen(&myPoint);
//		//menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,myPoint.x,myPoint.y,pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(1,0));
//		menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,myPoint.x,myPoint.y,pFrame->pLCJLFormView);
//	}
//	else if(ExapleStr.FunStr.Find(_T("LCCCHXX-MEMU"))!=-1)//排列列车进路时，命令下达后的车次后窗口
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CDlgInfoPromp dlgInfoPromp;
//		dlgInfoPromp.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("DCJLXX-MEMU"))!=-1)//排列调车进路时，命令下达后的车次后窗口
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CDlgDCTime dlgDCTime;
//		dlgDCTime.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("XDZL-MEMU"))!=-1)//排列调车进路时，命令下达后的车次后窗口
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CDlgEnsure dlgEnsure;
//	    dlgEnsure.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("QFPASS-MEMU"))!=-1)
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		/*处理引导闪烁后的状态*/
//		pDoc->SetYDZSButtonTwinkle(0x33);
//		/**************************/
//		CDlgPassword dlgPassword;
//		dlgPassword.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("LADA-MEMU"))!=-1)
//	{
//		/*CDlgJLANSelect dlg;
//		dlg.m_strXHDName = ExapleStr.DevName;
//		dlg.DoModal();*/
//	}
//	else if(ExapleStr.FunStr.Find(_T("CXHD-MEMU"))!=-1)//信号灯相关菜单
//	{

//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CMenu menu;
//		int xhdType;
//		CPoint setPoint = setPicShowPoint(ExapleStr.DevName,&xhdType);
//		if(ExapleStr.SubFunStr == "JZXHJ")
//			menu.LoadMenu(IDR_JZXHJ_ORDER);
//		else if(ExapleStr.SubFunStr == "CZXHJ")
//			menu.LoadMenu(IDR_CZXHJ_ORDER);
//		else if(ExapleStr.SubFunStr == "DCXHJ")
//			menu.LoadMenu(IDR_DCXHJ_ORDER);
//		else
//			menu.LoadMenu(IDR_JZXHJ_ORDER);

//		/*设置主窗体前置  20200806*/
//		HWND  hWnd = ::FindWindow(NULL,SYS_TITLE); //查找消息框的标题
//		if(hWnd)
//		{
//			ShowWindow( hWnd, SW_SHOWMAXIMIZED );
//			SetForegroundWindow(hWnd);
//		}

//		//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//		//menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//		//	setPoint.x,setPoint.y,pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(1,0));
//		pFrame->pSingStaView->ClientToScreen(&setPoint);
//		menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//			setPoint.x,setPoint.y,pFrame->pLCJLFormView);
//	}
//	else if(ExapleStr.FunStr.Find(_T("CGDDC-MEMU"))!=-1)//道岔相关菜单
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CMenu menu;
//		menu.LoadMenu(IDR_SWITCH_ORDER);
//		CPoint setPoint = setPicShowPoint(ExapleStr.DevName,NULL);
//		//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//		pFrame->pSingStaView->ClientToScreen(&setPoint);

//		DCmenu = menu.GetSubMenu(0);

//		/*设置主窗体前置  20200806*/
//		HWND  hWnd = ::FindWindow(NULL,SYS_TITLE); //查找消息框的标题
//		if(hWnd)
//		{
//			ShowWindow( hWnd, SW_SHOWMAXIMIZED );
//			SetForegroundWindow(hWnd);
//		}

//		//DCmenu->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//		//	setPoint.x,setPoint.y,pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0));
//		DCmenu->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//			setPoint.x,setPoint.y,pFrame->pSingStaView);
//		if(hWnd)
//		   PostMessage(hWnd,WM_NULL,0,0); //解决托盘菜单不消失
//	}
//	else if(ExapleStr.FunStr.Find(_T("CGD-MEMU"))!=-1)//轨道相关菜单
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		CMenu menu;
//		menu.LoadMenu(IDR_GDQD_ORDER);

//		/*设置主窗体前置  20200806*/
//		HWND  hWnd = ::FindWindow(NULL,SYS_TITLE); //查找消息框的标题
//		if(hWnd)
//		{
//			ShowWindow( hWnd, SW_SHOWMAXIMIZED );
//			SetForegroundWindow(hWnd);
//		}

//		CPoint setPoint = setPicShowPoint(ExapleStr.DevName,NULL);
//		//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//		//menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//		//	setPoint.x,setPoint.y,pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(1,0));
//		pFrame->pSingStaView->ClientToScreen(&setPoint);
//		menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//			setPoint.x,setPoint.y,pFrame->pLCJLFormView);
//	}
//	else if(ExapleStr.FunStr.Find(_T("MainYDZS-MEMU"))!=-1)
//	{
//		pDoc->SetYDZSButtonTwinkle(0x11);
//	}
//	else if(ExapleStr.FunStr.Find(_T("MSZH-MEMU"))!=-1)
//	{
//		if(ExapleStr.DevName == "ZXKZ")
//		{
//			v_OrderArray.erase(v_OrderArray.begin()+index);
//			pDoc->MSZHTrain();
//		}
//		else if(ExapleStr.DevName == "ZH")
//		{
//			v_OrderArray.erase(v_OrderArray.begin()+index);
//			pFrame->StationModalSelect[0].nModeState = 0;
//		}
//		else if(ExapleStr.DevName == "NULL")
//		{
//			pDoc->SetMSZHButtonTwinkle(0x11);
//		}
//		else if(ExapleStr.DevName == "MAIN")
//		{
//			v_OrderArray.erase(v_OrderArray.begin()+index);
//			CMenu menu;
//			menu.LoadMenu(IDR_MODAL_ORDER);
//			CPoint setPoint(640,745);
//			//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//			//menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,setPoint.x,setPoint.y,
//			//	pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(1,0));
//			pFrame->pSingStaView->ClientToScreen(&setPoint);
//			menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,setPoint.x,setPoint.y,
//				pFrame->pLCJLFormView);
//		}
//	}
//	else if(ExapleStr.FunStr.Find(_T("JHJLXX-MEMU"))!=-1)//进路信息右键菜单
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		if(ExapleStr.DevName == "SCJL")
//		{
//			pDoc->SetJLXXTrainMessage(0x11);
//		}
//		else if(ExapleStr.DevName == "ZDCF")
//		{
//			pDoc->SetJLXXTrainMessage(0x22);
//		}
//		else if(ExapleStr.DevName == "RGCF")
//		{
//			pDoc->SetJLXXTrainMessage(0x33);
//		}
//		else if(ExapleStr.DevName == "CFJL")
//		{
//			pDoc->SetJLXXTrainMessage(0x44);
//		}
//		else if(ExapleStr.DevName == "ZJ-MENUE")
//		{
//			CMenu menu;
//			menu.LoadMenu(IDR_CHANGELCGRID_ORDER);
//			CPoint myPoint;
//			myPoint.SetPoint(280,820);
//			//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&myPoint);
//			//menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,myPoint.x,myPoint.y,
//			//	pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(1,0));
//			pFrame->pSingStaView->ClientToScreen(&myPoint);
//			menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,myPoint.x,myPoint.y,
//				pFrame->pLCJLFormView);
//		}
//		else if(ExapleStr.DevName == "YJ-MENUE")
//		{
//			CMenu menu;
//			menu.LoadMenu(IDR_DELLCGRID_ORDER);
//			CPoint myPoint;
//			myPoint.SetPoint(280,800);
//			//pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&myPoint);
//			//menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,myPoint.x,myPoint.y,
//			//	pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(1,0));
//			pFrame->pSingStaView->ClientToScreen(&myPoint);
//			menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,myPoint.x,myPoint.y,
//				pFrame->pLCJLFormView);
//		}
//	}
//	else if(ExapleStr.FunStr.Find(_T("SCCCH-MEMU"))!=-1)//删除车次号
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);

//		//CDlgDelCheCi dlgDelCheCi(((CCTC10View *)pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)), ExapleStr.DevName);
//		CDlgDelCheCi dlgDelCheCi(((CCTC10View *)pFrame->pSingStaView), ExapleStr.DevName);
//		dlgDelCheCi.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("XGCCH-MEMU"))!=-1)//修改车次号
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		unsigned int GDCode;
//		//GDCode = getDevNodeOfName(ExapleStr.DevName);
//		//CDlgEnsureCheci dlgCheCi(((CCTC10View *)pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)), ExapleStr.DevName);
//		CDlgEnsureCheci dlgCheCi(((CCTC10View *)pFrame->pSingStaView), ExapleStr.DevName);
//		dlgCheCi.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("TJCCH-MEMU"))!=-1)//车次号添加窗口
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		//CDlgAddCheCi dlgAddCheCi(((CCTC10View *)pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)), ExapleStr.DevName);
//		CDlgAddCheCi dlgAddCheCi(((CCTC10View *)pFrame->pSingStaView), ExapleStr.DevName);
//		dlgAddCheCi.DoModal();
//	}
//	else if(ExapleStr.FunStr.Find(_T("CCHSC-CTC"))!=-1)//车次号删除命令下发
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		unsigned int GDCode = getDevNodeOfName(ExapleStr.SubFunStr);
//        if (GDCode<pDoc->zcArray.GetSize() && ("CGD" == pDoc->zcArray[GDCode]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGD *pGD = (CGD*)pDoc->zcArray[GDCode];
//			pGD->m_strCheCiNumCTC = _T("");
//		}

//		//HWND  hWnd = ::FindWindow(NULL,_T("删除车次对话框")); //查找消息框的标题
//		//if(hWnd)
//		//	::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//		//hWnd = ::FindWindow(NULL,_T("确认")); //查找消息框的标题
//		//if(hWnd)
//		//	::EndDialog((HWND)hWnd,IDNO);
//	}
//	else if(ExapleStr.FunStr.Find(_T("CCHTJ-CTC"))!=-1)//车次号命令下发
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);

//		CStringA Trainnum = (CStringA)ExapleStr.DevName;
//		unsigned int GDCode = getDevNodeOfName(ExapleStr.SubFunStr);
//        if (GDCode<pDoc->zcArray.GetSize() && ("CGD" == pDoc->zcArray[GDCode]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGD *pGD = (CGD*)pDoc->zcArray[GDCode];
//			CTrain *pTrain = new CTrain;
//			pTrain->m_bActive  = TRUE;
//			pTrain->m_nPosCode = GDCode;

//			pTrain->m_bElectric = FALSE;
//			pGD->m_bElectric = FALSE;

//			pGD->m_strCheCiNumCTC = Trainnum;
//			pTrain->m_strCheCi = Trainnum;
//		}
//		//
//		//HWND  hWnd = ::FindWindow(NULL,_T("增加车次对话框")); //查找消息框的标题
//		//if(hWnd)
//		//	::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//	}
//	else if(ExapleStr.FunStr.Find(_T("CCHXG-CTC"))!=-1)//车次号命令下发
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);

//		CStringA Trainnum = (CStringA)ExapleStr.DevName;
//		unsigned int GDCode = getDevNodeOfName(ExapleStr.SubFunStr);
//        if (GDCode<pDoc->zcArray.GetSize() && ("CGD" == pDoc->zcArray[GDCode]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGD *pGD = (CGD*)pDoc->zcArray[GDCode];
//			pGD->m_strCheCiNumCTC = Trainnum;
//		}
//		//HWND  hWnd = ::FindWindow(NULL,_T("修改车次对话框")); //查找消息框的标题
//		//if(hWnd)
//		//	::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//	}
//	else if(ExapleStr.FunStr.Find(_T("CCH-MEMU"))!=-1)//车次号添加右键菜单
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);

//		CMenu menu;
//		CMenu *popup;
//		CPoint setPoint = setPicShowPoint(ExapleStr.DevName,NULL);
//        //pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0)->ClientToScreen(&setPoint);
//		pFrame->pSingStaView->ClientToScreen(&setPoint);
//		menu.LoadMenu(IDR_CHECI_ORDER);
//		popup=menu.GetSubMenu(0);
//		//menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//		//	setPoint.x,setPoint.y,pFrame->m_pSingFieldSplitterFrame->m_Splitter.GetPane(0,0));
//		menu.GetSubMenu(0)->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTBUTTON,
//			setPoint.x,setPoint.y,pFrame->pSingStaView);
//	}
//	else if(ExapleStr.FunStr.Find(_T("DCFLBL-CTC"))!=-1)//岔前分路不良
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		int nPos = getDevNodeOfName(ExapleStr.DevName);
//		if (nPos<pDoc->zcArray.GetSize() && ("CGDDC" == pDoc->zcArray[nPos]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGDDC *pGDDC = (CGDDC*)pDoc->zcArray[nPos];
//			if(ExapleStr.SubFunStr == "CQ")
//			{
//				if(pGDDC->getCQFLBL())
//				{
//					pGDDC->setCQFLBL(FALSE);
//				}
//				else
//				{
//					pGDDC->setCQFLBL(TRUE);
//				}
//			}
//			else if(ExapleStr.SubFunStr == "DW")
//			{
//				if(pGDDC->getDWFLBL())
//				{
//					pGDDC->setDWFLBL(FALSE);
//				}
//				else
//				{
//					pGDDC->setDWFLBL(TRUE);
//				}
//			}
//			else if(ExapleStr.SubFunStr == "FW")
//			{
//				if(pGDDC->getFWFLBL())
//				{
//					pGDDC->setFWFLBL(FALSE);
//				}
//				else
//				{
//					pGDDC->setFWFLBL(TRUE);
//				}
//			}
//		}
//	}
//	else if(ExapleStr.FunStr.Find(_T("FLBL-QDKX-CTC"))!=-1)//反位分路不良
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		int nPos = getDevNodeOfName(ExapleStr.DevName);
//		if (nPos<pDoc->zcArray.GetSize() && ("CGDDC" == pDoc->zcArray[nPos]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGDDC *pGDDC = (CGDDC*)pDoc->zcArray[nPos];
//			if(!pGDDC->m_nCQFLBLKX)
//			{
//				if(pGDDC->getCQFLBL()||pGDDC->getDWFLBL()||pGDDC->getFWFLBL())//pGDDC->m_nCQFLBLKX = TRUE;
//				{
//					if (pGDDC->m_nCQFLBLKX == TRUE)
//					{
//						pGDDC->m_nCQFLBLKX = FALSE;
//					}
//					else
//					{
//						pGDDC->m_nCQFLBLKX = TRUE;
//					}
//				}
//			}
//			else
//				pGDDC->m_nCQFLBLKX = FALSE;
//		}
//	}
//	else if(ExapleStr.FunStr.Find(_T("QXSZWD-CTC"))!=-1)//取消无电设置
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		int nPos = getDevNodeOfName(ExapleStr.DevName);
//		if (nPos<pDoc->zcArray.GetSize() && ("CGDDC" == pDoc->zcArray[nPos]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGDDC *pGDDC = (CGDDC*)pDoc->zcArray[nPos];
//			if(ExapleStr.SubFunStr == "DW")
//				pGDDC->isPowerCutDW = FALSE;
//			else if(ExapleStr.SubFunStr == "FW")
//				pGDDC->isPowerCutFW = FALSE;
//		}
//		else if (nPos<pDoc->zcArray.GetSize() && ("CGD" == pDoc->zcArray[nPos]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGD *pGD = (CGD*)pDoc->zcArray[nPos];
//			pGD->isPowerCut = FALSE;
//		}
//	}
//	else if(ExapleStr.FunStr.Find(_T("SZWD-CTC"))!=-1)//设置无电
//	{
//		v_OrderArray.erase(v_OrderArray.begin()+index);
//		int nPos = getDevNodeOfName(ExapleStr.DevName);
//		if (nPos<pDoc->zcArray.GetSize() && ("CGDDC" == pDoc->zcArray[nPos]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGDDC *pGDDC = (CGDDC*)pDoc->zcArray[nPos];
//			 if(ExapleStr.SubFunStr == "DW")
//				pGDDC->isPowerCutDW = TRUE;
//			else if(ExapleStr.SubFunStr == "FW")
//				pGDDC->isPowerCutFW = TRUE;
//		}
//		else if (nPos<pDoc->zcArray.GetSize() && ("CGD" == pDoc->zcArray[nPos]->GetRuntimeClass()->m_lpszClassName))
//		{
//			CGD *pGD = (CGD*)pDoc->zcArray[nPos];
//			pGD->isPowerCut = TRUE;
//		}
//	}
}
/**按压按钮指示箭头绘制 从上往下**/
void CTrainAndTest::DrawArrow_TOP(QPainter *painter,QPoint pt,QString m_str)
{
//	CBitmap m_MemuImage;

//	pt.SetPoint(pt.x-20,pt.y);
//	pDC->SetBkColor(BLACK);
//	pDC->SetTextColor(YELLOW);
//	CFont *pOldFont=pDC->SelectObject(&m_textFont);

//	m_MemuImage.LoadBitmap(IDB_BMP_ARROWTOP);

//	CBase::DrawBitmap(pDC, &m_MemuImage, pt, 21, 31);

//	pDC->TextOut(pt.x+21,pt.y+10,m_str);

//	pDC->SelectObject(pOldFont);
}
/**按压按钮指示箭头绘制**/
void CTrainAndTest::DrawArrow(QPainter *painter,QPoint pt,QString m_str)
{
//	if(pt.y > 800)
//	{
//		CBitmap m_MemuImage;

//		pt.SetPoint(pt.x-31,pt.y-21);
//		pDC->SetBkColor(BLACK);
//		pDC->SetTextColor(YELLOW);
//		CFont *pOldFont=pDC->SelectObject(&m_textFont);

//		m_MemuImage.LoadBitmap(IDB_BMP_ARROW);

//		CBase::DrawBitmap(pDC, &m_MemuImage, pt, 31, 21);

//		pDC->TextOut(pt.x,pt.y-15,m_str);

//		pDC->SelectObject(pOldFont);

//	}
//	else
//		DrawArrow_TOP(pDC,pt,m_str);

}
//根据设备编号获取设备名称
QString CTrainAndTest::getNameOfDevNode(unsigned int devnode)
{
//	if(!pDoc)
//		return (_T(""));

//	CGD *pGD;
//	CGDDC *pGDDC;
//	CXHD *pXHD;
//	CText *pTEXT;
//	CString className;
//	CString TextclassName;
//	int nArraySize=  pDoc->zcArray.GetSize();
//	for(int i=0;i<pDoc->zcArray.GetSize();i++)
//	{
//		className = pDoc->zcArray[i]->GetRuntimeClass()->m_lpszClassName;
//		if(("CXHD" == className))
//		{
//			pXHD = (CXHD *)pDoc->zcArray[i];
//			if(pXHD->m_nCode == devnode)
//			{
//				return (pXHD->m_strName);
//			}
//		}
//		else if(("CGDDC" == className))
//		{
//			pGDDC = (CGDDC *)pDoc->zcArray[i];
//			if(pGDDC->m_nCode == devnode)
//			{
//				return (pGDDC->m_strName);
//			}
//			//else if(pGDDC->m_nQDCode == devnode)
//			//{
//			//	for(int j=0;j<nArraySize;j++)    //遍历得到文本设备关联道岔名与道岔设备的名称比较
//			//	{
//			//		TextclassName = pDoc->zcArray[j]->GetRuntimeClass()->m_lpszClassName;
//			//		if("CText" == TextclassName)
//			//		{
//			//			pTEXT = (CText *)pDoc->zcArray[j];  //得到文本设备
//			//			if(pGDDC->m_strName == pTEXT->m_strDCName)    //比较获取到的道岔设备名称与文本设备关联道岔名作比较
//			//			{
//			//				return (pTEXT->m_strName);
//			//			}
//			//		}
//			//	}
//			//}
//		}
//		else if("CGD" == className)
//		{
//			pGD = (CGD *)pDoc->zcArray[i];
//			if(pGD->m_nCode == devnode)
//			{
//				return (pGD->m_strName);
//			}
//		}
//	}
    return ("");
}
//根据设备名称获取设备编号
unsigned int CTrainAndTest::getDevNodeOfName(QString strName)
{
//	if(!pDoc)
//		return 65535;

//	CGDDC *pGDDC;
//	CXHD *pXHD;
//	CText *pTEXT;
//	CGD *pGD;
//	CString className;
//	CString className2;
//	int nArraySize=  pDoc->zcArray.GetSize();
//	for(int i=0;i<pDoc->zcArray.GetSize();i++)
//	{
//		className = pDoc->zcArray[i]->GetRuntimeClass()->m_lpszClassName;
//		if(("CXHD" == className))
//		{
//			pXHD = (CXHD *)pDoc->zcArray[i];
//			if(pXHD->m_strName == strName)
//			{
//				return pXHD->m_nCode;
//			}
//		}
//		else if(("CGDDC" == className))
//		{
//			pGDDC = (CGDDC *)pDoc->zcArray[i];
//			if(pGDDC->m_strName == strName)
//			{
//				return pGDDC->m_nCode;
//			}
//		}
//		else if(("CGD" == className))
//		{
//			pGD = (CGD *)pDoc->zcArray[i];
//			if(pGD->m_strName == strName)
//			{
//				return pGD->m_nCode;
//			}
//		}
//		//else if(("CText" == className))
//		//{
//		//	pTEXT = (CText *)pDoc->zcArray[i];
//		//	if(pTEXT->m_strName == strName)
//		//	{
//		//		for(int j=0;j<nArraySize;j++)    //遍历得到文本设备关联道岔名与道岔设备的名称比较
//		//		{
//		//			className2 = pDoc->zcArray[j]->GetRuntimeClass()->m_lpszClassName;
//		//			if("CGDDC" == className2)
//		//			{
//		//				pGDDC = (CGDDC *)pDoc->zcArray[j];
//		//				if(pGDDC->m_strName == pTEXT->m_strDCName)
//		//				{
//		//					return pGDDC->m_nQDCode;
//		//				}
//		//			}
//		//		}
//		//	}
//		//}
//	}
    return 65535;
}
//根据指定设备名称获取对应指向设备的位图坐标点
QPoint CTrainAndTest::setPicShowPoint(QString strName,int *XHDType)
{
//	if(XHDType!=NULL)
//		*XHDType = 0;
//	if(!pDoc)
//		return CPoint(0,0);

//	int cx,cy;

//	CGDDC *pGDDC;
//	CXHD *pXHD;
//	CGD *pGD;
//	CText *pTEXT;
//	CString className;
//	cx = cy = 0;
//	int nArraySize=  pDoc->zcArray.GetSize();
//	for(int i=0;i<pDoc->zcArray.GetSize();i++)
//	{
//		className = pDoc->zcArray[i]->GetRuntimeClass()->m_lpszClassName;
//		if(("CXHD" == className))
//		{
//			pXHD = (CXHD *)pDoc->zcArray[i];
//			if(pXHD->m_strName == strName)
//			{
//				cx=pXHD->center.x+10+50;
//				cy=pXHD->center.y-10;   //设置坐标位置在信号灯中心偏右下
//				if(XHDType!=NULL)
//					*XHDType = pXHD->m_nXHDType;
//				break;
//			}
//		}
//		else if(("CGDDC" == className))
//		{
//			pGDDC = (CGDDC *)pDoc->zcArray[i];
//			if(pGDDC->m_strName == strName)
//			{
//				cx=pGDDC->center.x+10+5;
//				cy=pGDDC->center.y+10;   //设置坐标位置在道岔中心偏右下
//				break;
//			}
//		}
//		else if(("CGD" == className))
//		{
//			pGD = (CGD *)pDoc->zcArray[i];
//			if(pGD->m_strName == strName)
//			{
//				cx=pGD->center.x+10;
//				cy=pGD->center.y+10;   //设置坐标位置在轨道区段中心偏右下
//				break;
//			}
//		}
//		else if(("CText" == className))
//		{
//			pTEXT = (CText *)pDoc->zcArray[i];
//			if(pTEXT->m_strName == strName)
//			{
//				cx=pTEXT->center.x+10;
//				cy=pTEXT->center.y+10;   //设置坐标位置在文字中心偏右下
//				break;
//			}
//		}
//	}
//	return CPoint(cx,cy);
    return QPoint(0,0);
}
void CTrainAndTest::ArrowDrawByDev(QPainter *painter,QString m_ClassName,QString m_DevName,QString m_str)//根据设备绘制箭头
{
//	CXHD *xhd;
//	CGDDC *dc;
//	CGD *gd;
//	CRuntimeClass *className;
//	CFuncBtn *pFuncBtn;

//	INT_PTR m=pDoc->zcArray.GetSize();

//	CString m_name;
//	if(m_ClassName.Find(_T("XYDZSAN"))!=-1)
//	{
//		pDoc->SetYDZSButtonTwinkle(0x22);
//		for(int i=0; i<pDoc->m_ArrayFuncBtn.GetSize(); i++)
//		{
//			pFuncBtn = (CFuncBtn *)pDoc->m_ArrayFuncBtn[i];
//			if(pFuncBtn->m_strName == _T("X引导总锁"))
//			{
//				DrawArrow(pDC,CPoint(pFuncBtn->m_rectCenter.left,pFuncBtn->m_rectCenter.top),m_str);
//				return;
//			}
//		}
//	}
//	else if(m_ClassName.Find(_T("SYDZSAN"))!=-1)
//	{
//		pDoc->SetYDZSButtonTwinkle(0x22);
//		for(int i=0; i<pDoc->m_ArrayFuncBtn.GetSize(); i++)
//		{
//			pFuncBtn = (CFuncBtn *)pDoc->m_ArrayFuncBtn[i];
//			if(pFuncBtn->m_strName == _T("S引导总锁"))
//			{
//				DrawArrow(pDC,CPoint(pFuncBtn->m_rectCenter.left,pFuncBtn->m_rectCenter.bottom),m_str);
//				return;
//			}
//		}
//	}
//	if((m_ClassName=="CXHD-LC")||(m_ClassName=="CXHD-DC")||(m_ClassName=="CXHD-MEMU"))
//	{
//		m_name="CXHD";
//	}
//	else if(m_ClassName=="CGDDC-MEMU")
//	{
//		m_name="CGDDC";
//	}
//	else
//	{
//		m_name=m_ClassName;
//	}

//	for(int i=0;i<pDoc->zcArray.GetSize();i++)
//	{
//		className=pDoc->zcArray[i]->GetRuntimeClass();
//		if(className->m_lpszClassName==m_name)
//		{
//			if(m_ClassName=="CXHD-LC")
//			{
//				xhd=(CXHD *)pDoc->zcArray[i];
//				if(xhd->m_strName==m_DevName)
//				{
//					DrawArrow(pDC,CPoint(xhd->rectBtnLC.left+5,xhd->rectBtnLC.bottom+2),m_str);
//					break;
//				}
//			}
//			else if(m_ClassName=="CXHD-DC")
//			{
//				xhd=(CXHD *)pDoc->zcArray[i];
//				if(xhd->m_strName==m_DevName)
//				{
//					DrawArrow(pDC,CPoint(xhd->center.x-5,xhd->center.y+3),m_str);
//					break;
//				}
//			}
//			else if(m_ClassName=="CXHD-YD")
//			{
//				xhd=(CXHD *)pDoc->zcArray[i];
//				if(xhd->m_strName==m_DevName)
//				{
//					DrawArrow(pDC,CPoint(xhd->m_DJRectL.left+5-20,xhd->m_DJRectL.bottom+2),m_str);
//					break;
//				}
//			}
//			else if(m_ClassName=="CGDDC")
//			{
//				dc=(CGDDC *)pDoc->zcArray[i];
//				if(dc->m_strName==m_DevName)
//				{
//					DrawArrow(pDC,CPoint(dc->center.x-10,dc->center.y+5),m_str);
//					break;
//				}
//			}
//			else if(m_ClassName=="CGD")
//			{
//				gd=(CGD *)pDoc->zcArray[i];
//				if(gd->m_strName==m_DevName)
//				{
//					DrawArrow(pDC,CPoint(gd->center.x-15,gd->center.y+5),m_str);
//					break;
//				}
//			}
//			/*else if(m_ClassName=="CBuRect")
//			{
//			tg=(CBuRect *)pDoc->data.zcArray[i];
//			if(tg->m_strName==m_DevName)
//			{
//			TrainExampleButtonArrow(pDC,tg->m_gBuRect.left+5,tg->m_gBuRect.bottom+2,m_str);
//			}
//			}*/

//		}
//	}
}
void CTrainAndTest::CloseDlgOnTimer()
{
//	CMainFrame *pFrame=(CMainFrame *)AfxGetApp()->m_pMainWnd;

//	CStringArray TitleStr;
//	TitleStr.Add(_T("路票"));
//	TitleStr.Add(_T("出站跟踪调车通知书"));

//	if(TitleStr.GetSize()>0)
//	{
//		for(int i =0;i<TitleStr.GetSize();i++)
//		{
//			HWND  hWnd = ::FindWindow(NULL,TitleStr.GetAt(i)); //查找消息框的标题
//			if(hWnd)
//				::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
//		}
//	}
}

