#include "stdafx.h"
#include "swLab19fDoc.h"
#include "swLab19fView.h"
#include "MainFrm.h"
#include "s150614L01_ext.h"
#include "s150614L01_Mfc.h"

void SWL01::DarkenGrayscaleImage()
{
	Mat *pMat;
	pMat = &m_Mat1;
	string s1 = "8UC1";
	string st = type2str((*pMat).type());
	if (SWL01_inst.readImageF1 == true)
	{
		if (st.compare(s1) != 0) {
			AfxMessageBox(L"Only 8bit grayscale can be shown!", MB_OK, 0);
			return;
		}
		else
		{
			m_MatR.release();
			int h, w;
			h = m_height1;
			w = m_width1;
			unsigned char *pDibData = m_dibFile1.m_lpImage;
			m_MatR = Mat(h, w, CV_8UC1); 
			for (int r = 0; r < h; r++) 
				for (int c = 0; c < w; c++)
					m_MatR.at<uchar>(r, c) = (*pDibData++)/2;
			flip((m_MatR), (m_MatR), 0);
			Create_bmiInfoHeader(&m_MatR);  // 인포헤더를 갱신 
			processedF = true; // 처리 완료를 flag을 통하여 알린다 
			g_pView->Invalidate();   // OnDraw를 호출한다
		}
	}
	else {
		AfxMessageBox(L"Read image1 first!", MB_OK, 0);
		return;
	}
}

void SWL01::BrightenGrayscaleImage(void)
{
	Mat *pMat;
	pMat = &m_Mat1;
	string s1 = "8UC1";
	string st = type2str((*pMat).type());
	if (SWL01_inst.readImageF1 == true)
	{
		if (st.compare(s1) != 0) {
			AfxMessageBox(L"Only 8bit grayscale can be shown!", MB_OK, 0);
			return;
		}
		else
		{
			m_MatR.release();
			int h, w, t;
			h = m_height1;
			w = m_width1;
			unsigned char *pDibData = m_dibFile1.m_lpImage;
			m_MatR = Mat(h, w, CV_8UC1);
			for (int r = 0; r < h; r++)
				for (int c = 0; c < w; c++) {
					t = (*pDibData) * 2;
					if(t<=255) m_MatR.at<uchar>(r, c) = t;
					else if (t > 255) m_MatR.at<uchar>(r, c) = 255;
					*pDibData++;
				}
			flip((m_MatR), (m_MatR), 0);
			Create_bmiInfoHeader(&m_MatR);  // 인포헤더를 갱신 
			processedF = true; // 처리 완료를 flag을 통하여 알린다 
			g_pView->Invalidate();   // OnDraw를 호출한다
		}
	}
	else {
		AfxMessageBox(L"Read image1 first!", MB_OK, 0);
		return;
	}
}

void SWL01::Color24toGrayscale()
{
	Mat *pMat;
	pMat = &m_Mat1;
	string s1 = "8UC3";
	string st = type2str((*pMat).type());
	if (SWL01_inst.readImageF1 == true)
	{
		if (st.compare(s1) != 0) {
			AfxMessageBox(L"Only 24bit Colorscale can be shown!", MB_OK, 0);
			return;
		}
		else
		{
			m_MatR.release();
			int h, w;
			uchar r1, g, b;
			h = m_height1;
			w = m_width1;
			unsigned char *pDibData = m_dibFile1.m_lpImage;
			m_MatR = Mat(h, w, CV_8UC1);
			for (int r = 0; r < h; r++)
				for (int c = 0; c < w; c++)
				{
					r1 = m_Mat1.at<Vec3b>(r, c)[0];
					g = m_Mat1.at<Vec3b>(r, c)[1];
					b = m_Mat1.at<Vec3b>(r, c)[2];
					m_MatR.at<uchar>(r, c) = 0.299*r1 + 0.587*g + 0.114*b;
				}
			Create_bmiInfoHeader(&m_MatR);  // 인포헤더를 갱신 
			processedF = true; // 처리 완료를 flag을 통하여 알린다 
			g_pView->Invalidate();   // OnDraw를 호출한다
		}
	}
	else {
		AfxMessageBox(L"Read image1 first!", MB_OK, 0);
		return;
	}
}

void SWL01::Color16toGrayscale()
{
	Mat *pMat;
	pMat = &m_Mat1;
	string s1 = "16UC1";
	string st = type2str((*pMat).type());
	if (SWL01_inst.readImageF1 == true)
	{
		if (st.compare(s1) != 0) {
			AfxMessageBox(L"Only 16bit Colorscale can be shown!", MB_OK, 0);
			return;
		}
		else
		{
			m_MatR.release();
			int h, w;
			uchar r1,b,g;
			h = m_height1;
			w = m_width1;
			unsigned char *pDibData = m_dibFile1.m_lpImage;
			m_MatR = Mat(h, w, CV_8UC1);
			for (int r = 0; r < h; r++)
				for (int c = 0; c < w; c++)
				{
					ushort temp = m_Mat1.at<ushort>(r, c);
					b = temp & 0x1F;
					g = (temp>>5) & 0x1F;
					r1 = (temp >> 10) & 0x1F;
					m_MatR.at<uchar>(r, c) = (uchar)((0.299*r1 + 0.587*g + 0.114*b)*255/31);
				}
			Create_bmiInfoHeader(&m_MatR);  // 인포헤더를 갱신 
			processedF = true; // 처리 완료를 flag을 통하여 알린다 
			g_pView->Invalidate();   // OnDraw를 호출한다
		}
	}
	else {
		AfxMessageBox(L"Read image1 first!", MB_OK, 0);
		return;
	}
}